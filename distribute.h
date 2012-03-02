/*
 * =====================================================================================
 *
 *       Filename:  distribute.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27/01/12 23:03:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  
 *
 * =====================================================================================
 */
#include <sparsehash/sparsetable>
#include <sparsehash/sparse_hash_map>
#include <iostream>
#include "streetyp.h"
#include "maxmatdef.h"

using google::sparse_hash_map;

struct eqstr
{
    bool operator ()(Uchar *s1, Uchar *s2) const
    { 
        return (s1==s2) || (s1 && s2 && strcmp((const char *)s1, (const char *)s2) == 0);
    }
}; 

typedef google::sparse_hash_map<Uchar*, Uint*, std::tr1::hash<Uchar*>, eqstr> Table;
Uint getEdgelength(Uchar *left,Uchar *right);
void splitstreeH(Suffixtree *stree, Uint *consumption, Uint size);
void splitsubstreeH(Suffixtree *stree,const Table &table, Uchar *buffer,Uint *btptr, short int wordsize);
Uint encoding(Uchar *example);
void createTable(Matchprocessinfo *matchprocessinfo, short int wordsize);
