#
# Assignment #5
# AUTHOR:  Matt Borek
#

CXX = g++
CXXFLAGS = -g -Wall -Werror -std=c++14

all: rv32i 

rv32i: main.o rv32i_decode.o memory.o hex.o registerfile.o rv32i_hart.o cpu_single_hart.o
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.cpp rv32i_decode.h memory.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

rv32i_decode.o: rv32i_decode.cpp rv32i_decode.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

memory.o: memory.cpp memory.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

hex.o: hex.cpp hex.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

registerfile.o: registerfile.cpp registerfile.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

rv32i_hart.o: rv32i_hart.cpp rv32i_hart.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

cpu_single_hart.o: cpu_single_hart.cpp cpu_single_hart.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean download diff
clean:
	rm -rf rv32i *.o testdata outdata

download:
	mkdir -p testdata && wget --no-directories --directory-prefix=testdata --recursive --no-parent --accept .bin,.out https://faculty.cs.niu.edu/~winans/CS463/2022-fa/assignments/a5/handouts5/

diff: rv32i
	mkdir -p outdata

	./rv32i -i testdata/allinsns5.bin > outdata/allinsns5-i.out && sdiff -s testdata/allinsns5-i.out outdata/allinsns5-i.out
	./rv32i -X testdata/allinsns5.bin > outdata/allinsns5-X.out 2>&1 || true && sdiff -s testdata/allinsns5-X.out outdata/allinsns5-X.out
	./rv32i -dirz testdata/allinsns5.bin > outdata/allinsns5-dirz.out && sdiff -s testdata/allinsns5-dirz.out outdata/allinsns5-dirz.out
	./rv32i -dz testdata/allinsns5.bin > outdata/allinsns5-dz.out && sdiff -s testdata/allinsns5-dz.out outdata/allinsns5-dz.out
	./rv32i -m100 -iz testdata/allinsns5.bin > outdata/allinsns5-iz-m100.out && sdiff -s testdata/allinsns5-iz-m100.out outdata/allinsns5-iz-m100.out
	./rv32i -m100 -ir testdata/allinsns5.bin > outdata/allinsns5-ir-m100.out && sdiff -s testdata/allinsns5-ir-m100.out outdata/allinsns5-ir-m100.out
	./rv32i -m100 -irl2 testdata/allinsns5.bin > outdata/allinsns5-irl2-m100.out && sdiff -s testdata/allinsns5-irl2-m100.out outdata/allinsns5-irl2-m100.out
	./rv32i -dirz -m8500 testdata/torture5.bin > outdata/torture5-dirz-m8500.out && sdiff -s testdata/torture5-dirz-m8500.out outdata/torture5-dirz-m8500.out
	./rv32i -iz -m8500 testdata/torture5.bin > outdata/torture5-iz-m8500.out && sdiff -s testdata/torture5-iz-m8500.out outdata/torture5-iz-m8500.out
	./rv32i -z -m8500 testdata/torture5.bin > outdata/torture5-z-m8500.out && sdiff -s testdata/torture5-z-m8500.out outdata/torture5-z-m8500.out
	./rv32i -m8500 testdata/torture5.bin > outdata/torture5-m8500.out && sdiff -s testdata/torture5-m8500.out outdata/torture5-m8500.out
	./rv32i -z -m50000 testdata/sieve.bin | head -10 > outdata/sieve-z-m50000-head-10.out && sdiff -s testdata/sieve-z-m50000-head-10.out outdata/sieve-z-m50000-head-10.out
	./rv32i -z -m50000 testdata/sieve.bin | grep "^00034[01]" > outdata/sieve-z-m50000-grep-0003401.out && sdiff -s testdata/sieve-z-m50000-grep-0003401.out outdata/sieve-z-m50000-grep-0003401.out
	./rv32i -z -m50000 testdata/sieve.bin | tail -100 > outdata/sieve-z-m50000-tail-100.out && sdiff -s testdata/sieve-z-m50000-tail-100.out outdata/sieve-z-m50000-tail-100.out
	./rv32i -dirzl5 testdata/align.bin > outdata/align-dirzl5.out && sdiff -s testdata/align-dirzl5.out outdata/align-dirzl5.out
	./rv32i -dirz testdata/align2.bin > outdata/align2-dirz.out && sdiff -s testdata/align2-dirz.out outdata/align2-dirz.out