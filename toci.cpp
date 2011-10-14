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
#include "types.h"
#include "streetyp.h"

using namespace std;

int main(int argc, char *argv[])
{
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    Uchar *text;
    Uint textlen;
    char *filename;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    filename =  argv[1];

    text = (Uchar *) CREATEMEMORYMAP(filename, false, &textlen);
    if (rank== 0) 
    {
        Suffixtree stree;
        cout << "Toci application for genome alignment under HPC environments" << endl;
    }
    MPI_Finalize();
}
