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
#include <ctype.h>
#include <assert.h>

#include "libdefs.h"
#include "error.h"
#include "fileutils.h"
#include "arrays.h"
#include "alphabet.h"
#include "sequences.h"
#include "createfiles.h"
#include "filereader.h"
#include "verify.h"
#include "touint.h"

#define SKIP_CHARS(PTR,MAX,UNDEF)\
  while((PTR) < (MAX) && *(PTR) != (UNDEF))\
  {\
    ++(PTR);\
  }

#define SKIP_GAP(PTR,MAX,UNDEF)\
  while((PTR) < (MAX) && *(PTR) == (UNDEF))\
  {\
    ++(PTR);\
  }

#define WSIZECHECK_32(S)  assert((S)->uisize == fid_UINTSIZE_32)

#define WSIZECHECK_64(S)  assert((S)->uisize == fid_UINTSIZE_64)

/*!
 * \brief Initialize a fid_Mappedfile structure from a dynamic array.
 *
 * A mapped file does not differ very much from an array, so we can use that
 * structure to return sequences parsed to dynamic arrays. Problems only arise
 * when attempting to manipulate the mapped file structure, but not when just
 * reading from it.
 *
 * \param F A fid_Mappedfile structure.
 * \param A A dynamic array.
 * \param TYPE Type of the elements stored in \p A.
 */
#define FAKEFILE_FROM_ARRAY(F,A,TYPE)\
  fid_DYNARRAY_SHRINK(&(A),TYPE,/* nothing */);\
  if((A).occupied > 0)\
  {\
    fid_file_fake(&(F),(A).dyndata,(A).occupied*sizeof(TYPE));\
  }\
  else\
  {\
    (F).content=NULL;\
    (F).occupied=(F).allocated=0;\
  }

/*!
 * \brief Callback function for parsing sequences, called for each character.
 *
 * For each substring from a sequence, a function of this type will be called.
 * The function should handle the string somehow, like writing it to file,
 * possibly transforming it to a string of symbols.
 *
 * \param seq An array of characters as read by the parser. This is, in other
 * words, a string token.
 * \param len The number of characters in \p seq.
 * \param alpha An alphabet for transforming characters to symbols.
 * \param user_data Pointer to any data to be used by the function.
 * \param error Error messages go here.
 *
 * \returns 0 for success, anything else to indicate an error.
 */
typedef int (*Appendline)(const char *seq, size_t len,
                          const fid_Alphabet *alpha,
                          void *user_data, fid_Error *error);

/*!
 * \brief Dynamic array of 32 or 64 bit integers.
 *
 * \see fid_Uint48, fid_SWITCH48(), etc.
 */
typedef union
{
  fid_ArrayUint32 v_uint32;
  fid_ArrayUint64 v_uint64;
} ArrayUint48;

/*!
 * \brief In-memory alternative to fid_Sequences, but simpler.
 *
 * This data structure is used when parsing sequences to memory.
 *
 * \see fid_sequences_parse_from_file_to_memory(),
 * fid_sequences_parse_from_memory_to_memory().
 */
typedef struct
{
  fid_ArraySymbol tis;
  fid_Arraychar ois;
  fid_Arraychar des;
  ArrayUint48 ssp;
  ArrayUint48 sds;
  const char *filename;
} Sequencearrays;

/*!
 * \brief Callback for sequence parsers (#Appendline).
 *
 * This function takes an array of characters, and writes its contents to the
 * transformed or original sequence files (if they are attached to the passed
 * #fid_Sequences structure). Characters are transformed to symbols when
 * writing to the TIS file. Files are resized automatically when necessary.
 *
 * \param seq, len, alpha, error See #Appendline.
 * \param user_data A pointer to a #fid_Sequences structure.
 *
 * \returns 0 on success, -1 on error.
 */
static int append_seq_to_files(const char *seq, size_t len,
                               const fid_Alphabet *alpha,
                               void *user_data, fid_Error *error)/*{{{*/
{
  fid_Sequences *seqs=(fid_Sequences *)user_data;

  if(seqs->tisfile.content != NULL)
  {
    fid_MAPPEDFILE_CHECKSPACE(&seqs->tisfile,len,fid_MAPPEDFILE_GROWSIZE,error,
    {
      fid_error_throw(error,"Could not write transformed input sequence "
                      "file \"%s\".",seqs->tisfile.filename);
                      return -1;
    });
    (void)fid_alphabet_transform_string(alpha,seq,len,
                                        &seqs->tisfile.content[seqs->tisfile.occupied],
                                        0);
    seqs->tisfile.occupied+=len;
  }
  if(seqs->oisfile.content != NULL)
  {
    fid_MAPPEDFILE_CHECKSPACE(&seqs->oisfile,len,fid_MAPPEDFILE_GROWSIZE,error,
    {
      fid_error_throw(error,"Could not write original input sequence "
                      "file \"%s\".",seqs->oisfile.filename);
      return -1;
    });
    memcpy(&seqs->oisfile.content[seqs->oisfile.occupied],seq,len);
    seqs->oisfile.occupied+=len;
  }
  return 0;
}
/*}}}*/

