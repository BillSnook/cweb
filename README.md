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




mtrctllog[1253]: Started mtrctl as user - syslog + LOG_PERROR
mtrctllog[1253]: In setupManager
mtrctllog[1253]: In SitMap::setupSitMap(), before distanceMap, size: 19
mtrctllog[1253]: In SitMap::setupSitMap(), after distanceMap
mtrctllog[1253]: In setupCommander
mtrctllog[1253]: Setting I2C address: 0x6F, PWM freq: 50
mtrctllog[1253]: SPECIAL, oldmode read from PWM board: 0x0001 before rework
mtrctllog[1253]: Setting up speed array
mtrctllog[1253]: In setupTaskMaster
mtrctllog[1253]: mtrctl argc = 1
mtrctllog[1253]: Ready to service queue
mtrctllog[1253]: In runNextThread with managerThread, thread count 1
mtrctllog[1253]: In Manager::monitor, should only start once
mtrctllog[1253]: In runNextThread with listenThread, thread count 2
mtrctllog[1253]: Success binding to socket port 5555 on 0.0.0.0
mtrctllog[1253]: In acceptConnections, listening
mtrctllog[1253]: Accepted connection, clientAddr: 192.168.1.4
mtrctllog[1253]: In acceptConnections, listening
mtrctllog[1253]: In runNextThread with serverThread, thread count 3
mtrctllog[1253]: Here is a received message: a
mtrctllog[1253]: In runNextThread with commandThread, thread count 4
mtrctllog[1253]: In serviceCommand with: a
mtrctllog[1253]: Command a calls: setStatus()
mtrctllog[1253]: In Manager::setStatus()
mtrctllog[1253]: In Manager::request, i2c command writeI2C put on queue
mtrctllog[1253]: In runNextThread, exiting from commandThread, thread count 3
mtrctllog[1253]: In Manager::monitor, i2c command from queue
mtrctllog[1253]: In Manager::execute, command type: 2, 115 started
mtrctllog[1253]: In Manager::execute, command type: 2, 115 completed, 0x00000000 returned
mtrctllog[1253]: Here is a received message: b
mtrctllog[1253]: In runNextThread with commandThread, thread count 4
mtrctllog[1253]: In serviceCommand with: b
mtrctllog[1253]: In Manager::getStatus()
mtrctllog[1253]: In Manager::request, i2c command readI2C put on queue
mtrctllog[1253]: In Manager::request, wait for readWaitCond
mtrctllog[1253]: In Manager::monitor, i2c command from queue
mtrctllog[1253]: In Manager::execute, command type: 3, 4 started
mtrctllog[1253]: In Manager::execute, data read: AA 55 CC FF, command: 0x0004
mtrctllog[1253]: In Manager::execute, command type: 3, 0 completed, 0x00000000 returned
mtrctllog[1253]: In Manager::request, got readWaitCond: 4 - 0x01
mtrctllog[1253]: In Manager::request data read: AA55CCFF
mtrctllog[1253]: In Manager::getStatus data read: 0xAA55CCFF
mtrctllog[1253]: Command b calls: getStatus(): 0xAA55CCFF
mtrctllog[1253]: In runNextThread, exiting from commandThread, thread count 3
