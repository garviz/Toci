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

using namespace std;

int main () {
    string line;
    stringstream sequence;
    ifstream fasta;
    fasta.open ("afumig.fa");
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
    cout << "Length " << sequence.str().length() << endl;
    return 0;
}
