# Makefile for mtrctl project - control a motorized robot remotely

CXX = g++

CXXFLAGS = -std=c++11 -DDEBUG
LDLIBS = -lpthread
OBJ_DIR = obj

HEADERS = mtrctl.hpp signals.hpp threader.hpp commands.hpp listen.hpp sender.hpp hardware.hpp speed.hpp tasks.hpp filer.hpp minion.hpp manager.hpp
OBJECTS = mtrctl.o signals.o threader.o commands.o listen.o sender.o hardware.o speed.o tasks.o filer.o minion.o manager.o

INCLUDES = -I.

default: mtrctl

mtrctl: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -O0 -c -o $@ $<

$(OBJECTS): $(HEADERS)

.PHONY: clean
clean:
	-rm -f $(OBJECTS)
	-rm -f mtrctl
