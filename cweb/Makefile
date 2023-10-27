# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++
CXXFLAGS = -std=c++11 -DON_PI
LDLIBS = -lpthread -lArducamDepthCamera2c

#CC = gcc
#CFLAGS = -O0 -Wall -c

WORK_DIR = $(shell pwd)

OBJ_DIR = obj
# API_DIR = ${WORK_DIR}/vl53l0x
# CORE_DIR = ${API_DIR}/Api/core
ATOF_DIR = /usr/include/c

INCLUDES = -I. -I$(ATOF_DIR)

MainFiles = $(wildcard *.cpp)
# CoreSources := $(wildcard $(CORE_DIR)/src/*.c)
# CoreFiles := $(notdir $(CoreSources))
# RaspSources := $(wildcard $(RASP_DIR)/src/*.c)
# RaspFiles := $(notdir $(RaspSources))
MainFileNames := $(basename $(MainFiles))
# CoreFileNames := $(basename $(CoreFiles))
# RaspFileNames := $(basename $(RaspFiles))
ObjectFiles := $(addsuffix .o, $(MainFileNames))
#  $(CoreFileNames) $(RaspFileNames))
AllObjects := $(addprefix obj/, $(ObjectFiles))

$(shell mkdir -p $(OBJ_DIR))


.PHONY: clean info

default: mtrctl

mtrctl: $(AllObjects)
	$(CXX) $^ $(LDLIBS) -o $@

obj/%.o:: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) -o $@ $<

# obj/%.o:: ${CORE_DIR}/src/%.c
# 	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

# obj/%.o:: ${RASP_DIR}/src/%.c
# 	$(CXX) $(CXXFLAGS) $(CFLAGS) $(INCLUDES) -o $@ $<

info:
	@pwd
	@echo "AllObjects:"
	@echo $(AllObjects)
	
clean:
	-rm -rf $(OBJ_DIR)/*
	-rm -f mtrctl
