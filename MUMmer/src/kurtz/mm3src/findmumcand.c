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
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
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

static Uint mam(Suffixtree *st, int chunk, int chunks,  Uint minlen, Uchar *q, Uint qlen)
{
  Uchar *lptr, *left,
        *right = q + qlen - 1, 
        *querysuffix;
  Location loc;
  char file[15], buffer[64];
  snprintf(file,15,"/tmp/%d-%d.mum",chunk,chunks);
  Uint Size = 32768, N = 0;
  int fd = open(file, O_CREAT | O_WRONLY);
  if (fd < 0)
  {
      fprintf(stderr,"Can't create %s\n",file);
      return -1;
  }
  double start, start1, finish, finish1;
  int EventSet = PAPI_NULL;
  long long values[4];
  int Events[4] = { PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_L2_TCM, PAPI_L2_TCA }; 
  if (PAPI_create_eventset(&EventSet) != PAPI_OK) fprintf(stderr,"ERROR create EventSet\n");
  if (PAPI_add_events(EventSet, Events, 4) != PAPI_OK) fprintf(stderr,"ERROR add events\n");
  if (PAPI_start(EventSet) != PAPI_OK) fprintf (stderr,"ERROR PAPI_start\n");
  DEBUG1(2,"query of length %lu=",(Showuint) querylen);
  DEBUGCODE(2,(void) fwrite(query,sizeof(Uchar),(size_t) querylen,stdout));
  DEBUG0(2,"\n");
  start = omp_get_wtime();
  left = q + (Uint)(qlen/chunks*chunk);
  right = q + (Uint)(qlen/chunks*(chunk+1))-1;
  lptr = scanprefixfromnodestree (st, &loc, ROOT (st), q, right, 0);
  for (querysuffix = left; querysuffix<right && lptr != NULL; querysuffix++)
  {
      DEBUGCODE(2,showlocation(stdout,stree,&loc));
      if (loc.locstring.length >= minlen && loc.remain > 0 && loc.nextnode.toleaf)
      {
          if (querysuffix == q || loc.locstring.start == 0 || *(querysuffix - 1) != st->text[loc.locstring.start - 1])
          {
              snprintf(buffer,64,"%ld,%ld,%ld\n",loc.locstring.start,(Uint) (querysuffix-q),loc.locstring.length);
              write(fd,buffer,strlen(buffer));
          }
      }
      if (ROOTLOCATION (&loc))
          lptr = scanprefixfromnodestree (st, &loc, ROOT (st), lptr + 1, right, 0);
      else
      {
          linklocstree (st, &loc, &loc);
          lptr = scanprefixstree (st, &loc, &loc, lptr, right, 0);
      }
  }
  DEBUGCODE(2,showlocation(stdout,st,&loc));
  while (!ROOTLOCATION (&loc) && loc.locstring.length >= minlen)
  {
      if (loc.locstring.length >= minlen && loc.remain > 0 && loc.nextnode.toleaf)
      {
          if (querysuffix == q || loc.locstring.start == 0 || *(querysuffix - 1) != st->text[loc.locstring.start - 1])
          {
              snprintf(buffer,64,"%ld,%ld,%ld\n",loc.locstring.start,(Uint) (querysuffix-q),loc.locstring.length);
              write(fd,buffer,strlen(buffer));
          }
      }
      linklocstree (st, &loc, &loc);
      querysuffix++;
      DEBUGCODE(2,showlocation(stdout,stree,&loc));
  }
  finish = omp_get_wtime();
  if (PAPI_read(EventSet, values) != PAPI_OK) fprintf(stderr,"ERROR PAPI_Read\n");
  fprintf(stderr,"# CYC=%lld,INS=%lld,L2=%f,", values[1], values[0],(double) values[2]/(double)values[3]);
  if (PAPI_stop(EventSet, values) != PAPI_OK) fprintf(stderr,"ERROR PAPI-stop\n");
  fprintf(stderr,"Search_Matches=%f\n",(double) (finish-start));
  close(fd);
  return N;
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
  Uint N = 0, Size = 32768;
  int threads[4] = {1, 6, 12, 24};
  MUMcandidate *mumcand;
  mumcand = malloc(Size*sizeof *mumcand);
  double start, finish, start1, finish1;
  for (int ti=0; ti < 4; ti++)
  {
      N = 0;
      Size = 32768;
      omp_set_num_threads(threads[ti]);
#pragma omp parallel default(none) shared(stree, minmatchlength, query, querylen, seqnum, threads, ti, stderr) private(N)
      {
#pragma omp for schedule(static,1) nowait
          for (int i=0; i < threads[ti]; ++i)
          {
              N = mam(stree, i,threads[ti], minmatchlength, query, querylen);
          }
      }
  fprintf(stderr,"# Threads=%d,",threads[ti]); 
  Uint currentright, dbright = 0;
  BOOL ignorecurrent, ignoreprevious = False;
  FILE *fp;
  char *line = NULL, *pch;
  size_t len = 0;
  ssize_t read;

  start1 = omp_get_wtime();
  char file[15], buffer[64];
  for (int i=0; i < threads[ti]; ++i)
  {
      snprintf(file,15,"/tmp/%d-%d.mum",i,threads[ti]);
      fp = fopen(file, "r");
      if (fp == NULL)
      {
          fprintf(stderr,"Can't open merge file %s\n",file);
          return -1;
      }
      while ((read = getline(&line, &len, fp)) != -1) {
          if (N >= Size -1)
          {
              Size *= 2;
              mumcand = realloc (mumcand, Size * sizeof *mumcand);
              if (mumcand == NULL)
                  fprintf(stderr,"Can not realloc\n");
          }
          N++;
          mumcand[N].dbstart = strsep(&line, ",");
          mumcand[N].querystart = strsep(&line, ",");
          mumcand[N].mumlength = strsep(&line, ",");
      }
      fclose(fp);
  }
  int matches = 0;
  qsort(mumcand, (size_t) N, sizeof mumcand[0], compareMUMcandidates);
  for (int i=0; i< N; i++)
  {
      ignorecurrent = False;
      currentright = mumcand[i].dbstart + mumcand[i].mumlength - 1;
      if(dbright > currentright)
        ignorecurrent = True;
      else
      {
        if(dbright == currentright)
        {
          ignorecurrent = True;
          if(!ignoreprevious && mumcand[i-1].dbstart == mumcand[i].dbstart)
            ignoreprevious = True;
        } else
          dbright = currentright;
      }
      if(i > 0 && !ignoreprevious)
          matches++;
      ignoreprevious = ignorecurrent;
    }
    if(!ignoreprevious)
          matches++;
  finish1 = omp_get_wtime();
  fprintf(stderr,"Matches=%lu,",matches);
  fprintf(stderr,"Unique_Matches=%f\n",(double) (finish1-start1));
  } 
  return 0;
}
