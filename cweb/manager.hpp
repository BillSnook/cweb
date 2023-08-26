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

#include <queue>
#include <pthread.h>


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
    int         i2cCommand;     // Passed in as register or length, test for zero for done?
    int         i2cParam;
    char        *i2cData;
    
public:
    static I2CControl initControl( I2CType type, int file, int command, int param );
    static I2CControl initControl( I2CType type, int file, int command, char *buffer );
    const char *description();
};

class Manager {
	
    bool stopLoop;
    bool endLoop;
	long lastAnythingTime;
	long lastStatusTime;
    
    std::queue<I2CControl>      i2cQueue;
    pthread_mutex_t             i2cQueueMutex;
    pthread_cond_t              i2cQueueCond;

    pthread_mutex_t             readWaitMutex;
    pthread_cond_t              readWaitCond;

	long getNowMs();

public:
	ControllerMode	expectedControllerMode;
	
    int  arduino_i2c;           // Unused but may reappear in the future
    
	void setupManager();
	void shutdownManager();
	
	void monitor();     // In own thread in loop waiting for I2C request on queue and then executing it
    int openI2CFile( int address );                                 // Return open I2C file descriptor for address
    void execute( I2CControl i2cControl );                          // Run command
    void request( I2CType type, int file, int command, int param ); // Writes, two bytes or register then single byte
    long request( I2CType type, int file, int command );            // Reads with long result values
//    long request( I2CControl writelist[] );                       // Multiple address write

//    int readReg8( int file, int reg );
    
//	void setRange( unsigned int angle );
//	long getRangeResult();
//	unsigned int getRange();
	
//	void startVL();
//	void stopVL();
};

#endif /* manager_hpp */
