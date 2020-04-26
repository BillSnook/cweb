# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++
CXXFLAGS = -std=c++11
LDLIBS = -lpthread
# If on Raspberry Pi (as opposed to being developed on a Mac), uncomment these two lines:
CXXFLAGS += -DON_PI
LDLIBS += -lwiringPi


CC = gcc
CFLAGS = -O0 -Wall -c

WORK_DIR = $(shell pwd)

OBJ_DIR = obj
API_DIR = ${WORK_DIR}/vl53l0x
CORE_DIR = ${API_DIR}/Api/core
RASP_DIR = ${API_DIR}/VL53L0X_rasp/platform

HEADERS = mtrctl.hpp signals.hpp threader.hpp commands.hpp listen.hpp sender.hpp hardware.hpp speed.hpp tasks.hpp filer.hpp actions.hpp manager.hpp vl53l0x.hpp wrapper.h

VLHEADERS = ${CORE_DIR}/inc/vl53l0x_api.h ${CORE_DIR}/inc/vl53l0x_api_core.h ${CORE_DIR}/inc/vl53l0x_api_calibration.h ${CORE_DIR}/inc/vl53l0x_api_ranging.h ${CORE_DIR}/inc/vl53l0x_api_strings.h ${CORE_DIR}/inc/vl53l0x_def.h ${CORE_DIR}/inc/vl53l0x_device.h ${CORE_DIR}/inc/vl53l0x_tuning.h ${CORE_DIR}/inc/vl53l0x_interrupt_threshold_settings.h

PIHEADERS = ${RASP_DIR}/inc/platform.h ${RASP_DIR}/inc/platform_log.h ${RASP_DIR}/inc/types.h

OBJECTS = ${OBJ_DIR}/mtrctl.o ${OBJ_DIR}/signals.o ${OBJ_DIR}/threader.o ${OBJ_DIR}/commands.o ${OBJ_DIR}/listen.o ${OBJ_DIR}/sender.o ${OBJ_DIR}/hardware.o ${OBJ_DIR}/speed.o ${OBJ_DIR}/tasks.o ${OBJ_DIR}/filer.o ${OBJ_DIR}/actions.o ${OBJ_DIR}/manager.o ${OBJ_DIR}/vl53l0x.o ${OBJ_DIR}/wrapper.o
#OBJECTS1 = ${OBJ_DIR}/*.o

VLOBJECTS = ${OBJ_DIR}/vl53l0x_api_calibration.o ${OBJ_DIR}/vl53l0x_api_core.o ${OBJ_DIR}/vl53l0x_api_ranging.o ${OBJ_DIR}/vl53l0x_api_strings.o ${OBJ_DIR}/vl53l0x_api.o

PIOBJECTS = ${OBJ_DIR}/vl53l0x_platform.o

MainFiles = $(wildcard *.cpp)
CoreSources := $(wildcard $(CORE_DIR)/src/*.c)
CoreFiles := $(notdir $(CoreSources))
RaspSources := $(wildcard $(RASP_DIR)/src/*.c)
RaspFiles := $(notdir $(RaspSources))
#SourceFiles := $(MainFiles) $(CoreFiles) $(RaspFiles)
MainFileNames := $(basename $(MainFiles))
CoreFileNames := $(basename $(CoreFiles))
RaspFileNames := $(basename $(RaspFiles))
AllFileNames := $(MainFileNames) $(CoreFileNames) $(RaspFileNames)
FileObjects := $(addsuffix .o, $(AllFileNames))
AllObjects := $(addprefix obj/, $(FileObjects))

INCLUDES = -I. -I$(CORE_DIR)/inc -I$(RASP_DIR)/inc

#VPATH = ${CORE_DIR}/src ${RASP_DIR}/src


.PHONY: clean info

default: mtrctl

# Test to see if it works, it is simpler, no need for OBJECTS
#mtrctl: $(AllObjects)
mtrctl: $(AllObjects)
	$(CXX) $^ $(LDLIBS) -o $@

obj/%.o:: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) -o $@ $<

#$(OBJECTS): $(PIHEADERS) $(VLHEADERS) $(HEADERS)
#$(VLOBJECTS): $(PIHEADERS) $(VLHEADERS)
#$(PIOBJECTS): $(PIHEADERS) $(VLHEADERS)

obj/%.o:: ${CORE_DIR}/src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

obj/%.o:: ${RASP_DIR}/src/%.c
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(INCLUDES) -o $@ $<

info:
	@pwd
	@echo "OBJECTS:"
	@echo $(OBJECTS)
	@echo "MainFiles:"
	@echo $(MainFiles)
	@echo "MainFileNames:"
	@echo $(MainFileNames)
	@echo "AllFileNames:"
	@echo $(AllFileNames)
	@echo "AllObjects:"
	@echo $(AllObjects)
clean:
	-rm -rf $(OBJ_DIR)/*
	-rm -f mtrctl
