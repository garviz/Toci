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
void fillTable(Suffixtree *stree, Table &table, Uchar *buffer,Uint *btptr, short int wordsize)
{
  Uint *largeptr, *succptr, leafindex, succdepth, edgelen, succ, distance, 
       depth, headposition;
  Uchar *leftpointer;
  
  size_t size = std::strlen((const char*)buffer);
  int i;
  vector<Uint> v;
  Suffixes s;
  GETBOTH(depth,headposition,btptr);
  succ = GETCHILD(btptr);
  do 
  {
    if(ISLEAF(succ))
    {
      leafindex = GETLEAFINDEX(succ);
      leftpointer = stree->text + depth + leafindex;
      Uchar *ptr;
      if (size < wordsize)
      {
          for (i=size, ptr=leftpointer; ptr<=stree->sentinel && ptr < leftpointer + wordsize; ptr++, i++)
              buffer[size+i]=*ptr;
          buffer[size+i]='\0';
      } 
      v.push_back(leafindex);
      succ = LEAFBROTHERVAL(stree->leaftab[leafindex]);
    } else
    {
      succptr = stree->branchtab + GETBRANCHINDEX(succ);
      if (size < wordsize)
      {
          GETBOTH(succdepth,headposition,succptr);
          leftpointer = stree->text + depth + headposition;
          edgelen = succdepth - depth;
          Uchar *ptr, *end;
          end=leftpointer + edgelen - 1;
          for (i=size, ptr=leftpointer; ptr<=end && ptr < leftpointer + wordsize; ptr++, i++)
              buffer[i]=*ptr;
          buffer[i]='\0';
      }
      fillTable(stree,table,buffer,succptr,wordsize);
      succ = GETBROTHER(succptr);
    }  
   } while(!NILPTR(succ));
   Uint enc = encoding(buffer,wordsize);
   if (table.count(enc))
   {
       s = table[enc];
       if (s.count(depth))
       {
           s[depth].insert(s[depth].end(),v.begin(),v.end());
           table[enc] = s;
       }
       else
       {
           s[depth] = v;
           table[enc] = s;
       }
   }
   else
   {
       s[depth] = v;
       table[enc] = s;
   }
} 

void createTable(Matchprocessinfo *matchprocessinfo) 
{
    Uint *largeptr, *btptr, *succptr, *rcptr, i, succdepth, distance, nodeaddress, succ, depth, child, brother, headposition, suffixlink;
    Uint leafindex, edgelen;
    Uchar *leftpointer, *buffer;
    buffer = (Uchar *)malloc(sizeof(Uchar *)*matchprocessinfo->prefix);
    for(rcptr = matchprocessinfo->stree.rootchildren; 
        rcptr <= matchprocessinfo->stree.rootchildren + LARGESTCHARINDEX;rcptr++)
    { 
        if(*rcptr != UNDEFINEDREFERENCE)
         {
            buffer[0]=(Uchar) (rcptr - matchprocessinfo->stree.rootchildren);
            buffer[1]='\0';
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


