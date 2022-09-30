#
# PROGRAM: Lab Training Exercise
# AUTHOR:  Matt Borek
#

CXXFLAGS = -Wall -Werror -std=c++11

labtrain: labtrain.o
	g++ $(CXXFLAGS) -o labtrain labtrain.o

labtrain.o: labtrain.cpp
	g++ $(CXXFLAGS) -c labtrain.cpp

clean:
	rm -f *.o labtrain