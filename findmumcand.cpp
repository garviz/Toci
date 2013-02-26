/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

//\IgnoreLatex{

#include <stdio.h>
#include <ctype.h>
#include <omp.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <papi.h>
#include <assert.h>
#include "streedef.h"
#include "spacedef.h"
#include "maxmatdef.h"
#include "distribute.h"

//}

/*EE
  This module contains functions to compute MUM-candidates using
  a linear time suffix tree traversal. 
*/

static Uint lcp(Uchar *start1,Uchar *end1,Uchar *start2,Uchar *end2)
{
  register Uchar *ptr1 = start1, 
                  *ptr2 = start2;
  while(ptr1 <= end1 && 
        ptr2 <= end2 &&
        *ptr1 == *ptr2)
  {
    ptr1++;
    ptr2++;
  }
  return (Uint) (ptr1-start1);
}

static void  Filter_Matches (Match_t * A, int & N)

//  Remove from  A [0 .. (N - 1)]  any matches that are internal to a repeat,
//  e.g., if seq1 has 27 As and seq2 has 20 then the first and
//  last matches will be kept, but the 6 matches in the middle will
//  be eliminated.  Also combine overlapping matches on the same
//  diagonal.  Pack all remaining matches into the front of  A  and
//  reduce the value of  N  if any matches are removed.
//  Matches in  A  *MUST* be sorted by  Start2  value.

  {
   int  i, j;

   for  (i = 0;  i < N - 1;  i ++)
     {
      int  i_diag, i_end;

      if  (A [i] . Good)
          continue;

      i_diag = A [i] . Q - A [i] . R;
      i_end = A [i] . Q + A [i] . Len;

      for  (j = i + 1;  j < N && A [j].Q <= i_end;  j ++)
        {
         int  olap;
         int  j_diag;

         if (A[i].Q <= A[j].Q)
         assert (A[i].Q <= A[j].Q);

         if  (! A[j].Good)
             continue;

         j_diag = A[j].Q - A[j].R;
         if  (i_diag == j_diag)
             {
              int  j_extent;

              j_extent = A [j] . Len + A[j].Q - A[i].Q;
              if  (j_extent > A[i].Len)
                  {
                   A[i].Len = j_extent;
                   i_end = A[i].Q + j_extent;
                  }
              A[j].Good = false;
             }
         else if  (A[i].R == A[j].R)
             {
              olap = A[i].Q + A[i].Len - A[j].Q;
              if  (A[i].Len < A[j].Len)
                  {
                   if  (olap >=  A[i].Len/2)
                       {
                        A[i].Good = false;
                        break;
                       }
                  }
              else if  (A[j].Len < A[i].Len)
                  {
                   if  (olap >= A[j].Len/2)
                       {
                        A[j].Good = false;
                       }
                  }
                else
                  {
                   if  (olap >= A[i].Len/2)
                       {
                        A[j].Tentative = true;
                        if  (A[i].Tentative)
                            {
                             A[i].Good = false;
                             break;
                            }
                       }
                  }
             }
         else if  (A[i].Q == A[j].Q)
             {
              olap = A[i].R + A[i].Len - A[j].R;
              if  (A[i].Len < A[j].Len)
                  {
                   if  (olap >=  A[i].Len/2)
                       {
                        A[i].Good = false;
                        break;
                       }
                  }
              else if  (A[j].Len < A[i].Len)
                  {
                   if  (olap >= A[j].Len/2)
                       {
                        A[j].Good = false;
                       }
                  }
                else
                  {
                   if  (olap >= A[i].Len/2)
                       {
                        A[j].Tentative = true;
                        if  (A[i].Tentative)
                            {
                             A[i].Good = false;
                             break;
                            }
                       }
                  }
             }
        }
     }

   for  (i = j = 0;  i < N;  i ++)
     if  (A[i].Good)
         {
          if  (i != j)
              A[j] = A[i];
          j ++;
         }
   N = j;

   return;
  }

static int  By_Q (const void * A, const void * B)

//  Return how  A  and  B  compare if converted to  Match_t
//  based on Query pos.  If  Query pos  values are equal use
//  Reference pos  values for comparison.

  {
   Match_t  * x, * y;

   x = (Match_t *) A;
   y = (Match_t *) B;

   return (x->Q < y->Q) ? -1 : 1;
   return (x->R < y->R) ? -1 : 1;
   return  0;
  }

static int  By_R (const void * A, const void * B)

//  Return how  A  and  B  compare if converted to  Match_t
//  based on Reference pos.  If  Reference pos  values are equal use
//  length  values for comparison.

  {
   Match_t  * x, * y;

   x = (Match_t *) A;
   y = (Match_t *) B;

  if(x->R == y->R)
  {
    return (x->Len < y->Len) ? 1 : -1;
  }
  return (x->R > y->R) ? 1 : -1;
  }

