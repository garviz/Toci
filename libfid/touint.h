/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2008  Robert Homann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public   
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */

#ifndef TOUINT_H
#define TOUINT_H
#if fid_WORDSIZE == 32
#define TO_UINT_32(D,V,ERR,ERRCODE)  (D)=(fid_Uint32)(V)
#elif fid_WORDSIZE == 64
#define TO_UINT_32(D,V,ERR,ERRCODE)\
  if((fid_Uint64)(V) > fid_UINT32_MAX)\
  {\
    fid_error_throw(ERR,"Integer " fid_U64FMT " too large.",(fid_Uint64)(V));\
    ERRCODE\
  }\
  else\
  {\
    (D)=(fid_Uint32)(V);\
  }
#endif /* fid_WORDSIZE */

#define TO_UINT_64(D,V,ERR,ERRCODE)  (D)=(fid_Uint64)(V)
#endif /* TOUINT_H */
