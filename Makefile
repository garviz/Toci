#export TAU_MAKEFILE=/home/garviz/tau-2.21.4/x86_64/lib/Makefile.tau-mpi-pdt-openmp-profile-trace-mpitrace

#include /home/garviz/tau-2.21.4/include/Makefile
#CC=tau_cxx.sh
#CC=kinst-ompp mpic++
CC=mpic++

#TAU_MPI_LIBS=/usr/lib/openmpi/lib/
#TAU_LIBS=$(HOME)/tau-2.21.4/x86_64/lib/
#TAU_MPI_INCLUDE=/usr/local/include/
#TAU_INCLUDE=$(HOME)/tau-2.21.4/include/
OMPP=$(HOME)/ompp-0.7.1/lib/libompp.a

#INCLUDE=$(TAU_INCLUDE) $(TAU_MPI_INCLUDE)
#LIB=$(TAU_MPI_LIBS) $(TAU_LIBS)

#CXX		= $(TAU_CXX)

#CC		= $(TAU_CC)

CFLAGS          = -lrt -std=gnu++0x -m64 -lgomp -fopenmp -lstdc++ -Wno-write-strings -Wno-format -ltcmalloc -lprofiler -I$(HOME)/ompp-0.7.1/include -O3

LIBS            = $(TAU_MPI_LIBS) $(TAU_LIBS) -lm

LDFLAGS	      	= $(CFLAGS)  $(TAU_LDFLAGS)

all:
	$(CC) $(INCLUDE) $(CFLAGS) $(LDFLAGS) toci.cpp access.cpp construct.cpp mapfile.cpp space.cpp  seterror.cpp maxmatopt.cpp procopt.cpp safescpy.cpp maxmatinp.cpp procmaxmat.cpp multiseq.cpp findmaxmat.cpp findmumcand.cpp cleanMUMcand.cpp streedbg.cpp scanpref.cpp linkloc.cpp dfs.cpp depthtab.cpp distribute.cpp -o toci $(LIBS)

clean:
	rm toci *.mod.* *.inc
