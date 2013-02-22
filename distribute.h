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
#ifndef DISTRIBUTE_H
#define DISTRIBUTE_H
#include "streetyp.h"
#include "maxmatdef.h"

void fillTable(Suffixtree *stree,Table& table,Uchar *buffer,Uint *btptr, short int wordsize);
Uint encoding(Uchar *example, int wordsize);
void createTable(Matchprocessinfo *matchprocessinfo);
void *Safe_realloc  (void * Q, size_t Len);
void *Safe_malloc  (size_t Len);

#endif
