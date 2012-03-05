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
#include <google/sparsetable>
#include <math.h>
#include "distribute.h"
#include "streedef.h"
#include "streeacc.h"
#include "spacedef.h"
#include "maxmatdef.h"
#include "distribute.h"

using google::sparsetable;
//}

/*EE
  This file contains functions to compute maximal matches of some
  minimum length between the subject-sequence and the query-sequence.
  This is done by a traversal of the suffix tree of the subject-sequence.
  For each suffix, say \(s\), of the query, the location \texttt{ploc}
  of some prefix \(p\) of \(s\) is determined. 
  Let \texttt{pmax} be the longest prefix
  of \(s\) that occurs as a substring of the subject-sequence.
  \(p\) is determined as follows.
  \begin{itemize}
  \item
  If the length of \texttt{pmax} is \(\leq\mathtt{minmatchlength}\), 
  then \(p=\texttt{pmax}\).
  \item
  If the length of \texttt{pmax} is \(>\texttt{minmatchlength}\), then
  \(p\) is the prefix of \texttt{pmax} of length 
  \(\texttt{minmatchlength}\).
  \end{itemize}
  Given \texttt{ploc}, the location \texttt{maxloc} of \texttt{pmax} is 
  determined, thereby keeping track of the branching nodes 
  visited during this matching
  step. Finally, the suffix tree below location \texttt{ploc}
  is traversed
  in a depth first strategy. This delivers the set of suffixes representing
  a match of length at least \(\texttt{minmatchlength}\) against 
  a prefix of \(s\).
  Using a stack one keeps track of the length of the longest common prefix 
  of each encountered suffix and \(s\). Finally, it is checked whether the
  match between the match is maximal by comparing the characters
  to the left and to the right of the two instances of the match.
*/

/*EE
  For the depth first traversal we need a stack containing elements
  of the following type. Each stack element stores information for
  a branching node of the suffix tree. The top of the stack corresponds
  to the branching node, say \(b\), currently visited.
  \texttt{querycommondepth} is the length of the longest common prefix
  of \(s\) and all suffixes represented by leaves in the subtree below 
  \(b\). \texttt{onmaxpath} is \texttt{true} iff the sequence 
  corresponding to \(b\) is a prefix of the query.
*/

struct Nodeinfo
{
  Uint querycommondepth;
  bool onmaxpath;
};

/*EE
  The stack is represented by a dynamic array of elements of type 
  \texttt{Nodeinfo}.
*/

DECLAREARRAYSTRUCT(Nodeinfo);

/*EE
  The following type contains all information required during the
  computation of the maximal matches.
*/

struct Maxmatchinfo
{
  Suffixtree *stree;              // reference to suffix tree of subject-seq
  ArrayNodeinfo commondepthstack; // stack to store depth values
  ArrayPathinfo matchpath;        // path of br. nodes from ploc to maxloc
  Location maxloc;                // location of \texttt{pmax}
  Uchar *query,                   // the query string
        *querysuffix;             // current suffix of query
  Uint querylen,                  // length of the current query
       queryseqnum,               // number of query sequence
       minmatchlength,            // min length of a match to be reported
       depthofpreviousmaxloc;     // the depth of the previous maxloc
  Processmatchfunction processmatch; // this function processes found match
  void *processinfo;            // first arg. when calling previous function
};

//\IgnoreLatex{

