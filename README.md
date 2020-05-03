# cweb
Server code in C++ for Raspberry Pi as command line program. Can be run at startup. Does not need to background.


Sockets working

Threading working

Communication using threads working

Get i2c working

Got motor speed table working

Scanning servo working

Define and implement control api - in process

Get ultrasonic rangefinder working in microcontroller

Get light ranger working

Profit!

To control whether or not mtrctl will run in daemon (background) mode,
the program needs to be compiled with the macro MAKE_DAEMON defined
in mtrctl.cpp.              
If that is so, the mode is enabled at run time if there are 2 command line 
arguments, usually set as 'become daemon' as a mnemonic, with the command.
In the system file /etc/rc.local, there should be a line at the end like:
./full/path/to/program/mtrctl become daemon
This line should be commented out to prevent the program from becoming a daemon.


mtrctl
    Main module, controls startup, initialization and cleanup.
    Start listener to wait for connections on WiFi.
threader
    Initializes thread creation and management process.
    Start manager and it's I2C queue thread.
    Start commander to process commands on WiFi
        from cartroller app in it's own thread.
    Start taskmanager for specific tasks to be created when needed.
signals
    Manage processing of system signals, specifically kill and interrupt.
filer
    Utility to manage files for persistent data on the device.
    
listen
    Start thread to liten for connections and open a comm channel,
        only used if we are not a sender as determined at startup.
sender
    Initialize and open a connection to another device.,
        only used if we are not a listener as determined at startup.

hardware
    Initializes motor controller hardware, sets up PWM parameters,
        reads speed adjustment table, initializes search and pattern.
speed
    Manages speed table to adjust for large motor speed differences.
map
    Search and map functions to help determine location and obstacles
vl53l0x
    Light based distance measurements
    
commands
    Start thread to parse and execute commands from cartroller over WiFi.
manager
    Starts and manages a queue to gate requests for the I2C bus,
        then executes them.
tasks
    Run specific tasks on their own queue for persistence and performance.
actions
    Interface tests for vl53l0x, start specific operations for entire device control.
wrapper
    Interface file from vl53l0x to I2C queue to perform platform (Pi) specific operations.

vl53l0x library code
    Perform stand-alone operations.
    
    
