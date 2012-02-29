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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "types.h"
#include "intbits.h"
#include "visible.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"
#include "spacedef.h"
#include "maxmatdef.h"

Uint getEdgelength(Uchar *left,Uchar *right)
{
    return (Uint)(right-left+1);
}

Uint encoding(char *example) 
{
    Uint encoded=0;
    for (int i=31; i>=0; i--)
        encoded &= ~(1<<i);
    int len=(int) strlen(example);
    int bits=pow(4,strlen(example));
    
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
    for(int i=0, j=0; i<len && j<bits; i++, j+=2) {
        switch (*(example+i))
        {
            case 'A':
            case 'a': 
                encoded &= ~(1<<j); encoded &= ~(1<<(j+1));
                break;
            case 'C':
            case 'c':
                encoded &= ~(1<<j); encoded |= (1<<(j+1));
                break;
            case 'G':
            case 'g':
                encoded |= (1<<j); encoded &= ~(1<<(j+1));
                break;
            case 'T':
            case 't':
                encoded |= (1<<j); encoded |= (1<<(j+1));
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
      //splitsubstreeH(stree,consumption,size,succptr);
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
            printf("rootchildren[%c]\n",(char) (rcptr - matchprocessinfo->stree.rootchildren));
                 (void) fflush(stdout);
                //strncat((char *)buffer,(const char *)leftpointer,(size_t) (leftpointer + succdepth - 1));
                btptr = matchprocessinfo->stree.branchtab + GETBRANCHINDEX(*rcptr);
                MYGETBOTH(succdepth,headposition,btptr);
                leftpointer = stree->text + headposition;
                for (i=UintConst(1); i < matchprocessinfo->stree.nodecount; i++)
                {
                    nodeaddress = btptr - matchprocessinfo->stree.branchtab;
                    child = GETCHILD(btptr);
                    brother = GETBROTHER(btptr);
                    if (matchprocessinfo->stree.chainstart != NULL && btptr >= matchprocessinfo->stree.chainstart)
                    {
                        distance = 1 + DIVBYSMALLINTS((Uint) (matchprocessinfo->stree.nextfreebranch - btptr));
                        succdepth = matchprocessinfo->stree.currentdepth + distance;
                        headposition = matchprocessinfo->stree.nextfreeleafnum - distance;
                    } else
                    {
                        if(ISLARGE(*btptr))
                        {
                            succdepth = GETDEPTH(btptr);
                            headposition = GETHEADPOS(btptr);
                        } else
                        {
                            distance = GETDISTANCE(btptr);
                            GETCHAINEND(largeptr,btptr,distance);
                            succdepth = GETDEPTH(largeptr) + distance;
                            headposition = GETHEADPOS(largeptr) - distance;
                        }
                    }
                    if (ISLARGE(*btptr))
                        btptr += LARGEINTS;
                    else
                        btptr += SMALLINTS;
                    //showthesymbolstring(stdout,matchprocessinfo->stree.sentinel,matchprocessinfo->stree.text + headposition,
                    //                   matchprocessinfo->stree.text + headposition + depth - 1);
                    succ = child;
                    do 
                    {
                        if (ISLEAF(succ))
                        {
                            leafindex = GETLEAFINDEX(succ);
                            leftpointer = matchprocessinfo->stree.text + depth + leafindex;
                //            showthesymbolstring(stdout,matchprocessinfo->stree.sentinel,leftpointer,matchprocessinfo->stree.sentinel);
                //            printf(",Leaf %lu)",(Uint) leafindex);
                            succ = LEAFBROTHERVAL(matchprocessinfo->stree.leaftab[leafindex]);
                        } else
                        {
                            succptr = matchprocessinfo->stree.branchtab + GETBRANCHINDEX(succ);
                            if (matchprocessinfo->stree.chainstart != NULL && succptr >= matchprocessinfo->stree.chainstart)
                            {
                                distance = 1 + DIVBYSMALLINTS((Uint) (matchprocessinfo->stree.nextfreebranch - succptr));
                                succdepth = matchprocessinfo->stree.currentdepth + distance;
                                headposition = matchprocessinfo->stree.nextfreeleafnum - distance;
                            } else
                            {
                                if (ISLARGE(*succptr))
                                {
                                    succdepth = GETDEPTH(succptr);
                                    headposition = GETHEADPOS(succptr);
                                } else
                                {
                                    distance = GETDISTANCE(succptr);
                                    GETCHAINEND(largeptr,succptr,distance);
                                    succdepth = GETDEPTH(largeptr) + distance;
                                    headposition = GETHEADPOS(largeptr) - distance;
                                }
                            }
                            leftpointer = matchprocessinfo->stree.text + depth + headposition;
                            edgelen = succdepth - depth;
                //            showthesymbolstring(stdout,matchprocessinfo->stree.sentinel,leftpointer,leftpointer + edgelen - 1);
                //            printf(",%s %lu)",ISLARGE(*succptr) ? "Large" : "Small",
                //                   (Uint) GETBRANCHINDEX(succ));
                            succ = GETBROTHER(succptr);
                        }
                    } while(!NILPTR(succ));
                }
        }
    }
    //Location ploc;
    //(void) scanprefixfromnodestree (&matchprocessinfo->stree, &ploc, 
    //      ROOT (&matchprocessinfo->stree), matchprocessinfo->stree.text, matchprocessinfo->stree.text+wordsize,0);
    //string prefix = ploc->firstptr;
    //unsigned long int enc=encoding(prefix);
   //showlocation(stderr,&matchprocessinfo->stree,&ploc);
}
