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
/*#include <google/sparsetable>
#include <google/sparse_hash_map>*/
#include <iostream>
#include "streetyp.h"
#include "maxmatdef.h"

/*using google::sparse_hash_map;
using google::sparsetable;*/

Uint getEdgelength(Uchar *left,Uchar *right);
void splitstreeH(Suffixtree *stree, Uint *consumption, Uint size);
void splitsubstreeH(Suffixtree *stree,/*sparsetable<Uint*>  &table,*/ Uchar *buffer,Uint *btptr, short int wordsize);
Uint encoding(Uchar *example, int wordsize);
void createTable(Suffixtree *stree, /*sparsetable<Uint*> &table,*/Uint wordsize);
void *Safe_realloc  (void * Q, size_t Len);
void *Safe_malloc  (size_t Len);
