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
//#include <mpi.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <limits>
#include <string>
#include <fstream>
#include <unordered_map>
#include <stack>
#include <parallel/algorithm>
#include "libfid/libfidxx.h"
#include "libfid/libfid.64"
#include "smhasher-read-only/MurmurHash3.h"

#define SIZE 5

using namespace std;

struct vec_uchar {
  struct item_t{
    item_t(size_t i, int v) { idx = i; val = v; }
    size_t idx; int val;
    bool operator < (item_t t) const { return idx < t.idx;  }
  };
  vector<unsigned char> vec;  // LCP values from 0-65534
  vector<item_t> M;
  void resize(size_t N) { vec.resize(N); }
  // Vector X[i] notation to get LCP values.
 int operator[] (size_t idx) {
    if(vec[idx] == numeric_limits<unsigned char>::max()) 
      return lower_bound(M.begin(), M.end(), item_t(idx,0))->val;
    else
      return vec[idx]; 
  }
  // Actually set LCP values, distinguishes large and small LCP
  // values.
  void set(size_t idx, int v) {
    if (v >= numeric_limits<unsigned char>::max()) {
      vec.at(idx) = numeric_limits<unsigned char>::max();
      M.push_back(item_t(idx, v));
    }
    else { 
        vec.at(idx) = (unsigned char)v; 
    }
  }
  // Once all the values are set, call init. This will assure the
  // values >= 255 are sorted by index for fast retrieval.
  void init() {
      __gnu_parallel::sort(M.begin(), M.end()); 
  }
  size_t size(void) {
    return vec.size()+M.size();
  }
};

struct hashMmH3 { 
    size_t operator() (string preffix) const
    { 
        unsigned int hash;
        uint32_t seed = 42; 
        MurmurHash3_x86_32(preffix.c_str(), 8, seed, &hash);
        return hash;
    }
};

struct inter {
    long l = -1, r = -2;
    inter() : l(-1), r(-2) {}
    inter(long lv, long rv) : l(lv), r(rv) {}
    void update(long val) {
        if (val<l) l=val;
        else if (val>r) r=val;
    }
    long size() {return r-l+1;}
};