#define SEQUENCES_32BIT_VERSION
#include "check32in64.h"
#undef SEQUENCES_32BIT_VERSION

static const char too_large_for_32bits_error[]=
  "Size of %s portion of file \"%s\" is too large to be represented by "
  "32 bits. Please consider constructing a 64 bit index.";

/*!
 * \brief Callback for sequence parsers (#Appendline).
 *
 * This function takes an array of characters, and appends its contents to
 * dynamic arrays of symbols (transforming characters to symbols), or
 * characters.
 *
 * \param seq, len, alpha, error See #Appendline.
 * \param user_data A pointer to a #Sequencearrays structure.
 *
 * \returns 0 on success, -1 on error.
 */
static int append_seq_to_dynarrays(const char *seq, size_t len,
                                   const fid_Alphabet *alpha,
                                   void *user_data, fid_Error *error)/*{{{*/
{
  Sequencearrays *ptrs=(Sequencearrays *)user_data;
#if fid_WORDSIZE == 64
  const char *what=NULL;
#endif /* fid_WORDSIZE == 64 */

  if(ptrs->tis.dyndata != NULL)
  {
    fid_DYNARRAY_ENSURE_NFREE(&ptrs->tis,fid_Symbol,len,
                             fid_OUTOFMEM(error);
                             return -1;);
    (void)fid_alphabet_transform_string(alpha,seq,len,
                                        &ptrs->tis.dyndata[ptrs->tis.occupied],
                                        0);
    ptrs->tis.occupied+=len;
    CHECK_32_IN_64_ENV(ptrs->tis,"transformed sequences (tis)");
  }
  if(ptrs->ois.dyndata != NULL)
  {
    fid_DYNARRAY_ENSURE_NFREE(&ptrs->ois,char,len,
                              fid_OUTOFMEM(error);
                              return -1;);
    memcpy(&ptrs->ois.dyndata[ptrs->ois.occupied],seq,len);
    ptrs->ois.occupied+=len;
    CHECK_32_IN_64_ENV(ptrs->ois,"sequences (ois)");
  }
  return 0;

#if fid_WORDSIZE == 64
bail_out:
  fid_error_throw(error,too_large_for_32bits_error,what,ptrs->filename);
  return -1;
#endif /* fid_WORDSIZE == 64 */
}
/*}}}*/

/*!
 * \brief Initialize an fid_Sequences structure.
 * 
 * \param seqs The structure to be initialized.
 * \param uisize Required word size. Some fields in fid_Sequences, especially
 * the tables of sequence separator positions and sequence description
 * separator positions, depend on how these data are stored on file.
 * \param alpha The alphabet to be associated with this structure. It is
 * legal to pass a \c NULL pointer.
 */
