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

#ifndef SEQUENCES_H
#define SEQUENCES_H
/*!\addtogroup sequences Binary encoded multiple sequences
 * This group contains data structures and functions for handling sequences.
 */
/*@{*/

/*!
 * \brief A structure that holds references to persistent suffix array tables.
 * 
 * When re-mapping fid_Sequences::sspfile or fid_Sequences::sdsfile, always
 * remember to set fid_Sequences::descriptions and fid_Sequences::separator,
 * respectively, to the corresponding content pointer since this might have
 * changed after the re-mapping.
 */
typedef struct
{
  fid_Uintsize uisize;           /*!<\brief Integer size. */
  const fid_Alphabet *alpha;     /*!<\brief Reference to the alphabet the
                                  * sequence data is encoded with. */
  fid_Uint48 num_of_sequences;   /*!<\brief Number of sequences represented. */
  fid_Uint48 total_length;       /*!<\brief Total length of all sequences when
                                  * concatenating them, including the sequence
                                  * separators. */
  fid_Uint48constptr descriptions; /*!<\brief Content of fid_Sequences::sdsfile
                                    * as unsigned integers. */
  fid_Uint48constptr separators;   /*!<\brief Content of fid_Sequences::sspfile
                                    * as unsigned integers. */
  double distribution[UCHAR_MAX+1]; /*!<\brief Character distribution. */
  fid_Mappedfile tisfile;  /*!<\brief Sequences in binary representation. */
  fid_Mappedfile oisfile;  /*!<\brief Sequences in original/textual
                            * representation. */
  fid_Mappedfile sspfile;  /*!<\brief Sequence separators. */
  fid_Mappedfile desfile;  /*!<\brief Sequence descriptions. */
  fid_Mappedfile sdsfile;  /*!<\brief Sequence description separators. */
} fid_Sequences;

/*!
 * \brief Enumeration of supported biological sequence file formats.
 * 
 * These values can be used to force the sequence parser to try a specific
 * format instead of leaving the decision on how to read the data to the
 * parser. However, enabling auto-detection by choosing
 * fid_Fileformat::fid_FORMAT_UNDEF should be good enough in virtually
 * any case.
 */
typedef enum
{
  fid_FORMAT_UNDEF,   /*!<\brief No specific file format. Use for
                       * auto-detection. */
  fid_FORMAT_FASTA,   /*!<\brief FASTA file format. */
  fid_FORMAT_UNIPROT  /*!<\brief UniProt file format. */
} fid_Fileformat;

/*!
 * \brief Length information about sequence file.
 *
 * This structure contains some information about sequence data files, and is
 * filled by functions #fid_sequences_parse_from_file_to_lengths() and
 * #fid_sequences_parse_from_memory_to_lengths().
 */
typedef struct
{
  size_t seqlen;            /*!<\brief Total length of sequence data, i.e.,
                             * size of the <code>.tis</code> file. */
  size_t desclen;           /*!<\brief Total length of description data, i.e.,
                             * size of the <code>.des</code> file. */
  size_t num_of_sequences;  /*!<\brief Total number of sequences. */
  size_t input_file_size;   /*!<\brief Size of unparsed sequence data file. */
} fid_Sequencefileinfo;

/*!
 * \brief Type for encoding enhanced suffix array table requests.
 *
 * When mapping an enhanced suffix array into memory, pass a value of this
 * type to the corresponding function to tell it which tables it should read.
 * This is a bit vector that must be initialized by OR'ing predefined values
 * such like #fid_TABLE_TIS or #fid_TABLE_SUF. Always use this type for it has
 * a fixed size on all architectures.
 */
typedef fid_Uint32  fid_Tablerequest;

/*!\brief Table request: nothing. */
#define fid_TABLE_NONE          ((fid_Tablerequest)0x00000000)

/*!\brief Table request: transformed input sequence. */
#define fid_TABLE_TIS           ((fid_Tablerequest)0x00000001)

