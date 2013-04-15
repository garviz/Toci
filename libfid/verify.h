/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2006, 2007  Robert Homann
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

#ifndef VERIFY_H
#define VERIFY_H
#define VERIFY_ONLINE_REQUEST(TR,ERR)\
  if(((TR)&~(fid_TABLE_TIS|fid_TABLE_OIS|fid_TABLE_DES)) != 0)\
  {\
    fid_error_throw(ERR,"%s(): unsupported values in online tables request "\
                        "0x" fid_TABREQFMT,__func__,(fid_Tablerequest)(TR));\
    fid_NOTIMPLEMENTED(ERR);\
  }

#define VERIFY_OFFLINE_REQUEST(TR,ERR)\
  if(((TR)&~(fid_TABLE_SUF|fid_TABLE_LCP|fid_TABLE_SKP|fid_TABLE_STI)) != 0)\
  {\
    fid_error_throw(ERR,"%s(): unsupported values in offline tables request "\
                        "0x" fid_TABREQFMT,__func__,(fid_Tablerequest)(TR));\
    fid_NOTIMPLEMENTED(ERR);\
  }

#define VERIFY_REQUEST(TR,ERR)\
  VERIFY_ONLINE_REQUEST((TR)&fid_TABLES_ONLINE_MASK,ERR);\
  VERIFY_OFFLINE_REQUEST((TR)&fid_TABLES_OFFLINE_MASK,ERR)

#endif /* !VERIFY_H */
