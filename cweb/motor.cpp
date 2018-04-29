//
//  motor.cpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "motor.hpp"

#include <syslog.h>			// close read write
#include <stdio.h>			// printf
#include <fcntl.h>			// open
#include <sys/ioctl.h>
#include <getopt.h>


Motor::Motor() {
	
	//    motorRunning = 1;
}

bool Motor::setupForMotor() {
	
#ifdef ON_PI
	int setupResult = wiringPiSetup();
	if ( setupResult == -1 ) {
		syslog(LOG_NOTICE, "Error setting up wiringPi." );
		return false;
	}
	syslog(LOG_NOTICE, "Pi version: %d", setupResult );
	
	pinMode( L1, SOFT_PWM_OUTPUT );
	softPwmCreate( L1, 0, 100 );
	pinMode( L2, OUTPUT );
	pinMode( L3, OUTPUT );
	pinMode( L4, OUTPUT );
	
	pinMode( M1En, SOFT_PWM_OUTPUT );
	pinMode( M1Fw, OUTPUT );
	pinMode( M1Rv, OUTPUT );
	
	pinMode( M2En, SOFT_PWM_OUTPUT );
	pinMode( M2Fw, OUTPUT );
	pinMode( M2Rv, OUTPUT );
	
	softPwmCreate( M1En, 0, 100 );
	softPwmCreate( M2En, 0, 100 );
#endif  // ON_PI
	
	//    motorRunning = 1;
	return true;
}

bool Motor::resetForMotor() {
	
#ifdef ON_PI
	softPwmStop( L1 );
	softPwmStop( M1En );
	softPwmStop( M2En );
#endif  // ON_PI
	
	//    motorRunning = 0;
	return false;
}

void Motor::blinkLED() {    // Now steps brightness using soft PWM
	
	for (int i = 10; i < 100; i += 20 ) {
#ifdef ON_PI
		//        digitalWrite (L1, HIGH) ;	// On
		//        delay (500) ;                // mS
		//        digitalWrite (L1, LOW) ;	// Off
		//        delay (500) ;
		setPWMPin( L1, i );
		delay( 1000 );
#endif  // ON_PI
	}
	setPWMPin( L1, 0 );
}

void Motor::onPin( int pin ) {
	
#ifdef ON_PI
	digitalWrite (pin, HIGH) ;	// On
#endif  // ON_PI
}

void Motor::offPin( int pin ) {
	
#ifdef ON_PI
	digitalWrite (pin, LOW) ;	// Off
#endif  // ON_PI
}

void Motor::setPWMPin( int pin, int value ) {
	
#ifdef ON_PI
	softPwmWrite( pin, value ) ;
#endif  // ON_PI
}

void Motor::checkMotor(int motor, int direction , int speed) {
	
	syslog(LOG_NOTICE, "checkMotor motor == %d", motor);
	if ( motor == 1 ) {
		setPWMPin( M1En, 0 );
		if ( direction == 1 ) {
			onPin( M1Fw );
			offPin( M1Rv );
		} else {
			offPin( M1Fw );
			onPin( M1Rv );
		}
		
		setPWMPin( M1En, speed );
#ifdef ON_PI
		delay( 1000 );
#endif  // ON_PI
		setPWMPin( M1En, 0 );
		
	}
	if ( motor == 2 ) {
		setPWMPin( M2En, 0 );
		if ( direction == 1 ) {
			onPin( M2Fw );
			offPin( M2Rv );
		} else {
			offPin( M2Fw );
			onPin( M2Rv );
		}
		
		setPWMPin( M2En, speed );
#ifdef ON_PI
		delay( 1000 );
#endif  // ON_PI
		setPWMPin( M2En, 0 );
		
	}
}

void Motor::setMtrDirSpd(int motor, int direction , int speed) {
	
	syslog(LOG_NOTICE, "setMtrDirSpd m %d, d: %s, s: %d", motor, direction ? "f" : "r", speed);
	if ( motor == 1 ) {
		if ( direction == 1 ) {
			onPin( M1Fw );
			offPin( M1Rv );
		} else {
			offPin( M1Fw );
			onPin( M1Rv );
		}
		setPWMPin( M1En, speed );
	}
	if ( motor == 2 ) {
		if ( direction == 1 ) {
			onPin( M2Fw );
			offPin( M2Rv );
		} else {
			offPin( M2Fw );
			onPin( M2Rv );
		}
		setPWMPin( M2En, speed );
	}
}

void Motor::setMtrSpd(int motor, int speed) {
	
	syslog(LOG_NOTICE, "setMtrSpd m %d, s: %d", motor, speed);
	if ( motor == 1 ) {
		setPWMPin( M1En, speed );
	}
	if ( motor == 2 ) {
		setPWMPin( M2En, speed );
	}
}
