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
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <vector>
#include <iostream>
#include <assert.h>
#include "types.h"
#include "intbits.h"
#include "visible.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"
#include "spacedef.h"
#include "maxmatdef.h"
#include "distribute.h"

Uint encoding(Uchar *example, int wordsize) 
{
    Uint encoded=0;
    for(int i=0, j=wordsize*2-1; j>0; i++, j-=2) 
    {
        switch (*(example+i))
        {
            case 'A':
            case 'a':
                encoded &= ~(1<<(j)); encoded &= ~(1<<(j-1)); //00
                break;
            case 'C':
            case 'c':
                encoded &= ~(1<<(j)); encoded |= (1<<(j-1)); //01
                break;
            case 'G':
            case 'g':
                encoded |= (1<<(j)); encoded &= ~(1<<(j-1)); //10
                break;
            case 'T':
            case 't':
                encoded |= (1<<(j)); encoded |= (1<<(j-1)); //11
                break;
            default:
                break;
         }
   } 
    return encoded;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  splitsubstreeH
 *  Description:  Traverse every branch of suffix tree and it stops when it
 *  reaches the max characters to save in table.
 * =====================================================================================
 */
void fillTable(Suffixtree *stree, Table& table, Uchar *buffer,Uint *btptr, short int wordsize)
{
  Uint *largeptr, *succptr, leafindex, succdepth, succ, depth, headposition, distance, enc;
  Uchar *ptr, *end;
  
  size_t size;
  int i;
  suffix tmp;
  Suffixes s;
  GETBOTH(depth,headposition,btptr);
  succ = GETCHILD(btptr);
  do 
  { 
    size = strlen((const char *)buffer);
    if(ISLEAF(succ))
    {
      leafindex = GETLEAFINDEX(succ);
      if (size < wordsize)
      {
          for (i=size, ptr=stree->text+depth+leafindex; i < wordsize && ptr<=stree->sentinel; ptr++, i++)
              buffer[i]=*ptr;
          buffer[i]='\0';
          tmp.depth=depth;
          tmp.position=leafindex;
          s.push_back(tmp);
          enc = encoding(buffer,wordsize);
          if (table.count(enc))
              table[enc].insert(table[enc].end(),s.begin(),s.end());
          else
              table[enc] = s;
      }
      else
      {
          tmp.depth=depth;
          tmp.position=leafindex;
          s.push_back(tmp);
      }
      buffer[size]='\0';
      succ = LEAFBROTHERVAL(stree->leaftab[leafindex]);
    } else
    {
      succptr = stree->branchtab + GETBRANCHINDEX(succ);
      if (size < wordsize)
      {
          GETBOTH(succdepth,headposition,succptr);
          end=stree->text + depth + headposition + succdepth - depth - 1;
          for (i=size, ptr=stree->text+depth+headposition; i < wordsize &&  ptr<=end; ptr++, i++)
              buffer[i]=*ptr;
          buffer[i]='\0';
      }
      fillTable(stree,table,buffer,succptr,wordsize);
      buffer[size]='\0';
      succ = GETBROTHER(succptr);
    }  
   } while(!NILPTR(succ));
   enc = encoding(buffer,wordsize);
   if (table.count(enc))
       table[enc].insert(table[enc].end(),s.begin(),s.end());
   else
       table[enc] = s;
} 

void createTable(Matchprocessinfo *matchprocessinfo) 
 {
    Uint *largeptr, *btptr, *succptr, *rcptr, i, succdepth, distance, nodeaddress, succ, depth, child, brother, headposition, suffixlink;
    Uint leafindex, edgelen;
    Uchar *leftpointer, *buffer;
    
    buffer = (Uchar *) malloc(matchprocessinfo->prefix*sizeof(Uchar *));
    for(rcptr = matchprocessinfo->stree.rootchildren; 
        rcptr <= matchprocessinfo->stree.rootchildren + LARGESTCHARINDEX;rcptr++)
     { 
        if(*rcptr != UNDEFINEDREFERENCE)
        {
            buffer[0] = (Uchar) (rcptr - matchprocessinfo->stree.rootchildren);
            buffer[1] = '\0';
            btptr = matchprocessinfo->stree.branchtab + GETBRANCHINDEX(*rcptr);
            fillTable(&matchprocessinfo->stree,matchprocessinfo->table,buffer,btptr,matchprocessinfo->prefix);
        }
    } 
}

/* Reallocate memory for  Q  to  Len  bytes and return a
*  pointer to the new memory.  Exit if fail. */
void *  Safe_realloc  (void * Q, size_t Len)
  {
   void  * P;

   P = realloc (Q, Len);
   if  (P == NULL)
   { 
      fprintf (stderr, "# ERROR:  realloc failed, there is not enough memory\n");
      //MPI_Finalize();
   } 

   return  P;
  }  

/* Allocate and return a pointer to  Len  bytes of memory.
*  Exit if fail. */
void *  Safe_malloc  (size_t Len)
  {
   void  * P;

   P = malloc (Len);
   if  (P == NULL)
   {
      fprintf (stderr,"# ERROR:  malloc failed, there is not enough memory\n");
      //MPI_Finalize();
   }

   return  P;
  }


