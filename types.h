/*
 * =====================================================================================
 *
 *       Filename:  types.h
 *
 *    Description:  Definition of types used in Toci
 *
 *        Version:  1.0
 *        Created:  10/10/11 16:48:22
 *       Revision:  none
 *       Compiler:  mpic++
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx 
 *        Company:  CAOS
 *
 * =====================================================================================
 */

#ifndef TYPES_H
#define TYPES_H
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char Uchar;
typedef unsigned short Ushort;
typedef unsigned long Uint;
typedef const unsigned long UintConst;
typedef signed long Sint;
typedef int (*Qsortcomparefunction)(const void *,const void *);

#define SHOWBOOL(B) ((B) ? "True" : "False")

#define LOGWORDSIZE 6

typedef Uint * Bref;    
typedef Uint * Lref;  

#define DIV2(N)      ((N) >> 1)

struct ListType
{
      Uint start, length;
}; 

typedef ListType Stringtype;

struct PairUint
{
    Uint uint0, uint1;
};

struct ThreeUint
{
      Uint uint0, uint1, uint2;
};
#endif
