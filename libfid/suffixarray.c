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
#include <assert.h>

#ifdef HAVE_MADVISE
#include <sys/types.h>
#include <sys/mman.h>
#endif /* HAVE_MADVISE */

#include "libdefs.h"
#include "error.h"
#include "arrays.h"
#include "alphabet.h"
#include "fileutils.h"
#include "sequences.h"
#include "dbfiles.h"
#include "projectfile.h"
#include "suffixarray.h"
#include "verify.h"
#include "touint.h"

/* Format for printing suffix array table values. */
#if fid_WORDSIZE == 64
#define TABVALFMT          "%6lu"
#else /* fid_WORDSIZE != 64 */
#define TABVALFMT          "%6llu"
#endif /* fid_WORDSIZE == 64 */

/*!
 * \brief Initialize enhanced suffix array structure.
 * 
 * \param esa The structure to be initialized.
 * \param uisize The size of integers in enhanced suffix array tables. This
 * must be fid_UINTSIZE_64 for sequences data longer than 4GB.
 */
void fid_suffixarray_init(fid_Suffixarray *esa, fid_Uintsize uisize) /*{{{*/
{
  assert(esa != NULL);
  fid_sequences_init(&esa->sequences,uisize,&esa->alpha);
  esa->uisize=uisize;
  fid_SWITCH48(uisize,
  {
    esa->suftab.v_uint32=NULL;
    esa->skiptab.v_uint32=NULL;
    esa->stitab.v_uint32=NULL;
    esa->num_of_large_lcps.v_uint32=0;
  },
  {
    esa->suftab.v_uint64=NULL;
    esa->skiptab.v_uint64=NULL;
    esa->stitab.v_uint64=NULL;
    esa->num_of_large_lcps.v_uint64=0;
  });
  esa->suffile.content=NULL;
  esa->lcpfile.content=NULL;
  esa->llvfile.content=NULL;
  esa->skpfile.content=NULL;
  esa->stifile.content=NULL;
}

/*}}}*/

/*!
 * \brief Map file read-only with certain extension.
 * 
 * This function just simplifies the code below.
 * 
 * \param file The mapped file is represented by this structure.
 * \param fnamebuf Buffer for generating file names.
 * \param fileext File name extension, such like "tis". Note that this
 * absolutely \e must consist of three printable zero-terminated characters,
 * and should not include a leading dot.
 * \param may_prefetch If true, then the file may be prefetched.
 * \param error Error messages go here.
 * 
 *  \returns The return code of fid_file_map(), which is 0 on success,
 * and -1 on error.
 */
static int map_file(fid_Mappedfile *file, fid_Filenamebuffer *fnamebuf,
                    const char *fileext, int may_prefetch, fid_Error *error) /*{{{*/
{
  memcpy(fnamebuf->bufptr,fileext,(size_t)4);
  return fid_file_map(file,fnamebuf->buffer,0,may_prefetch,error);
}

/*}}}*/

/*!
 * \brief Map offline tables.
 * 
 * \param esa Tables will be associated with this structure.
 * \param tables Requested tables, offline only.
 * \param fnamebuf Filename buffer, assumed to be initialized and filled.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int map_more_tables(fid_Suffixarray *esa, fid_Tablerequest tables,
                           fid_Filenamebuffer *fnamebuf, fid_Error *error) /*{{{*/
{
  int retcode=0;

  assert(esa != NULL);
  assert(esa->sequences.alpha != NULL);
  assert(fnamebuf != NULL);
  assert(fnamebuf->buffer != NULL);

  if((tables&fid_TABLE_SUF) != 0)
  {
    retcode=map_file(&esa->suffile,fnamebuf,"suf",1,error);
  }

  if(retcode == 0 && (tables&fid_TABLE_LCP) != 0)
  {
    if((retcode=map_file(&esa->lcpfile,fnamebuf,"lcp",1,error)) == 0)
    {
      (void)map_file(&esa->llvfile,fnamebuf,"llv",1,NULL);
    }
  }

  if(retcode == 0 && (tables&fid_TABLE_SKP) != 0)
  {
    retcode=map_file(&esa->skpfile,fnamebuf,"skp",1,error);
  }

  if(retcode == 0 && (tables&fid_TABLE_STI) != 0)
  {
    retcode=map_file(&esa->stifile,fnamebuf,"sti",1,error);
  }

  if(retcode != 0)
  {
    fid_file_unmap(&esa->suffile);
    fid_file_unmap(&esa->lcpfile);
    fid_file_unmap(&esa->llvfile);
    fid_file_unmap(&esa->skpfile);
    fid_file_unmap(&esa->stifile);
  }

  return retcode;
}

