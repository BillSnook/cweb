//
//  speed.hpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef speed_hpp
#define speed_hpp


// There are 4096 possible counts per interval. This supplies full voltage to the motors.
// Micros seem to freak out after about 6v. So we keep our count below 2048 with a 12v supply
// For the tank we have < ~9v and the motors can take that.
#define SPEED_INDEX_MAX         9		// Number of distinct speeds we can select - f and r
#define SPEED_ADJUSTMENT        512     // Half for now to solve crash if too high
#define SPEED_MAX_PWM           4095    // Maximum PWM value to send to device

struct speed_array {
    int left;
    int right;
};


class Speed {
	
	bool    		debug;
	int				calibrationTestIndex;	// Keep track of working index
    int             speedLimit

public:
    explicit        Speed();

	speed_array 	forward[SPEED_INDEX_MAX];   // 0...8
	speed_array 	reverse[SPEED_INDEX_MAX];

	
	bool            setupForSpeed();
	bool            resetForSpeed();
	
	void            initializeSpeedArray();
	void            resetSpeedArray();
    
    void            returnSpeedArray( char *displayString );

    void            printSpeedArray();


	char            *displaySpeedArray( char * displayString );
	char            *setSpeedTestIndex( int newSpeedIndex );

	int             speedLeft( int speedIndex );
	int             speedRight( int speedIndex );

    void            setSpeedBoth( int speedIndex, int leftSpeed, int rightSpeed );

	void            setSpeedLeft( int speedIndex, int newSpeed );
	void            setSpeedRight( int speedIndex, int newSpeed );
	
	void            setSpeedLeft( int newSpeed );
	void            setSpeedRight( int newSpeed );
	
	void            setSpeedForward();
	void            setSpeedReverse();
    
    void            saveSpeedArray();
    void            makeSpeedArray();
};

#endif /* speed_hpp */
