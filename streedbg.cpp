/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/
#include <string.h>
#include <google/sparsetable>
#include "types.h"
#include "intbits.h"
#include "visible.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"
#include "distribute.h"

#define SETLEAFUSED(V)   SETIBIT(leafused,V)
#define ISLEAFUSED(V)    ISIBITSET(leafused,V)

#define SETBRANCHUSED(V) SETIBIT(branchused,V)
#define ISBRANCHUSED(V)  ISIBITSET(branchused,V)

#define PROCESSALL(STOP,DP)\
        for(r = (Sint) loc.edgelen-1; r >= STOP; r--)\
        {\
          loc.remain = (Uint) r;\
          loc.locstring.length = DP + loc.edgelen - r;\
          showlocation(stdout,stree,&loc);\
          printf("\n");\
          processloc(stree,&loc);\
        }

using google::sparsetable;

void showthesymbolstring(FILE *fp,Uchar *tlast,Uchar *left,
                                Uchar *right)
{
  Uchar *ptr;
  for(ptr=left; ptr<=right; ptr++)
  {
    if(ptr == tlast)
    {
      (void) putc('~',fp);
      return;
    } 
    if(ptr > left + 10)
    {
      fprintf(fp,"...");
      return;
    }
    SHOWCHARFP(fp,*ptr);
  }
}

char *showsymbol(Uchar c)
{
  static char outbuf[100+1];

  if(INVISIBLE(c))
  {
    sprintf(outbuf,"\\%lu",(Uint) c);
  } else
  {
    sprintf(outbuf,"%c",c);
  }
  return outbuf;
}
 
 Uint getlargelinkstree(Suffixtree *stree,Uint *btptr,Uint depth);

void showtable(Suffixtree *stree,bool final)
{
  Uint *largeptr, *btptr, *succptr, *rcptr, i,
       succdepth, distance, 
       nodeaddress, succ, depth, depthSL, child, brother, 
       headposition, suffixlink;
  Uint leafindex, edgelen;
  Uchar *leftpointer;
  Branchinfo branchinfo;

  btptr = stree->branchtab + LARGEINTS; // skip the root
  cerr << "# " << stree->nodecount << " nodes" << endl;
  for(i=UintConst(1); i < stree->nodecount; i++)
  {
    nodeaddress = BRADDR2NUM(stree,btptr);
    child = GETCHILD(btptr);
    brother = GETBROTHER(btptr);
    GETBOTH(depth,headposition,btptr);
    getbranchinfostree(stree,ACCESSSUFFIXLINK,&branchinfo,btptr);
    if(ISLARGE(*btptr))
    {
      cerr << "# L-" << (Uint) nodeaddress;
      suffixlink = getlargelinkstree(stree,btptr,depth);
      btptr += LARGEINTS;
    } else
    { 
      cerr << "# S-" << (Uint) nodeaddress;
      suffixlink = nodeaddress + SMALLINTS;
      btptr += SMALLINTS;
    }
    GETONLYDEPTH(depthSL,branchinfo.suffixlink);
    fprintf(stderr,",D=%lu,HP=%lu,SL=[%lu|%lu],C=",(Uint) depth, (Uint) headposition, (Uint) suffixlink, (Uint) depthSL);
    SHOWINDEX(child);
    fprintf(stderr,",B=");
    SHOWINDEX(brother);
    fprintf(stderr,"\n");
    succ = child;
  }
}

/* 
   Check the following:
   (1) for each branching node there exist between 2 and 257 successors
   (2) for each branching node the list of successors is strictly ordered
       according to the first character of the edge label
   (3) there are no empty edge labels
   (4) there are \(n+1\) leaves and for each leaf there is exactly one 
       incoming edge
   (5) for each branching node (except for the root) there is exactly one
       incomming edge.
   (6) each suffix link point to a node whose depth is one smaller and
       whose headposition is either smaller or one larger than the previous node
   (7) the last branching node is not small
*/

