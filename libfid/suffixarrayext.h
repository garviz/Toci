/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2006, 2007, 2008  Robert Homann
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

#ifndef SUFFIXARRAYEXT_H
#define SUFFIXARRAYEXT_H
/*!\addtogroup suffixarrays */
/*@{*/
#ifdef __cplusplus
extern "C" {
#endif
int fid_suffixarray_load_from_project(fid_Suffixarray *esa,
                                      fid_Projectfile *project,
                                      fid_Tablerequest tables,
                                      fid_Error *error);
int fid_suffixarray_load_special(fid_Suffixarray *esa, const char *prjbasename,
                                 fid_Tablerequest tables,
                                 fid_Uintsize uisize_request,
                                 int ignore_byteorder, fid_Error *error);
int fid_suffixarray_load(fid_Suffixarray *esa, const char *prjbasename,
                         fid_Tablerequest tables, fid_Error *error);
int fid_projectfile_init_from_suffixarray(fid_Projectfile *project,
                                          const fid_DBfiles *dbfiles,
                                          const char *prjbasename,
                                          const fid_Suffixarray *esa,
                                          fid_Error *error);
#ifdef __cplusplus
}
#endif
/*@}*/

#endif /* !SUFFIXARRAYEXT_H */
