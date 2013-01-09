#CC=kinst-ompp mpic++
CC=g++

OMPP=./ompp-0.7.1/lib/libompp.a

SPARSETABLE=./sparsehash-2.0.2/src

CFLAGS  = -std=gnu++0x -m64 -fopenmp -fno-permissive -Wno-write-strings -Wno-format -msse4.2

INCLUDE = -I$(SPARSETABLE) -I/soft/likwid-3.0/include/ -I/soft/papi-5.0.1/include/

<<<<<<< HEAD
LIBS    = -llikwid -lstdc++ -lpapi
=======
LIBS    = $(TAU_MPI_LIBS) $(TAU_LIBS) -llikwid -lpapi
>>>>>>> e0176a900032e6a5ff5334778a22bd8f9f71d521

LDFLAGS	= -L/soft/likwid-3.0/lib/ -L/soft/papi-5.0.1/lib

all:
	$(CC) $(INCLUDE) $(CFLAGS) $(LDFLAGS) toci.cpp access.cpp construct.cpp mapfile.cpp space.cpp  seterror.cpp maxmatopt.cpp procopt.cpp safescpy.cpp maxmatinp.cpp procmaxmat.cpp multiseq.cpp findmaxmat.cpp findmumcand.cpp cleanMUMcand.cpp streedbg.cpp scanpref.cpp linkloc.cpp dfs.cpp depthtab.cpp distribute.cpp -o toci $(LIBS)

clean:
	rm toci 
