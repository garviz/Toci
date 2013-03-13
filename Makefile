CXXFLAGS = -O3 -g -m64 -fopenmp -D_GLIBCXX_PARALLEL -mtune=native -msse4.2 -std=gnu++11
CFLAGS = -O3 -g -m64 -fopenmp -D_GLIBCXX_PARALLEL -mtune=native -msse4.2
LIBS = -lstdc++ -lpapi -lpthread
SRC = mummer.cpp qsufsort.c sparseSA.cpp fasta.cpp

all: essaMEM 

essaMEM: mummer.o qsufsort.o sparseSA.o fasta.o
	g++ $(CXXFLAGS) $^ -o $@ $(LIBS)

.cpp.o:
	g++ $(CXXFLAGS) -Wall -c $<

.c.o:
	gcc $(CFLAGS) -Wall -c $<

# .PHONY assures clean is exected even if there is a file "./clean" in
# the directory. The same for doc.
.PHONY: clean doc
doc: 
	doxygen
clean: 
	rm -f *.o *~ .depend essaMEM

# Create all the dependencies between the source files. 
.depend:
	g++ -MM $(SRC) > .depend

# The - prevents make from complaining about a missing .depend
-include .depend
