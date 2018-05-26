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

//  #define PWM_COUNT               4096
// There are 4096 possible counts per interval. This supplies 12V to the motors.
// They seem to freak out after about 6v. So we keep our count below 2048.
#define ACTUAL_PWM_COUNT		2048	// For forward AND for reverse
#define SPEED_ARRAY             9		// Number of distinct speeds we can select - f and r
#define SPEED_ADJUSTMENT        256     // Half for now to solve crash if too high


class Speed {
	
	bool    		debug;
	int				calibrationTestIndex;	// Keep track of working index
	
public:
	explicit Speed();
	
	speed_array 	forward[SPEED_ARRAY];
	speed_array 	reverse[SPEED_ARRAY];

	
	bool setupForSpeed();
	bool resetForSpeed();
	
	void initializeSpeedArray();
	void resetSpeedArray();
	char * displaySpeedArray();
	char * setSpeedTestIndex( int newSpeedIndex );

	int speedLeft( int speedIndex );
	int speedRight( int speedIndex );

	void setSpeedLeft( int speedIndex, int newSpeed );
	void setSpeedRight( int speedIndex, int newSpeed );
	
	void setSpeedLeft( int newSpeed );
	void setSpeedRight( int newSpeed );
};

#endif /* speed_hpp */
