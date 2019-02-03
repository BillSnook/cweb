//
//  manager.hpp
//  cweb
//
//  Created by William Snook on 1/12/19.
//  Copyright © 2019 billsnook. All rights reserved.
//

#ifndef manager_hpp
#define manager_hpp

// This class manages interations with the
// Arduino microcontroller over I2C

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
};


class Manager {
	
	bool stopLoop;
	long lastAnythingTime;
	long lastStatusTime;
	long status;
	bool busy;

	long getNowMs();

public:
	SearchPattern	pattern;
	SitMap			sitMap;
	
	
	void setupManager();
	void shutdownManager();
	
	void monitor();

	void setStatus();
	long getStatus();
	
	void setRange( unsigned int index );
	long getRangeResult();
	unsigned int getRange();

	void updateMap( long reading );
};

extern Manager	manager;


#endif /* manager_hpp */