void checkstree(Suffixtree *stree)
{
  Uint *largeptr, *btptr, *succptr, *slinkptr, lastsmall = 0, succdepth, 
       succheadposition, distance, succ, depth, leafindex, headposition, 
       edgelen, identitycount = 0, edgecount = 0,
       succcount, j, linkdepth, linkheadposition, *leafused, *branchused;
  Sint prevfirstchar, currentfirstchar;
  INITBITTAB(leafused,stree->textlen+1);
  INITBITTAB(branchused,stree->textlen+1);
  btptr = stree->branchtab; 
  while(btptr < stree->nextfreebranch)
  {
    succcount = 0;
    prevfirstchar = -1;
    GETBOTH(depth,headposition,btptr);
    succ = GETCHILD(btptr);
    do 
    {
      edgecount++;
      if(ISLEAF(succ))
      {
        leafindex = GETLEAFINDEX(succ);
        if(headposition == leafindex)
        {
          identitycount++;
        }
        if(ISLEAFUSED(leafindex))
        {
          fprintf(stderr,"Node %lu: more than one leaf edge to %lu\n",
                          (Uint) BRADDR2NUM(stree,btptr),
                          (Uint) leafindex);
          exit(EXIT_FAILURE);
        }
        SETLEAFUSED(leafindex);
        if(depth + leafindex < stree->textlen)
        {
          currentfirstchar = (Sint) stree->text[depth + leafindex];
        } else
        {
          currentfirstchar = LARGESTCHARINDEX + 1;
        }
        edgelen = stree->textlen + 1 - leafindex - depth;
        succ = LEAFBROTHERVAL(stree->leaftab[leafindex]);
      } else
      {
        succptr = stree->branchtab + GETBRANCHINDEX(succ);
        GETBOTH(succdepth,succheadposition,succptr);
        currentfirstchar = (Sint) stree->text[depth + succheadposition];
        edgelen = succdepth - depth;
        if(ISBRANCHUSED(succheadposition))
        {
          fprintf(stderr,"Node %lu: more than one edge to branch node %lu\n",
                          (Uint) GETBRANCHINDEX(succ),
                          (Uint) succheadposition);
          exit(EXIT_FAILURE);
        }
        SETBRANCHUSED(succheadposition);
        succ = GETBROTHER(succptr);
      }
      if(edgelen == 0)
      {
        fprintf(stderr,"Node %lu: outgoing '%s'-edge of length %lu\n",
                        (Uint) BRADDR2NUM(stree,btptr),
                        showsymbol((Uchar) currentfirstchar),
                        (Uint)edgelen);
        exit(EXIT_FAILURE);
      }
      if(prevfirstchar >= currentfirstchar)
      {
        fprintf(stderr,"Node %lu: '%s'-edge +",
                       (Uint) BRADDR2NUM(stree,btptr),
                       showsymbol((Uchar) prevfirstchar));
        fprintf(stderr," '%s'-edge not in correct order\n",
                       showsymbol((Uchar) currentfirstchar));
        exit(EXIT_FAILURE);
      }
      prevfirstchar = currentfirstchar;
      succcount++;
    } while(!NILPTR(succ));
    if(succcount < (Uint) 2 || succcount > (Uint) (LARGESTCHARINDEX + 2))
    {
      fprintf(stderr,"Node %lu: %lu successors\n",
                (Uint) BRADDR2NUM(stree,btptr),
                (Uint) succcount);
      exit(EXIT_FAILURE);
    }
    NEXTNODE(btptr);
  }
  for(j=0; j<=stree->textlen; j++)
  {
    if(!ISLEAFUSED(j))
    {
      fprintf(stderr,"no leaf edge to %lu\n",(Uint) j);
      exit(EXIT_FAILURE);
    }
  }
  btptr = stree->branchtab + LARGEINTS; 
  while(btptr < stree->nextfreebranch)
  {
    GETBOTH(depth,headposition,btptr);
    if(!ISBRANCHUSED(headposition))
    {
      fprintf(stderr,"no edge to branch node with headposition %lu\n",
                 (Uint) headposition);
      exit(EXIT_FAILURE);
    }
    if(ISLARGE(*btptr))
    {
      lastsmall = 0;
      slinkptr = stree->branchtab + getlargelinkstree(stree,btptr,depth);
    } else
    {
      lastsmall = BRADDR2NUM(stree,btptr);
      slinkptr = btptr + SMALLINTS;
    }
    GETBOTH(linkdepth,linkheadposition,slinkptr);
    if(linkdepth + 1 != depth)
    {
      fprintf(stderr,"Node %lu(depth %lu) is linked to node (%lu,depth %lu)\n",
                      (Uint) BRADDR2NUM(stree,btptr),
                      (Uint) depth,
                      (Uint) BRADDR2NUM(stree,slinkptr),
                      (Uint) linkdepth);
      exit(EXIT_FAILURE);
    }
    if(headposition + 1 < linkheadposition)
    {
      fprintf(stderr,"Node %lu(headposition %lu) is linked to node"
                     " %lu(headposition %lu)\n",
                      (Uint) BRADDR2NUM(stree,btptr),
                      (Uint) headposition,
                      (Uint) BRADDR2NUM(stree,slinkptr),
                      (Uint) linkheadposition);
      exit(EXIT_FAILURE);
    }
    NEXTNODE(btptr);
  }
  if(lastsmall != 0)
  {
    fprintf(stderr,"Node %lu is the last node but it is small\n",
                   (Uint) lastsmall);
    exit(EXIT_FAILURE);
  }
  FREESPACE(leafused);
  FREESPACE(branchused);
  fprintf(stdout,"#edgecount %lu identitycount %lu\n",
              (Sint) edgecount,
              (Sint) identitycount);
}

