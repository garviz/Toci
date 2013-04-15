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

#ifndef DBFILES_H
#define DBFILES_H
#include "arrays.h"
/*!
 * \brief Information about database file.
 *
 * This structure is used in conjunction with #fid_Projectfile. It stores
 * information about the input files that were used to produce the enhanced
 * suffix array project.
 */
typedef struct
{
  char *name;                  /*!<\brief Name of the database file. */
  fid_Uint64 inputfile_length; /*!<\brief Length of the database file. */
  fid_Uint64 sequence_length;  /*!<\brief Length of the sequence portion in
                                * the database file. */
} fid_DBfileinfo;

/*!\brief An array of database file information structures. */
fid_DYNARRAY_DECLARE(fid_DBfiles,fid_DBfileinfo);

void fid_dbfiles_free(fid_DBfiles *dbfiles);
int fid_dbfiles_append(fid_DBfiles *dbfiles, char *name, fid_Uint64 inputlen,
                       fid_Uint64 seqlen, fid_Error *error);
int fid_dbfiles_append_copy(fid_DBfiles *dbfiles, char *name,
                            fid_Uint64 inputlen, fid_Uint64 seqlen,
                            fid_Error *error);
int fid_dbfiles_copy(fid_DBfiles *dest, const fid_DBfiles *src,
                     fid_Error *error);
void fid_dbfiles_dump(const fid_DBfiles *dbfiles, FILE *stream);
#endif /* !DBFILES_H */
