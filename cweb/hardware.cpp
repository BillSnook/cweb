//
//  hardware.cpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include <syslog.h>			// close read write
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sched.h>
#include <sys/time.h>

#include "mtrctl.hpp"
#include "listen.hpp"
#include "hardware.hpp"
#include "manager.hpp"
#include "map.hpp"
#include "filer.hpp"
#include "tasks.hpp"

#ifdef ON_PI

#include <pigpio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#endif  // ON_PI

#define VERSION_REQUIRED_MAJOR 1
#define VERSION_REQUIRED_MINOR 0
#define VERSION_REQUIRED_BUILD 1


#define PWM_RESOLUTION          4096.0
#define PWM_COUNT               4096	// Also used as value for PWM pin to be all on
#define PWM_MAX                 4095	// Supplys full pulse width to motor
#define PWM_FREQ                50		// For servos, motors seem to not care

//#define SPEED_INDEX_MAX			PWM_COUNT / SPEED_ADJUSTMENT	// 8

// Now by definition, DEGREE_PER_PWM is 2 degrees per pulse width
// Now we will find the 90 degree point and consider it the center, and store this value
// We will subtract 180 to make a MIN_PWM and add 180 for the MAX_PWM
#define MIN_PWM					160		// For servos, 1.0 ms
#define MAX_PWM					520		// to 2.0 ms
#define	DEGREE_PER_PWM			2       //( MAX_PWM - MIN_PWM ) / 180	// == 2 per degree == 0.5 degree accuracy?

// Pi pins - ultrasonic range-finder
#define TRIG					0		// Trigger pin for ultrasonic range finder
#define ECHO					2		// Echo pulse pin for ultrasonic range finder

#define SCAN_INVERTED           5       // Pin set to 0 or 3.3v to tell us whether our scanner servo is inverted (1 = yes)


// Sub-addresses in motor hat i2c address
#define MODE1                   0x00
#define MODE2                   0x01
#define SUBADR1                 0x02
#define SUBADR2                 0x03
#define SUBADR3                 0x04
#define PRESCALE                0xFE

#define CHANNEL0_ON_L           0x06
#define CHANNEL0_ON_H           0x07
#define CHANNEL0_OFF_L          0x08
#define CHANNEL0_OFF_H          0x09

#define ALLCHANNEL_ON_L         0xFA
#define AllCHANNEL_ON_H         0xFB
#define ALLCHANNEL_OFF_L        0xFC
#define ALLCHANNEL_OFF_H        0xFD

// Bits
#define RESTART                 0x80
#define SLEEP                   0x10
#define ALLCALL                 0x01
#define INVRT                   0x10
#define OUTDRV                  0x04


// Address of PWM channels - Fw and Rv expect their on and off values to be 0 to PWM_MAX
// If a PWM channel is used as a off or on, the value would be 0 or PWM_COUNT respectively
#define M0Fw                    9       // Motor 1 Forward enable PWM channel
#define M0Rv                    10      // Motor 1 Reverse enable - both 0 is safe off
#define M0En                    8       // Motor 1 enable, values from 0 to PWM_MAX

#define M1Fw                    11
#define M1Rv                    12
#define M1En                    13

#define M2Fw                    4
#define M2Rv                    3
#define M2En                    2

#define M3Fw                    5
#define M3Rv                    6
#define M3En                    7

#define Scanner					14

#define CHANNEL_MAX             15

#define PingWaitTime			100000      // 1/10 second


extern  Manager     manager;

bool	scanLoop;


//  MARK: - i2c interface; open, read, and write support

I2C::I2C( int addr ) {
	
//    file_i2c = manager.openI2CFile( addr );

#ifdef ON_PI
    motor_i2c = i2cOpen(1, addr, 0);
#endif  // ON_PI
    syslog( LOG_NOTICE, "In openI2CFile, I2C device handle for addr %02X: %d\n", addr, motor_i2c );
}

int I2C::i2cRead( int reg ) {
	
//    return int( manager.request( readReg8I2C, file_i2c, reg ) );

#ifdef ON_PI
    return i2cReadByteData(motor_i2c, reg);
#else
    return 0;
#endif  // ON_PI

}


void I2C::i2cWrite(int reg, int data) {
    
//    manager.request( writeI2C, file_i2c, reg, data );

#ifdef ON_PI
    int result = i2cWriteByteData(motor_i2c, reg, data);
#endif  // ON_PI
}

