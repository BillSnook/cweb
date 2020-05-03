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

class SiteMap {
    
    SearchPattern        pattern;
    DistanceEntry        *distanceMap;
    
public:

    explicit SiteMap();
    explicit SiteMap( SearchPattern newPattern );

    void setupSiteMap();
    void resetSiteMap();
    void shutdownSiteMap();
    
    void updateEntry( long entry );
    char *returnMap( char *buffer );
    unsigned char *returnMapData( unsigned char *buffer );
};

extern SearchPattern    pattern;
extern SiteMap           siteMap;

#endif /* map_hpp */
