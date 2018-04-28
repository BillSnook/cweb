# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++

CXXFLAGS = -std=c++11 -DDEBUG
# If on Raspberry Pi (as opposed to being developed on a Mac), enable these two lines instead:
CXXFLAGS += -DON_PI
LDLIBS = -lwiringPi -lpthread

HEADERS = mtrctl.hpp signals.hpp threader.hpp commands.hpp listen.hpp sender.hpp hardware.hpp power.hpp
OBJECTS = mtrctl.o signals.o threader.o commands.o listen.o sender.o hardware.o power.o

default: mtrctl

mtrctl: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJECTS): $(HEADERS)

clean:
	-rm -f $(OBJECTS)
	-rm -f mtrctl
