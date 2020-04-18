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



mtrctllog[1254]: In serviceCommand with: b
mtrctllog[1254]: In Manager::getStatus()
mtrctllog[1254]: In Manager::request, i2c command put on queue
mtrctllog[1254]: In Manager::monitor, i2c command from queue
mtrctllog[1254]: In Manager::execute, command type: 3, 4 started
mtrctllog[1254]: In Manager::getStatus(), wait for readWaitCond
mtrctllog[1254]: In Manager::execute, data read: 17 AA 55 CC    0x0004
mtrctllog[1254]: In Manager::execute, command type: 3, 0 completed, 0 returned
mtrctllog[1254]: In Manager::getStatus(), got readWaitCond
mtrctllog[1254]: In Manager::getStatus(), wait for readWaitCond

mtrctllog[1284]: In serviceCommand with: b
mtrctllog[1284]: In Manager::getStatus()
mtrctllog[1284]: In Manager::request, i2c command put on queue
mtrctllog[1284]: In Manager::monitor, i2c command from queue
mtrctllog[1284]: In Manager::execute, command type: 3, 4 started
mtrctllog[1284]: In Manager::getStatus(), wait for readWaitCond
mtrctllog[1284]: In Manager::execute, data read: 17 AA 55 CC    0x0004
mtrctllog[1284]: In Manager::execute, command type: 3, 0 completed, 0 returned
mtrctllog[1284]: In Manager::getStatus(), got readWaitCond: 4 - 0x17
mtrctllog[1284]: In Manager::getStatus(), wait for readWaitCond


mtrctllog[1325]: In serviceCommand with: b
mtrctllog[1325]: In Manager::getStatus()
mtrctllog[1325]: In Manager::request, i2c command put on queue
mtrctllog[1325]: In Manager::monitor, i2c command from queue
mtrctllog[1325]: In Manager::execute, command type: 3, 4 started
mtrctllog[1325]: In Manager::execute, data read: 17 AA 55 CC    0x0004
mtrctllog[1325]: In Manager::execute, command type: 3, 0 completed, 0 returned
mtrctllog[1325]: In Manager::getStatus(), wait for readWaitCond
