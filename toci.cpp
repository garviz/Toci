/*
 * =====================================================================================
 *
 *       Filename:  toci.cpp
 *
 *    Description:  Main program of Toci application
 *
 *        Version:  1.0
 *        Created:  27/09/11 19:53:29
 *       Revision:  none
 *       Compiler:  mpic++
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  CAOS
 *
 * =====================================================================================
 */

#include <iostream>
#include <mpi.h>
#include <cstdio>
#include "types.h"
#include "spacedef.h"
#include "args.h"
#include "protodef.h"
#include "streedef.h"
#include "streeacc.h"
#include "megabytes.h"
#include "maxmatdef.h"
#include "streetyp.h"

using namespace std;

int main(int argc, char *argv[])
{
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    double start, finish;

    Uchar *text;
    Uint textlen;
    char *filename;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    MMcallinfo mmcallinfo;
    Multiseq subjectmultiseq;
    Suffixtree stree;
    start = MPI_Wtime();
    filename =  argv[1];

    text = (Uchar *)CREATEMEMORYMAP(filename, false, &textlen);
    if (text == NULL)
    {
        fprintf(stderr,"%s: cannot open file \"%s\" ",argv[0],filename);
        fprintf(stderr,"or file \"%s\" is empty\n",filename);
        return EXIT_FAILURE;

    }
    if(textlen == 0)
    {
        fprintf(stderr,"%s: file \"%s\" is empty\n",argv[0],filename);
        return EXIT_FAILURE;
    }
    fprintf(stderr,"# construct suffix tree for sequence of length %lu\n",
           (Sint) textlen);
    fprintf(stderr,"# (maximal input length is %lu)\n",
           (Sint) getmaxtextlenstree());
    if(constructstree(&stree,text,textlen,NULL,NULL,NULL) != 0)
    {
        fprintf(stderr,"%s %s: %s\n",argv[0],filename,messagespace());
        return EXIT_FAILURE;
    }          
    cout << "Toci application for genome alignment under HPC environments" << endl;
    finish = MPI_Wtime();
    MPI_Finalize();
    cout << "Final Time: " << finish-start << endl;
}
