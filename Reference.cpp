/*
 * =====================================================================================
 *
 *       Filename:  Reference.cpp
 *
 *    Description:  Implementation of Reference class
 *
 *        Version:  1.0
 *        Created:  11/10/11 12:14:41
 *       Revision:  none
 *       Compiler:  mpic++
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  CAOS
 *
 * =====================================================================================
 */

#include "Reference.h"
#include "types.h"

Reference::Reference(bool ctoleaf, Uint *caddress)
{
    toleaf = ctoleaf;
    *address = *caddress;
}

void Reference::SetToLeaf(bool ctoleaf)
{
    toleaf = ctoleaf;
}

void Reference::SetAddress(Uint *caddress)
{
    *address = *caddress;
}
