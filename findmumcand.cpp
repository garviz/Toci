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
#include <google/sparsetable>
#include <omp.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include "streedef.h"
#include "spacedef.h"
#include "maxmatdef.h"
#include "distribute.h"

using google::sparsetable;
//}

/*EE
  This module contains functions to compute MUM-candidates using
  a linear time suffix tree traversal. 
*/

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

static Sint checkiflocationisMUMcand (Location *loc, Uchar *subjectseq, Uchar *querysuffix, Uchar *query, Uint seqnum, Match_t *A, Uint N, Uint Size)
{
  if (loc->remain > 0 && loc->nextnode.toleaf)
  {
      /*if (*(querysuffix - 1) != subjectseq[loc->locstring.start - 1])
          cerr << loc->locstring.start << "," << (Uint) (querysuffix-query) << "," << loc->locstring.length << endl;*/
      if (querysuffix == query || loc->locstring.start == 0 || *(querysuffix - 1) != subjectseq[loc->locstring.start - 1])
      {
/*#pragma omp critical
          {*/
          if (N >= Size-1)
          {
              Size *= 2;
              A = (Match_t *) Safe_realloc(A, Size*sizeof(Match_t));
              cerr << "Safe_realloc " << Size << endl;
          }
          N++;
          cerr << N << " ";
          A[N].R = loc->locstring.start;
          A[N].Q = (Uint) (querysuffix-query);
          A[N].Len = loc->locstring.length;
          //}
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
                       Uint chunks,
                       Processmatchfunction processmumcandidate,
                       void *processinfo,
                       Uchar *query,
                       Uint querylen,
                       Uint seqnum)
{
  Uchar *lptr, *left, *right = query + querylen - 1, 
        *querysuffix;
  Location loc;
  bool flag;
  int i;
  char buf[40];
  string file;
  ostringstream name;
  int files[chunks];
  Uint N = 0;

  for (i=0; i<chunks; i++)
  { 
      name << i;
      files[i]=open(name.str().c_str(),O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
      if (files[i] < 0)
          return -1;
      name.str(std::string());
  }
  omp_set_num_threads(chunks);
  #pragma omp parallel for default (none) private(i,left,right,lptr,querysuffix,loc,flag,buf) shared(std::cerr,stderr,files,chunks,query,querylen,stree,minmatchlength,seqnum)  reduction(+:N)
  for (i=0; i<chunks; i++)
  { 
      left = query + (Uint)(querylen/chunks*i);
      right = query + (Uint)(querylen/chunks*(i+1))-1;
      lptr = scanprefixfromnodestree (stree, &loc, ROOT (stree), left, right, 0);
      for (querysuffix = left; querysuffix<right && lptr != NULL;  querysuffix++)
      { 
          if (loc.locstring.length >= minmatchlength && loc.remain > 0 && loc.nextnode.toleaf)
          {
               if (querysuffix == query || loc.locstring.start == 0 || *(querysuffix - 1) != stree->text[loc.locstring.start - 1])
               {
                   std::fill(&buf[0],&buf[40],0);
                   sprintf(buf,"%lu,%lu,%lu\n",loc.locstring.start,(Uint) (querysuffix-query),loc.locstring.length);
                   if (write(files[omp_get_thread_num()],buf,sizeof(buf))!=sizeof(buf))
                   fprintf(stderr,"ERROR R\n");
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
               if (querysuffix == query || loc.locstring.start == 0 || *(querysuffix - 1) != stree->text[loc.locstring.start - 1])
               {
                   std::fill(&buf[0],&buf[40],0);
                   sprintf(buf,"%lu,%lu,%lu\n",loc.locstring.start,(Uint) (querysuffix-query),loc.locstring.length);
                   if (write(files[omp_get_thread_num()],buf,sizeof(buf))!=sizeof(buf))
                       fprintf(stderr,"ERROR R\n");
                   N++;
               }
          }
          linklocstree (stree, &loc, &loc);
          querysuffix++;
      }
  } 
  for (i=0; i<chunks;i++)
      close(files[i]);
  return 0;
}