void extractsubtree(Suffixtree *stree,Uint *btptr,sparsetable<Uint> &tNodes)
{
    Uint nodeaddress, suffixlink, depth, headposition, *largeptr, distance; 
  for(Uint i=UintConst(1); i < stree->nodecount; i++)
  {
    nodeaddress = BRADDR2NUM(stree,btptr);
    GETBOTH(depth,headposition,btptr);
    if(ISLARGE(*btptr))
    {
        suffixlink = getlargelinkstree(stree,btptr,depth);
        btptr += LARGEINTS;
    } else
    { 
        suffixlink = nodeaddress + SMALLINTS;
        btptr += SMALLINTS;
    }
    if (nodeaddress < tNodes.size())
        tNodes[nodeaddress] = suffixlink;
    else
        cerr << "# nodeaddress " << nodeaddress << endl;
  }
}

Uint getMaxNodesNumber(Suffixtree *stree)
{
  Uint nodeaddress, suffixlink, depth, headposition, *btptr, *largeptr, distance; 
  btptr = ROOT(stree);
  for(Uint i=UintConst(1); i < stree->nodecount; i++)
  {
    nodeaddress = BRADDR2NUM(stree,btptr);
    GETBOTH(depth,headposition,btptr);
    if(ISLARGE(*btptr))
    {
        suffixlink = getlargelinkstree(stree,btptr,depth);
        btptr += LARGEINTS;
    } else
    { 
        suffixlink = nodeaddress + SMALLINTS;
        btptr += SMALLINTS;
    }
  }
  return nodeaddress;
}

