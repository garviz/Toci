/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/30/2013 19:13:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  
 *
 * =====================================================================================
 */

#include <string>
#include <boost/dynamic_bitset.hpp>
#include <bitset>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <parallel/algorithm>
#include <cstring>
#include <fstream>
#include <ctype.h>
#include <omp.h>

#include "fasta.hpp"
#include <divsufsort64.h>

using namespace std;

int main(int argc, char *argv[]){
  vector<boost::dynamic_bitset<> > sequence;
  saidx64_t *SA;
  const boost::dynamic_bitset<> a(3, 1ul);
  const boost::dynamic_bitset<> c(3, 2ul);
  const boost::dynamic_bitset<> g(3, 3ul);
  const boost::dynamic_bitset<> t(3, 4ul);
  const boost::dynamic_bitset<> n(3, 5ul);
  string ref;
  vector<string> refdescr;
  vector<long long> startpos;
  string ref_fasta = argv[1];
  load_fasta(ref_fasta, ref, refdescr, startpos);
  SA = (saidx64_t *)malloc((size_t)ref.length() * sizeof(saidx64_t));
  if(SA == NULL) {
      fprintf(stderr, "%s: Cannot allocate memory.\n", argv[0]);
      exit(EXIT_FAILURE);
  }
  if(divsufsort64((const sauchar_t*) ref.c_str(), SA, (saidx64_t)ref.length()) != 0) {
      fprintf(stderr, "%s: Cannot allocate memory.\n", argv[0]);
      exit(EXIT_FAILURE);
  }
  free(SA);
  //vector<unsigned long long> SA(ref.size());
  //uint *radixSA = Radix((uchar*)ref.c_str(), ref.length()).build();
  //for (saidx64_t i = 0; i < ref.length(); ++i) {
  //    cout << ref.substr(SA[i]) << endl;
          //fprintf(stdout, "%d ", radixSA[i]);
  //}
  //delete[] radixSA;
  //for (std::size_t i = 0; i < ref.size(); ++i)
  //{
      //sequence.push_back(boost::dynamic_bitset<>(3,int(&myString[i])));
      //SA[i]=i;//myString.substr(i);
  //}
  //__gnu_parallel::sort(SA.begin(), SA.end(), compare);
  //sort(SA.begin(), SA.end(), compare);
  //for (int i=0; i<SA.size(); i++)
  //{
     // cout << ref.substr(SA[i]) << endl;
/*      if (sequence[i]==a) 
          cout << sequence[i] << "->" << "a" << endl;
      else if (sequence[i]==c)
          cout << sequence[i] << "->" << "c" << endl;
      else if (sequence[i]==g)
          cout << sequence[i] << "->" << "g" << endl;
      else if (sequence[i]==t)
          cout << sequence[i] << "->" << "t" << endl;
      else 
          cout << sequence[i] << "->" << "n" << endl;
*/
  //}
  
  return 0;
}
