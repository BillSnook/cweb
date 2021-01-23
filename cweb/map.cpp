//
//  map.cpp
//  cweb
//
//  Created by William Snook on 4/26/20.
//  Copyright © 2020 billsnook. All rights reserved.
//

#include <stdlib.h>            // malloc
#include <stdio.h>             // sprintf
#include <syslog.h>            // close read write
#include <string.h>            // strcat
#include <unistd.h>


#include "map.hpp"


SearchPattern    pattern;
SiteMap          siteMap;


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

// MARK: SiteMap
SiteMap::SiteMap() {
    
    pattern = SearchPattern();
}

SiteMap::SiteMap( SearchPattern newPattern ) {
    
    pattern = newPattern;
}

void SiteMap::setupSiteMap() {
    
//    syslog(LOG_NOTICE, "In SiteMap::setupSiteMap(), before distanceMap, size: %d", pattern.indexCount );
    distanceMap = new DistanceEntry[pattern.indexCount];
//    syslog(LOG_NOTICE, "In SiteMap::setupSiteMap(), after distanceMap" );
    for ( int i = 0; i < pattern.indexCount; i++ ) {
        distanceMap[ i ].angle = 0;
        distanceMap[ i ].range = 0;
    }
}

void SiteMap::resetSiteMap() {
    
    for ( int i = 0; i < pattern.indexCount; i++ ) {
        distanceMap[ i ].angle = 0;
        distanceMap[ i ].range = 0;
    }
}

void SiteMap::shutdownSiteMap() {
    
    delete distanceMap;
}

void SiteMap::updateEntry( long entry ) {
    
    unsigned int angle = (entry >> 16) & 0x0FFFF;        // Actual range value
    unsigned int range = entry & 0x0FFFF;                // Angle used to track value of range
//    syslog(LOG_NOTICE, "In SiteMap::updateEntry(), angle: %u, range: %u", angle, range );
    updateEntry( angle, range );
}

void SiteMap::updateEntry( unsigned int angle, unsigned int range ) {
    
    if ( ( angle <= pattern.endAngle ) && ( angle >= pattern.startAngle ) ) {
        unsigned int index = ( angle - pattern.startAngle ) / pattern.incrementAngle;
//        syslog(LOG_NOTICE, "In SiteMap::updateEntry(), index: %u", index );
        if ( ( index >= 0 ) && ( index < pattern.indexCount ) ) {
            distanceMap[ index ].range = range;
            distanceMap[ index ].angle = angle;
        }
    }
}

char *SiteMap::returnEntry( char *buffer, unsigned int angle, unsigned int range ) {

    updateEntry( angle, range );
//    sprintf( buffer, "@Rng\n%s%4d  %5d\n", buffer, angle, range );
    memcpy( buffer, "No Mas", 6)
    
//    syslog(LOG_NOTICE, "In SiteMap::returnEntry()\n%s", buffer );
    return buffer;
}

char *SiteMap::returnMap( char *buffer ) {

    sprintf( buffer, "@Map\n" );
    for ( int i = 0; i < pattern.indexCount; i++ ) {
        sprintf( buffer, "%s %4d  %5d\n", buffer, distanceMap[ i ].angle, distanceMap[ i ].range );
    }
    sprintf( buffer, "%s\n", buffer );    // Terminate string
//    syslog(LOG_NOTICE, "In SiteMap::returnMap()\n%s", buffer );
    return buffer;
}

unsigned char *SiteMap::returnMapData( unsigned char *buffer ) {    // buffer is 1024 bytes

//    for ( int i = 0; i < pattern.indexCount; i++ ) {
//        sprintf( buffer, "%s %4d  %4d\n", buffer, distanceMap[ i ].angle, distanceMap[ i ].range );
//    }
//    sprintf( buffer, "%s\n", buffer );
    memcpy( buffer, &pattern, sizeof( pattern ) );
    memcpy( buffer + sizeof( pattern ), distanceMap, sizeof( distanceMap ) * pattern.indexCount );
    return buffer;
}