static Uint lcp(Uchar *start1,Uchar *end1,Uchar *start2,Uchar *end2)
{
  Uchar *ptr1 = start1,
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

static void checkquerycommonprefix(Maxmatchinfo *maxmatchinfo,
                                   Bref nodeptr,
                                   Uint computeddepth)
{
  Uint prefixlength;
  Uchar *nodestring;
  Branchinfo branchinfo;

  getbranchinfostree(maxmatchinfo->stree,
                     ACCESSDEPTH | ACCESSHEADPOS,&branchinfo,
                     nodeptr);
  nodestring = maxmatchinfo->stree->text + branchinfo.headposition;
  prefixlength = lcp(nodestring,nodestring+branchinfo.depth-1,
                     maxmatchinfo->querysuffix,
                     maxmatchinfo->query+maxmatchinfo->querylen-1);
  if(prefixlength != computeddepth)
  {
    printf("prefixlength=%lu!=%lu=computeddepth\n",
            (long unsigned int) prefixlength,
            (long unsigned int) computeddepth);
    printf("nodepath=");
    (void) fwrite(nodestring,sizeof(Uchar),(size_t) branchinfo.depth,stdout);
    printf("\nquery=");
    (void) fwrite(maxmatchinfo->querysuffix,sizeof(Uchar),
                  (size_t) (maxmatchinfo->querylen - 
                            (Uint) (maxmatchinfo->querysuffix - 
                                    maxmatchinfo->query)),stdout);
    printf("\n");
    exit(EXIT_FAILURE);
  }
}

static void showgreedymatchresult(Maxmatchinfo *maxmatchinfo,
                                  Location *ploc)
{
  Uint i;

  printf("greedymatchresult: start at ploc ");
  showlocation(stdout,maxmatchinfo->stree,ploc);
  printf("\n");
  printf("and ends at location ");
  showlocation(stdout,maxmatchinfo->stree,&maxmatchinfo->maxloc);
  printf("\n");
  if(maxmatchinfo->matchpath.nextfreePathinfo > 0)
  {
    printf("and the matchpath is a follows\n");
  }
  for(i=0; i<maxmatchinfo->matchpath.nextfreePathinfo; i++)
  {
    printf("matchpath[%lu]=Branch %lu\n",(long unsigned int) i,
           (long unsigned int) BRADDR2NUM(maxmatchinfo->stree,
                                 maxmatchinfo->matchpath.
                                               spacePathinfo[i].ref));
  }
}

#define CHECKIFLOCATIONISVALID(LOC)\
        if((LOC)->remain == 0)\
        {\
          fprintf(stderr,"location is branch location\n");\
          exit(EXIT_FAILURE);\
        }\
        if(!(LOC)->nextnode.toleaf)\
        {\
          fprintf(stderr,"location is not leaf location\n");\
          exit(EXIT_FAILURE);\
        }\
        if(LEAFADDR2NUM(maxmatchinfo->stree,\
                        (LOC)->nextnode.address) != leafindex)\
        {\
          fprintf(stderr,"location differes from leaf index\n");\
          exit(EXIT_FAILURE);\
        } 

//}

/*
  The following function is applied to each leaf visited during
  the depth first traversal. It checks if corresponding match is
  left maximal. In this case, the length of the longest common
  prefix of this suffix and \(s\) is computed.
*/

static Sint processleaf(Uint leafindex,/*@unused@*/ Bref lcpnode,void *info)
{
  Maxmatchinfo *maxmatchinfo = (Maxmatchinfo *) info;

  if(leafindex == 0 ||
     maxmatchinfo->query == maxmatchinfo->querysuffix ||
     maxmatchinfo->stree->text[leafindex - 1] != 
     *(maxmatchinfo->querysuffix-1))
  {
    Uint lcplength;

    if(maxmatchinfo->commondepthstack.nextfreeNodeinfo == 0)
    {
      CHECKIFLOCATIONISVALID(&maxmatchinfo->maxloc);
      lcplength = maxmatchinfo->maxloc.locstring.length;
    } else
    {
      Nodeinfo *father = maxmatchinfo->commondepthstack.spaceNodeinfo + 
                         maxmatchinfo->commondepthstack.nextfreeNodeinfo-1;
      if(father->onmaxpath &&
         LEAFADDR2NUM(maxmatchinfo->stree,
                      maxmatchinfo->maxloc.nextnode.address) == leafindex)
      {
        lcplength = maxmatchinfo->maxloc.locstring.length;
      } else
      {
        lcplength = father->querycommondepth;
      }
    }
    if(maxmatchinfo->processmatch(
                maxmatchinfo->processinfo,
                lcplength,                           // length of match
	        leafindex,                           // dbstart
                maxmatchinfo->queryseqnum,
                (Uint) (maxmatchinfo->querysuffix - 
                        maxmatchinfo->query)) != 0)  // querystart
    {
      return -1;
    }
  }
  return 0;
}

/*
  The following functions inherits information from the maximal matchpath
  to the stack used in the depth first traversal.
*/

static void inheritfrompath(ArrayPathinfo *matchpath,Location *maxloc,
                            Nodeinfo *stacktop,Bref nodeptr,
                            Uint accessindex,
                            Uint inheritdepth)
{
  if(accessindex > matchpath->nextfreePathinfo)
  {
    stacktop->onmaxpath = false;
    stacktop->querycommondepth = inheritdepth;
  } else
  {
    if(accessindex == matchpath->nextfreePathinfo)
    {
      if(maxloc->remain == 0)
      {
        if(maxloc->nextnode.address == nodeptr)
        {
          stacktop->onmaxpath = true;
          stacktop->querycommondepth = maxloc->locstring.length;
        } else
        {
          stacktop->onmaxpath = false;
          stacktop->querycommondepth = inheritdepth;
        }
      } else
      {
        stacktop->onmaxpath = false;
        if(maxloc->nextnode.address == nodeptr)
        {
          stacktop->querycommondepth = maxloc->locstring.length;
        } else
        {
          stacktop->querycommondepth = inheritdepth;
        }
      }
    } else
    {
      if(matchpath->spacePathinfo[accessindex].ref == nodeptr)
      {
        stacktop->onmaxpath = true;
        stacktop->querycommondepth 
          = matchpath->spacePathinfo[accessindex].depth;
      } else
      {
        stacktop->onmaxpath = false;
        stacktop->querycommondepth = inheritdepth;
      }
    }
  }
}

/*
  The following function is called whenever during a depth first traversal
  of a subtree of the suffix tree, each time
  a branching node is visited for the first time.
  The arguments are the branching node \texttt{nodeptr} and 
  a pointer \texttt{info} to some information passed to the function 
  \texttt{depthfirststree}. If the \texttt{commondepthstack}
  is empty or the father of the current node is on the maximal path,
  then the commondepthstack inherits information from the appropriate
  value of the maximal match path. Otherwise, the information about
  the maximal matching prefix length is propagated down the stack.
  The function always return \texttt{true} and thus the depth first
  traversal continues.
*/

static bool processbranch1(Bref nodeptr,void *info)
{
  Maxmatchinfo *maxmatchinfo = (Maxmatchinfo *) info;
  Nodeinfo *stacktop, 
           *father;
  GETNEXTFREEINARRAY(stacktop,&maxmatchinfo->commondepthstack,Nodeinfo,32);
  if(stacktop == maxmatchinfo->commondepthstack.spaceNodeinfo)
  {
    inheritfrompath(&maxmatchinfo->matchpath,
                    &maxmatchinfo->maxloc,
                    stacktop,
                    nodeptr,
                    0,
                    maxmatchinfo->minmatchlength);
  } else
  {
    father = stacktop-1;
    if(father->onmaxpath)
    {
      inheritfrompath(&maxmatchinfo->matchpath,
                      &maxmatchinfo->maxloc,
                      stacktop,
                      nodeptr,
                      maxmatchinfo->commondepthstack.nextfreeNodeinfo-1,
                      father->querycommondepth);
    } else
    {
      stacktop->onmaxpath = false;
      stacktop->querycommondepth = father->querycommondepth;
    }
  }
  checkquerycommonprefix(maxmatchinfo,nodeptr,stacktop->querycommondepth);
  return true;
}

/*
  The following function is called whenever a branching node 
  \texttt{nodeptr}
  is visited for the second time (i.e.\ the entire subtree below 
  \texttt{nodeptr} has been processed).
  \texttt{info} is a pointer to some information passed to the function
  \texttt{depthfirststree}.
*/

static Sint processbranch2(/*@unused@*/ Bref nodeptr,void *info)
{
  Maxmatchinfo *maxmatchinfo = (Maxmatchinfo *) info;

  maxmatchinfo->commondepthstack.nextfreeNodeinfo--;
  return 0;
}

/*
  The following function computes the maximal matches below location
  \(ploc\). All global information is passed via the 
  \texttt{Maxmatchinfo}-record. At first the number 
  \texttt{rescanprefixlength} is determined. This is the length of 
  the current \texttt{querysuffix} that definitely match some suffix 
  of the subject-sequence. Then the suffix tree is scanned starting at
  \texttt{ploc} to find \texttt{maxloc}. During this matching phase,
  all branching nodes visited are stored in \texttt{matchpath}.
  If \texttt{ploc} is a leaf location, then the corresponding leaf
  is directly processed by the function \texttt{processleaf}.
  Otherwise, the \texttt{nextnode} component of \texttt{ploc}
  is pushed on a stack and a depth first traversal of 
  the suffix tree below node \texttt{nextnode} is performed.
*/

static Sint enumeratemaxmatches (Maxmatchinfo *maxmatchinfo,
                                 Location *ploc)
{
  Uint rescanprefixlength;

  maxmatchinfo->matchpath.nextfreePathinfo = 0;
  if(maxmatchinfo->depthofpreviousmaxloc > UintConst(1))
  {
    rescanprefixlength = maxmatchinfo->depthofpreviousmaxloc - 1;
  } else
  {
    rescanprefixlength = 0;
  }
  (void) findprefixpathstree (maxmatchinfo->stree, 
                              &maxmatchinfo->matchpath,
                              &maxmatchinfo->maxloc, 
                              ploc,
                              maxmatchinfo->querysuffix
                                  + maxmatchinfo->minmatchlength,
                              maxmatchinfo->query 
			          + maxmatchinfo->querylen - 1,
                              rescanprefixlength);
  maxmatchinfo->depthofpreviousmaxloc 
    = maxmatchinfo->maxloc.locstring.length;
  maxmatchinfo->commondepthstack.nextfreeNodeinfo = 0;
  if(ploc->nextnode.toleaf)
  {
    if(processleaf(LEAFADDR2NUM(maxmatchinfo->stree,ploc->nextnode.address),
                   NULL,(void *) maxmatchinfo) != 0)
    {
      return -1;
    }
  } else
  {
    (void) processbranch1(ploc->nextnode.address,(void *) maxmatchinfo);
    if(depthfirststree(maxmatchinfo->stree,
                       &ploc->nextnode,
                       processleaf,
                       processbranch1,
                       processbranch2,
                       NULL,
                       NULL,
                       (void *) maxmatchinfo) != 0)
    {
      return -2;
    }
  }
  return 0;
}

/*EE
  The following function finds all maximal matches between the 
  subject sequence and the query sequence of length at least
  \texttt{minmatchlength}. To each match the function \texttt{processmatch}
  is applied, with \texttt{processinfo} as its first argument.
  \texttt{query} is the reference to the query, \texttt{querylen} is the
  length of the query and \texttt{queryseqnum} is the number of the
  query sequence. Initially, the function appropriately intializes the
  \texttt{maxmatchinfo}-record. It then scans \texttt{query} to find
  \texttt{ploc} for the longest suffix of \texttt{query}.
  The depth of \texttt{ploc} is stored in \texttt{depthofpreviousmaxloc}.
  In the \texttt{for}-loop each instance of \texttt{ploc} is determined
  and processed further by \texttt{enumeratemaxmatches} whenever its 
  depth is longer than the minimum match length.
*/

Sint findmaxmatches(Suffixtree *stree,
                    Uint minmatchlength,
                    Uint wordsize,
                    Processmatchfunction processmatch,
                    void *processinfo,
                    Uchar *query,
                    Uint querylen,
                    Uint queryseqnum)
{
  Uchar *querysubstringend;  // ref to end of querysubs. of len. minmatchl.
  Location ploc;
  Maxmatchinfo maxmatchinfo;
  Uchar *omp_Iterator;
  bool back = false;
  int res = 0;
  if(querylen < minmatchlength)
    return res;
  maxmatchinfo.stree = stree;
  INITARRAY(&maxmatchinfo.commondepthstack,Nodeinfo);
  INITARRAY(&maxmatchinfo.matchpath,Pathinfo);
  maxmatchinfo.querysuffix = maxmatchinfo.query = query;
  maxmatchinfo.querylen = querylen;
  maxmatchinfo.minmatchlength = minmatchlength;
  maxmatchinfo.queryseqnum = queryseqnum;
  maxmatchinfo.processmatch = processmatch;
  maxmatchinfo.processinfo = processinfo;
  Uint size = pow(2,3*wordsize);
  double start, finish;
  start = MPI::Wtime();
  sparsetable<Uint*> table(size);
  createTable(stree,table,wordsize);
  finish = MPI::Wtime();
  cerr << "createTable Time: " << finish-start << endl;
  Uint tmp = pow(2,3*wordsize);
  /*Uint sum=0;
  for ( Uint it = 0; it < tmp; ++it ) {
      if ( table[it] != 0)
          sum++;
  }
  cerr << "Total " << sum << endl;*/
  querysubstringend = query + minmatchlength - 1;
  (void) scanprefixfromnodestree (stree, &ploc, ROOT (stree), 
                                  query, querysubstringend,0);
  maxmatchinfo.depthofpreviousmaxloc = ploc.locstring.length;
  //#pragma omp parallel for num_threads(4) schedule (static) ordered
  for (/*omp_Iterator = querysubstringend*/; /* omp_Iterator */querysubstringend < query + querylen - 1; 
      maxmatchinfo.querysuffix++, querysubstringend++/* omp_Iterator++*/)
  {
    /*fprintf(stdout,"Thread:%d omp_Iterator:%lu %lu %lu %lu\n",omp_get_thread_num(),(Uint)omp_Iterator,
            ploc.locstring.start+1, 
                   (Uint) 
                   (maxmatchinfo.querysuffix-maxmatchinfo.query)+1,
                   ploc.locstring.length);*/
   /* #pragma omp flush (back)
      if (!back)
      {*/
        string s1 ((char *)maxmatchinfo.querysuffix,wordsize);
        //cout << s1 << " " << 
        (void) table[encoding((Uchar*)s1.c_str(),wordsize)];
        //<< endl;
        if(ploc.locstring.length >= minmatchlength && 
            enumeratemaxmatches(&maxmatchinfo,&ploc) != 0) 
        {
         /* back = true;
            #pragma omp flush (back)*/
            res = -1;
            return res;
        }
        if (ROOTLOCATION (&ploc)) 
        {
            (void) scanprefixfromnodestree (stree, &ploc, ROOT (stree), 
                                      maxmatchinfo.querysuffix+1, 
                                      querysubstringend+1,0);
        }
        else 
        {
            linklocstree (stree, &ploc, &ploc);
            (void) scanprefixstree (stree, &ploc, &ploc,
                              maxmatchinfo.querysuffix
                                 + ploc.locstring.length+1,
                              querysubstringend+1,0);
        }
      //}
  } 
  while (!ROOTLOCATION (&ploc) && ploc.locstring.length >= minmatchlength)
  {
    /*fprintf(stdout,"%lu %lu %lu\n",ploc.locstring.start+1, 
            (Uint) (maxmatchinfo.querysuffix-maxmatchinfo.query)+1,
            ploc.locstring.length);*/
    if(enumeratemaxmatches (&maxmatchinfo,&ploc) != 0)
    {
      res = -2;
    }
    linklocstree (stree, &ploc, &ploc);
    maxmatchinfo.querysuffix++;
  }
  FREEARRAY(&maxmatchinfo.commondepthstack,Nodeinfo);
  FREEARRAY(&maxmatchinfo.matchpath,Pathinfo);
  return res;
}
