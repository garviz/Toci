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
#ifndef FILEREADER_H
#define FILEREADER_H

#ifdef WITH_ZLIB_SUPPORT
#ifndef S_SPLINT_S
#include <zlib.h>
#else /* S_SPLINT_S */
#undef WITH_ZLIB_SUPPORT
#endif /* !S_SPLINT_S */
#endif /* WITH_ZLIB_SUPPORT */

#ifdef WITH_LIBBZ2_SUPPORT
#include <bzlib.h>
#endif /* WITH_LIBBZ2_SUPPORT */

#ifdef WITH_XZ_SUPPORT
#include <lzma.h>
#endif /* WITH_XZ_SUPPORT */

typedef enum
{
  FILETYPE_FLAT,
  FILETYPE_GZIP,
  FILETYPE_BZIP2,
  FILETYPE_XZ
} Filereadertype;

/*!
 * \brief Control structure for reading files.
 *
 * \internal
 */
typedef struct
{
  Filereadertype type;
  unsigned char *buffer;
  size_t buffersize;
  char *filename;
  
  fid_Mappedfile flatfile;

#ifdef WITH_ZLIB_SUPPORT
  gzFile gzfile;
#endif /* WITH_ZLIB_SUPPORT */

#if defined WITH_LIBBZ2_SUPPORT || defined WITH_XZ_SUPPORT
  FILE *file;
#endif /* !WITH_LIBBZ2_SUPPORT && !WITH_XZ_SUPPORT */

#ifdef WITH_LIBBZ2_SUPPORT
  BZFILE *bzfile;
#endif /* WITH_LIBBZ2_SUPPORT */

#ifdef WITH_XZ_SUPPORT
  lzma_stream strm;
#endif /* WITH_XZ_SUPPORT */
} Filereader;

int filereader_open(Filereader *reader, const char *filename, fid_Error *error);
int filereader_read_complete(Filereader *reader, fid_Error *error);
void filereader_close(Filereader *reader);

#endif /* !FILEREADER_H */
