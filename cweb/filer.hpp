//
//  filer.hpp
//  cweb
//
//  Created by William Snook on 5/1/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef filer_hpp
#define filer_hpp


#define SPEED_ARRAY             8      // Number of distinct speeds we can select

#ifdef ON_PI

#define SPEED_FILE_NAME         "/home/pi/code/c/cweb/speed.bin"

#else   // ON_PI

#define SPEED_FILE_NAME         "/Users/bill/Code/RaspberryPi/speed.bin"

#endif  // ON_PI


struct speed_array {
	int left;
	int right;
};

class Filer {
public:
	explicit Filer();
	
	void saveData( speed_array *spd );
	bool readData( speed_array *spd );
};


#endif /* filer_hpp */
