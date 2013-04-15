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

#ifndef UTILITIES_H
#define UTILITIES_H
/*!\addtogroup misc */
/*@{*/
#ifdef __cplusplus
extern "C" {
#endif
int fid_utils_parse_uint_32(const char *string, fid_Uint32 *ui,
                            fid_Error *error);
int fid_utils_parse_uint_64(const char *string, fid_Uint64 *ui,
                            fid_Error *error);
#ifdef __cplusplus
}
#endif
/*@}*/

#endif /*! UTILITIES_H */