/*!\brief Table request: original input sequence. */
#define fid_TABLE_OIS           ((fid_Tablerequest)0x00000002)

/*!\brief Table request: sequence descriptions. */
#define fid_TABLE_DES           ((fid_Tablerequest)0x00000004)

/*!\brief Table request: all tables related to online matching. */
#define fid_TABLES_ONLINE       (fid_TABLE_TIS|fid_TABLE_OIS|fid_TABLE_DES)

/*!\brief Table request bit mask: bits related to online matching. */
#define fid_TABLES_ONLINE_MASK  ((fid_Tablerequest)0x000000ff)

/*!\def fid_READ_SYMBOL
 * \brief Read symbol from transformed input sequences of a #fid_Sequences
 * structure.
 *
 * \param SEQS Pointer to a #fid_Sequences structure.
 * \param I Absolute sequence position.
 *
 * \returns The symbol at position \p I.
 */
#if defined DEBUG && defined __GNUC__ && !defined __ICC
#ifdef __cplusplus
#include <cassert>
#else /* !__cplusplus */
#include <assert.h>
#endif /* __cplusplus */
#define fid_READ_SYMBOL(SEQS,I)\
  ({ fid_SWITCH48((SEQS)->uisize,\
                  assert((I) < (SEQS)->total_length.v_uint32);,\
                  assert((I) < (SEQS)->total_length.v_uint64););\
     (SEQS)->tisfile.content[I]; })
#else /* !DEBUG || !__GNUC__ || __ICC */
#define fid_READ_SYMBOL(SEQS,I)  (SEQS)->tisfile.content[I]
#endif /* DEBUG && __GNUC__ && !__ICC*/

/*!
 * \brief Return pointer to description of sequence \p I.
 * 
 * \param S Pointer to an fid_Sequences structure.
 * \param I Sequence index.
 *
 * \returns Pointer to the description of sequence \p I.
 */
#define fid_SEQUENCE_DESCRIPTION_32(S,I)\
  ((const char *)((S)->desfile.content+(S)->descriptions.v_uint32[I]))

/*! \see fid_SEQUENCE_DESCRIPTION_32() */
#define fid_SEQUENCE_DESCRIPTION_64(S,I)\
  ((const char *)((S)->desfile.content+(S)->descriptions.v_uint64[I]))

/*!
 * \brief Return the length of the description of sequence \p I.
 * 
 * \param S Pointer to an fid_Sequences structure.
 * \param I Sequence index.
 *
 * \returns Length of the description of sequence \p I.
 */
#define fid_SEQUENCE_DESCRIPTION_LENGTH_32(S,I)\
  ((S)->descriptions.v_uint32[(I)+1]-(S)->descriptions.v_uint32[I])

/*! \see fid_SEQUENCE_DESCRIPTION_LENGTH_32() */
#define fid_SEQUENCE_DESCRIPTION_LENGTH_64(S,I)\
  ((S)->descriptions.v_uint64[(I)+1]-(S)->descriptions.v_uint64[I])

/*!
 * \brief Type of callback function expected by #fid_sequences_iterate_32().
 * 
 * To use the sequence iterator functions, the caller must declare a callback
 * function correponding to this type and pass the function pointer to the
 * iterator function. For each sequence in a fid_Sequences structure, that
 * callback function is called.
 * 
 * \param seqs The structure that contains the processed sequences.
 * \param seqnum Current sequence index.
 * \param sequence Direct pointer to the current sequence.
 * \param seqlen Length of the current sequence.
 * \param user_data Arbitrary pointer passed through by the iterator
 * functions to omit the need for global variables.
 * 
 * \returns 0 to continue iterating over sequences, any other value to stop.
 *
 * \see #fid_Sequenceiterfun_64
 */
typedef int (*fid_Sequenceiterfun_32)(const fid_Sequences *seqs,
                                      fid_Uint32 seqnum,
                                      const fid_Symbol *sequence,
                                      fid_Uint32 seqlen, void *user_data);

