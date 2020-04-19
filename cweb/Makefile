# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++

CC = $(CROSS_COMPILE)gcc

CFLAGS = -O0 -g -Wall -c

OBJ_DIR = obj

LIB_SRCS = \
	vl53l0x_api_calibration.c \
	vl53l0x_api_core.c \
	vl53l0x_api_ranging.c \
	vl53l0x_api_strings.c \
	vl53l0x_api.c \
	vl53l0x_platform.c

LIB_OBJS  = $(LIB_SRCS:%.c=$(OBJ_DIR)/%.o)

API_DIR = /home/pi/code/c/cweb/cweb/vl53l0x
CORE_DIR = ${API_DIR}/Api/core/src
RASP_DIR = ${API_DIR}/VL53L0X_rasp/platform/src

CXXFLAGS = -std=c++11 -DDEBUG
LDLIBS = -lpthread
# If on Raspberry Pi (as opposed to being developed on a Mac), uncomment these two lines:
CXXFLAGS += -DON_PI
LDLIBS += -lwiringPi

HEADERS = mtrctl.hpp signals.hpp threader.hpp commands.hpp listen.hpp sender.hpp hardware.hpp speed.hpp tasks.hpp filer.hpp actions.hpp manager.hpp vl53l0x.hpp
OBJECTS = mtrctl.o signals.o threader.o commands.o listen.o sender.o hardware.o speed.o tasks.o filer.o actions.o manager.o vl53l0x.o
VLOBJECTS = ${CORE_DIR}/vl53l0x_api_calibration.o ${CORE_DIR}/vl53l0x_api_core.o ${CORE_DIR}/vl53l0x_api_ranging.o ${CORE_DIR}/vl53l0x_api_strings.o ${RASP_DIR}/vl53l0x_platform.o

INCLUDES = -I. -I/home/pi/code/c/cweb/cweb/vl53l0x/Api/core/inc -I/home/pi/code/c/cweb/cweb/vl53l0x/VL53L0X_rasp/platform/inc
#LIB_DIR = -L/home/pi/code/c/cweb/lib
#LDLIBS += -lVL53L0X_Rasp

VPATH = ${CORE_DIR} ${RASP_DIR}



default: mtrctl

mtrctl: $(OBJECTS) $(VLOBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LIB_DIR) $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -O0 -c -o $@ $<

$(OBJECTS): $(HEADERS)

$(OBJ_DIR)/%.o:%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

.PHONY: clean
clean:
	-rm -f $(OBJECTS)
	-rm -rf ./$(OBJ_DIR)/*
	-rm -f mtrctl
