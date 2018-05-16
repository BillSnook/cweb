//
//  speed.hpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef speed_hpp
#define speed_hpp

#include "filer.hpp"

#ifdef ON_PI

//#include <wiringPi.h>
//#include <linux/i2c-dev.h>
//#include <wiringPiI2C.h>

#endif  // ON_PI

//  #define PWM_COUNT               4096
// There are 4096 possible counts per interval. This supplies 12V to the motors.
// They seem to freak out after about 6v. So we keep our count below 2048.
#define ACTUAL_PWM_COUNT		2048	// For forward AND for reverse
#define SPEED_ARRAY             8		// Number of distinct speeds we can select - f and r
#define SPEED_ADJUSTMENT        128     // Half for now to solve crash if too high

#ifdef ON_PI

#define SPEED_FILE_NAME         "/home/pi/code/c/cweb/speed.bin"

#else   // ON_PI

#define SPEED_FILE_NAME         "/Users/bill/Code/iOS/rTest/cweb/cweb/speed.bin"

#endif  // ON_PI


class Speed {
	
	bool    		debug;
	int				calibrationTestIndex;
	
public:
	explicit Speed();
	
	speed_array 	forward[SPEED_ARRAY];
	speed_array 	reverse[SPEED_ARRAY];

	
	bool setupForSpeed();
	bool resetForSpeed();
	
	void initializeSpeedArray();
	void resetSpeedArray();
	void displaySpeedArray();
	void setSpeedTestIndex( int newSpeedIndex );

	int speedLeft( int speedIndex );
	int speedRight( int speedIndex );

	void setSpeedLeft( int speedIndex, int newSpeed );
	void setSpeedRight( int speedIndex, int newSpeed );
	
	void setSpeedLeft( int newSpeed );
	void setSpeedRight( int newSpeed );
};

#endif /* speed_hpp */
