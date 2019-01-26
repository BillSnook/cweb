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
#include <errno.h>

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
	syslog(LOG_NOTICE, "In setupMinion, opened I2C bus on port %d", file_i2c);

#endif  // ON_PI
	
	return true;
}

bool Minion::resetMinion() {
	
	syslog(LOG_NOTICE, "In resetMinion" );
	return true;
}

long Minion::getI2CCmd() {

#ifdef ON_PI
	unsigned char buffer[20] = {0};

	//----- READ BYTES -----
	int length = 8;			// Number of bytes to read
	int response = read(file_i2c, buffer, length);
	if (response != length) {	// read() returns the number of bytes actually read,
								// if it doesn't match then an error occurred
								// (e.g. no response from the device)
		//ERROR HANDLING: i2c transaction failed
		char *errStr = strerror( errno );
		syslog(LOG_NOTICE, "In Minion::getI2CCmd, i2c read error %d: %s.\n", response, errStr );
//		syslog(LOG_NOTICE, "In Minion::getI2CCmd, failed to read from the i2c bus, only read %d bytes.\n", len);
		return -1L;
	}
	syslog(LOG_NOTICE, "Data read: %02X %02X %02X %02X\n", buffer[0], buffer[1], buffer[2], buffer[3]);
	long resp = ((buffer[0] && 0xFF) << 24) || ((buffer[1] && 0xFF) << 16) || ((buffer[2] && 0xFF) << 8) || (buffer[3] && 0xFF);
	return resp;

#else	// Else not ON_PI
	return 0;
#endif  // ON_PI

}

bool Minion::getI2CData( unsigned char *buff ) {
	
#ifdef ON_PI
	//----- READ BYTES -----
	int length = 8;			// Number of bytes to read
	int response = read(file_i2c, buff, length);
	if (response != length) {	// read() returns the number of bytes actually read,
								// if it doesn't match then an error occurred
								// (e.g. no response from the device)
		//ERROR HANDLING: i2c transaction failed
		char *errStr = strerror( errno );
		syslog(LOG_NOTICE, "In Minion::getI2CData, i2c read error %d: %s.\n", response, errStr );
		return false;
	}
//	buffer[length] = 0;	// Terminate string?
	syslog(LOG_NOTICE, "In Minion::getI2CData data read: %02X %02X %02X %02X\n", buff[0], buff[1], buff[2], buff[3]);
#endif  // ON_PI
	return true;
}

void Minion::putI2CCmd( unsigned char command, unsigned char parameter ) {

#ifdef ON_PI
	unsigned char buffer[4] = {0};
	buffer[0] = command;	// Send this command
	buffer[1] = parameter;	// With this ptional parameter
	int length = 2;			//  Number of bytes to write
	int response = write(file_i2c, buffer, length);
	if (response != length) {	// write() returns the number of bytes actually written,
								// if it doesn't match then an error occurred
								// (e.g. no response from the device)
		/* ERROR HANDLING: i2c transaction failed */
		char *errStr = strerror( errno );
		syslog(LOG_NOTICE, "In Minion::putI2CCmd, i2c write error %d: %s.\n", response, errStr );
//		syslog(LOG_NOTICE, "In Minion::putI2CCmd, failed to write to the i2c bus, got %d.", len);
	} else {
		syslog(LOG_NOTICE, "In Minion::putI2CCmd, success" );
	}
#endif  // ON_PI

}

bool Minion::putI2CData( unsigned char *newData ) {
	
#ifdef ON_PI
	int length = strlen( (const char *)newData ); //  Number of bytes to write
	syslog(LOG_NOTICE, "putI2CData, length: %d, data: %s.", length, newData);
	int response = write(file_i2c, newData, length);
	if ( response != length) {	// write() returns the number of bytes actually written,
								// if it doesn't match then an error occurred
								// (e.g. no response from the device)
		/* ERROR HANDLING: i2c transaction failed */
		char *errStr = strerror( errno );
		syslog(LOG_NOTICE, "In Minion::putI2CData, i2c write error %d: %s.\n", response, errStr );
		return false;
	} else {
		syslog(LOG_NOTICE, "In Minion::putI2CCmd, success" );
	}
#endif  // ON_PI
	return true;
}

//int Minion::testRead() {
//
//	unsigned char buffSpace[20] = {0};
//	unsigned char *buffer = buffSpace;
//	getI2CData( buffer );
//	int got = (int)strlen( (const char *)buffer );
//	syslog(LOG_NOTICE, "In Minion::testRead, %d bytes from I2C device", got);
//
//	return got;
//}
//
//void Minion::testWrite(unsigned char *data) {
//
//	putI2CData(data);
//}

// MARK: Command modes

/// At startup, mtrctl should send a startup request to the Arduino
/// and get back a status report. Thereafter, mtrctl should send a
/// heartbeat at regular intervals.

void Minion::setStatus() {
	
	putI2CCmd( 's', 0 );
	syslog(LOG_NOTICE, "In Minion::setStatus" );
}

long Minion::getStatus() {
	
//	putI2CCmd( 's' );
//	usleep( 10000 );	// 20 ms delay before reading
	unsigned char buffSpace[10] = {0};
	unsigned char *buffer = buffSpace;
	getI2CData( buffer );
	syslog(LOG_NOTICE, "In Minion::getStatus data read: %02X %02X %02X %02X\n", buffSpace[0], buffSpace[1], buffSpace[2], buffSpace[3]);

	return 4;
}

void Minion::setRange( unsigned char index ) {
	
	putI2CCmd( 'p', index );
	syslog(LOG_NOTICE, "In Minion::setRange" );
}

long Minion::getRange() {
	
	//	putI2CCmd( 's' );
	//	usleep( 10000 );	// 20 ms delay before reading
	unsigned char buffSpace[10] = {0};
	unsigned char *buffer = buffSpace;
	getI2CData( buffer );
	syslog(LOG_NOTICE, "In Minion::getRange data read: %02X %02X %02X %02X\n", buffSpace[0], buffSpace[1], buffSpace[2], buffSpace[3]);
	long range = buffSpace[3] | ( buffSpace[2] << 8 );
//	| ( buffSpace[1] << 16 )
//	| ( buffSpace[0] << 24 );
	return range;
}

