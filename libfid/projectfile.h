/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2006, 2007, 2008, 2009  Robert Homann
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

#ifndef PROJECTFILE_H
#define PROJECTFILE_H
/*!\addtogroup suffixarrays */
/*@{*/

/*!
 * \brief Representation of a project file.
 *
 * Project files are text files containing information about enhanced suffix
 * arrays that cannot be derived from the binary data themselves.
 * Examples for such information include the integer size (32 or 64 bits), the
 * byte order (little or big endian), or names of original input files.
 *
 * Some of the members in this structures are just there for pure compatibility
 * with the \e mkvtree program.
 */
typedef struct
{
  fid_Uintsize uisize;     /*!<\brief Integer size. */
  char *filename;          /*!<\brief Name of the project file. */
  char *prjbasename;       /*!<\brief Base name, i.e., the name without file
                            * name extension (<code>.prj</code>). */
  unsigned char integer_size_in_bits;  /*!<\brief Integer size in bits. */
  unsigned char integer_size_in_bytes; /*!<\brief Integer size in bytes. */
  int endianess_known:1;   /*!<\brief Set to \e true if endianess is known. */
  int littleendian:1;      /*!<\brief Set to \e true if data is stored in
                            * little endian format. Only valid if
                            * #fid_Projectfile::endianess_known is \e true. */
  fid_DBfiles dbfiles;     /*!<\brief Dynamic array of #fid_DBfileinfo
                            * structures. */
  fid_Uint64 totallength;  /*!<\brief Total length of the sequence. */
  fid_Uint64 num_of_sequences;        /*!<\brief Number of sequences. */
  fid_Uint64 num_of_query_sequences;  /*!<\brief Number of queries. */
  fid_Uint64 prefixlength;      /*!<\brief Prefix length of buckets. */
  fid_Uint64 large_lcp_values;  /*!<\brief Number of large LCP values. */
  fid_Uint64 max_branchdepth;   /*!<\brief Maximum branch depth. */
  fid_Uint64 num_of_specials;   /*!<\brief Number of special characters in the
                                 * sequence data. */
  fid_Uint64 num_of_special_ranges;  /*!<\brief Number of stretches of special
                                      * characters in the sequence data. */
  fid_Uint64 longest;           /*!<\brief Length of longest sequence. */
  fid_Uint64 len_of_special_prefix;  /*!<\brief Number of special characters
                                      * in the beginning of the sequence
                                      * data. */
  fid_Uint64 len_of_special_suffix;  /*!<\brief Number of special characters
                                      * in the end of the sequence data. */
} fid_Projectfile;
#ifdef __cplusplus
extern "C" {
#endif
int fid_projectfile_init(fid_Projectfile *project, const char *prjbasename,
                         fid_Error *error);
int fid_projectfile_parse_from_file(fid_Projectfile *project,
                                    const char *prjbasename, fid_Error *error);
int fid_projectfile_write(const fid_Projectfile *project, fid_Error *error);
void fid_projectfile_free(fid_Projectfile *project);
void fid_projectfile_dump(const fid_Projectfile *project, FILE *stream);
#ifdef __cplusplus
}
#endif
/*@}*/

#endif /* !PROJECTFILE_H */
