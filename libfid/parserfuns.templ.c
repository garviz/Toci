/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2008  Robert Homann
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

/*!
 * \brief Callback for sequence parsers (#Nextsequence).
 *
 * This function directly writes sequence separators to transformed and
 * original sequence files, and also writes the sequence separator positions.
 *
 * \param seq_position, error See #Nextsequence.
 * \param user_data A pointer to a fid_Sequences structure.
 *
 * \returns 0 on success, -1 on error.
 */
static int next_sequence_to_files(UINT seq_position, void *user_data,
                                  fid_Error *error)/*{{{*/
{
  fid_Sequences *seqs=(fid_Sequences *)user_data;

  assert(seqs->tisfile.content == NULL || (seqs->tisfile.content != NULL && seqs->tisfile.occupied == seq_position));
  fid_MAPPEDFILE_APPEND_TRY(&seqs->sspfile,UINT,seq_position,error,
  {
    fid_error_throw(error,"Could not write sequence separator position "
                    "file \"%s\".",seqs->sspfile.filename);
    return -1;
  });
  fid_MAPPEDFILE_APPEND_TRY(&seqs->tisfile,fid_Symbol,fid_SEPARATOR,error,
  {
    fid_error_throw(error,"Could not write transformed input sequence "
                    "file \"%s\".",seqs->tisfile.filename);
    return -1;
  });
  fid_MAPPEDFILE_APPEND_TRY(&seqs->oisfile,unsigned char,fid_SEPARATOR,error,
  {
    fid_error_throw(error,"Could not write original input sequence "
                    "file \"%s\".",seqs->oisfile.filename);
    return -1;
  });

  return 0;
}
/*}}}*/

/*!
 * \brief Callback for sequence parsers (#Appenddesc).
 *
 * This function writes sequence descriptions and sequence description
 * separators directly to files.
 *
 * \param desc, desc_length, write_separator, error See #Appenddesc.
 * \param user_data A pointer to a fid_Sequences structure.
 *
 * \returns 0 on success, -1 on error.
 */
static int append_desc_to_files(const char *desc, UINT desc_length,
                                int write_separator, void *user_data,
                                fid_Error *error)/*{{{*/
{
  fid_Sequences *seqs=(fid_Sequences *)user_data;

  assert(seqs->sdsfile.content != NULL || desc == NULL);
  if(write_separator)
  {
    fid_MAPPEDFILE_APPEND_GROW(&seqs->sdsfile,UINT,seqs->desfile.occupied,error,
    {
      fid_error_throw(error,"Could not write sequence description "
                      "separator file \"%s\".",seqs->sdsfile.filename);
      return -1;
    });
  }
  if(desc != NULL)
  {
    if(fid_file_ensure_free_space(&seqs->desfile,(size_t)desc_length,
                                  error) == -1)
    {
      fid_error_throw(error,"Could not write description file \"%s\".",
                      seqs->desfile.filename);
      return -1;
    }
    memcpy(&seqs->desfile.content[seqs->desfile.occupied],desc,
            (size_t)desc_length);
    seqs->desfile.occupied+=(size_t)desc_length;
  }
  return 0;
}
/*}}}*/

#include "check32in64.h"

/*!
 * \brief Callback for sequence parsers (#Nextsequence).
 *
 * This function appends sequence separators and sequence separator positions
 * to dynamic arrays.
 *
 * \param seq_position, error See #Nextsequence.
 * \param user_data A pointer to a Sequencearrays structure.
 *
 * \returns 0 on success, -1 on error.
 */
static int next_sequence_to_dynarrays(UINT seq_position, void *user_data,
                                      fid_Error *error)/*{{{*/
{
  Sequencearrays *ptrs=(Sequencearrays *)user_data;
#if defined SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64
  const char *what=NULL;
#endif /* SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64 */

  assert(ptrs->tis.dyndata == NULL || (ptrs->tis.dyndata != NULL && ptrs->tis.occupied == seq_position));
  if(ptrs->ssp.VU.dyndata != NULL)
  {
    fid_DYNARRAY_APPEND(&ptrs->ssp.VU,UINT,fid_MAPPEDFILE_GROWSIZE,seq_position,
                        fid_OUTOFMEM(error);
                        return -1;);
    CHECK_32_IN_64_ENV(ptrs->ssp.VU,"sequence separators (ssp)");
  }
  if(ptrs->tis.dyndata != NULL)
  {
    fid_DYNARRAY_APPEND(&ptrs->tis,fid_Symbol,fid_MAPPEDFILE_GROWSIZE,
                        fid_SEPARATOR,
                        fid_OUTOFMEM(error);
                        return -1;);
    CHECK_32_IN_64_ENV(ptrs->tis,"transformed sequences (tis)");
  }
  if(ptrs->ois.dyndata != NULL)
  {
    fid_DYNARRAY_APPEND(&ptrs->ois,char,fid_MAPPEDFILE_GROWSIZE,fid_SEPARATOR,
                        fid_OUTOFMEM(error);
                        return -1;);
    CHECK_32_IN_64_ENV(ptrs->ois,"sequences (ois)");
  }
  return 0;

#if defined SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64
bail_out:
  fid_error_throw(error,too_large_for_32bits_error,what,ptrs->filename);
  return -1;
#endif /* SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64 */
}
/*}}}*/

/*!
 * \brief Callback for sequence parsers (#Appenddesc).
 *
 * This function appends sequence descriptions and sequence description
 * separators to dynamic arrays.
 *
 * \param desc, desc_length, write_separator, error See #Appenddesc.
 * \param user_data A pointer to a Sequencearrays structure.
 *
 * \returns 0 on success, -1 on error.
 */
static int append_desc_to_dynarrays(const char *desc, UINT desc_length,
                                    int write_separator, void *user_data,
                                    fid_Error *error)/*{{{*/
{
  Sequencearrays *ptrs=(Sequencearrays *)user_data;
#if defined SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64
  const char *what=NULL;
#endif /* SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64 */

  assert(ptrs->sds.VU.dyndata != NULL || desc == NULL);
  if(write_separator)
  {
    fid_DYNARRAY_APPEND(&ptrs->sds.VU,UINT,fid_MAPPEDFILE_GROWSIZE,
                        (UINT)ptrs->des.occupied,
                        fid_OUTOFMEM(error);
                        return -1;);
    CHECK_32_IN_64_ENV(ptrs->sds.VU,"description separators (sds)");
  }
  if(desc != NULL)
  {
    fid_DYNARRAY_ENSURE_NFREE(&ptrs->des,char,(size_t)desc_length,
                              fid_OUTOFMEM(error);
                              return -1;);
    CHECK_32_IN_64_ENV(ptrs->des,"sequence descriptions (des)");
    memcpy(&ptrs->des.dyndata[ptrs->des.occupied],desc,(size_t)desc_length);
    ptrs->des.occupied+=(size_t)desc_length;
  }
  return 0;

#if defined SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64
bail_out:
  fid_error_throw(error,too_large_for_32bits_error,what,ptrs->filename);
  return -1;
#endif /* SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64 */
}
/*}}}*/