void printESA(const fid_Suffixarray *esa, fid_Projectfile *project) {
    UINT lcp=0, i;
    const char *ref;
    unsigned int hash;
    std::string file(project->prjbasename);
    file.append(".ois");
    ifstream ifs(file);
    string R;
    uint32_t seed = 42; 

    if (ifs.is_open())
        getline(ifs, R);
    ifs.close();
    vector<long long> SA;  // Suffix array.
    vector<long long> ISA;  // Inverse suffix array.
    //vector<long> LCP; // Simulates a vector<int> LCP.
    vec_uchar LCP; // Simulates a vector<int> LCP.
    vector<long long> CHILD; //child table
    unordered_map<string,inter,hashMmH3> offset;
    //unordered_map<string,inter> offset;
    //map<string,long> offset;
/*    for (unsigned int i=0; i < offset.size(); i++) {
        inter t(0,0);
        offset[i]=t;
    }*/
    long size;

    SA.resize(project->totallength);
    ISA.resize(project->totallength);
    CHILD.resize(project->totallength);
    LCP.resize(project->totallength);
    //offset.resize(std::numeric_limits<unsigned int>::max()+1);
    for (i=0; i<project->totallength-8; i++) {
        fid_LCP(lcp,esa,i+1);
        SA[i]=esa->suftab.VU[i];
        ISA[i]=esa->stitab.VU[i];
        long m = ISA[i]; 
        if(m==0) LCP.set(m, 0); 
        else LCP.set(m, lcp); 
        if (offset.count(R.substr(SA[i],8))>0) {
            offset[R.substr(SA[i],8)].update(i);
        } else {
            inter t(i,i);
            offset[R.substr(SA[i],8)]=t;
        }    
    }
/*    LCP.init();
    fill(CHILD.begin(), CHILD.end(),-1);
    //Compute up and down values
    long long lastIndex  = -1;
    stack<long long,vector<long long> > stapelUD;
    stapelUD.push(0);
    for(long long i = 1; i  < project->totallength; i++){
        while(LCP[i] < LCP[stapelUD.top()]){
            lastIndex = stapelUD.top();
            stapelUD.pop();
            if(LCP[i] <= LCP[stapelUD.top()] && LCP[stapelUD.top()] != LCP[lastIndex]){
                CHILD[stapelUD.top()] = lastIndex;
            }
        }
        //now LCP[i] >= LCP[top] holds
        if(lastIndex != -1){
            CHILD[i-1] = lastIndex;
            lastIndex = -1;
        }
        stapelUD.push(i);
    }
    while(0 < LCP[stapelUD.top()]){//last row (fix for last character of sequence not being unique
        lastIndex = stapelUD.top();
        stapelUD.pop();
        if(0 <= LCP[stapelUD.top()] && LCP[stapelUD.top()] != LCP[lastIndex]){
            CHILD[stapelUD.top()] = lastIndex;
        }
    }
    //Compute Next L-index values
    stack<long long,vector<long long> > stapelNL;
    stapelNL.push(0);
    for(long long i = 1; i < project->totallength; i++){
        while(LCP[i] < LCP[stapelNL.top()])
            stapelNL.pop();
            lastIndex = stapelNL.top();
            if(LCP[i] == LCP[lastIndex]){
                stapelNL.pop();
                CHILD[lastIndex] = i;
            }
            stapelNL.push(i);
    }  
    string file2(project->prjbasename);
    file2.append(".SA");
    ofstream sa(file2, ios::binary);
    size = SA.size();
    sa.write(reinterpret_cast<char*>(&size), sizeof(size));
    sa.write(reinterpret_cast<char*>(&SA[0]), size*sizeof(SA[0]));
    sa.close();
    string file3(project->prjbasename);
    file3.append(".ISA");
    ofstream isa(file3, ios::binary);
    size = ISA.size();
    isa.write(reinterpret_cast<char*>(&size), sizeof(size));
    isa.write(reinterpret_cast<char*>(&ISA[0]), size*sizeof(ISA[0]));
    isa.close();
    string file4(project->prjbasename);
    file4.append(".LCP");
    ofstream lcpf(file4);
    size = LCP.size();
    lcpf << size << endl;
    //lcpf.write(reinterpret_cast<char*>(&size), sizeof(size));
    for (long i=0;i<LCP.size(); i++)
        lcpf << LCP[i] << endl;
    //lcpf.write(reinterpret_cast<char*>(&LCP[0]), size*sizeof(LCP[0]));
    lcpf.close();
    string file5(project->prjbasename);
    file5.append(".CHILD");
    ofstream ch(file5, ios::binary);
    size = CHILD.size();
    ch.write(reinterpret_cast<char*>(&size), sizeof(size));
    ch.write(reinterpret_cast<char*>(&CHILD[0]), size*sizeof(CHILD[0]));
    ch.close();*/
    string file6(project->prjbasename);
    file6.append(".off");
    ofstream of(file6);
    for (auto it = offset.begin(); it != offset.end(); ++it)
        of << it->first << " " << it->second.l << " " << it->second.r << endl;
    of.close();
/*         ifstream is("suf.dat", ios::binary);
 *         vector<long> SA2;
 *         long size2;
 *         is.read(reinterpret_cast<char*>(&size2), sizeof(size2));
 *         cout << is.tellg() << endl;
 *         SA2.resize(size2);
 *         is.read(reinterpret_cast<char*>(&SA2[0]),size2*sizeof(SA2[0]));
 *         is.close();
 */
}

int main (int argc, char *argv[]) {
    int id, p, retcode;
    fid_Error error;
    fid_Projectfile prjfile;
    fid_Suffixarray esa;
    fid_Suffixinterval_64 interval;
/*    MPI::Init( argc, argv );
    p = MPI::COMM_WORLD.Get_size();
    id = MPI::COMM_WORLD.Get_rank();
    if (id==0) {*/
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

/*        //fprintf(stdout,"depth=%ld,left=%ld,right=%ld\n", interval.depth, interval.left, interval.right);
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
 */
    return 0;
}
