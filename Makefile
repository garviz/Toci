CC = g++

CFLAGS  = -std=gnu++0x -m64 -fopenmp -msse4.2 -O3

INCLUDE = -I/soft/papi-5.0.1/include/ 

LIBS    = -lstdc++ -lpapi -ldivsufsort -ldivsufsort64

LDFLAGS	= -L/soft/papi-5.0.1/lib 

all:
	$(CC) $(INCLUDE) $(CFLAGS) $(LDFLAGS) toci.cpp -o toci $(LIBS)

clean:
	rm toci 