/*}}}*/

#ifdef HAVE_MADVISE
/*!
 * \brief Call \c madvise() and set \p advice for the given file if mapped.
 *
 * \param file A pointer to an initialized fid_Mappedfile structure, possibly
 * not referring to a memory mapped file.
 * \param advice The argument for the \c madvise() system call.
 */
static void tune_table(fid_Mappedfile *file, int advice)/*{{{*/
{
#ifdef DEBUG
  int retval;
#endif /* DEBUG */

  if(file->content != NULL)
  {
    /*@-noeffect@*/
#ifdef DEBUG
    retval=
#else /* !DEBUG */
    (void)
#endif /* DEBUG */
    madvise((caddr_t)file->content,file->occupied,advice);
    /*@+noeffect@*/
#ifdef DEBUG
    if(retval == -1)
    {
      abort();
    }
#endif /* DEBUG */
  }
}
/*}}}*/

/*!
 * \brief Try to improve performance by advising the kernel on how to handle
 * the files of an enhanced suffix array.
 *
 * The advice given to the kernel by this function is based on the assumption
 * that all the tables mapped by the enhanced suffix array are actually used.
 * That is, if, for instance, the suffix array and the lcp-table are mapped in
 * addition to the sequence data, then the algorithms that will run later are
 * assumed to make use of them, and to follow an access scheme that is
 * "typical" for the given set of tables.
 *
 * \param esa An enhanced suffix array.
 *
 * \todo The guesses made by this function might go horribly wrong. It should
 * be checked how good the assumptions made by this function generalize in
 * practice, and there should probably be a way to do the tuning not based on
 * guesses by passing in a bit vector of table settings or something like that.
 * It is probably also a good idea to be able to switch off the whole advising,
 * maybe by means of an environment variable.
 */
static void tune_tables(fid_Suffixarray *esa)/*{{{*/
{
  int onlinefiles, suffixarray;

  onlinefiles=(esa->suffile.content == NULL)?MADV_SEQUENTIAL:MADV_RANDOM;
  suffixarray=(esa->skpfile.content == NULL)?MADV_RANDOM:MADV_SEQUENTIAL;

  tune_table(&esa->sequences.tisfile,onlinefiles);
  tune_table(&esa->sequences.oisfile,onlinefiles);
  tune_table(&esa->sequences.sspfile,onlinefiles);
  tune_table(&esa->sequences.desfile,onlinefiles);
  tune_table(&esa->sequences.sdsfile,onlinefiles);

  tune_table(&esa->suffile,suffixarray);
  tune_table(&esa->lcpfile,suffixarray);
  tune_table(&esa->llvfile,MADV_RANDOM);
  tune_table(&esa->skpfile,MADV_SEQUENTIAL);
  tune_table(&esa->stifile,MADV_RANDOM);
}
#endif /* HAVE_MADVISE */
/*}}}*/

