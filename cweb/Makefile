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

INCLUDES = -I. -I$(CORE_DIR)/inc -I$(RASP_DIR)/inc

MainFiles = $(wildcard *.cpp)
CoreSources := $(wildcard $(CORE_DIR)/src/*.c)
CoreFiles := $(notdir $(CoreSources))
RaspSources := $(wildcard $(RASP_DIR)/src/*.c)
RaspFiles := $(notdir $(RaspSources))
MainFileNames := $(basename $(MainFiles))
CoreFileNames := $(basename $(CoreFiles))
RaspFileNames := $(basename $(RaspFiles))
ObjectFiles := $(addsuffix .o, $(MainFileNames) $(CoreFileNames) $(RaspFileNames))
AllObjects := $(addprefix obj/, $(ObjectFiles))

$(shell mkdir -p $(OBJ_DIR))


.PHONY: clean info

default: mtrctl

mtrctl: $(AllObjects)
	$(CXX) $^ $(LDLIBS) -o $@

obj/%.o:: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) -o $@ $<

obj/%.o:: ${CORE_DIR}/src/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

obj/%.o:: ${RASP_DIR}/src/%.c
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(INCLUDES) -o $@ $<

info:
	@pwd
	@echo "AllObjects:"
	@echo $(AllObjects)
	
clean:
	-rm -rf $(OBJ_DIR)/*
	-rm -f mtrctl
