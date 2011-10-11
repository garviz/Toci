/*
 * =====================================================================================
 *
 *       Filename:  Reference.h
 *
 *    Description:  Reference class
 *
 *        Version:  1.0
 *        Created:  11/10/11 12:14:04
 *       Revision:  none
 *       Compiler:  mpic++
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  CAOS
 *
 * =====================================================================================
 */

#ifndef REFERENCE_H
#define REFERENCE_H

#include "types.h"

class Reference
{
    private: 
        bool toleaf;
        Uint *address;
        
    public:
        Reference(bool ctoleaf, Uint *caddress);
        void SetToLeaf(bool ctoleaf);
        void SetAddress(Uint *caddress);
        bool GetToLeaf() { return toleaf; }
        Uint GetAddress() { return *address; }
};
#endif