static void showsubtree(Suffixtree *stree,Uint indent,Uint *btptr)
{
  Uint *largeptr, *succptr, leafindex, succdepth, edgelen, succ, distance, 
       depth, headposition; 
  Uchar *leftpointer;

  GETBOTH(depth,headposition,btptr);
  succ = GETCHILD(btptr);
  do 
  {
    fprintf(stderr,"%*.*s",(int) indent,(int) indent,"");
    fprintf(stderr,"\n# D-%lu ", indent+1);
    if(ISLEAF(succ))
    { 
      SHOWINDEX((Uint) succ);
      leafindex = GETLEAFINDEX(succ);
      leftpointer = stree->text + depth + leafindex;
      showthesymbolstring(stderr,stree->sentinel,leftpointer,stree->sentinel);
      //cerr << endl;
      /*fprintf(stdout,"%lu\n",getEdgelength(leftpointer,stree->sentinel));*/
      succ = LEAFBROTHERVAL(stree->leaftab[leafindex]);
    } else
    {
      succptr = stree->branchtab + GETBRANCHINDEX(succ);
      GETBOTH(succdepth,headposition,succptr);
      leftpointer = stree->text + depth + headposition;
      edgelen = succdepth - depth;
      //showthesymbolstring(stderr,stree->sentinel,leftpointer,leftpointer + edgelen - 1);
      //cerr << endl;
      /*fprintf(stdout,"%lu\n",getEdgelength(leftpointer,leftpointer+edgelen-1));*/
      showsubtree(stree,indent+1,succptr);
      succ = GETBROTHER(succptr);
    } 
  } while(!NILPTR(succ));
}

void showstree(Suffixtree *stree)
{
  Uint *btptr, *rcptr, *largeptr, *succptr, succcount, depth, distance, headposition, succ, succdepth, edgelen, i;
  Uchar *leftpointer;

  for(rcptr = stree->rootchildren, i=0; 
      rcptr <= stree->rootchildren + LARGESTCHARINDEX;
      rcptr++, i++)
  {
    succcount = 0;
    if(*rcptr != UNDEFINEDREFERENCE)
    {
      cerr << "# D-1 ";
      if(ISLEAF(*rcptr))
      {
        SHOWINDEX((Uint) BRADDR2NUM(stree,rcptr));
        leftpointer = stree->text + GETLEAFINDEX(*rcptr);
        showthesymbolstring(stderr,stree->sentinel,leftpointer,stree->sentinel);
        //fprintf(stdout,"%lu\n",getEdgelength(leftpointer,stree->sentinel));
        //cerr << endl;
      } else
      {
        btptr = stree->branchtab + GETBRANCHINDEX(*rcptr);
        GETBOTH(succdepth,headposition,btptr);
        leftpointer = stree->text + headposition;
        //showthesymbolstring(stderr,stree->sentinel,leftpointer,leftpointer + succdepth - 1);
        //fprintf(stdout,"%lu\n",getEdgelength(leftpointer,leftpointer+succdepth-1));
        showsubtree(stree,UintConst(1),btptr);
      }
      cerr << endl;
    }
  }
  fprintf(stderr,"# ~\n");
}

void showstate(Suffixtree *stree)
{
  if(stree->headend == NULL)
  {
    printf(" head=%lu of depth %lu\n",
                (Uint) BRADDR2NUM(stree,stree->headnode),
                (Uint) stree->headnodedepth);
  } else
  {
    printf(" head=[%lu,",(Uint) BRADDR2NUM(stree,stree->headnode));
    showthesymbolstring(stdout,stree->sentinel,stree->headstart,stree->headend);
    printf("] where headnode is of depth %lu\n",
                         (Uint) stree->headnodedepth);
  }
  printf(" insertnode=");
  SHOWINDEX(stree->insertnode);
  printf("\n insertprev=");
  SHOWINDEX(stree->insertprev);
  printf("\n smallnode=%lu\n",(Uint) stree->smallnode);
  printf(" largenode=%lu\n",(Uint) stree->largenode);
  printf(" nodecount=%lu\n",(Uint) stree->nodecount);
  if(stree->chainstart == NULL)
  {
    printf(" chainstart=UNDEFINEDREFERENCE\n");
  } else
  {
    printf(" chainstart=%lu\n",(Uint) BRADDR2NUM(stree,stree->chainstart));
  }
  printf(" smallnotcompleted=%lu\n",(Uint) stree->smallnotcompleted);
  printf(" nextfreebranch=%lu\n",
            (Uint) BRADDR2NUM(stree,stree->nextfreebranch));
  printf(" nextfreeleafnum=%lu\n",(Uint) stree->nextfreeleafnum);
  printf(" tail of length (%lu)=",(Uint) (stree->sentinel-stree->tailptr));
  (void) fflush(stdout);
  showthesymbolstring(stdout,stree->sentinel,stree->tailptr,stree->sentinel);
  (void) putchar('\n');
  (void) fflush(stdout);
}

