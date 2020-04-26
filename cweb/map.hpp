//
//  map.hpp
//  cweb
//
//  Created by William Snook on 4/26/20.
//  Copyright © 2020 billsnook. All rights reserved.
//

#ifndef map_hpp
#define map_hpp


struct DistanceEntry {
    unsigned int    range;
    unsigned int    angle;
};

class SearchPattern {
public:
    explicit SearchPattern();
    explicit SearchPattern( int start, int end, int inc );

    int    startAngle;
    int    endAngle;
    int    incrementAngle;
    int indexCount;
};

class SitMap {
    
    SearchPattern        pattern;
    DistanceEntry        *distanceMap;
    
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

extern SearchPattern    pattern;
extern SitMap           sitMap;

#endif /* map_hpp */
