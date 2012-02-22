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
#include <bitset>
#include <iostream>
#include <string>
#include "types.h"
#include "intbits.h"
#include "visible.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"
#include "spacedef.h"
#include "maxmatdef.h"

using namespace std;


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
    
unsigned long int encoding(string example) {
    unsigned long int enc_string;
    for (int i=31; i>=0; i--)
        enc_string &= ~(1<<i);

    for(int i=0, j=31; i<16 && j>0; i++, j-=2) {
        switch (example.at(i))
        {
            case 'A': 
                enc_string &= ~(1<<j); enc_string &= ~(1<<(j-1));
                break;
            case 'C':
                enc_string &= ~(1<<j); enc_string |= (1<<(j-1));
                break;
            case 'G':
                enc_string |= (1<<j); enc_string &= ~(1<<(j-1));
                break;
            case 'T':
                enc_string |= (1<<j); enc_string |= (1<<(j-1));
                break;
        }
   }
   for (int i=31;i>=0;i--) {
       if (enc_string & (1<<i)) {
           fprintf(stderr,"1");
       } else {
           fprintf(stderr,"0");
       }
   }
   fprintf(stderr,"\n");
   return enc_string;
}

void createTable(Matchprocessinfo *matchprocessinfo, int wordsize) {
    Location ploc;
    (void) scanprefixfromnodestree (&matchprocessinfo->stree, &ploc, 
          ROOT (&matchprocessinfo->stree), matchprocessinfo->stree.text, matchprocessinfo->stree.text+wordsize,0);
    //string prefix = ploc->firstptr;
    //unsigned long int enc=encoding(prefix);
    showlocation(stderr,&matchprocessinfo->stree,&ploc);
}
