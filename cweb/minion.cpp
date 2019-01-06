//
//  minion.cpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include <unistd.h>			// close read write
#include <stdio.h>			// printf
#include <fcntl.h>			// open
#include <sys/ioctl.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>

#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#ifdef ON_PI
#include "/usr/include/linux/i2c-dev.h"		//Needed for I2C port
#endif  // ON_PI

#include "minion.hpp"


Minion::Minion() {
	
}

bool Minion::setupMinion( int i2cAddr ) {
	
	pi2c = i2cAddr;
	
#ifdef ON_PI
	
	//----- OPEN THE I2C BUS -----
	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0) {
		//ERROR HANDLING: you can check errno to see what went wrong
		syslog(LOG_NOTICE, "Failed to open the i2c bus");
		return false;
	}
	
	if (ioctl(file_i2c, I2C_SLAVE, pi2c) < 0) {
		syslog(LOG_NOTICE, "Failed to acquire bus access and/or talk to slave");
		//ERROR HANDLING; you can check errno to see what went wrong
		return false;
	}
	syslog(LOG_NOTICE, "In setupMinion, opened %s on device %d", filename, file_i2c);

#endif  // ON_PI
	
	return true;
}

bool Minion::resetMinion() {
	
	syslog(LOG_NOTICE, "In resetMinion" );
	return true;
}

int Minion::getI2CReg() {

#ifdef ON_PI
	unsigned char buffer[20] = {0};
	int length;

	//----- READ BYTES -----
	length = 4;			// Number of bytes to read
	if (read(file_i2c, buffer, length) != length) {		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
		
		//ERROR HANDLING: i2c transaction failed
		syslog(LOG_NOTICE, "Failed to read from the i2c bus.\n");
		return -1;
	} else {
		syslog(LOG_NOTICE, "Data read: %s\n", buffer);
		return buffer[0];
	}
#else	// Else not ON_PI
	return 0;
#endif  // ON_PI

}

unsigned char *Minion::getI2CData() {
	
#ifdef ON_PI
	unsigned char buffer[20];
	int length;
	
	//----- READ BYTES -----
	length = 4;			// Number of bytes to read
	if (read(file_i2c, buffer, length) != length) {		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
		
		//ERROR HANDLING: i2c transaction failed
		syslog(LOG_NOTICE, "Failed to read from the i2c bus.\n");
	} else {
//		buffer[length] = 0;	// Terminate string?
		syslog(LOG_NOTICE, "Data read: %s\n", buffer);
	}
	return buffer;
#else	// Else not ON_PI
	return 0;
#endif  // ON_PI
	
}

void Minion::putI2CReg( int newValue ) {

#ifdef ON_PI
	unsigned char buffer[20] = {0};
	int length;

	//----- WRITE BYTES -----
	buffer[0] = 0x01;
	buffer[1] = newValue;
	length = 2;			//  Number of bytes to write
	if (write(file_i2c, buffer, length) != length) {		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
		
		/* ERROR HANDLING: i2c transaction failed */
		syslog(LOG_NOTICE, "Failed to write to the i2c bus.");
	}
#endif  // ON_PI

}

bool Minion::putI2CData( unsigned char *newData ) {
	
#ifdef ON_PI
//	unsigned char buffer[20] = {0};
	int length;
	
	//----- WRITE BYTES -----
//	buffer[0] = 0x01;
//	buffer[1] = newValue;
	length = strlen( (const char *)newData ); // 2;			//  Number of bytes to write
	if (write(file_i2c, newData, length) != length) {		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
		
		/* ERROR HANDLING: i2c transaction failed */
		syslog(LOG_NOTICE, "Failed to write to the i2c bus.");
		return false;
	}
#endif  // ON_PI
	return true;
}

int Minion::testRead() {
	
	int got = 0;
#ifdef ON_PI
	unsigned char *result = getI2CData();
	got = strlen( (const char *)result );
	syslog(LOG_NOTICE, "Read 0x%X from I2C device", got);
#endif // ON_PI
	
	return got;
}

void Minion::testWrite(unsigned char *data) {
	
	putI2CData(data);

}
