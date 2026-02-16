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
#include "hardware.hpp"


class Filer {

public:
    
    char hostName[32];
    char speedFileName[64];

    void getHostName();
    
    void setupFiles();
    
    void saveSpeedArrays( speed_array *forward, speed_array *reverse );
    bool readSpeedArrays( speed_array *forward, speed_array *reverse );
};

extern  Filer       filer;

#endif /* filer_hpp */
