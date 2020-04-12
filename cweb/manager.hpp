//
//  manager.hpp
//  cweb
//
//  Created by William Snook on 1/12/19.
//  Copyright © 2019 billsnook. All rights reserved.
//

#ifndef manager_hpp
#define manager_hpp

// This class manages communication with the
// Arduino microcontroller over I2C

#include    <queue>

struct DistanceEntry {
	unsigned int	range;
	unsigned int	angle;
};

class SearchPattern {
public:
	explicit SearchPattern();
	explicit SearchPattern( int start, int end, int inc );

	int	startAngle;
	int	endAngle;
	int	incrementAngle;
	int indexCount;
};

class SitMap {
	
	SearchPattern		pattern;
	DistanceEntry		*distanceMap;
	
public:

	explicit SitMap();
	explicit SitMap( SearchPattern newPattern );

	void setupSitMap();
	void resetSitMap();
	void shutdownSitMap();
	
	void updateEntry( long entry );
	char *returnMap( char *buffer );
	unsigned char *returnMapData( unsigned char *buffer );
};

enum ControllerMode {
	initialMode = 0,
	statusMode,
	rangeMode
};

enum I2CType {
    writeI2C = 0,
    readI2C = 1,
    otherI2C = 2
};


class I2CControl {
public:
    I2CType         i2cType;
    int             i2cCommand;
    int             i2cParam;
    unsigned char   *i2cData;
    
public:
    static I2CControl initControl( I2CType type, int command, int param );
    static I2CControl initControl( I2CType type, int command, char *buffer );
    const char *description();
};

class Manager {
	
	bool stopLoop;
	long lastAnythingTime;
	long lastStatusTime;
	long status;
	bool busy;
    
    std::queue<I2CControl>      i2cQueue;
    pthread_mutex_t             i2cMutex;
    pthread_cond_t              i2cCond;

    int     file_i2c;

	long getNowMs();

public:
	SearchPattern	pattern;
	SitMap			sitMap;
	
	ControllerMode	expectedControllerMode;
	
	void setupManager();
	void resetPattern( int start, int end, int inc );
	void shutdownManager();
	
	void monitor();
    void execute( I2CControl i2cControl );
    void request( I2CControl i2cControl );

	void setStatus();
	long getStatus();
	
	void setRange( unsigned int angle );
	long getRangeResult();
	unsigned int getRange();
	
	void setMotorPower( bool On );
	
	void startVL();
	void stopVL();
};

extern Manager	manager;


#endif /* manager_hpp */
