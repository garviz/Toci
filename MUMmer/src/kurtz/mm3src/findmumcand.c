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
#include <papi.h>
#include <omp.h>
#include <stdlib.h>
#include "streedef.h"
#include "debugdef.h"
#include "spacedef.h"
#include "maxmatdef.h"
#include "protodef.h"

//}

/*EE
  This module contains functions to compute MUM-candidates using
  a linear time suffix tree traversal. 
*/

/*
  The following function compares two MUM-candidates. The MUM-candidate
  with smaller \texttt{dbstart}-value comes first.
  If both MUMs have the same \texttt{dbstart}-value, then the MUM-candidate
  with the larger length comes first.
*/

static Sint compareMUMcandidates(const void *p,const void *q)
{
    MUMcandidate *P = (MUMcandidate *)p;
    MUMcandidate *Q = (MUMcandidate *)q;
    if(P->dbstart == Q->dbstart)
        return (P->mumlength < Q->mumlength) ? 1 : -1;
    return (P->dbstart > Q->dbstart) ? 1 : -1;
}

/*
  The following function checks if a location \texttt{loc} (of length 
  larger than \texttt{minmatchlength}) in the suffix tree represents 
  a MUM-candiate. The parameters are as follows:

  \begin{enumerate}
  \item
  \texttt{subjectseq} points to the subject sequence
  \item
  \texttt{querysuffix} points to the current suffix of the query sequence
  \item
  \texttt{query} points to the query sequence
  \item
  \texttt{seqnum} is the number of the query sequence currently considered
  \item
  \texttt{processmumcandidate} is the function to further process a 
  MUM-candidate.
  \item
  \texttt{processinfo} points to some values additionally required by
  the function \texttt{processmumcandidate}.
  \end{enumerate}
  By construction, the location in the suffix tree represents a 
  substring of the subject sequence which maximaly matches a prefix of
  \texttt{querysuffix}. Thus it is only necessary to verify that,
  the substring of the subject sequence is long enough, that it
  is unique in the subject sequence and that the match
  is also left maximal. This is done as follows:
  
  \begin{enumerate}
  \item
  does \texttt{loc} represent a substring of length at least 
  \texttt{minmatchlength}?
  \item
  does \texttt{loc} correspond to a leaf edge? Then then the string 
  represented by the location is unique in the subject sequence.
  \item
  is the substring left maximal? This is true if one of the following
  conditions hold:
  \begin{itemize}
  \item
  the suffix of the query currently considered is the first suffix, or 
  \item
  the string represented by \texttt{loc} is a prefix of the subject string,
  or
  \item
  the characters immediately to the left of the matching strings
  in the subject sequence and the query sequence are different
  \end{itemize}
  \end{enumerate}
  If all conditions 1-3 are true, then a function 
  \texttt{processmumcandidate} is called. It takes the necessary 
  information about the MUM-candidate as its arguments.
  In case an error occurs, a negative number is returned. Otherwise,
  0 is returned.
*/

