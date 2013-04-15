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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "libdefs.h"
#include "error.h"
#include "fileutils.h"
#include "filereader.h"

#define READ_BUFFERSIZE   (65536UL)
#define WRITE_BUFFERSIZE  (2UL*65536UL)

#ifdef WITH_DECOMPRESSION
#undef WITH_DECOMPRESSION
#endif /* WITH_DECOMPRESSION */

#if defined WITH_ZLIB_SUPPORT || defined WITH_LIBBZ2_SUPPORT || defined WITH_XZ_SUPPORT
#define WITH_DECOMPRESSION
#endif /* !WITH_ZLIB_SUPPORT && !WITH_LIBBZ2_SUPPORT && !WITH_XZ_SUPPORT */

#ifdef WITH_DECOMPRESSION
/*!
 * \brief Check if the file name has a given extension.
 */
static int has_extension(const char *filename, size_t fnamelen,
                         const char *ext, size_t extlen)
{
  return (fnamelen > extlen &&
          memcmp(filename+fnamelen-extlen,ext,extlen) == 0)?1:0;
}
#endif /* !WITH_DECOMPRESSION */

/*!
 * \brief Open compressed or uncompressed file for reading.
 *
 * \internal
 *
 * Note that, unfortunately, \e zlib and \e libbz2, do not throw
 * an error when given an uncompressed file, and there
 * seems to be no documented way to find out whether or not an opened file
 * is compressed. This is a good thing when working with only one of
 * them at a time, but autodetection of compression format is impossible
 * when working with both of them. Hence, the file name extension is pulled
 * in to decide between compression formats and uncompressed input. This is
 * kind of unfortunate, but should not impose big problems in practice
 * since it is common for files to have correct extensions.
 *
 * This function does not attempt to read any data from the file and does not
 * allocate extra memory (besides a private copy of the provided filename),
 * and can therefore also be used to just detect the type of the input file.
 *
 * \note Support for xz compressed files is rather fresh. These, too, are
 * only detected by looking at file name extensions. Nevertheless, it might
 * be possible to detect xz format more reliably since the API used in
 * here is on a lower level.
 *
 * \note Formats gzip and bzip2 might also be detectable more reliably by
 * using the libraries' lower level APIs.
 *
 * \param reader The file reader structure to be associated with the file.
 * \param filename Name of the file to be opened.
 * \param error Error messages go here.
 *
 * \returns 0 on success, -1 on error.
 */
int filereader_open(Filereader *reader, const char *filename, fid_Error *error) /*{{{*/
{
#ifdef WITH_LIBBZ2_SUPPORT
  int err;
#endif /* WITH_LIBBZ2_SUPPORT */
#ifdef WITH_DECOMPRESSION
  size_t fnamelen;
#endif /* WITH_DECOMPRESSION */
 
  assert(reader != NULL);
  assert(filename != NULL);

  reader->buffer=NULL;
  reader->buffersize=0;
  if((reader->filename=strdup(filename)) == NULL)
  {
    fid_OUTOFMEM(error);
    return -1;
  }
 
#ifdef WITH_DECOMPRESSION
  fnamelen=strlen(filename);
#endif /* WITH_DECOMPRESSION */

#ifdef WITH_ZLIB_SUPPORT
  if(has_extension(filename,fnamelen,".gz",3) &&
     (reader->gzfile=gzopen(filename,"r")) != NULL)
  {
    reader->type=FILETYPE_GZIP;
    return 0;
  }
#endif /* WITH_ZLIB_SUPPORT */

#ifdef WITH_LIBBZ2_SUPPORT
  if(has_extension(filename,fnamelen,".bz2",4) &&
     (reader->file=fopen(filename,"r")) != NULL)
  {
    if((reader->bzfile=BZ2_bzReadOpen(&err,reader->file,0,0,NULL,0)) != NULL &&
       err == BZ_OK)
    {
      reader->type=FILETYPE_BZIP2;
      return 0;
    }
    (void)fclose(reader->file);
    reader->file=NULL;
  }
#endif /* WITH_LIBBZ2_SUPPORT */

#ifdef WITH_XZ_SUPPORT
  if(has_extension(filename,fnamelen,".xz",3) &&
     (reader->file=fopen(filename,"r")) != NULL)
  {
    reader->type=FILETYPE_XZ;
    return 0;
  }
#endif /* WITH_XZ_SUPPORT */

  if(fid_file_map(&reader->flatfile,filename,0,0,error) == 0)
  {
    reader->type=FILETYPE_FLAT;
    return 0;
  }

  return -1;
}

/*}}}*/

