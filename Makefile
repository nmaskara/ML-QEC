OBJS = Lattice.o decoder.o Base_Lattice.o Triangle.o Hexagonal.o Triangle_ColorCode.o Lattice2.o
CC = g++
DEBUG = -g
CFLAGS = -std=c++11 -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all: $(OBJS) tester.o runtest.o gendata.o genopendata.o
	$(CC) $(LFLAGS) -o tester $(OBJS) tester.o
	$(CC) $(LFLAGS) -o gendata $(OBJS) gendata.o
	$(CC) $(LFLAGS) -o genopendata $(OBJS) genopendata.o
	$(CC) $(LFLAGS) -o runtest $(OBJS) runtest.o

%.o: %.cpp
	$(CC) $(CFLAGS) $<

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) $<
	
clean:
	rm $(OBJS) tester.o runtest.o gendata.o