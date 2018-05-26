//
//  filer.hpp
//  cweb
//
//  Created by William Snook on 5/1/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef filer_hpp
#define filer_hpp

#ifdef ON_PI

#define SPEED_FILE_NAME         "/home/pi/code/c/cweb/cweb/speed.bin"

#else   // ON_PI

#define SPEED_FILE_NAME         "/Users/bill/Code/iOS/rTest/cweb/cweb/speed.bin"

#endif  // ON_PI


struct speed_array {
	int left;
	int right;
};


class Filer {
public:
	explicit Filer();
	
	void saveData( speed_array *forward, speed_array *reverse );
	bool readData( speed_array *forward, speed_array *reverse );
};

#endif /* filer_hpp */
