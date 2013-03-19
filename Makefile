CC = $(HOME)/ompp/bin/kinst-ompp-papi g++
CXXFLAGS = -O3 -g -m64 -fopenmp -D_GLIBCXX_PARALLEL -msse4.2 
CFLAGS = -O3 -g -m64 -fopenmp -D_GLIBCXX_PARALLEL -msse4.2
LIBS = -lstdc++ -lpapi -lpthread -lcupti -lcudart
SRC = mummer.cpp qsufsort.c sparseSA.cpp fasta.cpp
LDFLAGS = -Wl,-rpath, -L/soft/papi-5.0.1/lib -L/soft/cuda-5.0/extras/CUPTI/lib64 -L/soft/cuda-5.0/lib64 
INCLUDE = -I/soft/papi-5.0.1/include

all: essaMEM 

essaMEM: mummer.o qsufsort.o sparseSA.o fasta.o
	$(CC) $(CXXFLAGS) $(INCLUDE) $(LDFLAGS) $^ -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CXXFLAGS) $(INCLUDE) -Wall -c $<

.c.o:
	gcc $(CFLAGS) -Wall -c $<

# .PHONY assures clean is exected even if there is a file "./clean" in
# the directory. The same for doc.
.PHONY: clean doc
doc: 
	doxygen
clean: 
	rm -f *.o *~ .depend essaMEM
	rm *opari.*

# Create all the dependencies between the source files. 
.depend:
	g++ -MM $(SRC) > .depend

# The - prevents make from complaining about a missing .depend
-include .depend
