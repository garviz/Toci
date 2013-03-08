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
    long i, j, k, p;

    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << "Reference_genome Query_genome length" << std::endl;
        return 1;
    }
    ifstream ref(argv[1]);
    ifstream qry(argv[2]);
    long L = atoi(argv[3]);
    long l = 0;

    while (getline(ref,line))
    {
        if (line.find(">",0,1)!=std::string::npos)
            continue;
        ss << line;
    }
    ref.close();
    ss << '#';
    p = ss.str().length()-1;
    while (getline(qry,line))
    {
        if (line.find(">",0,1)!=std::string::npos)
            continue;
        ss << line;
     }
    qry.close();
    //ss << '$';
    const std::string& tmp = ss.str();
    sauchar_t *T = (sauchar_t*) tmp.c_str();
    sauchar_t *bwttab;
    saidx_t *suftab, *lcptab;
    saidx_t size = ss.str().length();
    suftab = (saidx_t *)malloc(size * sizeof(saidx_t));
    lcptab = (saidx_t *)malloc(size * sizeof(saidx_t));
    bwttab = (sauchar_t *)malloc(size * sizeof(sauchar_t));
    if (suftab == NULL || bwttab == NULL || T == NULL || lcptab == NULL) 
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
    ret =divbwt(T, bwttab, NULL, size);
    if (ret < 0)
    {
        fprintf(stderr,"%s: Failed to create BWT %d\n", argv[0], ret);
        exit(EXIT_FAILURE);
    }
    lcptab[0] = 0;
    for (i=1; i<size; i++)
        lcptab[i]=lcp(&T[suftab[i-1]], &T[suftab[i]],&T[size-1]);
    for (i=1; i<size; i++)
    {
        //fprintf(stdout,"%d,%d,%d,%c,%s\n",i,suftab[i],lcptab[i],bwttab[i],&T[suftab[i]]);
        k = i+1;
        l = lcptab[k];
        //if (l>=L)
        //{
            while (lcptab[k] == l)
                j=k++;
            fprintf(stdout,"%ld-[%ld..%ld]: ",l,i,j);
            for (int m=i;m<=j;++m)
                fprintf(stdout,"%d-%d,%c ",lcptab[m],suftab[m],bwttab[m]);
            cout << endl;
            if (i+1==j && bwttab[i]!=bwttab[j] && suftab[i] < p && p < suftab[j])
            {
                cout << suftab[i] << ' ' << lcptab[i] << ':';

                    cout << ss.str().substr(suftab[i], suftab[i]+lcptab[i]-1) << endl;
            }
            i++;
        //}
    }

    exit(EXIT_SUCCESS);
}
