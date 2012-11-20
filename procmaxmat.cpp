/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

//\IgnoreLatex{

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <ctype.h>
//#include <google/sparsetable>
#include <math.h>
#include <omp.h>
#include <iterator>
#include <likwid.h>
#include "types.h"
#include "errordef.h"
#include "protodef.h"
#include "spacedef.h"
#include "streedef.h"
#include "streeacc.h"
#include "maxmatdef.h"
#include "distribute.h"

//using google::sparsetable;
//}

/*EE
  This file contains functions to appropriately
  call the function \texttt{findmumcandidates} and \texttt{findmaxmatches}
  and to process their result according to the options given by the user.
*/


//\IgnoreLatex{

/*
  The following is the type for the functions to finding maximal matches or
  MUM candidates.
*/

typedef Sint (*Findmatchfunction)(Suffixtree *,
                                 // sparsetable<Uint*>,
                                  Uint,
                                  Uint,
                                  Processmatchfunction,
                                  void *,
                                  Uchar *,
                                  Uint,
                                  Uint);

/*
  The following function is imported from \texttt{findmumcand.c}.
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
   for  (i = 0;  i < N;  i ++)
     A [i] . Good = true;

   for  (i = 0;  i < N - 1;  i ++)
     {
      int  i_diag, i_end;

      if  (! A [i] . Good)
          continue;

      i_diag = A [i] . Q - A [i] . R;
      i_end = A [i] . Q + A [i] . Len;

      for  (j = i + 1;  j < N && A [j].Q <= i_end;  j ++)
        {
         int  olap;
         int  j_diag;

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

   for  (i = 0;  i < N;  i ++)
     A[i].Good = false;

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

   if  (x->Q < y->Q)
       return  -1;
   else if  (x->Q > y->Q)
       return  1;
   else if  (x->R < y->R)
       return  -1;
   else if  (x->R > y->R)
       return  1;
     else
       return  0;
  }

static void  Process_Matches (Match_t * A, int N) //  Process matches  A [1 .. N].
  {
   long int  cluster_size, sep;
   int  i;

   if  (N <= 0)
       return;
   qsort (A + 1, N, sizeof (Match_t), By_Q);

   Filter_Matches (A + 1, N);

   for  (i = 1;  i < N;  i ++)
   { 
       if  (!A[i].Good)
           printf ("%8ld  %8ld  %8ld\n", A[i].R, A[i].Q, A[i].Len);
   }
   return;
  }

Sint findmumcandidates(Suffixtree *stree,
                       Uint minmatchlength,
                       Uint wordsize,
                       Processmatchfunction processmatch,
                       void *processinfo,
                       Uchar *query,
                       Uint querylen,
                       Uint seqnum);

/*
  The following function is imported from \texttt{findmaxmat.c}
*/

Sint findmaxmatches(Suffixtree *stree,
                    Uint minmatchlength,
                    Uint wordsize,
                    Processmatchfunction processmatch,
                    void *processinfo,
                    Uchar *query,
                    Uint querylen,
                    Uint seqnum);

/*
  The following function is imported from \texttt{maxmatinp.c}
*/

Sint scanmultiplefastafile (Multiseq *multiseq,
                            char *filename,
                            Uchar replacewildcardchar,
                            Uchar *input,
                            Uint inputlen);

//}

/*EE
  The following code fragement is used when computing MUMs.
  It calls the function \texttt{mumuniqueinquery}.
  The MUM-candidates are stored in the dynamic 
  array \texttt{mumcandtab}.  After the real MUMs are output, the table 
  of MUM-candidates are declared to be empty.
*/

#define PROCESSREALMUMS\
        if(matchprocessinfo->cmum)\
        {\
          if(mumuniqueinquery(info,\
                              matchprocessinfo->showstring ?\
                                 showseqandmaximalmatch :\
                                 showmaximalmatch,\
                              &matchprocessinfo->mumcandtab) != 0)\
          {\
            return -2;\
          }\
          matchprocessinfo->mumcandtab.nextfreeMUMcandidate = 0;\
        }

/*
  The following function assigns for a given base the corresponding
  complement character. It also handles wild card characters 
  appropriately.
*/

