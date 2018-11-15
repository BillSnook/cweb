# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++

CXXFLAGS = -std=c++11 -DDEBUG
LDLIBS = -lpthread
# If on Raspberry Pi (as opposed to being developed on a Mac), uncomment these two lines:
CXXFLAGS += -DON_PI
LDLIBS += -lwiringPi

HEADERS = mtrctl.hpp signals.hpp threader.hpp commands.hpp tasks.hpp listen.hpp sender.hpp hardware.hpp speed.hpp power.hpp filer.hpp
OBJECTS = mtrctl.o signals.o threader.o commands.o tasks.o listen.o sender.o hardware.o speed.o power.o filer.o

default: mtrctl

mtrctl: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJECTS): $(HEADERS)

clean:
	-rm -f $(OBJECTS)
	-rm -f mtrctl
