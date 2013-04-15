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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libdefs.h"
#include "error.h"
#include "arrays.h"
#include "dbfiles.h"

void fid_dbfiles_free(fid_DBfiles *dbfiles) /*{{{*/
{
  size_t i;
  
  if(dbfiles == NULL || dbfiles->dyndata == NULL)
  {
    return;
  }
  
  for(i=0; i < dbfiles->occupied; ++i)
  {
    if(dbfiles->dyndata[i].name != NULL)
    {
      free(dbfiles->dyndata[i].name);
    }
  }
  fid_DYNARRAY_FREE(dbfiles,fid_DBfileinfo);
}

/*}}}*/

int fid_dbfiles_append(fid_DBfiles *dbfiles, char *name, fid_Uint64 inputlen,
                       fid_Uint64 seqlen, fid_Error *error) /*{{{*/
{
  fid_DBfileinfo *entry;

  fid_DYNARRAY_ENSURE_NEXT(dbfiles,fid_DBfileinfo,8,
                           fid_OUTOFMEM(error);
                           return -1;);
  
  entry=&dbfiles->dyndata[dbfiles->occupied++];
  entry->name=name;
  entry->inputfile_length=inputlen;
  entry->sequence_length=seqlen;
  return 0;
}

/*}}}*/

int fid_dbfiles_append_copy(fid_DBfiles *dbfiles, char *name,
                            fid_Uint64 inputlen, fid_Uint64 seqlen,
                            fid_Error *error)
{
  char *temp;
  int retval;
  
  assert(name != NULL);
  if((temp=strdup(name)) == NULL)
  {
    fid_OUTOFMEM(error);
    return -1;
  }
  
  if((retval=fid_dbfiles_append(dbfiles,temp,inputlen,seqlen,error)) == 0)
  {
    return 0;
  }
  else
  {
    free(temp);
    return retval;
  }
}

int fid_dbfiles_copy(fid_DBfiles *dest, const fid_DBfiles *src,
                     fid_Error *error) /*{{{*/
{
  size_t i;
  char *temp;
  int retcode=0;
  
  assert(dest != NULL);
  assert(src != NULL);
  
  fid_DYNARRAY_INIT(dest,fid_DBfileinfo);
  for(i=0; retcode == 0 && i < src->occupied; ++i)
  {
    if((temp=strdup(src->dyndata[i].name)) == NULL)
    {
      retcode=-1;
      break;
    }
    
    if((retcode=fid_dbfiles_append(dest,temp,
                                   src->dyndata[i].inputfile_length,
                                   src->dyndata[i].sequence_length,
                                   error)) == -1)
    {
      free(temp);
    }
  }
  return retcode;
}

/*}}}*/

void fid_dbfiles_dump(const fid_DBfiles *dbfiles, FILE *stream) /*{{{*/
{
  size_t i;
  const fid_DBfileinfo *entry;
  
  fprintf(stream,"DUMP %lu dbfiles:\n",(unsigned long)dbfiles->occupied);
  for(i=0; i < dbfiles->occupied; ++i)
  {
    entry=&dbfiles->dyndata[i];
    fprintf(stream,"  \"%s\"\n"
            "   input file size " fid_U64FMT ", sequence size "
            fid_U64FMT "\n",
            entry->name,entry->inputfile_length,entry->sequence_length);
  }
}

/*}}}*/