#ifdef WITH_ZLIB_SUPPORT
/*!
 * \brief Decompress complete gzip'ed file into buffer.
 */
static int read_complete_gzip(Filereader *reader, fid_Error *error)/*{{{*/
{
  int readbytes=WRITE_BUFFERSIZE;
  size_t offset=0;

  while(readbytes == WRITE_BUFFERSIZE)
  {
    reader->buffer=(unsigned char *)realloc(reader->buffer,offset+WRITE_BUFFERSIZE);
    readbytes=gzread(reader->gzfile,reader->buffer+offset,WRITE_BUFFERSIZE);
    offset+=readbytes;
    if(readbytes == -1)
    {
      fid_error_throw(error,"Failed to decompress gzipped file \"%s\".",reader->filename);
      free(reader->buffer);
      reader->buffer=NULL;
      return -1;
    }
  }

  if(offset == 0)
  {
    fid_error_throw(error,"Decompressed 0 bytes from gzipped file \"%s\".",reader->filename);
    free(reader->buffer);
    reader->buffer=NULL;
    return -1;
  }

  reader->buffer=(unsigned char *)realloc(reader->buffer,offset);
  reader->buffersize=offset;
  return 0;
}
/*}}}*/
#endif /* WITH_ZLIB_SUPPORT */

#ifdef WITH_LIBBZ2_SUPPORT
/*!
 * \brief Decompress complete bzip2'ed file into buffer.
 */
static int read_complete_bzip2(Filereader *reader, fid_Error *error)/*{{{*/
{
  size_t offset=0;
  int readbytes;
  int err;

  while(1)
  {
    reader->buffer=(unsigned char *)realloc(reader->buffer,offset+WRITE_BUFFERSIZE);
    readbytes=BZ2_bzRead(&err,reader->bzfile,reader->buffer+offset,WRITE_BUFFERSIZE);
    offset+=readbytes;
    if(err == BZ_STREAM_END)
    {
      break;
    }
    else if(err != BZ_OK)
    {
      fid_error_throw(error,"Failed to decompress bzipped file \"%s\".",reader->filename);
      free(reader->buffer);
      reader->buffer=NULL;
      return -1;
    }
  }

  if(offset == 0)
  {
    fid_error_throw(error,"Decompressed 0 bytes from bzipped file \"%s\".",reader->filename);
    free(reader->buffer);
    reader->buffer=NULL;
    return -1;
  }

  reader->buffer=(unsigned char *)realloc(reader->buffer,offset);
  reader->buffersize=offset;
  return 0;
}
/*}}}*/
#endif /* WITH_LIBBZ2_SUPPORT */

#ifdef WITH_XZ_SUPPORT
/*!
 * \brief Initialize XZ decoder.
 *
 * We need a temporary for initializing the \c lzma_stream structure, so
 * the code is here now, in its own function.
 */
static int init_lzma_dec(lzma_stream *strm)/*{{{*/
{
  lzma_stream temp=LZMA_STREAM_INIT;

  *strm=temp;
  return (lzma_stream_decoder(strm,256UL*1024UL*1024UL,0) == LZMA_OK)?0:-1;
}
/*}}}*/

static int read_complete_xz(Filereader *reader, fid_Error *error)/*{{{*/
{
  size_t offset=0;
  lzma_ret ret;
  int retval=0;
  unsigned char *input_buffer=(unsigned char *)malloc(READ_BUFFERSIZE);

  if(init_lzma_dec(&reader->strm) == -1)
  {
    fid_error_throw(error,"Failed initializing xz decoder.");
    return -1;
  }

  if(input_buffer == NULL)
  {
    fid_OUTOFMEM(error);
    return -1;
  }

  reader->buffer=(unsigned char *)malloc(WRITE_BUFFERSIZE);
  reader->strm.next_out=reader->buffer;
  reader->strm.avail_out=WRITE_BUFFERSIZE;

  reader->strm.avail_in=0;

  for(;;)
  {
    if(reader->strm.avail_in == 0)
    {
      reader->strm.next_in=input_buffer;
      reader->strm.avail_in=fread(input_buffer,1,READ_BUFFERSIZE,
                                  reader->file);

      if(ferror(reader->file))
      {
        fid_error_throw_errno(error,errno,"Failed reading input file \"%s\".",
                              reader->filename);
        goto cleanup_exit;
      }
    }

    ret=lzma_code(&reader->strm,LZMA_RUN);

    if(reader->strm.avail_out == 0 || ret != LZMA_OK)
    {
      offset+=WRITE_BUFFERSIZE;
      reader->buffer=(unsigned char *)realloc(reader->buffer,offset+WRITE_BUFFERSIZE);
      reader->strm.next_out=reader->buffer+offset;
      reader->strm.avail_out=WRITE_BUFFERSIZE;
    }

    if(ret != LZMA_OK)
    {
      switch(ret)
      {
       case LZMA_STREAM_END:
        break;
       case LZMA_MEM_ERROR:
        fid_OUTOFMEM(error);
        goto cleanup_exit;
       case LZMA_MEMLIMIT_ERROR:
        fid_error_throw(error,"Memory limit for xz decompression exceeded.");
        goto cleanup_exit;
       case LZMA_DATA_ERROR:
        fid_error_throw(error,"xz compressed file \"%s\" is corrupt.",
                        reader->filename);
        goto cleanup_exit;
       case LZMA_BUF_ERROR:
        fid_error_throw(error,"Unexpected end of input in file \"%s\".",
                        reader->filename);
        goto cleanup_exit;
       default:
        fid_error_throw(error,"Error while decompressing file \"%s\".",
                        reader->filename);
        goto cleanup_exit;
      }
      break;
    }
  }

  if(0)
  {
cleanup_exit:
    free(reader->buffer);
    reader->buffer=NULL;
    retval=-1;
  }

  if(reader->buffer != NULL)
  {
    reader->buffersize=reader->strm.total_out;
    reader->buffer=(unsigned char *)realloc(reader->buffer,reader->buffersize);
  }

  lzma_end(&reader->strm);
  free(input_buffer);
  return retval;
}
/*}}}*/
#endif /* WITH_XZ_SUPPORT */

