//
//  filer.hpp
//  cweb
//
//  Created by Bill Snook on 1/18/21.
//  Copyright © 2021 billsnook. All rights reserved.
//

#ifndef filer_hpp
#define filer_hpp

#include "speed.hpp"


class Filer {

    char hostName[32];
    char fileName[64];
    char *speedFileName = fileName;

public:
    
    char *getHostName();
    
    void setFile( int whichFile );
    void saveData( speed_array *forward, speed_array *reverse );
    bool readData( speed_array *forward, speed_array *reverse );
};


#endif /* filer_hpp */
