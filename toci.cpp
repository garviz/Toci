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
#include <cstdlib>
#include <cstring>
#include "types.h"
#include "protodef.h"
#include "errordef.h"
#include "maxmatdef.h"

/*EE
  This module contains the main function of maxmatch3. It calls
  the following three functions in an appropriate order and with
  proper arguments.
*/

/*EE
  The following function is imported form \texttt{maxmatopt.c}.
*/

Sint parsemaxmatoptions (MMcallinfo *maxmatcallinfo,
                         int argc,
                         char **argv);

/*EE
  The following function is imported form \texttt{maxmatinp.c}.
*/

Sint getmaxmatinput (Multiseq *subjectmultiseq,
                     bool matchnucleotidesonly,
                     char *subjectfile);

/*EE
  The following function is imported form \texttt{procmaxmat.c}.
*/

Sint procmaxmatches(MMcallinfo *mmcallinfo,
                    Multiseq *subjectmultiseq);

using namespace std;

int main(int argc, char *argv[])
{
    Sint retcode;
    MMcallinfo mmcallinfo;
    Multiseq subjectmultiseq;
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    double start, finish;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    retcode = parsemaxmatoptions (&mmcallinfo, argc, argv);
    if (retcode < 0) {
        fprintf(stderr,"%s: %s\n",argv[0],messagespace());
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    if (retcode == 1) {
        checkspaceleak();
        mmcheckspaceleak();
        MPI_Finalize();
        return EXIT_SUCCESS;
    }
    if (rank == 0) {
        start = MPI_Wtime();
        if (getmaxmatinput(&subjectmultiseq, mmcallinfo.matchnucleotidesonly,
                        &mmcallinfo.subjectfile[0]) != 0) {
            fprintf(stderr,"%s: %s\n",argv[0],messagespace());
            MPI_Finalize();
            return EXIT_FAILURE;
        }
        if(procmaxmatches(&mmcallinfo,&subjectmultiseq) != 0) {
            fprintf(stderr,"%s: %s\n",argv[0],messagespace());
            MPI_Finalize();
            return EXIT_FAILURE;
        }
        freemultiseq(&subjectmultiseq);
        checkspaceleak();
        mmcheckspaceleak();
        cout << "Toci application for genome alignment under HPC environments" << endl;
        finish = MPI_Wtime();
    } else {
        cout << "Process: " << rank << endl;
    }
    MPI_Finalize();
    cout << "Final Time: " << finish-start << endl;
    return EXIT_SUCCESS;
}