// MARK: - PWM support

PWM::PWM( int addr ) {
	
	i2c = new I2C( addr );
    if (i2c->motor_i2c >= 0) {
        setPWMAll( 0, 0 );                  // Clear all to 0 - stop motors
        i2c->i2cWrite( MODE2, OUTDRV );
        i2c->i2cWrite( MODE1, ALLCALL );
        i2c->i2cWrite( CHANNEL0_OFF_L, 0 );
        i2c->i2cWrite( CHANNEL0_OFF_H, 0 );

#ifdef ON_PI
        gpioDelay( 1000 );                  // Millisecond to let oscillator setup
#endif  // ON_PI
    }

}

void PWM::setPWMFrequency( int freq ) {
	
    if (i2c->motor_i2c < 0) {
        return;
    }
	int prescaleval = 25000000.0;           // Nominal clock freq 25MHz
	prescaleval /= PWM_RESOLUTION;          // 12-bit
	prescaleval /= float( freq );
	prescaleval -= 1.0;                     // For averaging
	float prescale = floor(prescaleval + 0.5);
//	syslog(LOG_NOTICE, "prescaleval: %d, prescale: %f", prescaleval, prescale );
	int prescaleSetting = int(floor(prescale));
	if ( prescaleSetting < 3 ) {
		prescaleSetting = 3;
	}
	
	int oldmode = i2c->i2cRead( MODE1 );
    syslog( LOG_NOTICE, "SPECIAL, oldmode read from PWM board: 0x%04X before rework", oldmode );
	int newmode = ( oldmode & 0x7F ) | SLEEP;   // sleep
	i2c->i2cWrite( MODE1, newmode );            // go to sleep while changing freq stuff
	i2c->i2cWrite( PRESCALE, prescaleSetting );
	i2c->i2cWrite( MODE1, oldmode );
	
#ifdef ON_PI
	gpioDelay( 1000 );                         // Millisecond to let oscillator stabilize
#endif  // ON_PI
}

void PWM::setPWM( int channel, int on, int off ) {
	
	if ( ( channel < 0 ) || ( channel > CHANNEL_MAX ) ) {
		syslog(LOG_ERR, "ERROR: PWM:setPWM channel: %d; should be 0 <= channel <= CHANNEL_MAX", channel);
		return;
	}
	if ( ( on < 0 ) || ( on > PWM_COUNT ) || ( off < 0 ) || ( off > PWM_COUNT ) ) {
		syslog(LOG_ERR, "ERROR: PWM:setPWM%d on: %d, off: %d; should be 0 <= on or off <= %d (PWM_COUNT)", channel, on, off, PWM_COUNT);
		return;
	}
	
	if ( on + off > PWM_COUNT ) {
		syslog(LOG_ERR, "ERROR: PWM:setPWM%d on: %d, off: %d; should be on + off <= %d (PWM_COUNT)", channel, on, off, PWM_COUNT);
		return;
	}
	
//	syslog(LOG_NOTICE, "PWM:setPWM%d on: %04X, off: %04X", channel, on, off);
	i2c->i2cWrite( CHANNEL0_ON_L + (4 * channel), on & 0xFF );
	i2c->i2cWrite( CHANNEL0_ON_H + (4 * channel), on >> 8 );
	i2c->i2cWrite( CHANNEL0_OFF_L + (4 * channel), off & 0xFF );
	i2c->i2cWrite( CHANNEL0_OFF_H + (4 * channel), off >> 8 );
}

void PWM::setPWMAll( int on, int off ) {
	
	if ( ( on < 0 ) || ( on > PWM_MAX ) || ( off < 0 ) || ( off > PWM_MAX ) ) {
		syslog(LOG_ERR, "ERROR: PWM:setPWMAll on: %d, off: %d; should be 0 <= on or off <= %d (PWM_MAX)", on, off, PWM_MAX);
		return;
	}
	i2c->i2cWrite( ALLCHANNEL_ON_L, on & 0xFF );
	i2c->i2cWrite( AllCHANNEL_ON_H, on >> 8 );
	i2c->i2cWrite( ALLCHANNEL_OFF_L, off & 0xFF );
	i2c->i2cWrite( ALLCHANNEL_OFF_H, off >> 8 );
}

