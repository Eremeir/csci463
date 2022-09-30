#
# Assignment #2
# AUTHOR:  Matt Borek
#

CXXFLAGS = -Wall -Werror -std=c++11

prog2: prog2.o
	g++ $(CXXFLAGS) -o prog2 prog2.o

prog2.o: prog2.cpp
	g++ $(CXXFLAGS) -c prog2.cpp

clean:
	rm -f *.o prog2