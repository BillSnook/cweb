//
//  message.c
//  cweb
//
//  Created by William Snook on 9/17/17.
//  Copyright © 2017 billsnook. All rights reserved.
//

#include "message.h"
#include "../../wiringPi/wiringPi/wiringPi.h"

//
//  Pin 0 = pin 17
//  Pin 1 = pin 18
//  Pin 2 = pin 21
//  Pin 3 = pin 22
//  Pin 4 = pin 23
//  Pin 5 = pin 24
//  Pin 6 = pin 25
//  Pin 7 = pin 4
//

#define LED1	1	// 18

//#define BUTTON1 7	// 4


int set = 0;

void messagesInit( void ) {
    
    wiringPiSetup();
    pinMode( LED1, OUTPUT );
    digitalWrite( LED1, 1 );
    
//    pinMode( BUTTON1, INPUT );
//    pullUpDnControl( BUTTON1, PUD_DOWN );
    
//    for( ; ; ) {
//        
//        if ( digitalRead( BUTTON1 ) == 1 ) {
//            digitalWrite( LED1, 1 );
//            //			printf( "Button on\n" );
//        } else {
//            digitalWrite( LED1, 0 );
//            //			printf( "Button off\n" );
//        }
//        //		delay( 500 );
//    }
    
}

void messageHandler( char *messageStr ) {
    
    printf( "%s", messageStr );
    if ( set == 0 ) {
        set = 1;
    } else {
        set = 0;
    }
    digitalWrite( LED1, set );
}
