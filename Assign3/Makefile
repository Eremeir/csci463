#
# Assignment #3
# AUTHOR:  Matt Borek
#

CXX = g++
CXXFLAGS = -g -ansi -pedantic -Wall -Werror -Wextra -std=c++14

all: memsim

memsim: main.o memory.o hex.o
	$(CXX) $(CXXFLAGS) -o $@ main.o memory.o hex.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ main.cpp

memory.o: memory.cpp memory.h
	$(CXX) $(CXXFLAGS) -c -o $@ memory.cpp

hex.o: hex.cpp hex.h
	$(CXX) $(CXXFLAGS) -c -o $@ hex.cpp

.PHONY: clean
clean:
	rm -f memsim *.o