// MARK: - Hardware interface setup and control  primitives

void Hardware::setupHardware() {
	
    syslog(LOG_NOTICE, "In setupHardware" );

    motor0Setup = false;
    motor1Setup = false;
    sweepOneWay = false;
    upsideDownScanner = false;
    cameraInitialized = false;
    gpioInitialised = false;

    setupPiGPIO();
    setupTOFCamera();

    if ( gpioInitialised ) {
        syslog(LOG_NOTICE, "In setupHardware, setting MotorI2C address: 0x%02X, PWM freq: %d", MOTOR_I2C_ADDRESS, PWM_FREQ );

        pwm = new PWM( MOTOR_I2C_ADDRESS );        // Default for Motor Hat PWM chip
        syslog(LOG_NOTICE, "In setupHardware, pwm initialized" );
        i2cDevice = pwm->i2c->motor_i2c;
        pwm->setPWMFrequency( PWM_FREQ );
        syslog(LOG_NOTICE, "In setupHardware, pwm setup" );

        speed = Speed();
        speed.initializeSpeedArray();

//     WFS - why is this being done here?  where should it be?
//        pattern = SearchPattern( 45, 135, 5 );  // Scan start, end, increment in degrees.
//        siteMap = SiteMap( pattern );
//        siteMap.setupSiteMap();

        scanLoop = false;
        i2cDevice = -1;
    }
}

void Hardware::shutdownHardware() {

//	scanStop();
//	centerServo();
	
	syslog(LOG_NOTICE, "In shutdownHardware" );
	
    shutdownTOFCamera();
    shutdownPiGPIO();

//	setPWM( M0En, 0 );		    // Turn off motors
//	setPin( M0Fw, 0 );
//	setPin( M0Rv, 0 );
//	setPWM( M1En, 0 );
//	setPin( M1Fw, 0 );
//	setPin( M1Rv, 0 );
//	
//	motor0Setup = false;
//	motor1Setup = false;
//	
//	setPWM( rangeData.servoPort, 0 );		// Unpower servos

//    siteMap.shutdownSiteMap();
}

void Hardware::setupPiGPIO() {

#ifdef ON_PI

    int cfg = gpioCfgGetInternals();
    cfg |= PI_CFG_NOSIGHANDLER;  // (1<<10) - allows us to manage signals
    gpioCfgSetInternals(cfg);

    gpioInitialised = gpioInitialise() >= 0;
    if ( ! gpioInitialised ) {
        syslog(LOG_NOTICE, "In setupHardware, gpioInitialise failed");
    }

#endif  // ON_PI

}

void Hardware::shutdownPiGPIO() {

#ifdef ON_PI

    if (gpioInitialised) {
        if (i2cDevice >= 0) {
            i2cClose(i2cDevice);
        }
        gpioTerminate();
        gpioInitialised = false;
    }

#endif  // ON_PI

}

void Hardware::setupTOFCamera() {

#ifdef ON_PI

    tof = createArducamDepthCamera();

    if ( arducamCameraOpen( tof, CSI, 0 ) ) {
        syslog(LOG_NOTICE, "In setupHardware, arducamCameraOpen failed");
    } else if ( arducamCameraStart( tof, DEPTH_FRAME ) ) {
        syslog(LOG_NOTICE, "In setupHardware, arducamCameraStart failed");
    } else {
        cameraInitialized = true;
    }

#endif  // ON_PI

}

void Hardware::shutdownTOFCamera() {

#ifdef ON_PI

    if (cameraInitialized) {
        if ( arducamCameraStop( tof ) ) {
            syslog(LOG_NOTICE, "arducamCameraStop failed");
        }
        //        if ( arducamCameraClose( &tof ) ) {
        //            syslog(LOG_NOTICE, "arducamCameraClose failed");
        //            return -1;
        //        }
        cameraInitialized = false;
    }

#endif  // ON_PI

}

long Hardware::getStatus() {

//    syslog(LOG_NOTICE, "In Hardware::getStatus()" );
    int setStatusFlags = 0;
    if ( upsideDownScanner ) {
        setStatusFlags |= statusScannerOrientation;
    }
    syslog(LOG_NOTICE, "In Hardware::getStatus response: 0x%04X\n", setStatusFlags);

    return setStatusFlags;
}

