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
#include "streetyp.h"
#include "maxmatdef.h"

Uint getEdgelength(Uchar *left,Uchar *right);
void splitstreeH(Suffixtree *stree, Uint *consumption, Uint size);
void splitsubstreeH(Suffixtree *stree,Uchar *buffer,Uint *btptr);
Uint encoding(char *example);
void createTable(Matchprocessinfo *matchprocessinfo, int wordsize);
