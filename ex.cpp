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
#include <limits>
#include <string>
#include <fstream>
#include "libfid/libfidxx.h"
#include "libfid/libfid.64"
#include "smhasher-read-only/MurmurHash3.h"

#define SIZE 5

using namespace std;

void printESA(const fid_Suffixarray *esa, fid_Projectfile *project) {
    UINT lcp=0, i;
    const char *ref;
    unsigned short hash=0;
    uint32_t seed = 42; 
    std::string file(project->prjbasename);
    file.append(".ois");
    ifstream ifs;
    ifs.open(file);
    string R;
    if (ifs.is_open())
        getline(ifs, R);
    ifs.close();
    vector<long> SA;  // Suffix array.
    vector<long> ISA;  // Inverse suffix array.
    vector<long> LCP; // Simulates a vector<int> LCP.
    vector<long> CHILD; //child table
    vector<vector<long> > offset;

    SA.resize(project->totallength);
    ISA.resize(project->totallength);
    CHILD.resize(project->totallength);
    LCP.resize(project->totallength);
    offset.resize(std::numeric_limits<unsigned short>::max()+1);
    for (i=0; i<project->totallength-8; i++) {
        MurmurHash3_x86_32(R.data()+esa->suftab.VU[i], 8, seed, &hash);
        fid_LCP(lcp,esa,i+1);
        SA.push_back(esa->suftab.VU[i]);
        ISA.push_back(esa->stitab.VU[i]);
        LCP.push_back(lcp);
        offset[hash].push_back(i);
        //printf("%ld,%ld,%ld,%ld\n",esa->suftab.VU[i], lcp, esa->stitab.VU[i],hash);
    }
    for(int i = 0; i < project->totallength; i++) {
        CHILD[i] = -1;
    }
        //Compute up and down values
        int lastIndex  = -1;
        vector<int> stapelUD;
        stapelUD.push_back(0);
        for(int i = 1; i < project->totallength; i++){
            while(LCP[i] < LCP[stapelUD.back()]){
                lastIndex = stapelUD.back();
                stapelUD.pop_back();
                if(LCP[i] <= LCP[stapelUD.back()] && LCP[stapelUD.back()] != LCP[lastIndex]){
                    CHILD[stapelUD.back()] = lastIndex;
                }
            }
            //now LCP[i] >= LCP[top] holds
            if(lastIndex != -1){
                CHILD[i-1] = lastIndex;
                lastIndex = -1;
            }
            stapelUD.push_back(i);
        }
        while(0 < LCP[stapelUD.back()]){//last row (fix for last character of sequence not being unique
            lastIndex = stapelUD.back();
            stapelUD.pop_back();
            if(0 <= LCP[stapelUD.back()] && LCP[stapelUD.back()] != LCP[lastIndex]){
                CHILD[stapelUD.back()] = lastIndex;
            }
        }
        //Compute Next L-index values
        vector<int> stapelNL;
        stapelNL.push_back(0);
        for(int i = 1; i < project->totallength; i++){
            while(LCP[i] < LCP[stapelNL.back()])
                stapelNL.pop_back();
            lastIndex = stapelNL.back();
            if(LCP[i] == LCP[lastIndex]){
                stapelNL.pop_back();
                CHILD[lastIndex] = i;
            }
            stapelNL.push_back(i);
        }
        vector< vector<long> >::iterator row;
        vector<long>::iterator col;
        for (row = offset.begin(); row != offset.end(); ++row) {
            for (col = row->begin(); col != row->end(); ++col) {
                std::cout << col << ",";
            }
            std::cout << endl;
        }
}

int main (int argc, char *argv[]) {
    int id, p, retcode;
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
    }
    MPI::Finalize ( );
    return 0;
}