void Hardware::setPin( int pin, int value ) {
	
	if ( ( pin < 0 ) || ( pin > CHANNEL_MAX ) ) {
        syslog(LOG_ERR, "ERROR: Hardware:setPin pin: %d; should be 0 <= pin <= CHANNEL_MAX", pin);
		return;
	}
	if ( value == 0 ) {
		pwm->setPWM( pin, 0, PWM_COUNT );
	} else {
		pwm->setPWM( pin, PWM_COUNT, 0 );
	}
	return;
}

void Hardware::setPWM( int pin, int value ) {
	
	if ( ( pin < 0 ) || ( pin > CHANNEL_MAX ) ) {
		syslog(LOG_ERR, "ERROR: Hardware:setPWM pin: %d; should be 0 <= pin <= CHANNEL_MAX", pin);
		return;
	}
	if ( ( value < 0 ) || ( value > PWM_MAX ) ) {
		syslog(LOG_ERR, "ERROR: Hardware::setPWM%d value: %d; should be 0 <= value <= %d", pin, value, PWM_MAX);
		return;
	}
	syslog(LOG_INFO, "INFO: Hardware::setPWM%d value: %d", pin, value);
	pwm->setPWM( pin, 0, value );
}

// MARK: motor  manaegment section
void Hardware::setMtrDirSpd(int motor, int direction , int speedIndex) {
	
//	if ( ( speedIndex < 0 ) || ( speedIndex > SPEED_INDEX_MAX ) ) {
//		syslog(LOG_ERR, "ERROR: Hardware::setMtrDirSpd speed: %d; should be 0 <= speed <= %d", speedIndex, SPEED_INDEX_MAX);
//		return;
//	}
	syslog(LOG_NOTICE, "setMtrDirSpd m%d, d: %s, speed: %d", motor, direction ? "f" : "r", speedIndex);
	if ( motor == 0 ) {
		if ( direction == 1 ) {
			setPin( M0Fw, 1 );
			setPin( M0Rv, 0 );
		} else {
			setPin( M0Fw, 0 );
			setPin( M0Rv, 1 );
		}
		motor0Setup = true;
		setPWM( M0En, speedIndex * SPEED_ADJUSTMENT );
        //       setPWM( M0En, speed.speedLeft( speedIndex ) );
	}
	if ( motor == 1 ) {
		if ( direction == 1 ) {
			setPin( M1Fw, 1 );
			setPin( M1Rv, 0 );
		} else {
			setPin( M1Fw, 0 );
			setPin( M1Rv, 1 );
		}
		motor1Setup = true;
		setPWM( M1En, speedIndex * SPEED_ADJUSTMENT );
        //		setPWM( M1En, speed.speedRight( speedIndex ) );
	}
}

void Hardware::setMtrSpd(int motor, int speedIndex) {
	
	syslog(LOG_NOTICE, "setMtrSpd m%d, speed: %d", motor, speedIndex);
	if ( ( motor == 0 ) && motor0Setup ) {
//		setPWM( M0En, speedIndex * SPEED_ADJUSTMENT );
		setPWM( M0En, speed.speedLeft( speedIndex ) );
	}
	if ( ( motor == 1 ) && motor1Setup ) {
//		setPWM( M1En, speedIndex * SPEED_ADJUSTMENT );
		setPWM( M1En, speed.speedRight( speedIndex ) );
	}
}

void Hardware::setMotorsPWM(int direction0, int pwm0, int direction1, int pwm1) {
    
    if ( ( pwm0 < 0 ) || ( pwm0 > PWM_COUNT ) ||
         ( pwm1 < 0 ) || ( pwm1 > PWM_COUNT ) ) {
        syslog(LOG_ERR, "ERROR: Hardware::setMtrDirPWM pwm left: %d, right %d; should be 0 <= pwm <= %d", pwm0, pwm1, PWM_COUNT);
        return;
    }
    if ( direction0 == 1 ) {
        setPin( M0Fw, 1 );
        setPin( M0Rv, 0 );
    } else {
        setPin( M0Fw, 0 );
        setPin( M0Rv, 1 );
    }
    if ( direction1 == 1 ) {
        setPin( M1Fw, 1 );
        setPin( M1Rv, 0 );
    } else {
        setPin( M1Fw, 0 );
        setPin( M1Rv, 1 );
    }

    setPWM( M0En, pwm0 );
    setPWM( M1En, pwm1 );

}

