# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++

CXXFLAGS = -std=c++11 -DDEBUG
LDLIBS = -lpthread
# If on Raspberry Pi (as opposed to being developed on a Mac), uncomment these two lines:
CXXFLAGS += -DON_PI
LDLIBS += -lwiringPi

HEADERS = mtrctl.hpp signals.hpp threader.hpp commands.hpp listen.hpp sender.hpp hardware.hpp speed.hpp tasks.hpp filer.hpp actions.hpp manager.hpp vl53l0x.hpp
OBJECTS = mtrctl.o signals.o threader.o commands.o listen.o sender.o hardware.o speed.o tasks.o filer.o actions.o manager.o vl53l0x.o vl53l0x_api_calibration.o vl53l0x_api_core.o vl53l0x_api_ranging.o vl53l0x_api_strings.o vl53l0x_api.o vl53l0x_platform.o

INCLUDES = -I. -I/home/pi/code/c/cweb/cweb/vl53l0x/Api/core/inc -I/home/pi/code/c/cweb/cweb/vl53l0x/VL53L0X_rasp/platform/inc
#LIB_DIR = -L/home/pi/code/c/cweb/lib
#LDLIBS += -lVL53L0X_Rasp

default: mtrctl

mtrctl: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -O0 -c -o $@ $<

$(OBJECTS): $(HEADERS)

.PHONY: clean
clean:
	-rm -f $(OBJECTS)
	-rm -f mtrctl