static void loc2stringstree(Suffixtree *stree,Stringtype *s,Location *loc)
{
  Branchinfo branchinfo;

  if(loc->nextnode.toleaf)
  {
    s->start = LEAFADDR2NUM(stree,loc->nextnode.address);
    s->length = stree->textlen - s->start - loc->remain + 1;
  } else
  {
    getbranchinfostree(stree,ACCESSDEPTH | ACCESSHEADPOS,&branchinfo,
                             loc->nextnode.address);
    s->start = branchinfo.headposition;
    s->length = branchinfo.depth - loc->remain;
  }
  if(s->length != loc->locstring.length)
  {
    fprintf(stderr,"s->length=%lu != %lu != loc->locstring.length\n",
                    (Uint) s->length,
                    (Uint) loc->locstring.length);
    exit(EXIT_FAILURE);
  }
  fprintf(stderr,"%s:start=%lu length=%lu\n",__func__,loc->locstring.start,loc->locstring.length);
  if(s->start != loc->locstring.start)
  {
    if(memcmp(stree->text+s->start,
              stree->text+loc->locstring.start,
              (size_t) s->length) != 0)
    {
      fprintf(stderr,"compare of strings failed: \"");
      (void) fwrite(stree->text+s->start,sizeof(Uchar),
                    (size_t) s->length,stderr);
      fprintf(stderr,"\" != \"");
      (void) fwrite(stree->text + loc->locstring.start,sizeof(Uchar),
                    (size_t) loc->locstring.length,stderr);
      fprintf(stderr,"\"\n");
      exit(EXIT_FAILURE);
    }
  }
}

void showlocation(FILE *fp,Suffixtree *stree,Location *loc)
{
  Stringtype lstr;

  fprintf(fp,"\"");
  loc2stringstree(stree,&lstr,loc);

  showthesymbolstring(fp,stree->sentinel,stree->text+lstr.start,
                             stree->text+lstr.start+lstr.length-1);
  fprintf(fp,"\"=(%lu,%lu,",(Uint) loc->locstring.start,
                            (Uint) loc->locstring.length);
  if(loc->remain > 0)
  {
    fprintf(fp,"Branch %lu,",(Uint) BRADDR2NUM(stree,loc->previousnode));
    showthesymbolstring(fp,stree->sentinel,loc->firstptr,
                               loc->firstptr+loc->edgelen-loc->remain-1);
    fprintf(fp,",");
    if(loc->nextnode.toleaf)
    {
      showthesymbolstring(fp,stree->sentinel,loc->firstptr+loc->edgelen-loc->remain,
                                 stree->sentinel);
    } else
    {
      showthesymbolstring(fp,stree->sentinel,loc->firstptr+loc->edgelen-loc->remain,
                                 loc->firstptr+loc->edgelen-1);
    }
    fprintf(fp,",");
  } 
  if(loc->nextnode.toleaf)
  {
    fprintf(fp,"Leaf %lu",
            (Uint) LEAFADDR2NUM(stree,loc->nextnode.address));
  } else
  {
    fprintf(fp,"Branch %lu",(Uint) BRADDR2NUM(stree,loc->nextnode.address));
  }
  fprintf(fp,")");
}

#define SHOWREF(R)\
        ((Uint) ((R).toleaf ? LEAFADDR2NUM(stree,(R).address)\
                                : BRADDR2NUM(stree,(R).address)))

