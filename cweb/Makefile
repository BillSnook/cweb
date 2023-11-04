# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++
CXXFLAGS = -std=c++11 -DON_PI
LDLIBS = -lpthread -lpigpio -lArducamDepthCamera2c


CC = gcc
CFLAGS = -O0 -Wall -c

WORK_DIR = $(shell pwd)

OBJ_DIR = obj
ATOF_DIR = /usr/include/c

INCLUDES = -I. -I$(ATOF_DIR)

MainFiles = $(wildcard *.cpp)
MainFileNames := $(basename $(MainFiles))
ObjectFiles := $(addsuffix .o, $(MainFileNames))
AllObjects := $(addprefix obj/, $(ObjectFiles))

$(shell mkdir -p $(OBJ_DIR))


.PHONY: clean info

default: sudo mtrctl

mtrctl: $(AllObjects)
	$(CXX) $^ $(LDLIBS) -o $@

obj/%.o:: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) -o $@ $<

info:
	@pwd
	@echo "AllObjects:"
	@echo $(AllObjects)
	
clean:
	-rm -rf $(OBJ_DIR)/*
	-rm -f mtrctl
