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
    Uint currentright, dbright = 0;
    bool ignorecurrent, ignoreprevious = false;

    for(int i=1; i<N; i++)
    {
      currentright = A[i].Q + A[i].Len - 1;
      if(dbright > currentright)
      {
        A[i].Good = true;
      } else
      {
        if(dbright == currentright)
        {
          if(A[i].Q >= A[i-1].Q)
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
static void  Process_Matches (Match_t * A, int N) //  Process matches  A [1 .. N].
  {
   int  i;

   if  (N <= 0)
       return;
   UniqueMumR(A, N);
   UniqueMumQ(A, N);

   //Filter_Matches (A, N);

   for  (i = 0;  i <= N;  i ++)
   { 
       if  (A[i].Good && A[i].Len > 0)
           printf ("%8ld  %8ld  %8ld\n", A[i].R, A[i].Q, A[i].Len);
   }
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

Sint findmumcandidates(Suffixtree *stree, Table &table, Uint minmatchlength, Uint chunks, Uint prefix, Processmatchfunction processmumcandidate, void *processinfo, Uchar *query, Uint querylen, Uint seqnum)
{
  Uchar *lptr, *left, *right = query + querylen - 1, *querysuffix;
  Location loc;
  int i, nthreads, *chunk_schedule;
  omp_sched_t *schedule;
  Uint N, Size, N2, Size2;
  Match_t  *A = NULL;
  Match_t *MUMs = NULL;
  unsigned long int tid;
  double start, end;
  double start1, end1;

  chunk_schedule = (int *) malloc(sizeof(int));
  schedule = (omp_sched_t *) malloc(sizeof(omp_sched_t));
  Suffixes subset;
  for (Table::iterator it= table.begin(); it!=table.end(); ++it)
  {
      if ((Uint) it->first >= minmatchlength) 
      {
          subset = table[it->first];
          pair<Suffixes::iterator, Suffixes::iterator> ret;
          for (querysuffix = left; querysuffix<query+querylen; querysuffix++)
          {
              int enc = encoding(querysuffix,prefix);
          }
          ret = subset.equal_range(1019);
          cout << it->first << " ===> 1019 ===>" ;
          for (Suffixes::iterator mit=ret.first; mit != ret.second; ++mit)
              cout << ' ' << mit->first;
          cout << endl;
      }
  }
  
  start = omp_get_wtime();
#pragma omp parallel default (none) private(i,left,right,lptr,querysuffix,loc,A,N,Size) shared(stdout,stderr,chunks,query,querylen,stree,table,minmatchlength,seqnum,nthreads,chunk_schedule,schedule,MUMs,N2,Size2)
  { 
  N = 0;
  N2 = 0;
  Size = 32768;
  Size2 = 32768;
  A = (Match_t *) Safe_malloc (Size * sizeof (Match_t));
  MUMs = (Match_t *) Safe_malloc (Size * sizeof (Match_t));
  #pragma omp for schedule(runtime) nowait
  for (i=0; i<chunks; i++)
  {
      omp_get_schedule(schedule,chunk_schedule);
      nthreads = omp_get_num_threads();
      left = query + (Uint)(querylen/chunks*i);
      right = query + (Uint)(querylen/chunks*(i+1))-1;
      lptr = scanprefixfromnodestree (stree, &loc, ROOT (stree), left, right, 0);
      for (querysuffix = left; /*querysuffix < right &&*/ lptr != NULL;  querysuffix++)
      {
          if (loc.locstring.length >= minmatchlength && loc.remain > 0 && loc.nextnode.toleaf)
          {
               if (querysuffix == left || loc.locstring.start == 0 || *(querysuffix - 1) != stree->text[loc.locstring.start - 1])
                {
                        if (N > Size)
                        {
                            Size *= 2;
                            A = (Match_t *) Safe_realloc (A, Size * sizeof (Match_t));
                        }
                        A[N].R = loc.locstring.start+1;
                        A[N].Q = (Uint) (querysuffix-query)+1;
                        A[N].Len = loc.locstring.length;
                        N++;
               }
          }
          if (ROOTLOCATION (&loc))
          {
              lptr = scanprefixfromnodestree (stree, &loc, ROOT (stree), lptr + 1, right, 0);
          }
          else
          {
              linklocstree (stree, &loc, &loc);
              lptr = scanprefixstree (stree, &loc, &loc, lptr, right, 0);
          } 
      }
      while (!ROOTLOCATION (&loc) && loc.locstring.length >= minmatchlength)
      { 
          if (loc.locstring.length >= minmatchlength && loc.remain > 0 && loc.nextnode.toleaf)
           {
               if (querysuffix == left || loc.locstring.start == 0 || *(querysuffix - 1) != stree->text[loc.locstring.start - 1])
                {
                        if (N >= Size -1)
                         {
                            Size *= 2;
                            A = (Match_t *) Safe_realloc (A, Size * sizeof (Match_t));
                        }
                        A[N].R = loc.locstring.start+1;
                        A[N].Q = (Uint) (querysuffix-query)+1;
                        A[N].Len = loc.locstring.length;
                        N++;
               }
          }
          linklocstree (stree, &loc, &loc);
          querysuffix++;
      }
/*#pragma omp critical
      {*/
          for(int i=0;i<N;i++)
          {
              //fprintf(stderr,"%8lu  %8lu  %8lu\n",A[i].R,A[i].Q,A[i].Len);
                  if (N2 > Size2)
                  {
                      Size2 *= 2;
                      MUMs = (Match_t *) Safe_realloc (A, Size2 * sizeof (Match_t));
                  }
                  MUMs[N2].R = A[i].R;
                  MUMs[N2].Q = A[i].Q;
                  MUMs[N2].Len = A[i].Len;
                  MUMs[N2].Good = true;
                  N2++;
          }
      //}
  }   
  }
  end = omp_get_wtime(); 
  start1 = omp_get_wtime();
  Process_Matches(MUMs,N2);
  end1 = omp_get_wtime();
  fprintf(stderr,"# Threads=%d,Chunks=%d,Chunk_Size=%lu,OMP_time=%f,RealMUM=%f,Schedule=%d,Chunk_Schd=%d,MUM=%d,MUMs=%d,Size=%d,",nthreads,chunks,querylen/chunks,(double) (end-start),(double) (end1-start1),*schedule,*chunk_schedule,minmatchlength,N2,Size2);
  return 0;
}
