/*
 * =====================================================================================
 *
 *       Filename:  ex.cpp
 *
 *    Description:  send and rec3ive vectors
 *
 *        Version:  1.0
 *        Created:  10/04/13 13:15:26
 *       Revision:  none
 *       Compiler:  mpic++
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  
 *
 * =====================================================================================
 */
#include <iostream>
#include <vector>
#include <mpi.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <climits>
#include "libfid/libfidxx.h"
#include "libfid/libfid.64"
#include "smhasher-read-only/MurmurHash3.h"

#define SIZE 5

void printESA(const fid_Suffixarray *esa, fid_Projectfile *project) {
    UINT lcp=0, i;
    for (i=0; i<project->totallength; i+=64) {
        fid_LCP(lcp,esa,i+1);
        printf("%ld,%ld,%ld\n",esa->suftab.VU[i], lcp, esa->stitab.VU[i]);
    }
}

int main (int argc, char *argv[]) {
    int id, p, retcode;
    unsigned short hash;
    uint32_t seed = 42; 
    unsigned short offset[USHRT_MAX];
    fid_Error error;
    fid_Projectfile prjfile;
    fid_Suffixarray esa;
    fid_Suffixinterval_64 interval;
    MPI::Init( argc, argv );
    p = MPI::COMM_WORLD.Get_size();
    id = MPI::COMM_WORLD.Get_rank();
    if (id==0) {
        fid_error_init_default(&error);
        fid_suffixarray_init(&esa,fid_UINTSIZE_64);
    	retcode = fid_projectfile_parse_from_file(&prjfile, argv[1], &error);
    	if(retcode != 0) {
            fprintf(stderr, "Error while loading project file for update mode.\n");
        }
        retcode = fid_suffixarray_load_from_project(&esa, &prjfile, fid_TABLES_OFFLINE, &error);
        if (retcode != 0) {
            fid_error_dump(&error,stdout);
        }
        printESA(&esa, &prjfile);

        //fprintf(stdout,"depth=%ld,left=%ld,right=%ld\n", interval.depth, interval.left, interval.right);
        long pos[SIZE];
        std::srand(std::time(0));
        int value;
        for (long i=0; i<SIZE; i++) {
            value = rand();
            pos[i] = value;
        }
        for (int i=1; i<p; i++)
            MPI::COMM_WORLD.Send(&pos, SIZE, MPI::LONG, i, 0);
    }
    else {
        long rec[SIZE];
        MPI::COMM_WORLD.Recv(&rec, SIZE, MPI::LONG, 0, 0);
        MurmurHash3_x86_32(argv[1], strlen(argv[1]), 0, &hash);
        std::cout << hash << std::endl;
    }
    MPI::Finalize ( );
    return 0;
}
