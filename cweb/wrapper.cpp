//
//  wrapper.cpp
//  cweb
//
//  Created by William Snook on 4/26/20.
//  Copyright © 2020 billsnook. All rights reserved.
//

#include "wrapper.h"
#include "manager.hpp"

extern Manager        manager;

void managerWrite( int file, int reg, int data ) { // wrapper function
    return manager.request( writeI2C, file, reg, data );
}

long managerRead( int file, int length ) {         // wrapper function
    return manager.request( readI2C, file, length );
}
