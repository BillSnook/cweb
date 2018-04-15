#

CXX = g++
CXXFLAGS = -std=c++11
#LDLIBS = -lwiringPi

HEADERS = mtrctl.hpp signals.hpp listen.hpp
OBJECTS = mtrctl.o signals.o listen.o

default: mtrctl

mtrctl: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJECTS): $(HEADERS)

clean:
	-rm -f $(OBJECTS)
	-rm -f mtrctl
