include $(HOME)/tau-2.22-p1/x86_64/lib/Makefile.tau-papi-pdt-openmp-opari

TAU_OPTIONS = -optKeepFiles -optVerbose

CC = $(TAU_COMPILER) $(TAU_OPTIONS) g++
#CC = kinst-ompp-papi g++
#CC = g++

OMPP=$(HOME)/ompp

SPARSETABLE=./sparsehash-2.0.2/src

CFLAGS  = -std=gnu++0x -m64 -fopenmp -fno-permissive -Wno-write-strings -Wno-format -msse4.2

INCLUDE = -I$(SPARSETABLE) -I/soft/likwid-3.0/include/ -I/soft/papi-5.0.1/include/ -I$(OMPP)/include/

LIBS    = -llikwid -lstdc++ -lpapi

LDFLAGS	= -L/soft/likwid-3.0/lib/ -L/soft/papi-5.0.1/lib -L$(OMPP)/lib/

all:
	$(CC) $(INCLUDE) $(CFLAGS) $(LDFLAGS) toci.cpp access.cpp construct.cpp mapfile.cpp space.cpp  seterror.cpp maxmatopt.cpp procopt.cpp safescpy.cpp maxmatinp.cpp procmaxmat.cpp multiseq.cpp findmaxmat.cpp findmumcand.cpp cleanMUMcand.cpp streedbg.cpp scanpref.cpp linkloc.cpp dfs.cpp depthtab.cpp distribute.cpp -o toci $(LIBS)

clean:
	rm toci 
