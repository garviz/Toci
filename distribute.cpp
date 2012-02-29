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
#include <cstring>
#include <bitset>
#include <iostream>
#include <string>
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <google/sparse_hash_map>
#include "types.h"
#include "intbits.h"
#include "visible.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"
#include "spacedef.h"
#include "maxmatdef.h"

struct eqstr
{
    bool operator ()(const char *s1, const char *s2) const
    {
        return (s1==s2) || (s1 && s2 && strcmp(1, s2) == 0);
    }
};

Uint getEdgelength(Uchar *left,Uchar *right)
{
    return (Uint)(right-left+1);
}

Uint encoding(Uchar *example) 
{
    Uint encoded=0;
    for (int i=31; i>=0; i--)
        encoded &= ~(1<<i);
    int len=strlen((char*)example);
    int bits=pow(4,len);
    
/*     if ( len < 13 ) {
 *         for (int i=31;i>26;i--) {
 *             if ( len & 0x01 ) {
 *                 *encoded |= (1<<i);
 *             }
 *             else {
 *                 *encoded &= ~(1<<i);
 *             }
 *         }
 *     }
 *     else {
 *         for ( int i=31;i>26 ;i-- )
 *             *encoded |= (1<<i);
 *     }
 */ 
    for(int i=0, j=0; i<len && j<bits; i++, j+=3) {
        switch (*(example+i))
        {
            case 'A':
            case 'a': 
                encoded &= ~(1<<(j+2)); encoded &= ~(1<<(j+1)); encoded |= (1<<j);//001
                break;
            case 'C':
            case 'c':
                encoded &= ~(1<<(j+2)); encoded |= (1<<(j+1)); encoded &= ~(1<<j);//010
                break;
            case 'G':
            case 'g':
                encoded &= ~(1<<(j+2)); encoded |= (1<<(j+1)); encoded |= (1<<j);//011
                break;
            case 'T':
            case 't':
                encoded |= (1<<(j+2)); encoded &= ~(1<<(j+1)); encoded &= ~(1<<j);//100
                break;
            default:
                break;
         }
   } 
/*    for (int i=31;i>=0;i--) {
 *        if (encoded & (1<<i)) {
 *            fprintf(stderr,"1");
 *        } else {
 *            fprintf(stderr,"0");
 *        }
 *    }
 */
   return encoded;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  splitsubstreeH
 *  Description:  Traverse every branch of suffix tree and it stops when it
 *  reaches the max characters to save in table.
 * =====================================================================================
 */
void splitsubstreeH(Suffixtree *stree,Uchar *buffer,Uint *btptr)
{
  Uint *largeptr, *succptr, leafindex, succdepth, edgelen, succ, distance, 
       depth, headposition, *table;
  table=(Uint *)malloc(sizeof(Uint)); 
  Uchar *leftpointer;
  
  std::fprintf(stdout,"table[%lu]=%lu\n",encoding(buffer),BRADDR2NUM(stree,btptr));

  GETBOTH(depth,headposition,btptr);
  succ = GETCHILD(btptr);
  do 
  {
    if(ISLEAF(succ))
    {
      leafindex = GETLEAFINDEX(succ);
      leftpointer = stree->text + depth + leafindex;
      succ = LEAFBROTHERVAL(stree->leaftab[leafindex]);
    } else
    {
      succptr = stree->branchtab + GETBRANCHINDEX(succ);
      GETBOTH(succdepth,headposition,succptr);
      leftpointer = stree->text + depth + headposition;
      edgelen = succdepth - depth;
      //if (std::strlen((const char*)buffer) > (size_t) 6) break;
      std::strncat((char *)buffer,(const char *)leftpointer,edgelen);
      splitsubstreeH(stree,buffer,succptr);
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
    char *string;
    string = (char *)malloc(sizeof(char*));
    succcount = 0;
    if(*rcptr != UNDEFINEDREFERENCE)
    {
      if(ISLEAF(*rcptr))
      {
        leftpointer = stree->text + GETLEAFINDEX(*rcptr);
        *consumption+=getEdgelength(leftpointer,stree->sentinel);
      } else
      {
        btptr = stree->branchtab + GETBRANCHINDEX(*rcptr);
        GETBOTH(succdepth,headposition,btptr);
        leftpointer = stree->text + headposition;
        *consumption+=getEdgelength(leftpointer,leftpointer+succdepth-1);
        //splitsubstreeH(stree,consumption,size,btptr);
      }
      fprintf(stderr,"string: %s\n", string);
    }
  }
}

void createTable(Matchprocessinfo *matchprocessinfo, int wordsize) 
{
    Uint size = pow(4,wordsize);
    vector <unsigned int> table(size,0);
    Uint *largeptr, *btptr, *succptr, *rcptr, i, succdepth, distance, 
         nodeaddress, succ, depth, child, brother, headposition, suffixlink;
    Uint leafindex, edgelen;
    Uchar *leftpointer, *buffer;
    
    buffer = (Uchar *)malloc(sizeof(Uchar *)*wordsize);
    for(rcptr = matchprocessinfo->stree.rootchildren; 
        rcptr <= matchprocessinfo->stree.rootchildren + LARGESTCHARINDEX;rcptr++)
    {
        if(*rcptr != UNDEFINEDREFERENCE)
        {
            buffer[0]=(Uchar) (rcptr - matchprocessinfo->stree.rootchildren);
            buffer[1]='\0';
            btptr = matchprocessinfo->stree.branchtab + GETBRANCHINDEX(*rcptr);
            splitsubstreeH(&matchprocessinfo->stree,buffer,btptr);
        }
    }
    Location ploc;
    (void) scanprefixfromnodestree (&matchprocessinfo->stree, &ploc, 
          ROOT (&matchprocessinfo->stree), matchprocessinfo->stree.text, matchprocessinfo->stree.text+wordsize,0);
    //string prefix = ploc->firstptr;
    //unsigned long int enc=encoding(prefix);
   //showlocation(stderr,&matchprocessinfo->stree,&ploc);
}
