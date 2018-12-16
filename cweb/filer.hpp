//
//  filer.hpp
//  cweb
//
//  Created by William Snook on 5/1/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef filer_hpp
#define filer_hpp


struct speed_array {
	int left;
	int right;
};


class Filer {

	char fileName[64];
	char *speedFileName = fileName;

public:
	explicit Filer();
	
	void setFile( int whichFile );
	void saveData( speed_array *forward, speed_array *reverse );
	bool readData( speed_array *forward, speed_array *reverse );
};

#endif /* filer_hpp */
