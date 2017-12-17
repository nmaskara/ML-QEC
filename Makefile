SRC = src
OBJS = src/Lattice.o src/decoder.o src/Base_Lattice.o src/Triangle.o src/Hexagonal.o src/Triangle_ColorCode.o src/Lattice2.o src/Triangle_ColorCode2.o src/Twist.o
CC = g++
DEBUG = -g
CFLAGS = -std=c++11 -Wall -O2 -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all: $(OBJS) src/tester.o src/runtest.o src/gendata.o src/genopendata.o
	$(CC) $(LFLAGS) -o tester $(OBJS) src/tester.o
	$(CC) $(LFLAGS) -o gendata $(OBJS) src/gendata.o
	$(CC) $(LFLAGS) -o genopendata $(OBJS) src/genopendata.o
	$(CC) $(LFLAGS) -o runtest $(OBJS) src/runtest.o

%.o: %.cpp
	$(CC) $(CFLAGS) $<

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) $<
	
clean:
	rm $(OBJS) tester.o runtest.o gendata.o genopendata.o
