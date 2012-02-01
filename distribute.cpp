/*
 * =====================================================================================
 *
 *       Filename:  distribute.cpp
 *
 *    Description:  Split of Suffix Tree horizontally and vertically
 *
 *        Version:  1.0
 *        Created:  27/01/12 22:17:38
 *       Revision:  none
 *       Compiler:  mpic++
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  
 *
 * =====================================================================================
 */
#include <string.h>
#include "types.h"
#include "intbits.h"
#include "visible.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"
#include "spacedef.h"

Uint getEdgelength(Uchar *left,Uchar *right)
{
    return (Uint)(right-left+1);
}

void splitsubstreeH(Suffixtree *stree,Uint *consumption,Uint size,Uint *btptr)
{
  Uint *largeptr, *succptr, leafindex, succdepth, edgelen, succ, distance, 
       depth, headposition; 
  Uchar *leftpointer;

  GETBOTH(depth,headposition,btptr);
  succ = GETCHILD(btptr);
  do 
  {
    printf("%*.*s",(int) 2,(int) 2,"");
    SHOWINDEX(succ);
    if(ISLEAF(succ))
    {
      leafindex = GETLEAFINDEX(succ);
      leftpointer = stree->text + depth + leafindex;
      *consumption+=getEdgelength(leftpointer,stree->sentinel);
      showthesymbolstring(stdout,stree->sentinel,leftpointer,stree->sentinel);
      fprintf(stdout," %lu",getEdgelength(leftpointer,stree->sentinel));
      (void) putchar('\n');
      succ = LEAFBROTHERVAL(stree->leaftab[leafindex]);
      if (*consumption>size) break;
    } else
    {
      succptr = stree->branchtab + GETBRANCHINDEX(succ);
      GETBOTH(succdepth,headposition,succptr);
      leftpointer = stree->text + depth + headposition;
      edgelen = succdepth - depth;
      showthesymbolstring(stdout,stree->sentinel,leftpointer,leftpointer + edgelen - 1);
      fprintf(stdout," %lu",getEdgelength(leftpointer,leftpointer+edgelen-1));
      (void) putchar('\n');
      *consumption+=getEdgelength(leftpointer,leftpointer+edgelen-1);
      if (*consumption>size) break;
      splitsubstreeH(stree,consumption,size,succptr);
      succ = GETBROTHER(succptr);
    } 
  } while(!NILPTR(succ));
}

void splitstreeH(Suffixtree *stree, Uint *consumption, Uint size)
{
  Uint *btptr, *rcptr, *largeptr, *succptr, succcount, depth, distance, headposition, succ, succdepth, edgelen;
  Uchar *leftpointer;

  for(rcptr = stree->rootchildren; 
      rcptr <= stree->rootchildren + LARGESTCHARINDEX;
      rcptr++)
  {
    succcount = 0;
    if(*rcptr != UNDEFINEDREFERENCE)
    {
      SHOWINDEX(*rcptr);
      if(ISLEAF(*rcptr))
      {
        leftpointer = stree->text + GETLEAFINDEX(*rcptr);
        *consumption+=getEdgelength(leftpointer,stree->sentinel);
        showthesymbolstring(stdout,stree->sentinel,leftpointer,stree->sentinel);
        fprintf(stdout," %lu",getEdgelength(leftpointer,stree->sentinel));
        (void) putchar('\n');
      } else
      {
        btptr = stree->branchtab + GETBRANCHINDEX(*rcptr);
        GETBOTH(succdepth,headposition,btptr);
        leftpointer = stree->text + headposition;
        *consumption+=getEdgelength(leftpointer,leftpointer+succdepth-1);
        showthesymbolstring(stdout,stree->sentinel,leftpointer,leftpointer + succdepth - 1);
        fprintf(stdout," %lu",getEdgelength(leftpointer,leftpointer+succdepth-1));
        (void) putchar('\n');
        splitsubstreeH(stree,consumption,size,btptr);
      }
    }
  }
  fprintf(stdout,"Consumption so far: %lu\n", *consumption);
}