#define ASSIGNMAXMATCOMPLEMENT(VL,VR)\
        if((VR) >= MMREPLACEMENTCHARQUERY)\
        {\
          VL = VR;\
        } else\
        {\
          switch (VR)\
          {\
            case 'a':\
              VL = (Uchar) 't';\
              break;\
            case 'c':\
              VL = (Uchar) 'g';\
              break;\
            case 'g':\
              VL = (Uchar) 'c';\
              break;\
            case 't':\
              VL = (Uchar) 'a';\
              break;\
            case 'r':                       /* a or g */\
              VL = (Uchar) 'y';\
              break;\
            case 'y':                       /* c or t */\
              VL = (Uchar) 'r';\
              break;\
            case 's':                       /* c or g */\
              VL = (Uchar) 's';\
              break;\
            case 'w':                       /* a or t */\
              VL = (Uchar) 'w';\
              break;\
            case 'm':                       /* a or c */\
              VL = (Uchar) 'k';\
              break;\
            case 'k':                       /* g or t */\
              VL = (Uchar) 'm';\
              break;\
            case 'b':                       /* c, g or t */\
              VL = (Uchar) 'v';\
              break;\
            case 'd':                       /* a, g or t */\
              VL = (Uchar) 'h';\
              break;\
            case 'h':                       /* a, c or t */\
              VL = (Uchar) 'd';\
              break;\
            case 'v':                       /* a, c or g */\
              VL = (Uchar) 'b';\
              break;\
            default:                        /* anything */\
              VL = (Uchar) 'n';\
              break;\
          }\
        }

/*
  The following function computes the Watson-Crick complement
  of the sequence pointed to by \texttt{seq}. The sequnce is
  of length \texttt{seqlen}. The result is computed in-place.
*/

static void wccSequence (Uchar *seq,
                         Uint seqlen)
{
  Uchar *front, *back, tmp = 0;

  for (front = seq, back = seq + seqlen - 1; 
       front <= back; front++, back--)
  {
    ASSIGNMAXMATCOMPLEMENT (tmp, *front);
    ASSIGNMAXMATCOMPLEMENT (*front, *back);
    *back = tmp;
  }
}

/*
  The following function shows the sequence description of sequence
  number \texttt{seqnum} in \texttt{multiseq}.
*/

static void showsequencedescription(Multiseq *multiseq, Uint maxdesclength,
                                    Uint seqnum)
{
  Uint i, desclength = DESCRIPTIONLENGTH(multiseq,seqnum);
  Uchar *desc = DESCRIPTIONPTR(multiseq,seqnum);

  for(i=0; i<desclength; i++)
  {
    if(isspace((int) desc[i]))
    {
      break;
    }
    (void) putchar((int) desc[i]);
  }
  if(desclength < maxdesclength)
  {
    for(i=0; i < maxdesclength - desclength; i++)
    {
      (void) putchar(' ');
    }
  }
}

/*
  The following function shows the sequence header for the
  sequence with number \texttt{seqnum} in the \texttt{Multiseq}-record. 
  \texttt{showsequencelengths} is true if the option \texttt{-L} was
  set. \texttt{currentisrcmatch} is True iff if the current matches
  to be reported (if any) are reverse complemented matches.
  \texttt{seqlen} is the length of the sequence for which the header
  is shown.
*/

static void showsequenceheader(Multiseq *multiseq,
                               bool showsequencelengths,
                               bool currentisrcmatch,
                               Uint seqnum,
                               Uint seqlen)
{

  printf("%c ",FASTASEPARATOR);
  showsequencedescription(multiseq,0,seqnum);
  if(currentisrcmatch)
  {
    printf(" Reverse");
  }
  if(showsequencelengths)
  {
    printf("  Len = %lu",(unsigned long int) seqlen);
  }
  printf("\n");
}

/*
  The following is one of three functions to further process 
  a MUM-candidate specfied by its length, its start position
  in the subject sequence, the number of the query in which
  it matches as well as the start of the match in the query.
  The function \texttt{showmaximalmatch} simply shows the 
  relevant information as a triple of three integers.
*/

static Sint showmaximalmatch (void *info,
                              Uint matchlength,
                              Uint subjectstart,
                              /*@unused@*/ Uint seqnum,
                              Uint querystart)
{
  /*if  (N >= Size - 1)
  {
      Size *= 2;
      A = (Match_t *) Safe_realloc (A, Size * sizeof (Match_t));
  }
  N++;
  */
  Matchprocessinfo *matchprocessinfo = (Matchprocessinfo *) info;

  if(matchprocessinfo->subjectmultiseq->numofsequences == UintConst(1)  && !matchprocessinfo->fourcolumn)
  {
    //A[N].R = (long unsigned int) (subjectstart+1);
  } else
  {       
    PairUint pp;

    if(pos2pospair(matchprocessinfo->subjectmultiseq,&pp,subjectstart) != 0)
     return -1;
    pp.uint0 = 0;
    pp.uint1 = subjectstart;
    showsequencedescription(matchprocessinfo->subjectmultiseq,matchprocessinfo->maxdesclength,pp.uint0);
    //A[N].R =(long unsigned int) (pp.uint1+1);
  }
  if(matchprocessinfo->currentisrcmatch && matchprocessinfo->showreversepositions)
  {
    //A[N].Q = (long unsigned int) (matchprocessinfo->currentquerylen - querystart);
  } else
  {
    //A[N].Q = (long unsigned int) (querystart+1);
  }
  //A[N].Len = (long unsigned int) matchlength;
  //A[N].Good = true;
  //A[N].Tentative = false;
  return 0;
}

