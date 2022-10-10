#
# Assignment #5
# AUTHOR:  Matt Borek
#

CXX = g++
CXXFLAGS = -g -Wall -Werror -std=c++14

all: rv32i 

rv32i: main.o rv32i_decode.o memory.o hex.o
	$(CXX) $(CXXFLAGS) -o $@ main.o rv32i_decode.o memory.o hex.o

main.o: main.cpp rv32i_decode.h memory.h
	$(CXX) $(CXXFLAGS) -c -o $@ main.cpp

rv32i_decode.o: rv32i_decode.cpp rv32i_decode.h
	$(CXX) $(CXXFLAGS) -c -o $@ rv32i_decode.cpp

memory.o: memory.cpp memory.h
	$(CXX) $(CXXFLAGS) -c -o $@ memory.cpp

hex.o: hex.cpp hex.h
	$(CXX) $(CXXFLAGS) -c -o $@ hex.cpp

registerfile.o: registerfile.cpp registerfile.h
	$(CXX) $(CXXFLAGS) -c -o $@ registerfile.cpp

rv32i_hart.o: rv32i_hart.cpp rv32i_hart.h
	$(CXX) $(CXXFLAGS) -c -o $@ rv32i_hart.cpp

cpu_single_hart.o: cpu_single_hart.cpp cpu_single_hart.h
	$(CXX) $(CXXFLAGS) -c -o $@ cpu_single_hart.cpp

.PHONY: clean download diff
clean:
	rm -rf rv32i *.o testdata outdata

download:
	mkdir -p testdata && wget --no-directories --directory-prefix=testdata --recursive --no-parent --accept .bin,.out https://faculty.cs.niu.edu/~winans/CS463/2022-fa/assignments/a5/handouts5/

diff: rv32i
	mkdir -p outdata
	./rv32i -m1 testdata/li.bin > outdata/li-m1.out && sdiff -s testdata/li-m1.out outdata/li-m1.out
	./rv32i -m20 testdata/reladdr.bin > outdata/reladdr-m20.out && sdiff -s testdata/reladdr-m20.out outdata/reladdr-m20.out
	./rv32i -m40 testdata/tinyprog.bin > outdata/tinyprog-m40.out && sdiff -s testdata/tinyprog-m40.out outdata/tinyprog-m40.out
	./rv32i -ma0 testdata/small.bin > outdata/small-ma0.out && sdiff -s testdata/small-ma0.out outdata/small-ma0.out
	./rv32i -mc0 testdata/allinsns4.bin > outdata/allinsns4-mc0.out && sdiff -s testdata/allinsns4-mc0.out outdata/allinsns4-mc0.out
	./rv32i -mc0 testdata/allinsns.bin > outdata/allinsns-mc0.out && sdiff -s testdata/allinsns-mc0.out outdata/allinsns-mc0.out
	./rv32i -m100 testdata/badhex.bin > outdata/badhex-m100.out && sdiff -s testdata/badhex-m100.out outdata/badhex-m100.out
	./rv32i testdata/allinsns5.bin > outdata/allinsns5.out && sdiff -s testdata/allinsns5.out outdata/allinsns5.out
	./rv32i testdata/master.bin > outdata/master.out && sdiff -s testdata/master.out outdata/master.out
	./rv32i -m410 testdata/torture.bin > outdata/torture-m410.out && sdiff -s testdata/torture-m410.out outdata/torture-m410.out
	./rv32i -m500 testdata/torture5.bin > outdata/torture5-m500.out && sdiff -s testdata/torture5-m500.out outdata/torture5-m500.out
	./rv32i -m1000 testdata/pcrel.bin > outdata/pcrel-m1000.out && sdiff -s testdata/pcrel-m1000.out outdata/pcrel-m1000.out
	./rv32i -m33e68 testdata/sieve.bin > outdata/sieve-m33e68.out && sdiff -s testdata/sieve-m33e68.out outdata/sieve-m33e68.out