/*!
 * \brief Load enhanced suffix array from disk.
 * 
 * On systems where the \c madvise() system call is available, the kernel will
 * be advised how to treat the mapped files in hope of improved performance.
 * It is not necessary to call fid_suffixarray_init() before calling this
 * function.
 * 
 * \param esa The structure to be filled.
 * \param basefilename The base filename of the suffix array tables on disk.
 * \param tables Requested tables, online and offline.
 * \param uisize When loading files from disk, assume that integers stored in
 * files are of this size.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_suffixarray_load_from_files(fid_Suffixarray *esa,
                                    const char *basefilename,
                                    fid_Tablerequest tables,
                                    fid_Uintsize uisize, fid_Error *error) /*{{{*/
{
  fid_Filenamebuffer fnamebuf;
  int retcode;
  
  assert(esa != NULL);
  assert(basefilename != NULL);
  
  VERIFY_REQUEST(tables,error);
  
  if((retcode=fid_filenamebuffer_init(&fnamebuf,basefilename,error)) == 0)
  {
    memcpy(fnamebuf.bufptr,"al1",(size_t)4);
    retcode=fid_alphabet_init_from_specfile(&esa->alpha,fnamebuf.buffer,error);
    
    /* map tables */
    if(retcode == 0)
    {
      fid_suffixarray_init(esa,uisize);
      if((retcode=fid_sequences_map(&esa->sequences,basefilename,
                                    tables&fid_TABLES_ONLINE_MASK,&fnamebuf,
                                    error)) == 0)
      {
        if((retcode=map_more_tables(esa,tables&fid_TABLES_OFFLINE_MASK,
                                    &fnamebuf,error)) == 0)
        {
          retcode=fid_suffixarray_realize(esa,tables,error);
#ifdef HAVE_MADVISE
          if(retcode == 0)
          {
            tune_tables(esa);
          }
#endif /* HAVE_MADVISE */
        }
        else
        {
          fid_sequences_free(&esa->sequences);
        }
      }
    }
    fid_filenamebuffer_free(&fnamebuf);
  }
  
  return retcode;
}

/*}}}*/

/*!
 * \brief Free enhanced suffix array structure.
 * 
 * All associated files will be closed.
 * 
 * \param esa The structure to be freed.
 */
void fid_suffixarray_free(fid_Suffixarray *esa) /*{{{*/
{
  fid_sequences_free(&esa->sequences);
  fid_file_unmap(&esa->suffile);
  fid_file_unmap(&esa->lcpfile);
  fid_file_unmap(&esa->llvfile);
  fid_file_unmap(&esa->skpfile);
  fid_file_unmap(&esa->stifile);
  (void)fid_suffixarray_realize(esa,fid_TABLES_OFFLINE,NULL);
} 

/*}}}*/

#include "sequences.32"
#include "suffixarray.32"
#include "suffixarray.templ.c"

#include "sequences.64"
#include "suffixarray.64"
#include "suffixarray.templ.c"

#include "sequences.undef"
#include "suffixarray.undef"

/*!
 * \brief Update data fields in fid_Suffixarray structure according to mapped
 * files.
 * 
 * All data fields whose correct values can be derived from the sizes of the
 * associated mapped files are updated by this function (e.g.,
 * fid_Sequences::num_of_large_lcps can be derived from the size of the file
 * associated with fid_Sequences::llvfile). No function should set these
 * fields manually, just call this function instead. A table request can be
 * defined to ask the function to look at only some of the files and update
 * only values derived from these. Note that also all online tables in
 * fid_Suffixarray::sequences can be updated if the request \p tables asks
 * for it.
 * 
 * \param esa The structure to be updated.
 * \param tables Update only values derivable from the tables specified in
 * this request. To update all values, pass #fid_TABLES_ALL, or
 * #fid_TABLES_OFFLINE to update only values derivable from the index tables.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_suffixarray_realize(fid_Suffixarray *esa, fid_Tablerequest tables,
                            fid_Error *error) /*{{{*/
{
  VERIFY_REQUEST(tables,error);

  if(fid_sequences_realize(&esa->sequences,tables&fid_TABLES_ONLINE_MASK,
                           error) == -1)
  {
    return -1;
  }
  
  fid_SWITCH48(esa->uisize,
               return suffixarray_realize_32(esa,tables,error);,
               return suffixarray_realize_64(esa,tables,error););
}

/*}}}*/

/*!
 * \brief Show information about an enhanced suffix array.
 * 
 * \param esa The enhanced suffix array structure to be printed out.
 * \param stream An output stream to which the structure is printed. If
 * \c NULL, nothing will be printed.
 */
