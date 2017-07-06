OBJS = Lattice.o decoder.o Base_Lattice.o Triangle.o Hexagonal.o
CC = g++
DEBUG = -g
CFLAGS = -std=c++11 -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
B5 = ../blossom5
BFILES = $(B5)/PerfectMatching.h $(B5)/PMinterface.o

all: $(OBJS) tester.o runtest.o gendata.o
	$(CC) $(LFLAGS) -o tester $(OBJS) tester.o
	$(CC) $(LFLAGS) -o gendata $(OBJS) gendata.o
	$(CC) $(LFLAGS) -o runtest $(OBJS) runtest.o

Lattice.o: Lattice.hpp Lattice.cpp
	$(CC) $(CFLAGS) Lattice.cpp

decoder.o: decoder.hpp decoder.cpp
	$(CC) $(CFLAGS) decoder.cpp

helper.o: helper.hpp
	$(CC) $(CFLAGS) helper.cpp

tester.o: tester.cpp
	$(CC) $(CFLAGS) tester.cpp

gendata.o: gendata.cpp
	$(CC) $(CFLAGS) gendata.cpp	

runtest.o: runtest.cpp
	$(CC) $(CFLAGS) runtest.cpp

Base_Lattice.o: Base_Lattice.cpp Base_Lattice.hpp
	$(CC) $(CFLAGS) Base_Lattice.cpp

Triangle.o: Triangle.cpp Triangle.hpp
	$(CC) $(CFLAGS) Triangle.cpp

Hexagonal.o: Hexagonal.cpp Hexagonal.hpp
	$(CC) $(CFLAGS) Hexagonal.cpp
	
clean:
	rm $(OBJS) tester.o runtest.o gendata.o