/*
  The following function shows the same information as the previous
  function and additionally the sequence information.
*/

static Sint showseqandmaximalmatch (void *info,
                                    Uint matchlength,
                                    Uint subjectstart,
                                    Uint seqnum,
                                    Uint querystart)
{
  Matchprocessinfo *matchprocessinfo = (Matchprocessinfo *) info;

  (void) showmaximalmatch (info,
                           matchlength,
                           subjectstart,
                           seqnum,
                           querystart);
  if (fwrite (matchprocessinfo->subjectmultiseq->sequence + subjectstart, 
              sizeof (Uchar), 
              (size_t) matchlength,
              stdout) != (size_t) matchlength)
  {
    fprintf(stderr, "cannot output string of length %lu", (long unsigned int) matchlength);
    return -1;
  }
  printf ("\n");
  return 0;
}

/*
  The following function stores the information about a MUM-candidate
  in the next free position of the dynamic array \texttt{mumcandtab}.
*/

static Sint storeMUMcandidate (void *info, Uint matchlength, Uint subjectstart, Uint seqnum, Uint querystart)
{
/*#pragma omp shared (A,size)
    {
  if  (N >= Size - 1)
  {
      Size *= 2;
      A = (Match_t *) Safe_realloc (A, Size * sizeof (Match_t));
  }
  N++;
  A[N].R = subjectstart;
  A[N].Q = querystart;
  A[N].Len = matchlength;
    }*/
#pragma omp critical
  {
  Matchprocessinfo *matchprocessinfo = (Matchprocessinfo *) info;
  MUMcandidate *mumcandptr;
  //fprintf(stdout,"%lu,%lu,%lu\n",subjectstart,querystart,matchlength);
  //GETNEXTFREEINARRAY(mumcandptr, &matchprocessinfo->mumcandtab, MUMcandidate,1024);
  }
  /*mumcandptr->mumlength = matchlength;
  mumcandptr->dbstart = subjectstart;
  mumcandptr->queryseq = seqnum;
  mumcandptr->querystart = querystart;*/
  return 0;
}

/*
  The following function searches for forward and reverse complemented
  MUM-candidates (if necessary) in the current query of length
  \texttt{querylen}. The number of the query sequence is 
  \texttt{querylen}.
*/

static Sint findmaxmatchesonbothstrands(void *info,Uint seqnum,
                                        Uchar *query,Uint querylen)
{
  Matchprocessinfo *matchprocessinfo = (Matchprocessinfo *) info;
  Processmatchfunction processmatch;
  Findmatchfunction findmatchfunction;
  if(matchprocessinfo->cmum)
  {
    processmatch = storeMUMcandidate;
    findmatchfunction = findmumcandidates;
  } else
  {
    if(matchprocessinfo->showstring)
    {
      processmatch = showseqandmaximalmatch;
    } else
    {
      processmatch = showmaximalmatch;
    }
    if(matchprocessinfo->cmumcand)
    {
      findmatchfunction = findmumcandidates;
    } else
    {
      findmatchfunction = findmaxmatches;
    }
  }
  matchprocessinfo->currentquerylen = querylen;
  if(matchprocessinfo->forward)
  {
    showsequenceheader(&matchprocessinfo->querymultiseq,
                       matchprocessinfo->showsequencelengths,
                       false,
                       seqnum,
                       querylen);
    matchprocessinfo->currentisrcmatch = false;
    if(findmatchfunction(&matchprocessinfo->stree,
                         matchprocessinfo->minmatchlength,
                         matchprocessinfo->chunks,
                         processmatch,
                         info,
                         query,
                         querylen,
                         seqnum) != 0)
    {
      return -1;
    }
    PROCESSREALMUMS;
  }
  if(matchprocessinfo->reversecomplement)
  {
    if(matchprocessinfo->cmum)
    {
      matchprocessinfo->mumcandtab.nextfreeMUMcandidate = 0;
    }
    showsequenceheader(&matchprocessinfo->querymultiseq,
                       matchprocessinfo->showsequencelengths,
                       true,
                       seqnum,
                       querylen);
    wccSequence(query,querylen);
    matchprocessinfo->currentisrcmatch = true;
    if(findmatchfunction(&matchprocessinfo->stree, 
                         matchprocessinfo->minmatchlength,
                         matchprocessinfo->chunks,
                         processmatch,
                         info,
                         query,
                         querylen,
                         seqnum) != 0)
    {
      return -2;
    }
    PROCESSREALMUMS;
  }
  return 0;
}