static void UniqueMumR (Match_t * A, int N)
{
  if(N > 0)
  {
    qsort (A, N, sizeof (Match_t), By_R);
    Uint currentright, dbright = 0;
    bool ignorecurrent, ignoreprevious = false;

    for(int i=1; i<N; i++)
    {
      currentright = A[i].R + A[i].Len - 1;
      if(dbright > currentright)
      {
        A[i].Good = true;
      } else
      {
        if(dbright == currentright)
        {
          if(A[i].R >= A[i-1].R)
          {
            A[i].Good = false;
          }
        } else
        {
          dbright = currentright;
        }
      }
    }
  }
  return;
}
static void UniqueMumQ (Match_t * A, int N)
{
  if(N > 0)
  {
    qsort (A, N, sizeof (Match_t), By_Q);
    Uint currentright, dbright = A[0].Q + A[0].Len -1;

    for(int i=1; i<N; i++)
    {
      A[i].Good = true;
      currentright = A[i].Q + A[i].Len - 1;
      if(dbright > currentright)
      {
        A[i].Good = false;
      } else
      {
        if(dbright == currentright)
        {
            A[i].Good = false;
            if (!A[i-1].Good && A[i-1].Q == A[i].Q)
            {
                A[i-1].Good = false;
            } 
        }
        else
        {
            dbright = currentright;
        }
      }
      A[i-1].Good = A[i].Good;
    }
  }
  return;
}
static void  Process_Matches (Match_t * A, int N) //  Process matches  A [1 .. N].
  {
   int  i;

   if  (N <= 0)
       return;
   UniqueMumQ(A, N);
   UniqueMumR(A, N);

   //Filter_Matches (A, N);

   /*for  (i = 0;  i <= N;  i ++)
   { 
       if  (A[i].Good && A[i].Len > 0)
           printf ("%8ld  %8ld  %8ld\n", A[i].R, A[i].Q, A[i].Len);
   }*/
   return;
  }

/*EE
  The following function traverses the suffix tree guided by
  some query string. The parameters are as follows:
  \begin{enumerate}
  \item
  \texttt{stree} is the suffix tree constructed from the subject
  sequence.
  \item
  \texttt{minmatchlength} is the minimal length of the MUMs as specified
  \item
  \texttt{processmumcandidate} is the function to further process a 
  MUM-candidate.
  \item
  \texttt{processinfo} points to some values additionally required by
  the function \texttt{processmumcandidate}.
  \item
  \texttt{query} points to the query which is of length 
  \texttt{querylen}
  \item
  \texttt{seqnum} is the number of the current query sequence
  in the \texttt{Multiseq}-record.
  \end{enumerate}
  For each suffix, say \(s\), of the query sequence
  the following function computes the location of the longest prefix of s 
  that is a substring of the subject sequence. This is done
  by iteratively calling the function \texttt{scanprefixfromnodestree}.
  In each step, the scan starts at a location which represents a prefix
  of the maximaly matching substring. The locations are computed
  using the function \texttt{linklocstree}.
  In case an error occurs, a negative number is returned. Otherwise,
  0 is returned.
*/

Sint findmumcandidates(Uchar *reference, Uint referencelen, Table &table, Uint minmatchlength, Uint chunks, Uint prefix, Processmatchfunction processmumcandidate, void *processinfo, Uchar *query, Uint querylen, Uint seqnum)
{
  Uchar *leftq, *rightq = query + querylen - 1, *querysuffix, *leftr, *rightr = reference + referencelen - 1;
  double start, end;
  Uint enc=0, N = 0, Size=32768;
  Match_t  *A = NULL;
  /*for (Table::iterator i=table.begin(); i!=table.end(); ++i)
  {
      cout << (*i).first << ":";
      for (Suffixes::iterator j=(*i).second.begin(); j!=(*i).second.end(); ++j)
          cout << ' ' << (*j).position;
      cout << endl;
  }*/
  A = (Match_t *) Safe_malloc (Size * sizeof (Match_t));
  /*vector<Uint> v, tmp;
  map<Uint,vector<Uint>> check;*/
  start = omp_get_wtime();
  for (leftq = query; leftq<rightq-prefix; leftq++) //Iterate query sequence
  {
      enc = encoding(leftq,prefix);
      /*v.push_back((Uint) (leftq-query));
      if (check.count(enc))
          check[enc].insert(check[enc].end(),v.begin(),v.end());
      else
          check[enc]=v;*/
      if (table.count(enc))
      {
          for (Suffixes::iterator i=table[enc].begin(); i!=table[enc].end(); ++i) //Iterate over the suffixes in reference
          {
              leftr = reference+(*i).position;
              if ((leftq == query || leftr == reference || *(leftq-1) != *(leftr-1)) && *(leftq+(*i).depth) == *(leftr+(*i).depth)) //Check left and right maximal
              {
          //cout << (Uint) (leftq-query)+1 << '*' << (*i).position+1 << endl;
              //Uint length = lcp(leftq+(*i).depth+1,rightq,leftr+(*i).depth+1,rightr)+(*i).depth+1;
                  Uint length = lcp(leftq+prefix,rightq,leftr+prefix,rightr)+prefix;
                  if (length >= minmatchlength)
                  {
                      if (N > Size)
                      {
                          Size *= 2;
                          A = (Match_t *) Safe_realloc (A, Size * sizeof (Match_t));
                      }  
                      A[N].R = (*i).position+1;
                      A[N].Q = (Uint) (leftq-query)+1;
                      A[N].Len = length;
                      A[N].Good = true;
                      N++;
                  }
              }
          }
      }
  }
  /*Uint comp=0;
  for (map<Uint,vector<Uint>>::iterator i=check.begin();i!=check.end();++i)
  {
      comp+=(table[(*i).first].size()*(*i).second.size());
      for (Suffixes::iterator j=table[(*i).first].begin(); j!=table[(*i).first].end(); ++j)
      {
          for (vector<Uint>::iterator k=(*i).second.begin(); k!=(*i).second.end(); ++k)
              cout << (*j).position << ',' << *k << endl;
      }
  }*/
  end = omp_get_wtime(); 
  Process_Matches(A,N);
  fprintf(stderr,"# Time=%f,",(double) (end-start));
  return 0;
}
