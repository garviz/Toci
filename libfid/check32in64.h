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

#ifdef CHECK_32_IN_64_ENV
#undef CHECK_32_IN_64_ENV
#endif /* CHECK_32_IN_64_ENV */

#if defined SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64
#define CHECK_32_IN_64_ENV(A,NAME)\
  if((A).occupied > fid_UINT32_MAX)\
  {\
    what=(NAME);\
    goto bail_out;\
  }
#else
#define CHECK_32_IN_64_ENV(A,ERR)
#endif /* SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64 */
