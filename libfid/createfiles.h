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

#ifndef CREATEFILES_H
#define CREATEFILES_H
/*!\addtogroup files */
/*@{*/
#ifdef __cplusplus
extern "C" {
#endif
int fid_create_online_files(fid_Sequences *seqs, const fid_Alphabet *alpha,
                            const char *basefilename, fid_Tablerequest tables,
                            fid_Uintsize uisize, fid_Error *error);
#ifdef __cplusplus
}
#endif
/*@}*/

#endif /* !CREATEFILES_H */
