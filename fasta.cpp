/*
 * =====================================================================================
 *
 *       Filename:  fasta.cpp
 *
 *    Description:  Testing prefixes
 *
 *        Version:  1.0
 *        Created:  20/10/11 14:19:52
 *       Revision:  none
 *       Compiler:  mpic++
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <vector>
#include <google/dense_hash_map>
#include <mpi.h>

using namespace std;
using google::dense_hash_map;
using tr1::hash;

struct eqstr {
    bool operator()(string s1, string s2) const {
        return (s1 == s2) || (s1.compare(s2) == 0);
    }
};

int main (int argc, char *argv[]) {
    string line;
    stringstream sequence;
    ifstream fasta;
    int id, coverage = 0;
    double start, finish;
    unordered_map<string, vector<int> > prefixes;
    MPI::Init ( argc, argv );
    id = MPI::COMM_WORLD.Get_rank ();
    if (id==0) {
        start = MPI_Wtime();
        fasta.open (argv[1]);
    //dense_hash_map<string, vector<int>, hash<string>, eqstr> prefix;
        if (fasta.is_open()) {
            while (getline(fasta,line)) {
                if (line.find(">")!=string::npos)
                {
                //cout << "Header" << endl;
                }
                else 
                {
                    sequence << line ;
                }
            }     
        }
        fasta.close();

        for (int i=0; i < sequence.str().length(); i++) {
            prefixes[sequence.str().substr( i, atoi (argv[2]) )].push_back(i);
        }
        for (unordered_map<string, vector<int> >::const_iterator it = prefixes.begin(); it != prefixes.end(); ++it) {
            vector<int> positions = (*it).second;
            cout << (*it).first << "|" << positions.size() << "|" ;
            for (unsigned i = 0; i < positions.size(); i++) {
                cout << "," << positions[i];
            }
            cout << endl;
            coverage += positions.size();
        }
        cout << "Length " << sequence.str().length() << endl;
        cout << "Prefixes: " << prefixes.size() << endl;
        cout << "Coverage: " << coverage << endl; 

        finish = MPI_Wtime();

        cout << "Time: " << finish-start << endl;
    }

    MPI::Finalize();

    return 0;
}
