//
//  wrapper.h
//  cweb
//
//  Created by William Snook on 4/26/20.
//  Copyright © 2020 billsnook. All rights reserved.
//

#ifndef wrapper_h
#define wrapper_h

#include <stdio.h>


extern "C" void managerWrite( int file, int reg, int data );

extern "C" long managerRead( int file, int length );

#endif /* wrapper_h */