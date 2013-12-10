CC = /Users/jvizcaino/gcc-4.8.1/bin/g++
#CC = kinst-ompp g++
CXXFLAGS = -g -O3 -m64 -fopenmp -D_GLIBCXX_PARALLEL -msse4.2 -std=c++11
LIBS = -lstdc++ -lpthread
SRC = mummer.cpp qsufsort.c sparseSA.cpp fasta.cpp smhasher-read-only/libSMHasherSupport.a

all: 
	$(CC) $(CXXFLAGS) $(INCLUDE) $(LDFLAGS) $(SRC) -o desamum $(LIBS)

# .PHONY assures clean is exected even if there is a file "./clean" in
# the directory. The same for doc.
.PHONY: clean doc
doc: 
	doxygen
clean: 
	rm -f *.o *~ desamum

