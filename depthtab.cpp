/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#include <stdio.h>
#include <sys/types.h>
#include "types.h"
#include "streedef.h"
#include "streeacc.h"
#include "spacedef.h"
#include "protodef.h"

#define ADDAMOUNT 128

void showdepthtab(ArrayUint *dt)
{
  Uint i;

  for(i=0; i<dt->nextfreeUint; i++)
  {
    if(dt->spaceUint[i] > 0)
    {
      printf("Depth %lu %lu\n",(Uint) i,(Uint) dt->spaceUint[i]);
    }
  }
}

static void setdepthtab(ArrayUint *depthtab,Uint depth)
{
  Uint i;

  if(depth >= depthtab->allocatedUint)
  {
    depthtab->spaceUint 
      = ALLOCSPACE(depthtab->spaceUint,Uint,depth+ADDAMOUNT);
    for(i= depthtab->allocatedUint; i<depth+ADDAMOUNT; i++)
    {
      depthtab->spaceUint[i] = 0; 
    }
    depthtab->allocatedUint = depth+ADDAMOUNT;
  } 
  if(depth + 1 > depthtab->nextfreeUint)
  {
    depthtab->nextfreeUint = depth+1;
  }
  depthtab->spaceUint[depth]++;
}

void makedepthtabstree(ArrayUint *depthtab,Suffixtree *stree)
{
  Uint depth, headposition, *btptr, *largeptr, distance;
  Uint counter;
  btptr = stree->branchtab; 
  while(btptr < stree->nextfreebranch)
  {
    if(ISLARGE(*btptr))
    {
      depth = GETDEPTH(btptr);
      headposition = GETHEADPOS(btptr);
      //setdepthtab(depthtab,depth);
      fprintf(stderr,"headposition:%lu, depth:%lu\n",headposition,depth);
      btptr += LARGEINTS;
    } else
    {
      distance = GETDISTANCE(btptr);
      GETCHAINEND(largeptr,btptr,distance);
      depth = GETDEPTH(largeptr);
      headposition = GETHEADPOS(largeptr);
      while(distance > 0)
      {
        //setdepthtab(depthtab,depth + distance);
        fprintf(stderr,"headposition:%lu, depth:%lu, distance:%lu\n",headposition,depth,distance);
        distance--;
        btptr += SMALLINTS;
      }
      //setdepthtab(depthtab,depth);
      fprintf(stderr,"headposition:%lu, depth:%lu\n",headposition,depth);
      btptr += LARGEINTS;
    }
  }
}
