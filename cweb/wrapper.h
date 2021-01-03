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

#ifdef __cplusplus
extern "C" {
#endif

int managerOpen( int address );

void managerWrite( int file, int reg, int data );

long managerRead( int file, int length );

#ifdef __cplusplus
}
#endif

#endif /* wrapper_h */
