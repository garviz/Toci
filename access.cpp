/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#include <cstdio>
#include <cstdlib>
#include <climits>
#include "streedef.h"
#include "streeacc.h"
#include "arraydef.h"
#include "spacedef.h"
#include "protodef.h"

Uint getlargelinkstree(/*@unused@*/ Suffixtree *stree,Bref btptr,Uint depth)
{
  if(depth == UintConst(1))
  {
    return 0;
  }
  return *(btptr+4);
}

static void int2ref(Suffixtree *stree,Reference *ref,Uint i)
{
  if(ISLEAF(i))
  {
    ref->toleaf = true;
    ref->address = stree->leaftab + GETLEAFINDEX(i);
  } else
  {
    ref->toleaf = false;
    ref->address = stree->branchtab + GETBRANCHINDEX(i);
  }
}

void getleafinfostree(Suffixtree *stree,Leafinfo *leafinfo,Lref lptr)
{
  Uint node = LEAFBROTHERVAL(*lptr);

  if(NILPTR(node))
  {
    leafinfo->address = NULL;
  } else
  {
    int2ref(stree,leafinfo,node);
  }
}

void getbranchinfostree(Suffixtree *stree,Uint whichinfo,
                        Branchinfo *branchinfo,Bref btptr)
{
  Uint which = whichinfo, node, distance, *largeptr;

  if(which & ACCESSSUFFIXLINK)
  {
    which |= ACCESSDEPTH;
  }
  if(which & (ACCESSDEPTH | ACCESSHEADPOS))
  {
    if(stree->chainstart != NULL && btptr >= stree->chainstart)
    {
      distance = DIVBYSMALLINTS((Uint) (stree->nextfreebranch - btptr));
      branchinfo->depth = stree->currentdepth + distance;
      branchinfo->headposition = stree->nextfreeleafnum - distance;
    } else
    {
      if(ISLARGE(*btptr))
      {
        if(which & ACCESSDEPTH)
        {
          branchinfo->depth = GETDEPTH(btptr);
        }
        if(which & ACCESSHEADPOS)
        {
          branchinfo->headposition = GETHEADPOS(btptr);
        }
      } else
      {
        distance = GETDISTANCE(btptr);
        GETCHAINEND(largeptr,btptr,distance);
        if(which & ACCESSDEPTH)
        {
          branchinfo->depth = GETDEPTH(largeptr) + distance;
        }
        if(which & ACCESSHEADPOS)
        {
          branchinfo->headposition = GETHEADPOS(largeptr) - distance;
        }
      }
    }
  }
  if(which & ACCESSSUFFIXLINK)
  {
    if((stree->chainstart != NULL && btptr >= stree->chainstart) || 
       !ISLARGE(*btptr))
    {
      branchinfo->suffixlink = btptr + SMALLINTS;
    } else
    {
      branchinfo->suffixlink = stree->branchtab + 
                               getlargelinkstree(stree,btptr,
                                                 branchinfo->depth);
    }
    /*SHOWINDEX((Uint) BRADDR2NUM(stree,btptr));
    fprintf(stdout,"--> ");
    SHOWINDEX((Uint) BRADDR2NUM(stree,branchinfo->suffixlink));
    fprintf(stdout,"\n");*/
  }
  if(which & ACCESSFIRSTCHILD)
  {
    int2ref(stree,&(branchinfo->firstchild),GETCHILD(btptr));
  }
  if(which & ACCESSBRANCHBROTHER)
  {
    node = GETBROTHER(btptr);
    if(NILPTR(node))
    {
      branchinfo->branchbrother.address = NULL;
    } else
    {
      int2ref(stree,&(branchinfo->branchbrother),node);
    }
  }
}

void getheadstringstree(Suffixtree *stree,Stringtype *str)
{
  Branchinfo branchinfo;
  Reference ref;

  if(stree->headend == NULL)
  {
    str->length = stree->headnodedepth;
    if(stree->headnodedepth > 0)
    {
      getbranchinfostree(stree,ACCESSHEADPOS,&branchinfo,stree->headnode);
      str->start = branchinfo.headposition;
    } else
    {
      str->start = 0;
    }
  } else
  {
    str->length = stree->headnodedepth + (Uint) (stree->headend-stree->headstart+1);
    int2ref(stree,&ref,stree->insertnode);
    if(ref.toleaf)
    {
      str->start = LEAFADDR2NUM(stree,ref.address);
    } else
    {
      getbranchinfostree(stree,ACCESSHEADPOS,&branchinfo,ref.address);
      str->start = branchinfo.headposition;
    }
  }
}

