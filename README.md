# cweb
Server code in C++ for Raspberry Pi as command line program. Can be run at startup. Does not need to background.


Sockets working

Threading working

Communication using threads working

Get i2c working

Got motor speed table working

Scanning servo working

Define and implement control api - in process

Got ultrasonic rangefinder working in Arduino

Got light ranger working on i2c from Pi

Need to bring ultrasonic/lidar and scanner on to Pi
  Lidar i2c, scanner , ultrasonic needs controller code

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
    
listen
    Start thread to listen for connections and start command listener thread for each,
        only used if we are not a sender as determined at startup.
    Listen for commands and start threads to execute them.
sender
    Initialize and open a connection to another device.,
        only used if we are not a listener as determined at startup.

hardware
    Initializes motor controller hardware, sets up PWM parameters,
        reads speed adjustment table, initializes search and pattern.
speed
    Manages speed table to adjust for large motor speed differences,
        writes and reads table from storage using filer.
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
    
    


====================

Commands from controller to robot

    General
Get robot status
Set robot mode

Safe shutdown command

    Movement control
Setup speed ranges
Set left track speed
Set right track speed
Set straight speed

Calibrate motor speeds

    Scanner/ranger control
Set scanner position
Set ranger rate and limits
Access ranger data

Calibrate scan repeatablility

    Maintainence
Diagnostics
Test/development support

