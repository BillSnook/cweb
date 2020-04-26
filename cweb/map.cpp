//
//  map.cpp
//  cweb
//
//  Created by William Snook on 4/26/20.
//  Copyright © 2020 billsnook. All rights reserved.
//

#include <syslog.h>            // close read write

#include "map.hpp"


SearchPattern    pattern;
SitMap            sitMap;


// MARK: SearchPattern
SearchPattern::SearchPattern() {

//    startAngle = 0;
//    endAngle = 180;
//    incrementAngle = 20;
//    indexCount = ( ( endAngle - startAngle ) / incrementAngle ) + 1;
    
    SearchPattern( 45, 135, 5 );
}

SearchPattern::SearchPattern( int start, int end, int inc ) {

    startAngle = start;
    endAngle = end;
    incrementAngle = inc;
    indexCount = ( ( endAngle - startAngle ) / incrementAngle ) + 1;
}

// MARK: SitMap
SitMap::SitMap() {
    
    pattern = SearchPattern();
}

SitMap::SitMap( SearchPattern newPattern ) {
    
    pattern = newPattern;
}

void SitMap::setupSitMap() {
    
    syslog(LOG_NOTICE, "In SitMap::setupSitMap(), before distanceMap, size: %d", pattern.indexCount );
    distanceMap = new DistanceEntry[pattern.indexCount];
    syslog(LOG_NOTICE, "In SitMap::setupSitMap(), after distanceMap" );
    for ( int i = 0; i < pattern.indexCount; i++ ) {
        distanceMap[ i ].angle = 0;
        distanceMap[ i ].range = 0;
    }
}

void SitMap::resetSitMap() {
    
    for ( int i = 0; i < pattern.indexCount; i++ ) {
        distanceMap[ i ].angle = 0;
        distanceMap[ i ].range = 0;
    }
}

void SitMap::shutdownSitMap() {
    
    delete distanceMap;
}

void SitMap::updateEntry( long entry ) {
    
    unsigned int range = (entry >> 16) & 0x0FFFF;        // Actual range value
    unsigned int angle = entry & 0x0FFFF;                // Angle used to track value of range
//    syslog(LOG_NOTICE, "In SitMap::updateEntry(), angle: %u, range: %u", angle, range );

    if ( ( angle <= pattern.endAngle ) && ( angle >= pattern.startAngle ) ) {
        unsigned int index = ( angle - pattern.startAngle ) / pattern.incrementAngle;
//        syslog(LOG_NOTICE, "In SitMap::updateEntry(), index: %u", index );
        if ( ( index >= 0 ) && ( index < pattern.indexCount ) ) {
            distanceMap[ index ].range = range;
            distanceMap[ index ].angle = angle;
        }
    }
}

char *SitMap::returnMap( char *buffer ) {

    sprintf( buffer, "@Map\n" );
    for ( int i = 0; i < pattern.indexCount; i++ ) {
        sprintf( buffer, "%s %4d  %5d\n", buffer, distanceMap[ i ].angle, distanceMap[ i ].range );
    }
    sprintf( buffer, "%s\n", buffer );    // Terminate string
//    syslog(LOG_NOTICE, "In SitMap::returnMap()\n%s", buffer );
    return buffer;
}

unsigned char *SitMap::returnMapData( unsigned char *buffer ) {    // buffer is 1024 bytes

//    for ( int i = 0; i < pattern.indexCount; i++ ) {
//        sprintf( buffer, "%s %4d  %4d\n", buffer, distanceMap[ i ].angle, distanceMap[ i ].range );
//    }
//    sprintf( buffer, "%s\n", buffer );
    memcpy( buffer, &pattern, sizeof( pattern ) );
    memcpy( buffer + sizeof( pattern ), distanceMap, sizeof( distanceMap ) * pattern.indexCount );
    return buffer;
}