void Hardware::setMotors(int direction0, int speedIndex0, int direction1, int speedIndex1) {
	if ( ( speedIndex0 < 0 ) || ( speedIndex0 > SPEED_INDEX_MAX ) ||
		 ( speedIndex1 < 0 ) || ( speedIndex1 > SPEED_INDEX_MAX ) ) {
		syslog(LOG_ERR, "ERROR: Hardware::setMtrDirSpd speed: %d - %d; should be 0 <= speed <= %d", speedIndex0, speedIndex1, SPEED_INDEX_MAX);
		return;
	}
	if ( direction0 == 1 ) {
		setPin( M0Fw, 1 );
		setPin( M0Rv, 0 );
	} else {
		setPin( M0Fw, 0 );
		setPin( M0Rv, 1 );
	}
	if ( direction1 == 1 ) {
		setPin( M1Fw, 1 );
		setPin( M1Rv, 0 );
	} else {
		setPin( M1Fw, 0 );
		setPin( M1Rv, 1 );
	}

	setPWM( M0En, speed.speedLeft( speedIndex0 ) );
	setPWM( M1En, speed.speedRight( speedIndex1 ) );

}

void Hardware::cmdSpeed( int speedIndex ) {
	
	if ( speedIndex == 0 ) {
		setPin( M0Fw, 0 );
		setPin( M0Rv, 0 );
		setPWM( M0En, 0 );
		setPin( M1Fw, 0 );
		setPin( M1Rv, 0 );
		setPWM( M1En, 0 );
		return;
	}
	int speedLeft = speed.speedLeft( speedIndex );	// Index says f or r but speedL or R is absolute
	int speedRight = speed.speedRight( speedIndex );
	syslog( LOG_NOTICE, "cmdSpeed, sl: %d, sr: %d", speedLeft, speedRight );
	if ( speedIndex < 0 ) {
		setPin( M0Fw, 1 );
		setPin( M0Rv, 0 );
		setPin( M1Fw, 1 );
		setPin( M1Rv, 0 );
	} else {
		setPin( M0Fw, 0 );
		setPin( M0Rv, 1 );
		setPin( M1Fw, 0 );
		setPin( M1Rv, 1 );
	}
	setPWM( M0En, speedLeft );
	setPWM( M1En, speedRight );
}


// MARK: servo section
int Hardware::angleToPWM( int angle ) {
	
	return minimumPWM + ( angle * DEGREE_PER_PWM );
}

void Hardware::cmdPWM( int pulseCount ) {
    
    setPWM( rangeData.servoPort, pulseCount );
}

void Hardware::cmdAngle( int angle ) {
    
	setPWM( rangeData.servoPort, angleToPWM( angle ) );	// Calibrated - adjust as needed
}

long Hardware::cmdPing() {

    long pingTime = doPing();
//    syslog(LOG_NOTICE, "Ping time is %ld useconds",  pingTime );
    usleep( 5000 );                         // WFS Test returning data

    return pingTime;
}

void Hardware::testPing(int no_of_measurements, int delay_ms) {
    
    for ( int measurement = 0; measurement < no_of_measurements; measurement++ ) {
        long pingTimeuSec = doPing();
        long cm = pingTimeuSec/29/2;
//        long inches = pingTimeuSec/74/2;
//        long mm = (pingTimeuSec*10)/29/2;
        syslog(LOG_NOTICE, "%d: %d cm\n", measurement, (int)cm );
        usleep( delay_ms * 1000 );
    }
}

