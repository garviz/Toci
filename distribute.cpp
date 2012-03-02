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
    Uchar *s;
    s=example;
    Uint encoded;//, mask=16;

    int len=strlen((char*)example);
/*    for (int i =0; i<len;++i)
            encoded ^= (encoded<<5)+(encoded>>2)+*(example+i);*/
/*    int bits;
    if ( len < 13 ) {
        bits=26-2*len;
        for (int i=31;i>26 && mask>0;i--) {
            if ( len & mask ) {
                printf("1");
                encoded |= (1<<i);
            } else { 
                printf("0");
                encoded &= ~(1<<i);
            }
            mask>>=1;
        }
    }
    else {
        return (Uint) 0;
    }
 */
    for(int i=0, j=wordsize*2; j>=0; i++, j-=2) {
        switch (*(example+i))
        {
            case 'A':
                encoded &= ~(1<<(j+1)); encoded &= ~(1<<j);//00
                printf("00");
                break;
            case 'a': 
                encoded &= ~(1<<(j+1)); encoded &= ~(1<<j);//00
                printf("00");
                break;
            case 'C':
                encoded &= ~(1<<(j+1)); encoded |= (1<<j); //01
                printf("01");
                break;
            case 'c':
                encoded &= ~(1<<(j+1)); encoded |= (1<<j); //01
                printf("01");
                break;
            case 'G':
                encoded |= (1<<(j+1)); encoded &= (1<<j); //10
                printf("10");
                break;
            case 'g':
                encoded |= (1<<(j+1)); encoded &= (1<<j); //10
                printf("10");
                break;
            case 'T':
                encoded |= (1<<(j+1)); encoded |= (1<<j); //11
                printf("11");
                break;
            case 't':
                encoded |= (1<<(j+1)); encoded |= (1<<j); //11
                printf("11");
                break;
         }
   } 
                printf(" %lu ", encoded);
   //return ((*(size_t*)s)>>wordsize)%wordsize;
    return encoded%wordsize;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  splitsubstreeH
 *  Description:  Traverse every branch of suffix tree and it stops when it
 *  reaches the max characters to save in table.
 * =====================================================================================
 */
void splitsubstreeH(Suffixtree *stree, sparsetable<Uint> &table, Uchar *buffer,Uint *btptr, short int wordsize)
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
          //table[(int)(((*(size_t*)buffer)>> (size_t)wordsize)%wordsize)]=btptr;
          std::fprintf(stdout,"%s[%lu]=%lu\n",buffer,encoding(buffer,wordsize),BRADDR2NUM(stree,btptr));
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
          //table[((*(size_t*)buffer)>> (size_t)wordsize)%wordsize]=btptr;
          //std::fprintf(stdout,"table[%s]=%lu\n",buffer,BRADDR2NUM(stree,btptr));
          std::fprintf(stdout,"%s[%lu]=%lu\n",buffer,encoding(buffer,wordsize),BRADDR2NUM(stree,btptr));
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
      fprintf(stderr,"string: %s\n", string);
    }
  }
}

void createTable(Matchprocessinfo *matchprocessinfo, short int wordsize) 
{
    Uint size = pow(4,wordsize);
    //vector <Uint> table(size,0);
    sparsetable<Uint> table(size);
    //google::sparse_hash_map<Uchar*, Uint*, std::tr1::hash<Uchar*>, eqstr> table;
    //table.set_deleted_key(NULL);
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
            splitsubstreeH(&matchprocessinfo->stree,table,buffer,btptr,wordsize);
         }
     }
    
    //for ( Table::iterator it = table.begin(); it != table.end(); ++it)
    //    std::cout <<"Key '" << it->first <<"' NODE " << BRADDR2NUM(&matchprocessinfo->stree,it->second) << endl;

    /*Table::iterator t = table.find(buffer);
    if ( t == table.end() )
        std::cout<<"Not found"<<endl;
    else
        std::cout <<"Entry in map for key '" << t->first <<"' is " << BRADDR2NUM(&matchprocessinfo->stree,t->second) <<"\n";*/
    Location ploc;
    (void) scanprefixfromnodestree (&matchprocessinfo->stree, &ploc, 
          ROOT (&matchprocessinfo->stree), matchprocessinfo->stree.text, matchprocessinfo->stree.text+wordsize,0);
    //string prefix = ploc->firstptr;
    //unsigned long int enc=encoding(prefix);
   //showlocation(stderr,&matchprocessinfo->stree,&ploc);
}
