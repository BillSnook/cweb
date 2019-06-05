# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++

CXXFLAGS = -std=c++11 -DDEBUG
LDLIBS = -lpthread
# If on Raspberry Pi (as opposed to being developed on a Mac), uncomment these two lines:
CXXFLAGS += -DON_PI
LDLIBS += -lwiringPi

HEADERS = mtrctl.hpp signals.hpp threader.hpp commands.hpp listen.hpp sender.hpp hardware.hpp speed.hpp power.hpp tasks.hpp filer.hpp actions.hpp minion.hpp manager.hpp vl53l0x.hpp
OBJECTS = mtrctl.o signals.o threader.o commands.o listen.o sender.o hardware.o speed.o power.o tasks.o filer.o actions.o minion.o manager.o vl53l0x.o

INCLUDES = -I. -I/home/pi/code/c/cweb/vl53l0x/Api/core/inc -I/home/pi/code/c/cweb/vl53l0x/VL53L0X_rasp/platform/inc
LIB_DIR = -L/home/pi/code/c/cweb/lib
LDLIBS += -lVL53L0X_Rasp

default: mtrctl

mtrctl: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LIB_DIR) $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJECTS): $(HEADERS)

clean:
	-rm -f $(OBJECTS)
	-rm -f mtrctl