long Hardware::doPing() {
    
    struct timeval tvStart, tvEnd;
//    syslog(LOG_NOTICE, "In doPing, ready to ping" );

#ifdef ON_PI
    
    int loopCount1 = 0;
    int loopCount2 = 0;
    int echoResponse;
/// WFS    digitalWrite( TRIG, 0);   // Make sure
/// WFS    usleep( 5 );
/// WFS    digitalWrite( TRIG, 1);
/// WFS    usleep( 15 );
/// WFS    digitalWrite( TRIG, 0);

    // Wait until echo goes high to indicate pulse start
    do {
        loopCount1 += 1;
/// WFS        echoResponse = digitalRead( ECHO );
    } while ( ( echoResponse == 0 ) && ( loopCount1 < 10000) );
    gettimeofday(&tvStart, NULL);

    // Wait for response on echo pin to go low indicating pulse end
    do {
        loopCount2 += 1;
/// WFS        echoResponse = digitalRead( ECHO );
    } while ( ( echoResponse != 0 ) && ( loopCount2 < 1000000) );
    gettimeofday(&tvEnd, NULL);
    
//    syslog(LOG_NOTICE, "In doPing, wait until echo goes high: %d, until echo goes low: %d", loopCount1, loopCount2 );

#endif  // ON_PI
 
    struct timeval diffTime;
    diffTime.tv_sec = tvEnd.tv_sec - tvStart.tv_sec;;
    diffTime.tv_usec = tvEnd.tv_usec - tvStart.tv_usec;
    if ( diffTime.tv_usec < 0 ) {
        diffTime.tv_sec -= 1;
        diffTime.tv_usec += 1000000;
    }
    long microSecondTime = ( diffTime.tv_sec * 1000000 ) + diffTime.tv_usec;
//    syslog(LOG_NOTICE, "In doPing, uSeconds: %ld", microSecondTime);

    return microSecondTime;
}

void Hardware::pinState( int pin, int state ) {
    
#ifdef ON_PI
/// WFS    pinMode( pin, OUTPUT );            // Make sure it is an output
/// WFS    digitalWrite( pin, state );
#endif // ON_PI
}

void Hardware::centerServo() {
	
	cmdAngle( 90 );
}

void Hardware::scanStop() {
	
	syslog(LOG_NOTICE, "In scanStop" );
	scanLoop = false;
}

// Just scan through angle range
void Hardware::scanTest() {
	if ( scanLoop ) {
		syslog(LOG_NOTICE, "Attempting to run scanTest multiple times" );
		return;				// If this is run multiple times, mayhem!
	}
	scanLoop = true;

	syslog(LOG_NOTICE, "In scanTest" );
	
	do {
		for( int angle = 45; angle < 135; angle += 5 ) {
			if ( !scanLoop ) {
				break;
			}
			syslog(LOG_NOTICE, "scanTest pin %d to %d", rangeData.servoPort, angle );
			cmdAngle( angle );
			usleep( PingWaitTime );	// .1 second
		}
		for( int angle = 135; angle > 45; angle -= 5 ) {
			if ( !scanLoop ) {
				break;
			}
			syslog(LOG_NOTICE, "scanTest pin %d to %d", rangeData.servoPort, angle );
			cmdAngle( angle );
			usleep( PingWaitTime );	// .1 second
		}
	} while ( scanLoop );
	centerServo();
}

// just ping repeatedly
void Hardware::pingLoop() {
	if ( scanLoop ) {
		syslog(LOG_NOTICE, "Attempting to run pingLoop multiple times" );
		return;				// If this is run multiple times, mayhem!
	}
	scanLoop = true;
	
	syslog(LOG_NOTICE, "In pingLoop" );
	
	do {
		ping( 0 );
		usleep( PingWaitTime );	// .1 second
	} while ( scanLoop );
}

void Hardware::prepPing( int start, int end, int inc ) {
	
    siteMap.shutdownSiteMap();
    pattern = SearchPattern( start, end, inc );
    siteMap = SiteMap( pattern );
    siteMap.setupSiteMap();
    
	sweepOneWay = false;			// For greater consistency
}

