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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libdefs.h"
#include "error.h"
#include "fileutils.h"
#include "arrays.h"
#include "alphabet.h"
#include "sequences.h"
#include "suffixarray.h"
#include "createfiles.h"
#include "verify.h"

/*!
 * \brief Make file name and create file on disk.
 * 
 * This is a dead simple function just to make the code in the other
 * functions more readable.
 * 
 * \param file The created file is represented by this structure.
 * \param fnamebuf Buffer for generating file names.
 * \param fileext File name extension, such like "tis". Note that this
 * absolutely \e must consist of three printable zero-terminated characters,
 * and should not include a leading dot.
 * \param error Error messages go here.
 * 
 * \returns The return code of fid_file_new(), which is 0 on success,
 * and -1 on error.
 */
static int make_new_file(fid_Mappedfile *file, fid_Filenamebuffer *fnamebuf,
                         const char *fileext, fid_Error *error) /*{{{*/
{
  memcpy(fnamebuf->bufptr,fileext,(size_t)4);
  return fid_file_new(file,fnamebuf->buffer,error);
}

/*}}}*/

/*!
 * \brief Create files according to requested tables.
 *
 * The files will be created writable with an initial size of
 * #fid_MAPPEDFILE_GROWSIZE. Existing files will be overwritten.
 * 
 * \param seqs Reference to the parsed sequences stored on disk in
 * binary form.
 * \param alpha The alphabet that is used to transform the textual data
 * into binary.
 * \param basefilename The base name of the generated tables.
 * \param tables Bitvector of requested tables. Any combination of requests
 * is allowed.
 * \param uisize The size of integers stored on files.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_create_online_files(fid_Sequences *seqs, const fid_Alphabet *alpha,
                            const char *basefilename, fid_Tablerequest tables,
                            fid_Uintsize uisize, fid_Error *error) /*{{{*/
{
  int retcode=0;
  fid_Filenamebuffer fnamebuf;
  
  assert(seqs != NULL);
  assert(alpha != NULL);
  assert(basefilename != NULL);
  assert(tables != fid_TABLE_NONE);
  
  VERIFY_ONLINE_REQUEST(tables,error);
  
  if(fid_filenamebuffer_init(&fnamebuf,basefilename,error) == -1)
  {
    return -1;
  }
  
  fid_sequences_init(seqs,uisize,alpha);
  
  if((tables&fid_TABLE_TIS) != 0)
  {
    if((retcode=make_new_file(&seqs->tisfile,&fnamebuf,"tis",error)) == 0)
    {
      retcode=make_new_file(&seqs->sspfile,&fnamebuf,"ssp",error);
    }
  }
  
  if(retcode == 0 && (tables&fid_TABLE_OIS) != 0)
  {
    if((retcode=make_new_file(&seqs->oisfile,&fnamebuf,"ois",error)) == 0 &&
       seqs->sspfile.content == NULL)
    {
      retcode=make_new_file(&seqs->sspfile,&fnamebuf,"ssp",error);
    }
  }
  
  if(retcode == 0 && (tables&fid_TABLE_DES) != 0)
  {
    if((retcode=make_new_file(&seqs->desfile,&fnamebuf,"des",error)) == 0)
    {
      retcode=make_new_file(&seqs->sdsfile,&fnamebuf,"sds",error);
    }
  }
  
  if(retcode != 0)
  {
    fid_file_cleanup(&seqs->tisfile);
    fid_file_cleanup(&seqs->oisfile);
    fid_file_cleanup(&seqs->sspfile);
    fid_file_cleanup(&seqs->desfile);
    fid_file_cleanup(&seqs->sdsfile);
  }
  
  fid_filenamebuffer_free(&fnamebuf);
  
  return retcode;
}

/*}}}*/
