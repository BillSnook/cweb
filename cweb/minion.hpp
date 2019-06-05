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

#include <wiringPi.h>
#include <linux/i2c-dev.h>
#include <wiringPiI2C.h>

#endif  // ON_PI

#define ArdI2CAddr				8

struct remoteStatus {
	int				vInValue;			// Current measured main power supply voltage
	bool			mainPowerState;		// If power is enabled to the motors and servos
	bool			otherState;			// TBD
};

class Minion {

	int				pi2c;
	int 			file_i2c;
	unsigned char	lastAngle;

public:
	remoteStatus	status;		// CUrrent status value
	
	explicit Minion();

	bool setupMinion( int i2cAddr, int fp );
	bool resetMinion();

	long getI2CCmd();
	bool getI2CData( unsigned char *buff );
	void putI2CCmd( unsigned char command, unsigned char parameter );
	bool putI2CData( unsigned char *newData );

//	int	testRead();
//	void testWrite( unsigned char *data );
//
	void setStatus();
	long getStatus();
	
	void setRange( unsigned char angle );
	long getRange();
	
	void setRelay( int relay, bool On );
};

#endif /* minion_hpp */