/*!
 * \brief Decompress complete file into allocated buffer.
 *
 * \internal
 * 
 * The function reads the entire file and puts it into Filereader::buffer.
 * In case of uncompressed files, the only thing that needs to be done is
 * to set the pointer to the mapped file, so no extra memory is allocated
 * in that case.
 * 
 * \param reader The file to be read.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int filereader_read_complete(Filereader *reader, fid_Error *error) /*{{{*/
{
  assert(reader != NULL);
  assert(reader->buffer == NULL);
#ifdef S_SPLINT_S
  error=NULL;
#endif /* S_SPLINT_S */
  
  switch(reader->type)
  {
#ifdef WITH_ZLIB_SUPPORT
   case FILETYPE_GZIP:
    return read_complete_gzip(reader,error);
#endif /* WITH_ZLIB_SUPPORT */
  
#ifdef WITH_LIBBZ2_SUPPORT
   case FILETYPE_BZIP2:
    return read_complete_bzip2(reader,error);
#endif /* WITH_LIBBZ2_SUPPORT */
    
#ifdef WITH_XZ_SUPPORT
   case FILETYPE_XZ:
    return read_complete_xz(reader,error);
#endif /* WITH_XZ_SUPPORT */

   default:
    assert(reader->flatfile.occupied > 0);
    reader->buffer=reader->flatfile.content;
    reader->buffersize=reader->flatfile.occupied;
    return 0;
  }
}

/*}}}*/

/*!
 * \brief Close compressed or uncompressed file.
 *
 * \internal
 * 
 * All allocated memory is freed by this function.
 * 
 * \param reader The file to be closed.
 */
void filereader_close(Filereader *reader) /*{{{*/
{
#ifdef WITH_LIBBZ2_SUPPORT
  int err;
#endif /* WITH_LIBBZ2_SUPPORT */

  assert(reader != NULL);
  assert(reader->filename != NULL);

  switch(reader->type)
  {
#ifdef WITH_ZLIB_SUPPORT
   case FILETYPE_GZIP:
    assert(reader->gzfile != NULL);
    (void)gzclose(reader->gzfile);
    if(reader->buffer != NULL)
    {
      free(reader->buffer);
    }
    break;
#endif /* WITH_ZLIB_SUPPORT */

#ifdef WITH_LIBBZ2_SUPPORT
   case FILETYPE_BZIP2:
    assert(reader->bzfile != NULL);
    BZ2_bzReadClose(&err,reader->bzfile);
    (void)fclose(reader->file);
    if(reader->buffer != NULL)
    {
      free(reader->buffer);
    }
    break;
#endif /* WITH_LIBBZ2_SUPPORT */

#ifdef WITH_XZ_SUPPORT
   case FILETYPE_XZ:
    (void)fclose(reader->file);
    if(reader->buffer != NULL)
    {
      free(reader->buffer);
    }
    break;
#endif /* WITH_XZ_SUPPORT */

   default:
    fid_file_unmap(&reader->flatfile);
    break;
  }

  free(reader->filename);
  reader->buffer=NULL;
  reader->filename=NULL;
}

/*}}}*/