static Sint comparelocs(Suffixtree *stree,Location *loc1,Location *loc2)
{
  if(loc1->remain != loc2->remain)
  {
    fprintf(stderr,"loc1->remain = %lu != %lu = loc2->remain\n",
              (Uint) loc1->remain,
              (Uint) loc2->remain);
    return -1;
  }
  if(loc1->nextnode.toleaf != loc2->nextnode.toleaf)
  {
    fprintf(stderr,"loc1->nextnode.toleaf = %s != %s = loc2->nextnode.toleaf\n",
                    SHOWBOOL(loc1->nextnode.toleaf),
                    SHOWBOOL(loc2->nextnode.toleaf));
    return -1;
  }
  if(loc1->nextnode.address != loc2->nextnode.address)
  {
    fprintf(stderr,"loc1->nextnode.address = %lu !="
                   " %lu = loc2->nextnode.address\n",
                    SHOWREF(loc1->nextnode),
                    SHOWREF(loc2->nextnode));
    return -1;
  }
  if(loc1->remain > 0)
  {
    if(loc1->firstptr != loc2->firstptr)
    {
      fprintf(stderr,"loc1->firstptr = %lu != %lu = loc2->firstptr\n",
                      (Uint) (loc1->firstptr - stree->text),
                      (Uint) (loc2->firstptr - stree->text));
      return -1;
    }
    if(loc1->edgelen != loc2->edgelen)
    {
      fprintf(stderr,"loc1->edgelen = %lu != %lu = loc2->edgelen\n",
                      (Uint) loc1->edgelen,
                      (Uint) loc2->edgelen);
      return -1;
    }
    if(loc1->previousnode != loc2->previousnode)
    {
      fprintf(stderr,"loc1->previousnode = %lu != %lu = loc2->previousnode\n",
                      (Uint) BRADDR2NUM(stree,loc1->previousnode),
                      (Uint) BRADDR2NUM(stree,loc2->previousnode));
      return -1;
    }
  }
  return 0;
}

void checklocation(Suffixtree *stree,Location *loc)
{
  Uint rescanlength;
  Stringtype lstr, llstr;
  Uchar *rest;
  Location scanprefixloc, rescanloc, linklocloc;

  loc2stringstree(stree,&lstr,loc);

  for(rescanlength = 0; rescanlength <= lstr.length; rescanlength++)
  {
    rest = scanprefixfromnodestree(stree,&scanprefixloc,stree->branchtab,
                                   stree->text+lstr.start,
                                   stree->text+lstr.start+lstr.length-1,
                                   rescanlength);
    if(rest != NULL)
    {
      fprintf(stderr,"string of location ");
      showlocation(stderr,stree,loc);
      fprintf(stderr,"\nscanned = ");
      showlocation(stderr,stree,&scanprefixloc);
      fprintf(stderr,"\nnot found\n");
      exit(EXIT_FAILURE);
    } 
    if(comparelocs(stree,loc,&scanprefixloc) == -1)
    {
      fprintf(stderr,"compare loc and scanprefixloc for string \"");
      (void) fwrite(stree->text+lstr.start,sizeof(Uchar),
                    (size_t) lstr.length,stderr);
      fprintf(stderr,"\"\n");
      showlocation(stderr,stree,loc);
      fprintf(stderr,"!=");
      showlocation(stderr,stree,&scanprefixloc);
      fprintf(stderr,"\n");
      exit(EXIT_FAILURE);
    }
  }
  rescanstree(stree,&rescanloc,stree->branchtab,stree->text+lstr.start,
              stree->text+lstr.start+lstr.length-1);
  if(comparelocs(stree,loc,&rescanloc) == -1)
  {
    fprintf(stderr,"compare loc and rescanloc for string \"");
    (void) fwrite(stree->text+lstr.start,sizeof(Uchar),
                  (size_t) lstr.length,stderr);
    fprintf(stderr,"\"\n");
    showlocation(stderr,stree,loc);
    fprintf(stderr,"!=");
    showlocation(stderr,stree,&rescanloc);
    fprintf(stderr,"\n");
    exit(EXIT_FAILURE);
  }
  if(!ROOTLOCATION(loc))
  {
    linklocstree(stree,&linklocloc,loc);
    loc2stringstree(stree,&llstr,&linklocloc);
    if(llstr.length + 1 != lstr.length || 
       memcmp(stree->text+lstr.start+1,stree->text+llstr.start,
              (size_t) llstr.length) != 0)
    {
      fprintf(stderr,"linkloc(");
      (void) fwrite(stree->text+lstr.start,sizeof(Uchar),
                    (size_t) lstr.length,stderr);
      fprintf(stderr,")=");
      (void) fwrite(stree->text+llstr.start,sizeof(Uchar),
                    (size_t) llstr.length,stderr);
      showlocation(stderr,stree,&linklocloc);
      fprintf(stderr," is wrong\n");
      exit(EXIT_FAILURE);
    }
  }
}