static Sint checkiflocationisMUMcand (Location *loc,
                                      Uchar *subjectseq,
				      Uchar *querysuffix,
				      Uchar *query,
                                      Uint seqnum,
                                      Processmatchfunction 
                                        processmumcandidate,
                                      void *processinfo)
{
  if (loc->remain > 0 
      && loc->nextnode.toleaf
      && (querysuffix == query || loc->locstring.start == 0
			       || *(querysuffix - 1) != 
                                  subjectseq[loc->locstring.start - 1]))
  {
    if(processmumcandidate(processinfo,
                           loc->locstring.length,   // matchlength
	                   loc->locstring.start,    // subject start
                           seqnum,                  // queryseq
                           (Uint) (querysuffix -   
                                   query)) != 0)    // querystart
    {
      return -1;
    }
  }
  return 0;
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

Sint findmumcandidates(Suffixtree *stree,
                       Uint minmatchlength,
                       Processmatchfunction processmumcandidate,
                       void *processinfo,
                       Uchar *query,
                       Uint querylen,
                       Uint seqnum)
{ 
  Uchar *lptr, *left,
        *right = query + querylen - 1, 
        *querysuffix;
  Location loc;
  int threads[4] = {1, 6, 12, 24};
  Uint N, Size, M = 0, Size1 = 32768;
  MUMcandidate *mumcandptr, *mumcand;
  double start, start1, finish, finish1;
  long_long values[4];
  int Events[4] = { PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_L2_TCM, PAPI_L2_TCA }, EventSet = PAPI_NULL;

  if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) fprintf(stderr,"PAPI library init error!\n");
  if (PAPI_thread_init((unsigned long (*)(void))(omp_get_thread_num())) != PAPI_OK) fprintf(stderr,"Doesn't work!\n");;
  if (PAPI_create_eventset(&EventSet) != PAPI_OK) fprintf(stderr,"ERROR create EventSet\n");
  if (PAPI_add_events(EventSet, Events, 4) != PAPI_OK) fprintf(stderr,"ERROR add events\n");

  DEBUG1(2,"query of length %lu=",(Showuint) querylen);
  DEBUGCODE(2,(void) fwrite(query,sizeof(Uchar),(size_t) querylen,stdout));
  DEBUG0(2,"\n");

  mumcand = malloc(Size1*sizeof *mumcandptr);

  for (int ti=0; ti < 4; ti++)
  {
      M = 0;
      omp_set_num_threads(threads[ti]);
      if (PAPI_start(EventSet) != PAPI_OK) fprintf (stderr,"ERROR PAPI_start\n");
      start = omp_get_wtime();
      #pragma omp parallel default(none) private (left, right, lptr, querysuffix, loc, ti) shared(stderr, threads, query, querylen, stree, minmatchlength, seqnum, M, Size1, mumcandptr, N, Size)
      {
#pragma omp for schedule(static,1) nowait
          for (int i=0; i < threads[ti]; i++)
          {
              N = 0;
              Size = 32768;
              left = query + (Uint)(querylen/threads[ti]*i);
              right = query + (Uint)(querylen/threads[ti]*(i+1))-1;
              mumcandptr = malloc(Size*sizeof *mumcandptr);
              lptr = scanprefixfromnodestree (stree, &loc, ROOT (stree), query, right, 0);
              for (querysuffix = left; querysuffix<right && lptr != NULL; querysuffix++)
               {
                  DEBUGCODE(2,showlocation(stdout,stree,&loc));
                  if (loc.locstring.length >= minmatchlength && loc.remain > 0 && loc.nextnode.toleaf)
                  {
                      if (querysuffix == query || loc.locstring.start == 0 || *(querysuffix - 1) != stree->text[loc.locstring.start - 1])
                      {
#pragma omp critical
                          {
                              if (N >= Size -1)
                              {
                                  Size *= 2;
                                  mumcandptr = realloc (mumcandptr, Size * sizeof *mumcandptr);
                                  if (mumcandptr == NULL)
                                      fprintf(stderr,"Can not realloc\n");
                              }
                              N++;
                              mumcandptr[N].mumlength = loc.locstring.length;
                              mumcandptr[N].dbstart = loc.locstring.start;
                              mumcandptr[N].queryseq = seqnum;
                              mumcandptr[N].querystart = (Uint) (querysuffix - query);
                          }
                      }
                  }
                  if (ROOTLOCATION (&loc))
                      lptr = scanprefixfromnodestree (stree, &loc, ROOT (stree), lptr + 1, right, 0);
                  else
                  {
                      linklocstree (stree, &loc, &loc);
                      lptr = scanprefixstree (stree, &loc, &loc, lptr, right, 0);
                  }
              }
              DEBUGCODE(2,showlocation(stdout,stree,&loc));
              while (!ROOTLOCATION (&loc) && loc.locstring.length >= minmatchlength)
              {
                  if (loc.locstring.length >= minmatchlength && loc.remain > 0 && loc.nextnode.toleaf)
                  {
                      if (querysuffix == query || loc.locstring.start == 0 || *(querysuffix - 1) != stree->text[loc.locstring.start - 1])
                      {
#pragma omp critical
                          {
                              if (N >= Size -1)
                              {
                                  Size *= 2;
                                  mumcandptr = realloc (mumcandptr, Size * sizeof *mumcandptr);
                                  if (mumcandptr == NULL)
                                      fprintf(stderr,"Can not realloc\n");
                              } 
                              N++;
                              mumcandptr[N].mumlength = loc.locstring.length;
                              mumcandptr[N].dbstart = loc.locstring.start;
                              mumcandptr[N].queryseq = seqnum;
                              mumcandptr[N].querystart = (Uint) (querysuffix - query);
                          }
                       }
                   }
                  linklocstree (stree, &loc, &loc);
                  querysuffix++;
                  DEBUGCODE(2,showlocation(stdout,stree,&loc));
               }
          }
      }
  finish = omp_get_wtime();
  Uint currentright, dbright = 0;
  BOOL ignorecurrent, ignoreprevious = False;

  start1 = omp_get_wtime();
  qsort(mumcandptr, (size_t) N, sizeof mumcandptr[0], compareMUMcandidates);
  for (int i=0; i< N; i++)
  {
      ignorecurrent = False;
      currentright = mumcandptr[i].dbstart + mumcandptr[i].mumlength - 1;
      if(dbright > currentright)
      {
        ignorecurrent = True;
      } else
      {
        if(dbright == currentright)
        {
          ignorecurrent = True;
          if(!ignoreprevious && mumcandptr[i-1].dbstart == mumcandptr[i].dbstart)
          {
            ignoreprevious = True;
          }
        } else
        {
          dbright = currentright;
        }
      }
      if(i > 0 && !ignoreprevious)
          M++;
      ignoreprevious = ignorecurrent;
    }
    if(!ignoreprevious)
          M++;
  finish1 = omp_get_wtime();
  fprintf(stderr,"# Threads=%d,",threads[ti]); 
  fprintf(stderr,"Matches=%lu,",M);
  fprintf(stderr,"Search_Matches=%f,",(double) (finish-start));
  fprintf(stderr,"Unique_Matches=%f,",(double) (finish1-start1));
  if (PAPI_read(EventSet, values) != PAPI_OK) fprintf(stderr,"ERROR PAPI_Read\n");
      fprintf(stderr,"CYC=%lld,INS=%lld,L2=%f\n", values[1], values[0],(double) values[2]/(double)values[3]);
  if (PAPI_stop(EventSet, values) != PAPI_OK) fprintf(stderr,"ERROR PAPI_stop\n");
  }
  return 0;
}
