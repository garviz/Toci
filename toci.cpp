/*
 * =====================================================================================
 *
 *       Filename:  toci.cpp
 *
 *    Description:  Main program to search for MUMs.
 *
 *        Version:  1.0
 *        Created:  26/02/13 13:52:28
 *       Revision:  none
 *       Compiler:  gcc
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
#include <cstring>
#include <divsufsort64.h>
#include <divsufsort.h>

using namespace std;

static long lcp(sauchar_t *start1,sauchar_t *start2,sauchar_t *end)
{ 
  register sauchar_t *ptr1 = start1, *ptr2 = start2;
  while(ptr1 <= end && ptr2 <= end && *ptr1 == *ptr2)
  {
    ptr1++;
    ptr2++;
  }
  return (long) (ptr1-start1);
}

int main(int argc, char *argv[])
{
    string line;
    stringstream ss;
    saint_t ret;

    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << "Reference_genome Query_genome length" << std::endl;
        return 1;
    }
    ifstream ref(argv[1]);
    ifstream qry(argv[2]);
    long L = atoi(argv[3]);

    while (getline(ref,line))
    {
        if (line.find(">",0,1)!=std::string::npos)
            continue;
        ss << line;
    }
    ref.close();
    ss << '#';
    while (getline(qry,line))
    {
        if (line.find(">",0,1)!=std::string::npos)
            continue;
        ss << line;
    }
    qry.close();
    const std::string& tmp = ss.str();
    sauchar_t *T = (sauchar_t*) tmp.c_str();
    sauchar_t *bwttab;
    saidx_t *suftab;
    saidx_t *lcptab;
    saidx_t *idx;
    saidx_t size = ss.str().length();
    suftab = (saidx_t *)malloc(size * sizeof(saidx_t));
    lcptab = (saidx_t *)malloc(size * sizeof(saidx_t));
    bwttab = (sauchar_t *)malloc(size * sizeof(sauchar_t));
    idx = (saidx_t *)malloc(sizeof(saidx_t));
    if (suftab == NULL || bwttab == NULL || T == NULL) 
    {
        fprintf(stderr, "%s: Cannot allocate memory.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    ret = divsufsort(T, suftab, size);
    if (ret != 0)
    {
        fprintf(stderr,"%s: Failed to create Suffix Array\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    ret =bw_transform(T, bwttab, NULL, size, idx);
    if (ret != 0)
    {
        fprintf(stderr,"%s: Failed to create BWT\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    lcptab[0] = 0;
    for (long i=1; i<size; i++)
        lcptab[i]=lcp(&T[suftab[i-1]], &T[suftab[i]],&T[size-1]);
    for (long i=0, j=i+1; i<size-1; i++)
    {
        if (lcptab[i] < L && lcptab[j+1] < L && lcptab[i] >= L)
            cout << lcptab[i] << endl;
    }

    exit(EXIT_SUCCESS);
}
