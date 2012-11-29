#CC=kinst-ompp mpic++
CC=g++

OMPP=./ompp-0.7.1/lib/libompp.a

SPARSETABLE=./sparsehash-2.0.2/src

CFLAGS  = -std=gnu++0x -m64 -fopenmp -lstdc++ -Wno-write-strings -Wno-format -msse4.2 -O3 -L/soft/likwid-2.3/lib/ -I/soft/likwid-2.3/include/

INCLUDE = -I$(SPARSETABLE)

LIBS    = $(TAU_MPI_LIBS) $(TAU_LIBS) -llikwid

LDFLAGS	= $(CFLAGS)  $(TAU_LDFLAGS)

all:
	$(CC) $(INCLUDE) $(CFLAGS) $(LDFLAGS) toci.cpp access.cpp construct.cpp mapfile.cpp space.cpp  seterror.cpp maxmatopt.cpp procopt.cpp safescpy.cpp maxmatinp.cpp procmaxmat.cpp multiseq.cpp findmaxmat.cpp findmumcand.cpp cleanMUMcand.cpp streedbg.cpp scanpref.cpp linkloc.cpp dfs.cpp depthtab.cpp distribute.cpp -o toci $(LIBS)

clean:
	rm toci *.mod.* *.inc
