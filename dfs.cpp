/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#include "arraydef.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"

#define SETCURRENT(V)\
        if(ISLEAF(V))\
        {\
          currentnode.address = stree->leaftab + GETLEAFINDEX(V);\
          currentnode.toleaf = true;\
        } else\
        {\
          currentnode.address = stree->branchtab + GETBRANCHINDEX(V);\
          currentnode.toleaf = false;\
        }

Sint depthfirststree(Suffixtree *stree,Reference *startnode,
                     Sint (*processleaf)(Uint,Bref,void *),
                     bool (*processbranch1)(Bref,void *),
                     Sint (*processbranch2)(Bref,void *),
                     bool (*stoptraversal)(void *),void *stopinfo,void *info)
{
  bool godown = true, readyforpop = false;
  Uint child, brotherval;
  Bref lcpnode = NULL;
  Reference currentnode;
  ArrayBref stack;

  if(startnode->toleaf)
  {
    if(processleaf((Uint) (startnode->address - stree->leaftab),NULL,info) != 0)
    {
      return -1;
    }
    return 0;
  }
  if(stoptraversal != NULL && stoptraversal(stopinfo))
  {
    return 0;
  }

  currentnode.toleaf = false;
  currentnode.address = startnode->address;
  INITARRAY(&stack,Bref);
  STOREINARRAY(&stack,Bref,128,currentnode.address);
  SETCURRENT(GETCHILD(currentnode.address));

  if(processbranch1 == NULL)
  {
#define PROCESSBRANCH1(A,B) /* Nothing */
#define PROCESSBRANCH2(A,B) godown = true
#include "dfs.gen"
  } else
  {
#undef PROCESSBRANCH1
#undef PROCESSBRANCH2
#define PROCESSBRANCH1(A,B) godown = processbranch1(A,B)
#define PROCESSBRANCH2(A,B) if(processbranch2(A,B) != 0)\
                            {\
                              return -2;\
                            }
#include "dfs.gen"
  }
  FREEARRAY(&stack,Bref);
  return 0;
}

static Sint insertinleaflist(Uint leafindex,/*@unused@*/ Bref lcpnode,
                             void *info)
{
  ArrayUint *leaflist = (ArrayUint *) info;

  fprintf(stderr,"insertinleaflist %lu\n",(long unsigned int) leafindex);
  CHECKARRAYSPACE(leaflist,Uint,256);
  leaflist->spaceUint[leaflist->nextfreeUint++] = leafindex;
  return 0;
}

Sint makeleaflist(Suffixtree *stree,ArrayUint *leaflist,Reference *start)
{
  if(depthfirststree(stree,start,insertinleaflist,NULL,NULL,
                     NULL,NULL,(void *) leaflist) != 0)
  {
    return -1;
  }
  return 0;
}