// Scan and ping through angle range
void Hardware::scanPing( int sockOrAddr ) {
    
    if ( sockOrAddr == 0 ) {
        return;
    }
	if ( scanLoop ) {
		syslog(LOG_NOTICE, "Attempting to run scanPing multiple times" );
		return;				// If this is run multiple times, mayhem!
	}
	scanLoop = true;
	
	int start = pattern.startAngle;
	int end = pattern.endAngle;
	int inc = pattern.incrementAngle;
	
	syslog(LOG_NOTICE, "In scanPing, %d - %d + %d", start, end, inc );
	
	char	*buffer = (char *)valloc( 256 );
	bzero( buffer, 256 );
	
	do {
		if ( sweepOneWay ) {
			for ( int angle = start; angle <= end; angle += inc ) {
				if ( !scanLoop ) {
					break;
				}
				unsigned int distance = ping( angle );
                if ( !scanLoop ) {
                    break;
                }
                siteMap.returnEntry( buffer, angle, distance );
                listener.writeBack( buffer, sockOrAddr );
				syslog(LOG_NOTICE, "scanPing angle: %d, distance: %u cm", angle, distance );
			}
            if ( !scanLoop ) {
                break;
            }
			cmdAngle( start );	// Start return sweep before returning map
			// 180º in .9 seconds = .005 sec / degree
			usleep( ( end - start ) * 4000 );	// .004 second / degree
			// Range newly scanned, sitmap updated - contact mother ship (app) with ping map
		} else {
			for( int angle = start; angle < end; angle += inc ) {
				if ( !scanLoop ) {
					break;
				}
				unsigned int distance = ping( angle );
                if ( !scanLoop ) {
                    break;
                }
                siteMap.returnEntry( buffer, angle, distance );
                listener.writeBack( buffer, sockOrAddr );
//				syslog(LOG_NOTICE, "scanPing angle: %d, distance: %u cm", angle, distance );
			}
			for( int angle = end; angle > start; angle -= inc ) {
				if ( !scanLoop ) {
					break;
				}
				unsigned int distance = ping( angle );	// Test
                if ( !scanLoop ) {
                    break;
                }
                siteMap.returnEntry( buffer, angle, distance );
                listener.writeBack( buffer, sockOrAddr );
//				syslog(LOG_NOTICE, "scanPing angle: %d, distance: %u cm", angle, distance );
			}
		}
	} while ( scanLoop );
//    scanLoop = false; // WFS test
	centerServo();

	free( buffer );
}


// MARK: range finder section
unsigned int Hardware::ping( unsigned int angle ) {
	
//	syslog(LOG_NOTICE, "In ping" );
	if ( !upsideDownScanner ) {
		angle = 180 - angle;
	}
    cmdAngle( angle );
    usleep( 100000 );    // 50ms to let it settle - 10 / second
    return (unsigned int)doPing();
}

long Hardware::pingTest( unsigned int angle ) {
    
    cmdAngle( angle );
    usleep(2000);   // 2ms to let it settle
    return cmdPing();
}

void Hardware::allStop() {
	
}

// MARK: Camera stuff

float Hardware::getCameraData(int socketOrAddr) {
    struct timeval tvNow;
    float savedDepth = 0.0;

    if (!cameraInitialized) {
        syslog(LOG_NOTICE, "In hardware, in getCameraData with camera not running" );
        return 0;
    }
    gettimeofday( &tvNow, NULL );
    syslog(LOG_NOTICE, "In hardware, in getCameraData started, time: %i", tvNow.tv_usec );

#ifdef ON_PI
    ArducamFrameBuffer frame;

    float *depth_ptr = 0;
//    float *amplitude_ptr = 0;
//    uint8_t *preview_ptr = (uint8_t *)malloc( 180 * 240 * sizeof(uint8_t) ) ;

    // Is this needed - apparently yes, preps format
    ArducamFrameFormat format;
    if ( ( frame = arducamCameraRequestFrame( tof, 200 ) ) != 0x00 ) {
        format = arducamCameraGetFormat( frame, DEPTH_FRAME );
        arducamCameraReleaseFrame( tof, frame );
    }

    for ( int i = 0; i < 10; i++ ) {
        if ( ( frame = arducamCameraRequestFrame( tof, 200 ) ) != 0x00 ) {
            depth_ptr = (float*)arducamCameraGetDepthData( frame );
//            amplitude_ptr = (float*)arducamCameraGetAmplitudeData( frame );
//            getPreview( preview_ptr, depth_ptr, amplitude_ptr );
            gettimeofday( &tvNow, NULL );
            savedDepth = depth_ptr[21720];
            syslog(LOG_NOTICE, "Center distance: %.2f, time: %i\n", depth_ptr[21720], tvNow.tv_usec);
//            listener.writeBack((char *)preview_ptr, socketOrAddr);
            arducamCameraReleaseFrame( tof, frame );
            usleep(1000000);
        }
    }
//    free(preview_ptr);

#endif  // ON_PI
    gettimeofday( &tvNow, NULL );
    syslog(LOG_NOTICE, "Clean exit from getCameraData routine, time: %i", tvNow.tv_usec );
    return savedDepth;
}

