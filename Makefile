CC = g++
CXXFLAGS = -g -pg -O3 -m64 -fopenmp -D_GLIBCXX_PARALLEL -msse4.2 -std=c++11 
LIBS = -lstdc++ -lpthread
#LIBS = -lstdc++ -lpapi -lpthread -lcupti -lcudart
SRC = mummer.cpp qsufsort.c sparseSA.cpp fasta.cpp smhasher-read-only/libSMHasherSupport.a
#LDFLAGS = -Wl,-rpath, -L/soft/papi-5.0.1/lib -L/soft/cuda-5.0/extras/CUPTI/lib64 -L/soft/cuda-5.0/lib64 
#INCLUDE = -I/soft/papi-5.0.1/include

#include ~/tau-2.22.1/x86_64/lib/Makefile.tau-papi-pdt-openmp-opari-trace

#TAU_OPTIONS = -optKeepFiles -optVerbose

#CC = $(TAU_COMPILER)$(TAU_OPTIONS) g++

all: 
	$(CC) $(CXXFLAGS) $(INCLUDE) $(LDFLAGS) $(SRC) -o desamum $(LIBS)

# .PHONY assures clean is exected even if there is a file "./clean" in
# the directory. The same for doc.
.PHONY: clean doc
doc: 
	doxygen
clean: 
	rm -f *.o *~ desamum

