//
//  minion.hpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef minion_hpp
#define minion_hpp

#ifdef ON_PI

//#include <wiringPi.h>
//#include <linux/i2c.h>
//#include <linux/i2c-dev.h>
//#include <wiringPiI2C.h>

#endif  // ON_PI

#include "hardware.hpp"

//#define I2C_SLAVE    0x0703
//#define I2C_SMBUS    0x0720    /* SMBus-level access */
//
//#define I2C_SMBUS_READ    1
//#define I2C_SMBUS_WRITE    0
//
//// SMBus transaction types
//
//#define I2C_SMBUS_QUICK             0
//#define I2C_SMBUS_BYTE              1
//#define I2C_SMBUS_BYTE_DATA         2
//#define I2C_SMBUS_WORD_DATA         3
//#define I2C_SMBUS_PROC_CALL         4
//#define I2C_SMBUS_BLOCK_DATA        5
//#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
//#define I2C_SMBUS_BLOCK_PROC_CALL   7        /* SMBus 2.0 */
//#define I2C_SMBUS_I2C_BLOCK_DATA    8

// SMBus messages

//#define I2C_SMBUS_BLOCK_MAX    32    /* As specified in SMBus standard */
//#define I2C_SMBUS_I2C_BLOCK_MAX    32    /* Not specified but we use same structure */


struct remoteStatus {
	int				vInValue;			// Current measured main power supply voltage
	bool			mainPowerState;		// If power is enabled to the motors and servos
	bool			otherState;			// TBD
};

class Minion {

	int				pi2c;
	unsigned char	lastAngle;

public:
	int 			file_i2c;
	remoteStatus	status;		// CUrrent status value
	
	explicit Minion();

	bool setupMinion( int i2cAddr );
	bool shutdownMinion();

	long getI2CCmd();
	bool getI2CData( unsigned char *buff );
	void putI2CCmd( unsigned char command, unsigned char parameter );
//	bool putI2CData( unsigned char *newData );

//	int	testRead();
//	void testWrite( unsigned char *data );
//
	void setStatus();
	long getStatus();
	
	void setRange( unsigned char angle );
	long getRange();
	
	void setRelay( int relay, bool On );
    
    void test1();
    void test2();
};

#endif /* minion_hpp */