/*!
 * \brief Type of callback function expected by #fid_sequences_iterate_64().
 * \see #fid_Sequenceiterfun_32
 */
typedef int (*fid_Sequenceiterfun_64)(const fid_Sequences *seqs,
                                      fid_Uint64 seqnum,
                                      const fid_Symbol *sequence,
                                      fid_Uint64 seqlen, void *user_data);

#ifdef __cplusplus
extern "C" {
#endif
void fid_sequences_init(fid_Sequences *seqs, fid_Uintsize uisize,
                        const fid_Alphabet *alpha);
int fid_sequences_map(fid_Sequences *seqs, const char *basefilename,
                      fid_Tablerequest tables, fid_Filenamebuffer *fnamebuf,
                      fid_Error *error);
int fid_sequences_realize(fid_Sequences *seqs, fid_Tablerequest tables,
                          fid_Error *error);
int fid_sequences_parse_from_file_to_file(const char *infilename,
                                          fid_Fileformat format,
                                          fid_Tablerequest tables,
                                          const char *basefilename,
                                          const fid_Alphabet *alpha,
                                          fid_Uintsize uisize,
                                          size_t *input_file_size,
                                          fid_Error *error);
int fid_sequences_parse_from_file_to_memory(const char *infilename,
                                            fid_Fileformat format,
                                            fid_Tablerequest tables,
                                            fid_Sequences *result,
                                            const fid_Alphabet *alpha,
                                            size_t padding, fid_Uintsize uisize,
                                            size_t *input_file_size,
                                            fid_Error *error);
int fid_sequences_parse_from_file_to_lengths(const char *infilename,
                                             fid_Fileformat format,
                                             fid_Sequencefileinfo *seqinfo,
                                             const fid_Alphabet *alpha,
                                             fid_Error *error);
int fid_sequences_parse_from_memory_to_file(const char *infilename,
                                            const char *buffer, size_t bufsize,
                                            fid_Fileformat format,
                                            fid_Tablerequest tables,
                                            const char *basefilename,
                                            const fid_Alphabet *alpha,
                                            fid_Uintsize uisize,
                                            fid_Error *error);
int fid_sequences_parse_from_memory_to_memory(const char *infilename,
                                              const char *buffer,
                                              size_t bufsize,
                                              fid_Fileformat format,
                                              fid_Tablerequest tables,
                                              fid_Sequences *result,
                                              const fid_Alphabet *alpha,
                                              size_t padding,
                                              fid_Uintsize uisize,
                                              fid_Error *error);
int fid_sequences_parse_from_memory_to_lengths(const char *infilename,
                                               const char *buffer,
                                               size_t bufsize,
                                               fid_Fileformat format,
                                               fid_Sequencefileinfo *seqinfo,
                                               const fid_Alphabet *alpha,
                                               fid_Error *error);
BEGIN_32_64
UINT fid_sequences_offset_to_index(const fid_Sequences *seqs,
                                   UINT offset);
void fid_sequences_index_to_boundaries(const fid_Sequences *seqs,
                                       UINT seqindex,
                                       UINT *left, UINT *right);
void fid_sequences_offset_to_boundaries(const fid_Sequences *seqs,
                                        UINT offset,
                                        UINT *left, UINT *right);
int fid_sequences_iterate_range(const fid_Sequences *seqs,
                                UINT from, UINT to,
                                const fid_Sequenceiterfun iterfun,
                                void *user_data);
int fid_sequences_iterate(const fid_Sequences *seqs,
                          const fid_Sequenceiterfun iterfun, void *user_data);
END_32_64
void fid_sequences_compute_distribution(fid_Sequences *seqs);
void fid_sequences_free(fid_Sequences *seqs);
BEGIN_32_64
void fid_sequences_dump_range(const fid_Symbol *seq, UINT length,
                              const fid_Alphabet *alpha, const char *str,
                              int stop_at_separator, FILE *stream);
END_32_64
#ifdef __cplusplus
}
#endif

/*@}*/
#endif /* !SEQUENCES_H */
