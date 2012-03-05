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
#include <sparsehash/sparsetable>
#include "distribute.h"
#include "types.h"
#include "intbits.h"
#include "visible.h"
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"
#include "spacedef.h"
#include "maxmatdef.h"

//Table table;

using google::sparsetable;

Uint getEdgelength(Uchar *left,Uchar *right)
{
    return (Uint)(right-left+1);
}

Uint encoding(Uchar *example, int wordsize) 
{
    const int len=3*wordsize;
    string binary;
    for (int i=0, j=len-1; i<wordsize; i++, j-=3) 
    {
        switch (*(example+i))
        {
            case 'A':
                binary.append("100");
                break;
            case 'a': 
                binary.append("100");
                break;
            case 'C':
                binary.append("101");
                break;
            case 'c':
                binary.append("101");
                break;
            case 'G':
                binary.append("110");
                break;
            case 'g':
                binary.append("110");
                break;
            case 'T':
                binary.append("111");
                break;
            case 't':
                binary.append("111");
                break;
        }
   } 
   bitset<32> number(binary);
   return (Uint) number.to_ulong();
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  splitsubstreeH
 *  Description:  Traverse every branch of suffix tree and it stops when it
 *  reaches the max characters to save in table.
 * =====================================================================================
 */
void splitsubstreeH(Suffixtree *stree, sparsetable<Uint*> &table, Uchar *buffer,Uint *btptr, short int wordsize)
{
  Uint *largeptr, *succptr, leafindex, succdepth, edgelen, succ, distance, 
       depth, headposition;
  Uchar *leftpointer;
  
  size_t size = std::strlen((const char*)buffer);
  int i;
  GETBOTH(depth,headposition,btptr);
  succ = GETCHILD(btptr);
  do 
  {
    if(ISLEAF(succ))
    {
      leafindex = GETLEAFINDEX(succ);
      leftpointer = stree->text + depth + leafindex;
      Uchar *ptr;
      for (i=0, ptr=leftpointer; ptr<=stree->sentinel; ptr++, i++)
      {
          if (ptr == stree->sentinel) 
          {
              buffer[size+i]='\0';
              break;
           }
          if (ptr > leftpointer + wordsize)
           {
              buffer[size+i]='\0';
              break;
          }
          buffer[size+i]=*ptr;
       }
       if (std::strlen((const char*)buffer) > (size_t) wordsize) {
          table[encoding(buffer,wordsize)]=btptr;
          break;
      }
      succ = LEAFBROTHERVAL(stree->leaftab[leafindex]);
     } else
    {
      succptr = stree->branchtab + GETBRANCHINDEX(succ);
      GETBOTH(succdepth,headposition,succptr);
      leftpointer = stree->text + depth + headposition;
      edgelen = succdepth - depth;
      Uchar *ptr, *end;
      end=leftpointer + edgelen - 1;
      for (i=0, ptr=leftpointer; ptr<=end; ptr++, i++)
       {
          if (ptr == end) 
           {
              buffer[size+i]='\0';
              break;
          }
          if (ptr > leftpointer + wordsize)
           {
              buffer[size+i]='\0';
              break;
          }
          buffer[size+i]=*ptr;
      }
      if (std::strlen((const char*)buffer) > (size_t) wordsize) {
          table[encoding(buffer,wordsize)]=btptr;
          break;
      }
      splitsubstreeH(stree,table,buffer,succptr,wordsize);
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
    }
  }
}

void createTable(Suffixtree *stree, sparsetable<Uint*> &table, Uint wordsize) 
{ 
    //vector <Uint> table(size,0);
    //google::sparse_hash_map<Uchar*, Uint*, std::tr1::hash<Uchar*>, eqstr> table;
    //table.set_deleted_key(NULL);
    Uint *largeptr, *btptr, *succptr, *rcptr, i, succdepth, distance, 
         nodeaddress, succ, depth, child, brother, headposition, suffixlink;
    Uint leafindex, edgelen;
    Uchar *leftpointer, *buffer;
    buffer = (Uchar *)malloc(sizeof(Uchar *)*wordsize);
    for(rcptr = stree->rootchildren; 
        rcptr <= stree->rootchildren + LARGESTCHARINDEX;rcptr++)
    { 
        if(*rcptr != UNDEFINEDREFERENCE)
         {
            buffer[0]=(Uchar) (rcptr - stree->rootchildren);
            buffer[1]='\0';
            btptr = stree->branchtab + GETBRANCHINDEX(*rcptr);
            splitsubstreeH(stree,table,buffer,btptr,wordsize);
         }
     }
}
