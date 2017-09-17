//
//  main.c
//  cweb
//
//  Created by William Snook on 9/17/17.
//  Copyright © 2017 billsnook. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#include "client.h"
#include "server.h"


int main(int argc, const char * argv[]) {
    
    if ( ( argc != 2 ) || ( ( strcmp( argv[1],"listen" ) != 0 ) && ( strcmp(argv[1], "sender" ) != 0 ) ) ) {
        printf( "\nWelcome to the Machine\n" );
        printf( "\nPlease enter either:" );
        printf( "\n    %s listen    to listen for messages and provide outputs  or", argv[0] );
        printf( "\n    %s sender    to send messages in response to inputs\n\n", argv[0] );
        return(0);
    }

    if ( strcmp( argv[1],"listen" ) == 0 ) {
        printf("\nStarting web connectivity\n");
        servermain();
    } else {
        clientmain();
    }
    
    return 0;
}