static Sint getmaxdesclen(Multiseq *multiseq)
{
  Uint desclen, maxdesclen, seqnum;
  if(multiseq->numofsequences == 0)
  {
    cerr << "multiple sequence contains 0 sequences" << endl;
    return -1;
  }
  maxdesclen = DESCRIPTIONLENGTH(multiseq,0);
  for(seqnum = UintConst(1); seqnum < multiseq->numofsequences; seqnum++)
  {
    desclen = DESCRIPTIONLENGTH(multiseq,seqnum);
    if(desclen > maxdesclen)
    {
      maxdesclen = desclen;
    }
  }
  return (Sint) maxdesclen;
}

/*EE
  The following function constructs the suffix tree,
  initializes the \texttt{Matchprocessinfo}-record appropriately,
  initializes the dynamic array \texttt{mumcandtab} (if necessary),
  and then iterates the function \texttt{findmaxmatchesonbothstrands}
  over all sequences in \texttt{querymultiseq}. Finally, the space
  allocated for the suffix tree and the space for \texttt{mumcandtab}
  is freed.
*/

Sint procmaxmatches(MMcallinfo *mmcallinfo,Multiseq *subjectmultiseq)
{ 
  Matchprocessinfo matchprocessinfo;
  Uint filenum, filelen, dsl=0;
  Sint retcode;
  Uchar *filecontent;
  Location ploc;
  double start, finish;

  fprintf(stderr,"# construct suffix tree for sequence of length %lu\n", (long unsigned int) subjectmultiseq->totallength);
  /*fprintf(stderr,"# (maximum reference length is %lu)\n", (long unsigned int) getmaxtextlenstree());
  fprintf(stderr,"# (maximum query length is %lu)\n", (long unsigned int) ~((Uint)0));*/
  start = MPI::Wtime();
  if(constructprogressstree (&matchprocessinfo.stree,subjectmultiseq->sequence,subjectmultiseq->totallength,NULL,NULL,NULL) != 0)
    return -1;
  finish = MPI::Wtime();
  cerr << "# createST Time: " << finish-start << endl;
  matchprocessinfo.subjectmultiseq = subjectmultiseq;
  matchprocessinfo.minmatchlength = mmcallinfo->minmatchlength;
  matchprocessinfo.showstring = mmcallinfo->showstring;
  matchprocessinfo.showsequencelengths = mmcallinfo->showsequencelengths;
  matchprocessinfo.showreversepositions = mmcallinfo->showreversepositions;
  matchprocessinfo.forward = mmcallinfo->forward;
  matchprocessinfo.fourcolumn = mmcallinfo->fourcolumn;
  matchprocessinfo.cmum = mmcallinfo->cmum;
  matchprocessinfo.cmumcand = mmcallinfo->cmumcand;
  matchprocessinfo.reversecomplement = mmcallinfo->reversecomplement;
  matchprocessinfo.chunks = mmcallinfo->chunks;
  if(mmcallinfo->cmum)
  {
    INITARRAY(&matchprocessinfo.mumcandtab,MUMcandidate);
  }
  retcode = getmaxdesclen(subjectmultiseq);
  if(retcode < 0)
  {
    return -2;
  }
  matchprocessinfo.maxdesclength = (Uint) retcode;
  /*Size = 500;
  A = (Match_t *) Safe_malloc (Size * sizeof (Match_t));*/
  for(filenum=0; filenum < mmcallinfo->numofqueryfiles; filenum++)
  {
    filecontent = (Uchar *) CREATEMEMORYMAP (mmcallinfo->queryfilelist[filenum],true,&filelen);
    if (filecontent == NULL || filelen == 0)
    {
      fprintf(stderr, "cannot open file \"%s\" or file \"%s\" is empty",mmcallinfo->queryfilelist[filenum],mmcallinfo->queryfilelist[filenum]);
      return -3;
    }
    if (scanmultiplefastafile (&matchprocessinfo.querymultiseq,mmcallinfo->queryfilelist[filenum],mmcallinfo->matchnucleotidesonly ? MMREPLACEMENTCHARQUERY : 0,
                               filecontent, filelen) != 0)
    {
      return -4;
    }
    fprintf(stderr, "# matching query-file \"%s\"\n# against subject-file \"%s\"\n", mmcallinfo->queryfilelist[filenum], mmcallinfo->subjectfile);
    if (overallsequences (false,&matchprocessinfo.querymultiseq,(void *) &matchprocessinfo,findmaxmatchesonbothstrands) != 0)
    { 
      return -5;
    }
    freemultiseq(&matchprocessinfo.querymultiseq);
    }
  //Process_Matches (A, N);
  if(mmcallinfo->cmum)
  {
    FREEARRAY(&matchprocessinfo.mumcandtab,MUMcandidate);
  }
  //fprintf(stderr,"# Matches=%lu\n",(Sint)N);
  freestree (&matchprocessinfo.stree);
  return 0;
}