static void enumlocationssubtree(Suffixtree *stree,Uint *btptr,
                                  void(*processloc)(Suffixtree *stree,
                                                    Location *))
{
  Location loc;
  Uint leafindex, succ, *largeptr, distance, depth, headposition; 
  Branchinfo branchinfo;
  Sint r;

  GETBOTH(depth,headposition,btptr);
  succ = GETCHILD(btptr);
  do 
  {
    SHOWINDEX(succ);
    if(ISLEAF(succ))
    {
      leafindex = GETLEAFINDEX(succ);
      loc.nextnode.toleaf = true;
      loc.nextnode.address = stree->leaftab + leafindex;
      loc.firstptr = stree->text + depth + leafindex;
      loc.edgelen = stree->textlen - (depth + leafindex) + 1;
      loc.previousnode = btptr;
      loc.locstring.start = leafindex;
      PROCESSALL(1,depth);
      succ = LEAFBROTHERVAL(stree->leaftab[leafindex]);
    } else
    {
      loc.nextnode.toleaf = false;
      loc.nextnode.address = stree->branchtab + GETBRANCHINDEX(succ);
      getbranchinfostree(stree,ACCESSDEPTH | ACCESSHEADPOS,&branchinfo,
                               loc.nextnode.address);
      loc.firstptr = stree->text + depth + branchinfo.headposition;
      loc.edgelen = branchinfo.depth - depth;
      loc.previousnode = btptr;
      loc.locstring.start = branchinfo.headposition;
      PROCESSALL(0,depth);
      enumlocationssubtree(stree,loc.nextnode.address,processloc);
      succ = GETBROTHER(loc.nextnode.address);
    } 
  } while(!NILPTR(succ));
}

void enumlocations(Suffixtree *stree,
                   void(*processloc)(Suffixtree *stree,Location *))
{
  Location loc;
  Uint leafindex, *rcptr;
  Branchinfo branchinfo;
  Sint r;

  loc.remain = 0;
  loc.nextnode.toleaf = false;
  loc.nextnode.address = stree->branchtab;
  loc.locstring.start = loc.locstring.length = 0;
  showlocation(stdout,stree,&loc);
  fprintf(stdout,"\n");
  processloc(stree,&loc);
  for(rcptr = stree->rootchildren; 
      rcptr <= stree->rootchildren + LARGESTCHARINDEX;
      rcptr++)
  {
    if(*rcptr != UNDEFINEDREFERENCE)
    {
      SHOWINDEX(*rcptr);
      if(ISLEAF(*rcptr))
      {
        leafindex = GETLEAFINDEX(*rcptr);
        loc.nextnode.toleaf = true;
        loc.nextnode.address = stree->leaftab + leafindex;
        loc.firstptr = stree->text + leafindex;
        loc.edgelen = stree->textlen - leafindex + 1;
        loc.previousnode = stree->branchtab;
        loc.locstring.start = leafindex;
        PROCESSALL(1,0);
      } else
      {
        loc.nextnode.toleaf = false;
        loc.nextnode.address = stree->branchtab + GETBRANCHINDEX(*rcptr);
        getbranchinfostree(stree,ACCESSDEPTH | ACCESSHEADPOS,&branchinfo,
                                 loc.nextnode.address);
        loc.firstptr = stree->text + branchinfo.headposition;
        loc.edgelen = branchinfo.depth;
        loc.previousnode = stree->branchtab;
        loc.locstring.start = branchinfo.headposition;
        PROCESSALL(0,0);
        enumlocationssubtree(stree,loc.nextnode.address,processloc);
      }
    }
  }
}