Uint getmaxtextlenstree(void)
{
  return MAXTEXTLEN;
}

void showpathstree(Suffixtree *stree,Bref bnode,
                   void (*showchar)(SYMBOL,void *),void *info)
{
  Branchinfo branchinfo;
  Uint i;
  
  getbranchinfostree(stree,ACCESSHEADPOS | ACCESSDEPTH,&branchinfo,bnode);
  for(i = 0; i < branchinfo.depth; i++)
  {
    showchar(stree->text[branchinfo.headposition + i],info);
  }
}

// use the following functions only for the root location.

void rootsucclocationsstree(Suffixtree *stree,ArraySimpleloc *ll)
{
  Uint headpos, leafindex, depth, distance, node, ch, *largeptr, *nodeptr;
  Simpleloc *llptr;

  CHECKARRAYSPACE(ll,Simpleloc,stree->alphasize+1);
  for(ch = 0; ch <= UCHAR_MAX; ch++)
  {
    if((node = stree->rootchildren[ch]) != UNDEFINEDREFERENCE)
    {
      llptr = ll->spaceSimpleloc + ll->nextfreeSimpleloc++;
      if(ISLEAF(node))
      {
        leafindex = GETLEAFINDEX(node);
        llptr->textpos = leafindex;
        llptr->remain = stree->textlen - leafindex;
        llptr->nextnode.toleaf = true;
        llptr->nextnode.address = stree->leaftab + leafindex;
      } else
      {
        nodeptr = stree->branchtab + GETBRANCHINDEX(node);
        GETBOTH(depth,headpos,nodeptr);
        llptr->textpos = headpos;
        llptr->remain = depth - 1;
        llptr->nextnode.toleaf = false;
        llptr->nextnode.address = nodeptr;
      }
    }
  }
}

// use the following functions only for non root location.

void succlocationsstree(Suffixtree *stree,bool nosentinel,Simpleloc *loc,
                        ArraySimpleloc *ll)
{
  Uint succdepth, succ, leafindex, distance, depth, headpos, 
       remain, *succptr, *largeptr, *nodeptr;
  Simpleloc *llptr;

  fprintf(stderr,"succlocationsstree\n");
  ll->nextfreeSimpleloc = 0;
  CHECKARRAYSPACE(ll,Simpleloc,stree->alphasize+1);
  if(loc->remain > 0)
  {
    if(nosentinel && loc->nextnode.toleaf && loc->remain <= UintConst(1))  
    {  // at the end of leaf edge: only a\$ remains
      return;
    } 
    llptr = ll->spaceSimpleloc + ll->nextfreeSimpleloc++;
    llptr->textpos = loc->textpos + 1;
    llptr->remain = loc->remain - 1;
    llptr->nextnode.address = loc->nextnode.address;
    llptr->nextnode.toleaf = loc->nextnode.toleaf;
    return;
  }
  nodeptr = loc->nextnode.address;
  GETONLYDEPTH(depth,nodeptr);
  succ = GETCHILD(nodeptr);
  do                   // traverse the list of successors
  {
    if(ISLEAF(succ))   // successor is leaf
    {
      leafindex = GETLEAFINDEX(succ);
      remain = stree->textlen - (depth + leafindex);
      if(!nosentinel || remain >= UintConst(1))
      {
        llptr = ll->spaceSimpleloc + ll->nextfreeSimpleloc++;
        llptr->remain = remain;
        llptr->textpos = depth + leafindex;
        llptr->nextnode.address = stree->leaftab + leafindex;
        llptr->nextnode.toleaf = true;
      }
      succ = LEAFBROTHERVAL(stree->leaftab[leafindex]);
    } else   // successor is branch node
    {
      succptr = stree->branchtab + GETBRANCHINDEX(succ);
      GETBOTH(succdepth,headpos,succptr);  // get info for branch node
      llptr = ll->spaceSimpleloc + ll->nextfreeSimpleloc++;
      llptr->textpos = depth + headpos;
      llptr->remain = succdepth - depth - 1;
      llptr->nextnode.toleaf = false;
      llptr->nextnode.address = succptr;
      succ = GETBROTHER(succptr);
    }
  } while(!NILPTR(succ));
}