void fid_sequences_init(fid_Sequences *seqs, fid_Uintsize uisize,
                        const fid_Alphabet *alpha) /*{{{*/
{
  assert(seqs != NULL);
  
  seqs->uisize=uisize;
  fid_SWITCH48(uisize,
  {
    seqs->num_of_sequences.v_uint32=0;
    seqs->total_length.v_uint32=0;
    seqs->descriptions.v_uint32=NULL;
    seqs->separators.v_uint32=NULL;
  },
  {
    seqs->num_of_sequences.v_uint64=0;
    seqs->total_length.v_uint64=0;
    seqs->descriptions.v_uint64=NULL;
    seqs->separators.v_uint64=NULL;
  });
  seqs->alpha=alpha;
  seqs->tisfile.content=NULL;
  seqs->oisfile.content=NULL;
  seqs->sspfile.content=NULL;
  seqs->desfile.content=NULL;
  seqs->sdsfile.content=NULL;
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
 * \brief Map online tables of enhanced suffix array into memory.
 * 
 * \param seqs This structure will be associated with the mapped file.
 * \param basefilename Basename from which filenames corresponding to the
 * mapped files are generated.
 * \param tables requested tables, online bits only.
 * \param fnamebuf Filename buffer for generating filenames. If \c NULL is
 * passed, then a local buffer will be used.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_sequences_map(fid_Sequences *seqs, const char *basefilename,
                      fid_Tablerequest tables, fid_Filenamebuffer *fnamebuf,
                      fid_Error *error) /*{{{*/
{
  int retcode=0, free_buffer;
  fid_Filenamebuffer local_buffer;
  
  assert(seqs != NULL);
  assert(basefilename != NULL);
  
  VERIFY_ONLINE_REQUEST(tables,error);
  
  if((free_buffer=fid_filenamebuffer_init_local(&local_buffer,&fnamebuf,
                                                basefilename,error)) == -1)
  {
    return -1;
  }
  
  if((tables&fid_TABLE_TIS) != 0)
  {
    if((retcode=map_file(&seqs->tisfile,fnamebuf,"tis",1,error)) == 0)
    {
      (void)map_file(&seqs->sspfile,fnamebuf,"ssp",1,NULL);
    }
  }
  
  if(retcode == 0 && (tables&fid_TABLE_OIS) != 0)
  {
    if((retcode=map_file(&seqs->oisfile,fnamebuf,"ois",1,error)) == 0 &&
       seqs->sspfile.content == NULL)
    {
      (void)map_file(&seqs->sspfile,fnamebuf,"ssp",1,NULL);
    }
  }
  
  if(retcode == 0 && (tables&fid_TABLE_DES) != 0)
  {
    if((retcode=map_file(&seqs->desfile,fnamebuf,"des",1,error)) == 0)
    {
      retcode=map_file(&seqs->sdsfile,fnamebuf,"sds",1,error);
    }
  }
  
  if(retcode == 0)
  {
    retcode=fid_sequences_realize(seqs,tables,error);
  }
  
  if(retcode != 0)
  {
    fid_file_unmap(&seqs->tisfile);
    fid_file_unmap(&seqs->oisfile);
    fid_file_unmap(&seqs->sspfile);
    fid_file_unmap(&seqs->desfile);
    fid_file_unmap(&seqs->sdsfile);
  }
  
  if(free_buffer)
  {
    fid_filenamebuffer_free(fnamebuf);
  }
  
  return retcode;
}

/*}}}*/

/*!
 * \brief Append some more padding symbols.
 *
 * This function only works on dynamic arrays.
 *
 * \param arrays Bunch of dynamic arrays.
 * \param padding The number of padding bytes to append to both, the parsed
 * transformed and the original sequence. The function does nothing if this
 * value is set to 0.
 * \param error Errors messages go here.
 *
 * \returns 0 on success, -1 on error.
 */
static int append_padding(Sequencearrays *arrays, size_t padding,
                          fid_Error *error)
{
  if(padding == 0)
  {
    return 0;
  }

  if(arrays->tis.dyndata != NULL)
  {
    fid_DYNARRAY_ENSURE_NFREE(&arrays->tis,fid_Symbol,padding,
                              fid_OUTOFMEM(error);
                              return -1;);
    memset(&arrays->tis.dyndata[arrays->tis.occupied],fid_SEPARATOR,padding);
    arrays->tis.occupied+=padding;
  }
  if(arrays->ois.dyndata != NULL)
  {
    fid_DYNARRAY_ENSURE_NFREE(&arrays->ois,char,padding,
                              fid_OUTOFMEM(error);
                              return -1;);
    memset(&arrays->ois.dyndata[arrays->ois.occupied],fid_SEPARATOR,padding);
    arrays->ois.occupied+=padding;
  }
  return 0;
}

#include "parserfuns.32"
#include "sequences.32"
#define SEQUENCES_32BIT_VERSION
#include "parserfuns.templ.c"
#include "sequences.templ.c"
#undef SEQUENCES_32BIT_VERSION

#include "parserfuns.64"
#include "sequences.64"
#include "parserfuns.templ.c"
#include "sequences.templ.c"

#include "sequences.undef"
#include "parserfuns.undef"

/*!
 * \brief Update data fields in fid_Sequences structure according to mapped
 * files.
 * 
 * All data fields whose correct values can be derived from the sizes of the
 * associated mapped files are updated by this function (e.g.,
 * fid_Sequences::num_of_sequences can be derived from the size of the file
 * associated with fid_Sequences::sspfile). No function should set these
 * fields manually, just call this function instead. A table request can be
 * defined to ask the function to look at only some of the files and update
 * only values derived from these.
 * 
 * \param seqs The structure to be updated.
 * \param tables Update only values derivable from the tables specified in
 * this request. To update all values, pass #fid_TABLES_ONLINE.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_sequences_realize(fid_Sequences *seqs, fid_Tablerequest tables,
                          fid_Error *error) /*{{{*/
{
  assert(seqs != NULL);
  
  VERIFY_ONLINE_REQUEST(tables,error);
  
  if((tables&(fid_TABLE_TIS|fid_TABLE_OIS)) == (fid_TABLE_TIS|fid_TABLE_OIS) &&
      seqs->tisfile.content != NULL && seqs->oisfile.content != NULL &&
      seqs->tisfile.occupied != seqs->oisfile.occupied)
  {
    fid_error_throw(error,"Sizes of transformed input sequence and "
                    "original input sequences differ, %lu vs %lu.",
                    (unsigned long)seqs->tisfile.occupied,
                    (unsigned long)seqs->oisfile.occupied);
    return -1;
  }

  fid_SWITCH48(seqs->uisize,
               return sequences_realize_32(seqs,tables,error);,
               return sequences_realize_64(seqs,tables,error););
}

/*}}}*/

/*!
 * \brief Peek into the given buffer to guess the file format.
 * 
 * \param buffer Buffer that contains textual sequence data.
 * \param bufsize Size of \p buffer in bytes.
 * 
 * \returns The file format indicated by a value from #fid_Fileformat.
 * If none of the supported formats have been recognized, return
 * #fid_Fileformat::fid_FORMAT_UNDEF.
 */
static fid_Fileformat guess_file_type(const char *buffer,
                                      size_t bufsize) /*{{{*/
{
  fid_Fileformat format=fid_FORMAT_UNDEF;
  
  if(bufsize > (size_t)2)
  {
    if(buffer[0] == '>')
    {
      format=fid_FORMAT_FASTA;
    }
    else if(memcmp(buffer,"ID ",3) == 0)
    {
      format=fid_FORMAT_UNIPROT;
    }
  }
  
  return format;
}

/*}}}*/

/*!
 * \brief Compute character distribution of given sequences.
 *
 * The fid_Sequences::distribution will be filled by this function, such
 * that each entry \em s contains to the relative frequency of symbol \em s.
 *
 * The length of the sequence is corrected by the number of sequence
 * separators present in the input sequence, so these will not be taken
 * into account. Note that the entry for wildcards will be filled twice,
 * once at index #fid_WILDCARD, and once at the first index after the
 * last normal symbol. The entries for separators and undefined characters
 * will both be set to 0.0.
 *
 * Note that fid_suffixarray_compute_distribution() is faster than this
 * function, especially on large datasets.
 *
 * \param seqs The sequences whose character distribution should be determined.
 */
void fid_sequences_compute_distribution(fid_Sequences *seqs)/*{{{*/
{
  const unsigned char *ptr, *maxptr;
  double dbsize;
  fid_Uint64 histo[UCHAR_MAX+1];
  fid_Symbol sym, wcsym;

  assert(seqs != NULL);
  assert(seqs->alpha != NULL);
  assert(seqs->tisfile.content != NULL);
  assert(seqs->tisfile.occupied > 0);

  memset(histo,0,sizeof(histo));
  ptr=seqs->tisfile.content;
  maxptr=ptr+seqs->tisfile.occupied;
  while(ptr < maxptr)
  {
    ++histo[(size_t)(*ptr++)];
  }

  memset(seqs->distribution,0,sizeof(seqs->distribution));
  fid_SWITCH48(seqs->uisize,
               dbsize=(double)(seqs->tisfile.occupied+1-
                               seqs->num_of_sequences.v_uint32);,
               dbsize=(double)(seqs->tisfile.occupied+1-
                               seqs->num_of_sequences.v_uint64););
  wcsym=seqs->alpha->num_of_syms-1;
  for(sym=0; sym < wcsym; ++sym)
  {
    seqs->distribution[(size_t)sym]=(double)histo[(size_t)sym]/dbsize;
  }
  seqs->distribution[(size_t)wcsym]=(double)histo[(size_t)fid_WILDCARD]/dbsize;
  seqs->distribution[(size_t)fid_WILDCARD]=seqs->distribution[(size_t)wcsym];
}
/*}}}*/

/*!
 * \brief Unmap sequences and free memory.
 * 
 * \param seqs The structure to be freed.
 */
void fid_sequences_free(fid_Sequences *seqs) /*{{{*/
{
  fid_file_unmap(&seqs->tisfile);
  fid_file_unmap(&seqs->oisfile);
  fid_file_unmap(&seqs->sspfile);
  fid_file_unmap(&seqs->desfile);
  fid_file_unmap(&seqs->sdsfile);
  (void)fid_sequences_realize(seqs,fid_TABLES_ONLINE,NULL);
}

/*}}}*/

/*!
 * \brief Parse sequence set from the given file, write output to files.
 * 
 * For a more descent description of this function, see
 * #fid_sequences_parse_from_memory_to_file() since this function depends
 * on that function.
 * 
 * \param infilename The name of a file that contains biological data in
 * some textual format such like FASTA.
 * \param format The predetermined file format. If auto-detection should be
 * tried, pass #fid_Fileformat::fid_FORMAT_UNDEF.
 * \param tables Bitvector representing a request which tables are to be
 * generated. This value should be the result of OR'ing one or more of
 * #fid_TABLE_TIS, #fid_TABLE_OIS, and #fid_TABLE_DES. Other requests are
 * ignored and do not generate an error.
 * \param basefilename The base name of the generated tables. This should be
 * a simple string suitable to generate a file name from. Suffixes ".tis",
 * ".ois", ".sds", ".des", and ".ssp"are appended to this string to generate
 * the final file names of the tables.
 * \param alpha The alphabet that is used to transform the textual data
 * into binary.
 * \param uisize Sequence separator positions and description separator
 * positions are stored as integers of this size.
 * \param input_file_size If not \c NULL, then the total size of the input
 * file \p infilename is returned in this argument.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_sequences_parse_from_file_to_file(const char *infilename,
                                          fid_Fileformat format,
                                          fid_Tablerequest tables,
                                          const char *basefilename,
                                          const fid_Alphabet *alpha,
                                          fid_Uintsize uisize,
                                          size_t *input_file_size,
                                          fid_Error *error) /*{{{*/
{
  Filereader infile;
  int retcode;
  
  assert(infilename != NULL);
  assert(basefilename != NULL);
  assert(alpha != NULL);
  
  VERIFY_ONLINE_REQUEST(tables,error);
  
  if((retcode=filereader_open(&infile,infilename,error)) == 0)
  {
    if((retcode=filereader_read_complete(&infile,error)) == 0)
    {
      retcode=
        fid_sequences_parse_from_memory_to_file(infilename,
                                                (const char *)infile.buffer,
                                                infile.buffersize,format,
                                                tables,basefilename,alpha,
                                                uisize,error);
      if(retcode == 0 && input_file_size != NULL)
      {
        *input_file_size=infile.buffersize;
      }
    }
    filereader_close(&infile);
  }
  return retcode;
}

/*}}}*/

/*!
 * \brief Parse sequence set from the given file to memory.
 * 
 * For a more descent description of this function, see
 * #fid_sequences_parse_from_memory_to_memory() since this function depends
 * on that function.
 * 
 * \param infilename The name of a file that contains biological data in
 * some textual format such like FASTA.
 * \param format The predetermined file format. If auto-detection should be
 * tried, pass #fid_Fileformat::fid_FORMAT_UNDEF.
 * \param tables Bitvector representing a request which tables are to be
 * generated. This value should be the result of OR'ing one or more of
 * #fid_TABLE_TIS, #fid_TABLE_OIS, and #fid_TABLE_DES. Other requests are
 * ignored and do not generate an error.
 * \param seqs The data structure that stores the parsed tables.
 * \param alpha The alphabet that is used to transform the textual data
 * into binary.
 * \param padding Number of symbols to append to the transformed sequence data.
 * This is useful for algorithms that can be simplified under the assumption
 * that the sequence data extends a certain number of symbols beyond the actual
 * end of sequence. The symbols appended to the end are #fid_SEPARATOR symbols.
 * The value of \p padding will not be reflected in the lengths stored in
 * \p seq.
 * \param uisize Sequence separator positions and description separator
 * positions are stored as integers of this size.
 * \param input_file_size If not \c NULL, then the total size of the input
 * file \p infilename is returned in this argument.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 *
 * \todo This function is at least 95% identical to
 * fid_sequences_parse_from_file_to_file(), so we should refactor this.
 */
int fid_sequences_parse_from_file_to_memory(const char *infilename,
                                            fid_Fileformat format,
                                            fid_Tablerequest tables,
                                            fid_Sequences *seqs,
                                            const fid_Alphabet *alpha,
                                            size_t padding, fid_Uintsize uisize,
                                            size_t *input_file_size,
                                            fid_Error *error) /*{{{*/
{
  Filereader infile;
  int retcode;
  
  assert(infilename != NULL);
  assert(seqs != NULL);
  assert(alpha != NULL);
  
  VERIFY_ONLINE_REQUEST(tables,error);
  
  if((retcode=filereader_open(&infile,infilename,error)) == 0)
  {
    if((retcode=filereader_read_complete(&infile,error)) == 0)
    {
      retcode=
        fid_sequences_parse_from_memory_to_memory(infilename,
                                                  (const char *)infile.buffer,
                                                  infile.buffersize,format,
                                                  tables,seqs,alpha,
                                                  padding,uisize,error);
      if(retcode == 0 && input_file_size != NULL)
      {
        *input_file_size=infile.buffersize;
      }
    }
    filereader_close(&infile);
  }
  return retcode;
}
/*}}}*/

/*!
 * \brief Parse sequence set from given file, but only gather statistics.
 *
 * For a more descent description of this function, see
 * #fid_sequences_parse_from_memory_to_lengths() since this function depends
 * on that function.
 *
 * \param infilename The name of a file that contains biological data in
 * some textual format such like FASTA.
 * \param format The predetermined file format. If auto-detection should be
 * tried, pass #fid_Fileformat::fid_FORMAT_UNDEF.
 * \param seqinfo The sizes of the transformed sequence data, descriptions,
 * etc., are returned here.
 * \param alpha The alphabet that is used to transform the textual data
 * into binary.
 * \param error Error messages go here.
 *
 * \returns 0 on success, -1 on error.
 *
 * \todo This function is at least 95% identical to
 * fid_sequences_parse_from_file_to_memory(), so we should refactor this.
 */
int fid_sequences_parse_from_file_to_lengths(const char *infilename,
                                             fid_Fileformat format,
                                             fid_Sequencefileinfo *seqinfo,
                                             const fid_Alphabet *alpha,
                                             fid_Error *error)/*{{{*/
{
  Filereader infile;
  int retcode;
  
  assert(infilename != NULL);
  assert(alpha != NULL);
  
  if((retcode=filereader_open(&infile,infilename,error)) == 0)
  {
    if((retcode=filereader_read_complete(&infile,error)) == 0)
    {
      retcode=
        fid_sequences_parse_from_memory_to_lengths(infilename,
                                                  (const char *)infile.buffer,
                                                  infile.buffersize,format,
                                                  seqinfo,alpha,error);
    }
    filereader_close(&infile);
  }
  return retcode;
}
/*}}}*/

/*!
 * \brief Auto-detect input format if requested.
 *
 * The name of this function might be a bit misleading. Let's not care about
 * this...
 *
 * \param infilename Input file name for error messages.
 * \param buffer, bufsize Input file content.
 * \param format The format guessed from input.
 * \param tables The tables requested by the caller.
 * \param error Error messages go here.
 *
 * \retval 0 on success
 * \retval -1 on error (could not guess file format)
 * \retval -2 if there is nothing to parse (no tables requested)
 */
static int generic_parser_init(const char *infilename, const char *buffer,
                               size_t bufsize, fid_Fileformat *format,
                               fid_Tablerequest tables, fid_Error *error)/*{{{*/
{
  assert(buffer != NULL);
  assert(*format >= fid_FORMAT_UNDEF && *format <= fid_FORMAT_FASTA);

  VERIFY_ONLINE_REQUEST(tables,error);
  
  if((tables&fid_TABLES_ONLINE) == fid_TABLE_NONE || bufsize == 0)
  {
    /* no error, but don't try to parse anything */
    return -2;
  }
  
  if(*format == fid_FORMAT_UNDEF)
  {
    *format=guess_file_type(buffer,bufsize);
  }
  if(*format == fid_FORMAT_UNDEF)
  {
    fid_error_throw(error,"Cannot read file \"%s\", unrecognized format.",
                    infilename);
    return -1;
  }

  return 0;
}
/*}}}*/

/*!
 * \brief Parse sequence set from the provided buffer, write output to files.
 * 
 * This function reads the content of \p buffer that contains a set of
 * biological sequences in some supported format, and writes it into a more
 * convenient, binary form as a set of tables. No index generation takes
 * place here, this is only the preprocessing of online data. The input
 * file format is attempted to be auto-detected, but can also be
 * predetermined. A memory mapped file provides sufficient input to this
 * function. The output is a set of files on disk corresponding to the
 * entities parsed from the input.
 * 
 * Internally, the set of sequences is parsed into an fid_Sequences
 * structure, whose file members fid_Sequences::tisfile,
 * fid_Sequences::oisfile, fid_Sequences::sspfile, fid_Sequences::desfile,
 * and fid_Sequences::sdsfile,(depending on the tables request) are used to
 * construct the corresponding table. That is, this function does not
 * allocate RAM, but solely relies on memory mapped files. It creates
 * growing files directly on disk rather than taking the risk to be swapped
 * out by the OS. When the function returns, all files will be closed
 * again. To operate on the preprocessed data (e.g., to construct an
 * enhanced suffix array), map it into memory again.
 *
 * \note Resizing of files on NFS-mounted volumes can be slow, depending on the
 * NFS implementation, so parsing to memory first via
 * fid_sequences_parse_from_file_to_memory() or
 * fid_sequences_parse_from_memory_to_memory(), and then writing the result to
 * file via fid_file_dump_to_file() might be more advantageous under certain
 * circumstances.
 * 
 * \param infilename The name of the input file (for error messages only).
 * \param buffer The buffer that contains biological data in some textual
 * format such like FASTA.
 * \param bufsize The size of \p buffer in bytes. By this, \p buffer is not
 * required to be zero-terminated. This value should be positive.
 * \param format The predetermined file format. If auto-detection should be
 * tried, pass #fid_Fileformat::fid_FORMAT_UNDEF.
 * \param tables Bitvector representing a request which tables are to be
 * generated. This value should be the result of OR'ing one or more of
 * #fid_TABLE_TIS, #fid_TABLE_OIS, and #fid_TABLE_DES. Other requests are
 * ignored and do not generate an error.
 * \param basefilename The base name of the generated tables. This should be
 * a simple string suitable to generate a file name from. Suffixes ".tis",
 * ".ois", ".ssp", ".des", and ".sds" are appended to this string to generate
 * the final file names of the tables.
 * \param alpha The alphabet that is used to transform the textual data
 * into binary.
 * \param uisize Sequence separator positions and description separator
 * positions are stored as integers of this size.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_sequences_parse_from_memory_to_file(const char *infilename,
                                            const char *buffer, size_t bufsize,
                                            fid_Fileformat format,
                                            fid_Tablerequest tables,
                                            const char *basefilename,
                                            const fid_Alphabet *alpha,
                                            fid_Uintsize uisize,
                                            fid_Error *error) /*{{{*/
{
  fid_Sequences seqs;
  Parserfuns_32 funs32;
  Parserfuns_64 funs64;
  int retcode;
  
  assert(basefilename != NULL);
  assert(alpha != NULL);
  
  if((retcode=generic_parser_init(infilename,buffer,bufsize,&format,tables,
                                  error)) < 0)
  {
    return (retcode == -2)?0:-1;
  }
  
  if((retcode=fid_create_online_files(&seqs,alpha,basefilename,
                                      tables&fid_TABLES_ONLINE,uisize,
                                      error)) == 0)
  {
    fid_SWITCH48(uisize,
    {
      seqs.num_of_sequences.v_uint32=0;
      funs32.appendlinefun=append_seq_to_files;
      funs32.appenddescfun=(tables&fid_TABLE_DES)?append_desc_to_files_32:NULL;
      funs32.nextseqfun=next_sequence_to_files_32;
      retcode=parse_sequences_32(infilename,buffer,bufsize,format,alpha,
                                 &funs32,&seqs,error);
    },
    {
      seqs.num_of_sequences.v_uint64=0;
      funs64.appendlinefun=append_seq_to_files;
      funs64.appenddescfun=(tables&fid_TABLE_DES)?append_desc_to_files_64:NULL;
      funs64.nextseqfun=next_sequence_to_files_64;
      retcode=parse_sequences_64(infilename,buffer,bufsize,format,alpha,
                                 &funs64,&seqs,error);
    });
    
    fid_file_unmap(&seqs.tisfile);
    fid_file_unmap(&seqs.oisfile);
    fid_file_unmap(&seqs.sspfile);
    fid_file_unmap(&seqs.desfile);
    fid_file_unmap(&seqs.sdsfile);
  }
  
  return retcode;
}

/*}}}*/

/*!
 * \brief Parse sequence set from the provided buffer to memory.
 * 
 * This function reads the content of \p buffer that contains a set of
 * biological sequences in some supported format, and writes it into a more
 * convenient, binary form as a set of tables. No index generation takes
 * place here, this is only the preprocessing of online data. The input
 * file format is attempted to be auto-detected, but can also be
 * predetermined. A memory mapped file provides sufficient input to this
 * function. The output of this function is binary identical to the output of
 * fid_sequences_parse_from_memory_to_file(), only that it is not written to
 * files directly.
 *
 * The output is a set of dynamic arrays corresponding to the entities parsed
 * from the input, stored in a fid_Sequences structure, \p seqs. The
 * fid_Mappedfile::content pointers of the files represented by \p seqs are
 * pointers to allocated memory, so they are not to be unmapped directly. The
 * sequences can be flushed to files after parsing, and must be freed via
 * fid_sequences_free() when they are not needed anymore.
 *
 * \param infilename The name of the input file (for error messages only).
 * \param buffer The buffer that contains biological data in some textual
 * format such like FASTA.
 * \param bufsize The size of \p buffer in bytes. By this, \p buffer is not
 * required to be zero-terminated. This value should be positive.
 * \param format The predetermined file format. If auto-detection should be
 * tried, pass #fid_Fileformat::fid_FORMAT_UNDEF.
 * \param tables Bitvector representing a request which tables are to be
 * generated. This value should be the result of OR'ing one or more of
 * #fid_TABLE_TIS, #fid_TABLE_OIS, and #fid_TABLE_DES. Other requests are
 * ignored and do not generate an error.
 * \param seqs The parsed sequences.
 * \param alpha The alphabet that is used to transform the textual data
 * into binary.
 * \param padding Number of symbols to append to the transformed sequence data.
 * This is useful for algorithms that can be simplified under the assumption
 * that the sequence data extends a certain number of symbols beyond the actual
 * end of sequence. The symbols appended to the end are #fid_SEPARATOR symbols.
 * The value of \p padding will not be reflected in the lengths stored in
 * \p seq.
 * \param uisize Sequence separator positions and description separator
 * positions are stored as integers of this size.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_sequences_parse_from_memory_to_memory(const char *infilename,
                                              const char *buffer,
                                              size_t bufsize,
                                              fid_Fileformat format,
                                              fid_Tablerequest tables,
                                              fid_Sequences *seqs,
                                              const fid_Alphabet *alpha,
                                              size_t padding,
                                              fid_Uintsize uisize,
                                              fid_Error *error)/*{{{*/
{
  int retcode;
  
  assert(alpha != NULL);
  
  if((retcode=generic_parser_init(infilename,buffer,bufsize,&format,tables,
                                  error)) < 0)
  {
    return (retcode == -2)?0:-1;
  }

  fid_SWITCH48(uisize,
  {
    retcode=parse_from_mem_to_mem_32(infilename,buffer,bufsize,format,tables,
                                     seqs,alpha,padding,error);
  },
  {
    retcode=parse_from_mem_to_mem_64(infilename,buffer,bufsize,format,tables,
                                     seqs,alpha,padding,error);
  });
  return retcode;
}
/*}}}*/

/*!
 * \brief Callback for sequence parsers (#Appenddesc).
 *
 * Count total description length.
 */
static int count_desc(/*@unused@*/ const char *desc, fid_Uint64 desc_length,
                      /*@unused@*/ int write_separator, void *user_data,
                      /*@unused@*/ fid_Error *error)/*{{{*/
{
  ((fid_Sequencefileinfo *)user_data)->desclen+=desc_length;
  return 0;
}
/*}}}*/

/*!
 * \brief Callback for sequence parsers (#Appendline).
 *
 * Count total sequence length.
 */
static int count_len(/*@unused@*/ const char *seq, size_t len,
                     /*@unused@*/ const fid_Alphabet *alpha, void *user_data,
                     /*@unused@*/ fid_Error *error)/*{{{*/
{
  ((fid_Sequencefileinfo *)user_data)->seqlen+=len;
  return 0;
}
/*}}}*/

/*!
 * \brief Callback for sequence parsers (#Nextsequence).
 *
 * Increase total sequence length and number of sequences by one for each
 * separator.
 */
static int count_sep(/*@unused@*/ fid_Uint64 seq_position, void *user_data,
                     /*@unused@*/ fid_Error *error)/*{{{*/
{
  ++((fid_Sequencefileinfo *)user_data)->seqlen;
  ++((fid_Sequencefileinfo *)user_data)->num_of_sequences;
  return 0;
}
/*}}}*/

/*!
 * \brief Parse sequence set from the provided buffer, but only gather
 * statistics.
 *
 * This function just parses the given buffer, but does not store the parsed
 * sequences anywhere. Instead, the function counts the number of sequences,
 * determines it total length, etc. (see #fid_Sequencefileinfo), and returns
 * these data to the caller. The main purpose of this function is to determine
 * if the length of the raw sequence data as stored in enhanced suffix arrays
 * can be represented in 32 bit integers or not.
 *
 * \param infilename The name of the input file (for error messages only).
 * \param buffer The buffer that contains biological data in some textual
 * format such like FASTA.
 * \param bufsize The size of \p buffer in bytes. By this, \p buffer is not
 * required to be zero-terminated. This value should be positive.
 * \param format The predetermined file format. If auto-detection should be
 * tried, pass #fid_Fileformat::fid_FORMAT_UNDEF.
 * \param seqinfo The information about the parsed sequences.
 * \param alpha The alphabet that is used to transform the textual data
 * into binary.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_sequences_parse_from_memory_to_lengths(const char *infilename,
                                               const char *buffer,
                                               size_t bufsize,
                                               fid_Fileformat format,
                                               fid_Sequencefileinfo *seqinfo,
                                               const fid_Alphabet *alpha,
                                               fid_Error *error)/*{{{*/
{
  int retcode;
  Parserfuns_64 funs;

  assert(alpha != NULL);

  if((retcode=generic_parser_init(infilename,buffer,bufsize,&format,
                                  fid_TABLES_ONLINE,error)) < 0)
  {
    return (retcode == -2)?0:-1;
  }

  memset(seqinfo,0,sizeof(fid_Sequencefileinfo));
  seqinfo->input_file_size=bufsize;
  seqinfo->num_of_sequences=1;

  funs.appendlinefun=count_len;
  funs.appenddescfun=count_desc;
  funs.nextseqfun=count_sep;

  return parse_sequences_64(infilename,buffer,bufsize,format,alpha,&funs,
                            seqinfo,error);
}
/*}}}*/
