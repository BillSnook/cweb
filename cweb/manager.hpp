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
    writeReg8I2C = 0,
    readReg8I2C,
    writeI2C,
    readI2C,
    otherI2C
};


class I2CControl {
public:
    I2CType     i2cType;
    int         i2cFile;
    int         i2cCommand;     // Also, passed in as length, test for zero for done
    int         i2cParam;
    char        *i2cData;
    
public:
    static I2CControl initControl( I2CType type, int file, int command, int param );
    static I2CControl initControl( I2CType type, int file, int command, char *buffer );
    const char *description();
};

class Manager {
	
	bool stopLoop;
	long lastAnythingTime;
	long lastStatusTime;
	long status;
	bool busy;
    
    std::queue<I2CControl>      i2cQueue;
    pthread_mutex_t             i2cQueueMutex;
    pthread_cond_t              i2cQueueCond;

    pthread_mutex_t             readWaitMutex;
    pthread_cond_t              readWaitCond;

	long getNowMs();

public:
	SearchPattern	pattern;
	SitMap			sitMap;
	
	ControllerMode	expectedControllerMode;
	
    int     file_i2c;
    
	void setupManager();
	void resetPattern( int start, int end, int inc );
	void shutdownManager();
	
	void monitor();     // In own thread in loop waiting for I2C request on queue and then executing it
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
