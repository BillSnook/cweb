# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++
CXXFLAGS = -std=c++11 -DDEBUG
LDLIBS = -lpthread
# If on Raspberry Pi (as opposed to being developed on a Mac), uncomment these two lines:
CXXFLAGS += -DON_PI
LDLIBS += -lwiringPi


CC = gcc
CFLAGS = -O0 -g -Wall -c

DIR = /home/pi/code/c/cweb/cweb
OBJ_DIR = ${DIR}/obj
API_DIR = ${DIR}/vl53l0x
CORE_DIR = ${API_DIR}/Api/core
RASP_DIR = ${API_DIR}/VL53L0X_rasp/platform

#LIB_SRCS = \
	vl53l0x_api_calibration.c \
	vl53l0x_api_core.c \
	vl53l0x_api_ranging.c \
	vl53l0x_api_strings.c \
	vl53l0x_api.c \
	vl53l0x_platform.c

#LIB_OBJS  = $(LIB_SRCS:%.c=$(OBJ_DIR)/%.o)


HEADERS = mtrctl.hpp signals.hpp threader.hpp commands.hpp listen.hpp sender.hpp hardware.hpp speed.hpp tasks.hpp filer.hpp actions.hpp manager.hpp vl53l0x.hpp
VLHEADERS = ${CORE_DIR}/inc/vl53l0x_api.h ${CORE_DIR}/inc/vl53l0x_api_core.h ${CORE_DIR}/inc/vl53l0x_api_calibration.h ${CORE_DIR}/inc/vl53l0x_api_ranging.h ${CORE_DIR}/inc/vl53l0x_api_strings.h ${CORE_DIR}/inc/vl53l0x_def.h ${CORE_DIR}/inc/vl53l0x_device.h ${CORE_DIR}/inc/vl53l0x_tuning.h ${CORE_DIR}/inc/vl53l0x_interrupt_threshold_settings.h ${RASP_DIR}/inc/platform.h ${RASP_DIR}/inc/platform_log.h ${RASP_DIR}/inc/types.h

#OBJECTS = mtrctl.o signals.o threader.o commands.o listen.o sender.o hardware.o speed.o tasks.o filer.o actions.o manager.o vl53l0x.o
OBJECTS = $(wildcard ${OBJ_DIR}/*)

VLOBJECTS = ${CORE_DIR}/src/vl53l0x_api_calibration.o ${CORE_DIR}/src/vl53l0x_api_core.o ${CORE_DIR}/src/vl53l0x_api_ranging.o ${CORE_DIR}/src/vl53l0x_api_strings.o ${RASP_DIR}/src/vl53l0x_platform.o

INCLUDES = -I. -I/home/pi/code/c/cweb/cweb/vl53l0x/Api/core/inc -I/home/pi/code/c/cweb/cweb/vl53l0x/VL53L0X_rasp/platform/inc
#LIB_DIR = -L/home/pi/code/c/cweb/lib
#LDLIBS += -lVL53L0X_Rasp

VPATH = ${CORE_DIR}/src ${RASP_DIR}/src


.PHONY: clean

default: mtrctl

mtrctl: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -O0 -c -o $(OBJ_DIR)/$@ $<

$(OBJECTS): $(HEADERS)

$(OBJ_DIR)/%.o: ${CORE_DIR}/src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

$(OBJ_DIR)/%.o: ${RASP_DIR}/src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

info:
	pwd
	echo ${OBJECTS}

clean:
#	-rm -f $(OBJECTS)
	-rm -rf $(OBJ_DIR)
	-rm -f mtrctl
