# cweb
Server code in C++ for Raspberry Pi as command line program. Can be run at startup. Does not need to background.


Sockets working

Threading working

Communication using threads working

Get i2c working

Got motor speed table working

Scanning servo working

Define and implement control api - in process

-- Replacing untrasonic/lidar with depth field camera - tof camera

Got ultrasonic rangefinder working in Arduino

Got light ranger working on i2c from Pi

Need to bring ultrasonic/lidar and scanner on to Pi
  Lidar i2c, scanner , ultrasonic needs controller code

-- End

Profit!

To control whether or not mtrctl will run in daemon (background) mode,
the program needs to be compiled with the macro MAKE_DAEMON defined
in mtrctl.cpp.              
If that is so, the mode is enabled at run time if there are 2 command line 
arguments, usually set as 'become daemon' as a mnemonic, with the command.
In the system file /etc/rc.local, there should be a line at the end like:
./full/path/to/program/mtrctl become daemon
This line should be commented out to prevent the program from becoming
a daemon at startup.

Currently I2C is not used and we are switching to tof camera for testing
vl53l0x, wrapper, and actions are being eliminated and manager for i2c may not be needed.


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
Got scanner working on Pi
Got uSonic ranger working on Pi 

Calibrate scan repeatablility

    Maintainence
Diagnostics
Test/development support



    ToDo:
  bit control
Get Lidar working on Pi - I2C


Router wpasupplicant

ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=US

network={
        ssid="Chronos"
        psk=e9ad1d7ad40adaeb6e8254b6234f6582429c78361c0b9e43a1e78e50c4409a99
}

//   Store values per machine (hostname)
PWM min and max

Motor speed balance values


Commands for testing servo

e x                cmdPWM                    Set servo by pulse - nominally 150 - 520 -- 2 per degree, center at ~340
f x                 cmdAngle                    Set servo by angle - 0 - 180
g                   doPing                        Single ping
y                   scanTest                     Repeated sweeps, nominally 45 - 135 x 5
n x y             pinState                      Set pin x to state y


For reference: 


ArducamDepthCamera.h:

/* Copyright 2021 Arducam Technology co., Ltd. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef _ARDUCAM_DEPTH_CAMERA_H_
#define _ARDUCAM_DEPTH_CAMERA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define Status int

typedef void* ArducamDepthCamera;

typedef void* ArducamFrameBuffer;

#endif

/**
 * @brief camera connection method.
 * 
*/
typedef enum { CSI = 0, USB, CONNECT_COUNT } ArducamCameraConn;

/**
 * @brief Some types of frame data
 *
 */
typedef enum {
    RAW_FRAME = 0,
    AMPLITUDE_FRAME,
    DEPTH_FRAME,
    FRAME_TYPE_COUNT,
} ArducamFrameType;

typedef enum {
    ArducamCameraRange = 0,
} ArducamCameraCtrl;

/**
 * @brief Description of frame data format
 *
 */
typedef struct {
    //! width of frame
    uint16_t width;  
    //! height of frame     
    uint16_t height;      
    //! type of frame
    ArducamFrameType type;
    //! timestamp of frame
    uint64_t timestamp;
} ArducamFrameFormat;

/**
 * @brief Create a camera instance.
 *
 * @return Return a ArducamDepthCamera instance.
 */
extern ArducamDepthCamera createArducamDepthCamera();

/**
 * @brief Initialize the camera configuration and turn on the camera, set the initialization frame according to the @ref
 * type.
 *
 * @param camera Camera instance, obtained through @ref createArducamDepthCamera().
 * @param path Device node, the default value is video0.
 *
 * @return Return Status code.
 */
extern Status arducamCameraOpen(ArducamDepthCamera camera, ArducamCameraConn conn, int path);

/**
 * @brief Close camera.
 *
 * @param camera Camera instance.
 *
 * @return Return Status code.
 */
extern Status arducamCameraClose(ArducamDepthCamera* camera);

/**
 * @brief Start the camera stream and start processing.
 *
 * @param camera Camera instance.
 *
 * @return Return Status code.
 */
extern Status arducamCameraStart(ArducamDepthCamera camera, ArducamFrameType type);

/**
 * @brief Stop camera stream and processing.
 *
 * @param camera Camera instance.
 * @param type Type of camera output frame.
 *
 * @return Return Status code.
 */
extern Status arducamCameraStop(ArducamDepthCamera camera);

// /**
//  * @brief Specifies the frame format.
//  *
//  * @param camera Camera instance.
//  * @param format Specifies the frame format. If the set format is not supported,
//  * the function will modify the value of this parameter to return the actual value used.
//  *
//  * @return Return Status code.
//  */
// extern Status setFormat(ArducamDepthCamera *camera, FrameFormat *format);

/**
 * @brief Get the format of the specified frame.
 *
 * @param camera Frame instance.
 * @param format Frame type.
 * This parameter can be one of the following values:
 *          @arg RAW_FRAME
 *          @arg AMPLITUDE_FRAME
 *          @arg DEPTH_FRAME
 *
 * @return Return frame format.
 */
extern ArducamFrameFormat arducamCameraGetFormat(ArducamFrameBuffer fb, ArducamFrameType type);

/**
 * @brief Get the current camera output format.
 *
 * @param camera Camera instance.
 * @param id The id of the control.
 * @param val The value that needs to be set.
 *
 * @return Return Status code.
 */
extern Status arducamCameraSetCtrl(ArducamDepthCamera camera, ArducamCameraCtrl id, int val);

/**
 * @brief Get the current camera output format.
 *
 * @param camera Camera instance.
 * @param id The id of the control.
 * @param val Return the value that needs to be get.
 *
 * @return Return Status code.
 */
extern Status arducamCameraGetCtrl(ArducamDepthCamera camera, ArducamCameraCtrl id, int *val);

/**
 * @brief Read frame from the camera.
 *
 * @param camera Camera instance.
 * @param timeout Timeout time, -1 means to wait all the time, 0 means immediate range,
 * other values indicate the maximum waiting time, the unit is milliseconds.
 *
 * @return Return Status code.
 */
extern ArducamFrameBuffer arducamCameraRequestFrame(ArducamDepthCamera camera, int timeout);

/**
 * @brief Release the ArducamFrameBuffer.
 *
 * @param camera Camera instance.
 * @param fb  ArducamFrameBuffer.
 *
 * @return Return Status code.
 */
extern Status arducamCameraReleaseFrame(ArducamDepthCamera camera, ArducamFrameBuffer fb);

/**
 * @brief Read depth data from the frame.
 * @note The output mode is the depth type, and the function can be called to obtain data
 *
 * @param fb dataframe object.
 *
 * @return Return Status code.
 */
extern void* arducamCameraGetDepthData(ArducamFrameBuffer fb);
/**
 * @brief Read depth data from the frame.
 * @note The output mode is the depth type, and the function can be called to obtain data.
 *
 * @param fb dataframe object.
 *
 * @return Return Status code.
 */
extern void* arducamCameraGetAmplitudeData(ArducamFrameBuffer fb);

/**
 * @brief Read raw data from the frame.
 * @note The output mode is the raw type, and the function can be called to obtain data.
 *
 * @param fb dataframe object.
 *
 * @return Return Status code.
 */
extern void* arducamCameraGetRawData(ArducamFrameBuffer fb);

#ifdef __cplusplus
}
#endif
#endif /*__ARDUCAM_DEPTH_CAMERA_H_*/