void fid_suffixarray_dump(const fid_Suffixarray *esa, FILE *stream) /*{{{*/
{
  assert(esa != NULL);
  
  if(stream == NULL)
  {
    return;
  }
  
  fprintf(stream,"----------\n"
          "This index is a %s bit index.\n"
          "Number of sequences in enhanced suffix array: " fid_U64FMT "\n"
          "Number of alphabet symbols/characters: %hu/%hu\n",
          (esa->uisize == fid_UINTSIZE_32)?"32":"64",
          fid_PRINT48(esa->uisize,esa->sequences.num_of_sequences),
          esa->alpha.num_of_syms,esa->alpha.num_of_chars);
  if(&esa->alpha != esa->sequences.alpha)
  {
    fprintf(stream,"*** NOTE: alphabets of enhanced suffix array and "
            "sequences may differ.\n");
  }
  fprintf(stream,"Mapped online files :%s%s%s%s\n",
          esa->sequences.tisfile.content == NULL?"":" TIS",
          esa->sequences.oisfile.content == NULL?"":" OIS",
          esa->sequences.desfile.content == NULL?"":" DES",
          esa->sequences.sspfile.content == NULL?"":" SSP");
  fprintf(stream,"Mapped offline files:%s%s%s%s%s\n",
          esa->suffile.content == NULL?"":" SUF",
          esa->stifile.content == NULL?"":" STI",
          esa->lcpfile.content == NULL?"":" LCP",
          esa->llvfile.content == NULL?"":" LLV",
          esa->skpfile.content == NULL?"":" SKP");
  
  fid_SWITCH48(esa->uisize,
               suffixarray_dump_32(esa,stream);,
               suffixarray_dump_64(esa,stream););

  fprintf(stream,"----------\n");
}

/*}}}*/

/*!
 * \brief Determine number of bits in table request, thus number of tables.
 */
static unsigned int number_of_tables(fid_Tablerequest request)/*{{{*/
{
  fid_Tablerequest tmp=request-((request >> 1)&033333333333)-
                               ((request >> 2)&011111111111);
  return ((tmp+(tmp >> 3))&030707070707)%63;
}
/*}}}*/

/*!
 * \brief Append table names to a string, separated by some other string.
 *
 * \param ptr Pointer to the end of the string the names should be copied to.
 * \param request A table request.
 * \param mask Start ANDing with this mask, shift by one to the left in each
 * step.
 * \param maxval Stop ANDing when \p mask reaches or exceeds this value.
 * \param sep Separator string.
 * \param seplen Number of characters in \p sep.
 * \param not_first Set this to \e false if the destination string is empty, or
 * to \e true otherwise.
 * \param names A table of names.
 *
 * \returns Pointer to the end of the string.
 */
static char *append_names(char *ptr, fid_Tablerequest request,
                          fid_Tablerequest mask, fid_Tablerequest maxval,
                          const char *sep, size_t seplen, int not_first,
                          const char *names[])/*{{{*/
{
  int i;

  for(i=0; mask < maxval; mask<<=1, ++i)
  {
    if((request&mask) != 0)
    {
      if(not_first)
      {
        memcpy(ptr,sep,seplen);
        ptr+=seplen;
      }
      else not_first=1;
      memcpy(ptr,names[i],3);
      ptr+=3;
    }
  }

  return ptr;
}
/*}}}*/

/*!
 * \brief Construct string from table request.
 *
 * \param request A table request.
 * \param sep Separator string that is to be inserted between table names.
 *
 * \returns A newly allocated string containing the names of the tables
 * requested in the given table request. The caller must free the memory when
 * it is not needed anymore.
 *
 * \note This function returns the \c NULL pointer in an out-of-memory
 * condition.
 */
char *fid_tablerequest_to_string(fid_Tablerequest request, const char *sep) /*{{{*/
{
  static const char *online_table_names[]={"TIS","OIS","DES"};
  static const char *offline_table_names[]={"SUF","LCP","SKP","STI"};
  unsigned int n=number_of_tables(request&fid_TABLES_ALL);
  size_t seplen=strlen(sep), len;
  char *string, *ptr;

  if(n > 0) len=seplen*(n-1)+3*n+1;
  else len=1;

  ptr=string=(char *)malloc(len);
  if(string == NULL) return NULL;

  if((request&fid_TABLES_ONLINE) != 0)
  {
    ptr=append_names(ptr,request,fid_TABLE_TIS,fid_TABLES_ONLINE,
                     sep,seplen,0,online_table_names);
  }

  if((request&fid_TABLES_OFFLINE) != 0)
  {
    ptr=append_names(ptr,request,fid_TABLE_SUF,fid_TABLES_OFFLINE,
                     sep,seplen,(ptr == string)?0:1,offline_table_names);
  }

  *ptr='\0';
  assert(ptr == string+seplen*(n-1)+3*n);

  return string;
}

/*}}}*/