int Hardware::cameraDataSend(int socketOrAddr) {
    struct timeval tvNow;

    if (!cameraInitialized) {
        syslog(LOG_NOTICE, "In hardware, in cameraDataSend with camera not started" );
        return 0;
    }
    gettimeofday( &tvNow, NULL );
    syslog(LOG_NOTICE, "In hardware, in cameraDataSend started, time: %i", tvNow.tv_usec );

#ifdef ON_PI
    // 240 x 180 = 43200, half is 21600
    ArducamFrameBuffer frame;
    float *depth_ptr = 0;
    float *depth_line = 0;
    uint8_t preview_data[242];
    uint8_t *preview_ptr;   //  = &preview_data[2];
    preview_data[0] = 0x43; // "C"
    preview_data[1] = 0x30; // "0"

    ArducamFrameFormat format;
    if ( ( frame = arducamCameraRequestFrame( tof, 200 ) ) != 0x00 ) {
        format = arducamCameraGetFormat( frame, DEPTH_FRAME );
        arducamCameraReleaseFrame( tof, frame );
    }

    if ( ( frame = arducamCameraRequestFrame( tof, 200 ) ) != 0x00 ) {
        depth_ptr = (float*)arducamCameraGetDepthData( frame );
        depth_line = &depth_ptr[21600];
        for (unsigned long int i = 0; i < 240; i++) {
            float phase = (*(depth_line + i) / 2) * 255;
            uint8_t depth = phase > 255 ? 255 : phase;
            *(preview_ptr + 2 + i) = depth;
        }
        listener.writeBackCount((char *)preview_ptr, 242, socketOrAddr);
        arducamCameraReleaseFrame( tof, frame );
    }
#endif  // ON_PI

    gettimeofday( &tvNow, NULL );
    syslog(LOG_NOTICE, "Clean exit from cameraDataSend routine, time: %i", tvNow.tv_usec );

    return 0;
}

void Hardware::cameraStreamTest(int socketOrAddr) {    // Print out messages so we know this task is running

    if ( !cameraInitialized ) {
        syslog(LOG_NOTICE, "In cameraStreamTest, camera has not been started" );
        return;
    }
#ifdef ON_PI
    char msg[64];
    float x = getCameraData(socketOrAddr);
    snprintf(msg, 64, "T In cameraStreamTest, data = %.2f", x);
    listener.writeBack(msg, socketOrAddr);
    syslog(LOG_NOTICE, "In cameraStreamTest, data = %.2f", x );
#endif  // ON_PI
}

//#ifdef ON_PI
//
//void getPreview(uint8_t *preview_ptr, float *phase_image_ptr, float *amplitude_image_ptr) {
//    unsigned long int len = 240 * 180;
//    for (unsigned long int i = 0; i < len; i++) {
//        uint8_t amplitude = *(amplitude_image_ptr + i) > 30 ? 254 : 0;
//        float phase = ((1 - (*(phase_image_ptr + i) / 2)) * 255);
//        uint8_t depth = phase > 255 ? 255 : phase;
//        *(preview_ptr + i) = depth & amplitude;
//    }
//}
//
//#endif  // ON_PI

/*  Startup sequence for device

 mtrctllog[2690]: In setupHardware

 mtrctllog[2690]: In setupHardware, gpioInitialise failed

 mtrctllog[2690]: In setupTaskMaster
 mtrctllog[2690]: In setupListener


 ----    ----    Run with sudo .mtrctl:

 mtrctllog[2696]: In setupHardware

 mtrctllog[2696]: In hardware, in startCamera
 open ***WARNING*** Could not open device node /dev/video0. Please check for permissions.
 mtrctllog[2696]: arducamCameraOpen failed
 mtrctllog[2696]: In cameraInit, failed to start camera, continuing

 mtrctllog[2696]: In setupHardware, setting MotorI2C address: 0x6F, PWM freq: 50
 mtrctllog[2696]: In openI2CFile, I2C device handle for addr 6F: 0
 mtrctllog[2696]: In setupHardware, pwm initialized
 mtrctllog[2696]: SPECIAL, oldmode read from PWM board: 0x0001 before rework
 mtrctllog[2696]: In setupHardware, pwm setup
 mtrctllog[2696]: readSpeedArrays opened file
 mtrctllog[2696]: Read speed array from file

 mtrctllog[2696]: In setupTaskMaster
 mtrctllog[2696]: In setupListener
*/
