//
//  filer.hpp
//  cweb
//
//  Created by William Snook on 5/1/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef filer_hpp
#define filer_hpp

//#include "speed.hpp"

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

Filer	filer;

#endif /* filer_hpp */
