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

#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H
/*!\addtogroup suffixarrays Enhanced Suffix Arrays
 * 
 * The enhanced suffix array data types and accompanying functions are
 * defined in this group. The integral data type is fid_Suffixarray.
 * Suffix-intervals are represented by the fid_Suffixinterval data
 * structure.
 */
/*@{*/
/*!
 * \brief Representation of an enhanced suffix array.
 *
 * This data structure gathers all data required to operate on the enhanced
 * suffix array.
 */
typedef struct
{
  fid_Uintsize uisize;     /*!<\brief Integer size. */
  fid_Sequences sequences; /*!<\brief Reference to sequence data. */
  fid_Alphabet alpha;      /*!<\brief The alphabet the sequence data is encoded
                            * with. */
  fid_Uint48constptr suftab;  /*!<\brief Plain suffix array, which is
                               * basically a correctly typed shortcut to the
                               * content of fid_Suffixarray::suffile. */
  fid_Uint48constptr skiptab; /*!<\brief Skip table, basically the correctly
                               * typed content of fid_Suffixarray::skpfile. */
  fid_Uint48constptr stitab;  /*!<\brief Inverse suffix array, basically the
                                * correctly typed content of
                                * fid_Suffixarray::stifile. */
  fid_Uint48 num_of_large_lcps; /*!<\brief The number of large lcp-values stored
                                 * in file fid_Suffixarray::llvfile. */

  fid_Mappedfile suffile;  /*!<\brief File containing the plain suffix array. */
  fid_Mappedfile lcpfile;  /*!<\brief File containing the lcp-table. */
  fid_Mappedfile llvfile;  /*!<\brief File containing the large lcp-values. */
  fid_Mappedfile skpfile;  /*!<\brief File containing the skip table. */
  fid_Mappedfile stifile;  /*!<\brief File containing the inverse suffix array. */
} fid_Suffixarray;

/*!
 * \brief Representation of a suffix-interval in 32 bit enhanced suffix array.
 *
 * Suffix-intervals are used to represent locations in an enhanced suffix
 * array. They basically consist of an interval in the suffix array, i.e., a
 * left and a right boundary (or lower and upper, depending on your
 * imagination), and a depth. A suffix-interval with depth \p d and boundaries
 * \p left and \p right is valid iff all suffixes within its boundaries share a
 * common prefix of length at least \p d. A suffix-interval is called an
 * lcp-interval if the longest common prefix of all suffixes in the
 * suffix-interval has length \p d. Suffix-intervals with left and right
 * boundaries being equal are called singletons and have no defined depth (see
 * #fid_SUFFIXINTERVAL_SINGLETON_32 and #fid_SUFFIXINTERVAL_SINGLETON_64).
 *
 * Use fid_suffixinterval_init() or fid_suffixinterval_init_root() (i.e.,
 * fid_suffixinterval_init_root_32() and fid_suffixinterval_init_root_64()) to
 * initialize this data structure. Function fid_suffixinterval_to_lcpinterval()
 * can be used to turn a valid suffix-interval into an lcp-interval.
 *
 * \see #fid_Suffixinterval_64
 */
typedef struct
{
  fid_Uint32 depth;     /*!< Depth of the suffix-interval. */
  fid_Uint32 left,      /*!< Left boundary. */
             right;     /*!< Right boundary. */
} fid_Suffixinterval_32;

/*!
 * \brief Representation of a suffix-interval in 64 bit enhanced suffix array.
 *
 * \see #fid_Suffixinterval_32
 */
typedef struct
{
  fid_Uint64 depth;     /*!< Depth of the suffix-interval. */
  fid_Uint64 left,      /*!< Left boundary. */
             right;     /*!< Right boundary. */
} fid_Suffixinterval_64;

/*!
 * \brief Special value for #fid_Suffixinterval_32::depth to indicate a
 * singleton.
 * \see #fid_SUFFIXINTERVAL_SINGLETON_64
 */
#define fid_SUFFIXINTERVAL_SINGLETON_32  (~(fid_Uint32)0)

/*!
 * \brief Special value for #fid_Suffixinterval_64::depth to indicate a
 * singleton.
 * \see #fid_SUFFIXINTERVAL_SINGLETON_32
 */
#define fid_SUFFIXINTERVAL_SINGLETON_64  (~(fid_Uint64)0)

/*!\brief Table request: suffix array. */
#define fid_TABLE_SUF            ((fid_Tablerequest)0x00000100)

/*!\brief Table request: lcp table. */
#define fid_TABLE_LCP            ((fid_Tablerequest)0x00000200)

/*!\brief Table request: skip table. */
#define fid_TABLE_SKP            ((fid_Tablerequest)0x00000400)

/*!\brief Table request: inverse suffix array. */
#define fid_TABLE_STI            ((fid_Tablerequest)0x00000800)

/*!\brief Table request: all tables related to offline matching. */
#define fid_TABLES_OFFLINE       (fid_TABLE_SUF|fid_TABLE_LCP|fid_TABLE_SKP|fid_TABLE_STI)

/*!\brief Table request bit mask: bits related to offline matching. */
#define fid_TABLES_OFFLINE_MASK  ((fid_Tablerequest)0xffffff00)

/*!\brief Table request: all tables. */
#define fid_TABLES_ALL           (fid_TABLES_ONLINE|fid_TABLES_OFFLINE)

/*!
 * \brief Size of the buffer for fid_suffixarray_get_intervals().
 * 
 * The size is determined as the maximum number of regular symbols, plus 1
 * for the left boundary for the special interval boundary, plus 1 for the
 * (non-existent) left boundary of the interval following the special
 * interval.
 */
#define fid_INTERVALBUFFERSIZE  ((size_t)fid_SYMBOLMAX+2)

/*!
 * \brief Determine lcp-value of a 32 bit suffix.
 *
 * This is implemented as a macro since it would be a huge performance penality
 * to call a function for something that is a simple assignment in the vast
 * majority of all cases. A function call occurs if the lcp-value stored in the
 * main lcp-table is equal to \c UCHAR_MAX (255), however, since then the real,
 * potentially larger, lcp-value must be looked up inside an exception table.
 *
 * \param L The lcp-value is assigned to this variable, which should be of type
 * #fid_Uint32 or #fid_Uint64.
 * \param ESA An enhanced suffix array.
 * \param I The suffix index for which the lcp-value should be determined.
 *
 * \see #fid_LCP_64()
 */
#define fid_LCP_32(L,ESA,I)\
  if(((L)=(ESA)->lcpfile.content[I]) == (unsigned char)UCHAR_MAX)\
  {\
    (L)=fid_suffixarray_find_large_lcp_32(ESA,I);\
  }

/*!
 * \brief Determine lcp-value of a 64 bit suffix.
 * \see #fid_LCP_32()
 */
#define fid_LCP_64(L,ESA,I)\
  if(((L)=(ESA)->lcpfile.content[I]) == (unsigned char)UCHAR_MAX)\
  {\
    (L)=fid_suffixarray_find_large_lcp_64(ESA,I);\
  }

/*!
 * \brief Initialize a fid_Suffixinterval structure.
 *
 * This has been implemented as a macro mainly for speed since it is used all
 * the time. We don't want to rely on the compiler to inline this
 * automatically, do we?
 *
 * \param SI The fid_Suffixinterval structure to be initialized.
 * \param D Depth.
 * \param L,R Left and right boundaries.
 */
#define fid_suffixinterval_init(SI,D,L,R)\
  (SI)->depth=(D);\
  (SI)->left=(L);\
  (SI)->right=(R)

/*!
 * \brief Initialize a #fid_Suffixinterval_32 structure corresponding to the
 * root interval of a given enhanced suffix array.
 *
 * \param SI The #fid_Suffixinterval_32 structure to be initialized.
 * \param ESA A 32 bit enhanced suffix array.
 *
 * \see #fid_suffixinterval_init_root_64()
 */
#define fid_suffixinterval_init_root_32(SI,ESA)\
  fid_suffixinterval_init(SI,0,0,(ESA)->sequences.total_length.v_uint32-1)

/*!
 * \brief Initialize a #fid_Suffixinterval_64 structure corresponding to the
 * root interval of a given enhanced suffix array.
 *
 * \param SI The #fid_Suffixinterval_64 structure to be initialized.
 * \param ESA A 64 bit enhanced suffix array.
 *
 * \see #fid_suffixinterval_init_root_32()
 */
#define fid_suffixinterval_init_root_64(SI,ESA)\
  fid_suffixinterval_init(SI,0,0,(ESA)->sequences.total_length.v_uint64-1)

/*!\brief Special return value for #fid_Esatraversecallback_32 and
 * #fid_Esatraversecallback_32 functions. */
#define fid_TRAVERSE_CONTINUE  ((int)0)

/*!\brief Special return value for #fid_Esatraversecallback_32 and
 * #fid_Esatraversecallback_32 functions. */
#define fid_TRAVERSE_SKIP      ((int)-1)

/*!
 * \brief Callback function prototype for enhanced suffix array traversals.
 *
 * For each lcp-interval in an enhanced suffix array passed to
 * fid_suffixarray_traverse(), a callback function is called. That callback
 * function must follow the signature of this type.
 *
 * \param esa The traversed enhanced suffix array.
 * \param si An lcp-interval. This is really an lcp-interval, not just a
 * suffix-interval, so there is no need to turn this into an lcp-interval
 * inside the function.
 * \param pdepth The depth of the parent lcp-interval.
 * \param error Error messages go here.
 * \param user_data Pointer to any data, may be used freely within the callback
 * function.
 *
 * \returns A positive integer for stopping the traversal, or one of the
 * following special return codes. If the function returns a positive integer,
 * then that exact value will be the return value of
 * fid_suffixarray_traverse().
 *
 * \retval #fid_TRAVERSE_CONTINUE Continue traversal.
 * \retval #fid_TRAVERSE_SKIP Do not traverse deeper into current subtree,
 * continue with next sibling.
 *
 * \see #fid_Esatraversecallback_64
 */
typedef int (*fid_Esatraversecallback_32)(const fid_Suffixarray *esa,
                                          const fid_Suffixinterval_32 *si,
                                          fid_Uint32 pdepth, fid_Error *error,
                                          void *user_data);

/*!
 * \brief Callback function prototype for enhanced suffix array traversals.
 * \see #fid_Esatraversecallback_32
 */
typedef int (*fid_Esatraversecallback_64)(const fid_Suffixarray *esa,
                                          const fid_Suffixinterval_64 *si,
                                          fid_Uint64 pdepth, fid_Error *error,
                                          void *user_data);

#ifdef __cplusplus
extern "C" {
#endif
void fid_suffixarray_init(fid_Suffixarray *esa, fid_Uintsize uisize);
int fid_suffixarray_load_from_files(fid_Suffixarray *esa,
                                    const char *basefilename,
                                    fid_Tablerequest tables,
                                    fid_Uintsize uisize, fid_Error *error);
int fid_suffixarray_realize(fid_Suffixarray *esa, fid_Tablerequest tables,
                            fid_Error *error);
/*!\copydoc #fid_suffixarray_traverse
 * \brief 32 bit version of #fid_suffixarray_traverse(). */
#line 278
int fid_suffixarray_traverse_32(const fid_Suffixarray *esa,
                                fid_Esatraversecallback_32 callback,
                                void *user_data, fid_Error *error);
/*!\copydoc #fid_suffixarray_traverse
 * \brief 64 bit version of #fid_suffixarray_traverse(). */
#line 278
int fid_suffixarray_traverse_64(const fid_Suffixarray *esa,
                                fid_Esatraversecallback_64 callback,
                                void *user_data, fid_Error *error);
/*!\copydoc #fid_suffixarray_get_intervals
 * \brief 32 bit version of #fid_suffixarray_get_intervals(). */
#line 281
fid_Uint32 fid_suffixarray_get_intervals_32(const fid_Suffixarray *esa,
                                      const fid_Suffixinterval_32 *si,
                                      fid_Uint32 *intervals);
/*!\copydoc #fid_suffixarray_get_intervals
 * \brief 64 bit version of #fid_suffixarray_get_intervals(). */
#line 281
fid_Uint64 fid_suffixarray_get_intervals_64(const fid_Suffixarray *esa,
                                      const fid_Suffixinterval_64 *si,
                                      fid_Uint64 *intervals);
/*!\copydoc #fid_suffixarray_find_embedded_interval
 * \brief 32 bit version of #fid_suffixarray_find_embedded_interval(). */
#line 284
int fid_suffixarray_find_embedded_interval_32(const fid_Suffixarray *esa,
                                              fid_Suffixinterval_32 *si,
                                              fid_Symbol symbol);
/*!\copydoc #fid_suffixarray_find_embedded_interval
 * \brief 64 bit version of #fid_suffixarray_find_embedded_interval(). */
#line 284
int fid_suffixarray_find_embedded_interval_64(const fid_Suffixarray *esa,
                                              fid_Suffixinterval_64 *si,
                                              fid_Symbol symbol);
/*!\copydoc #fid_suffixarray_extend_interval
 * \brief 32 bit version of #fid_suffixarray_extend_interval(). */
#line 287
int fid_suffixarray_extend_interval_32(const fid_Suffixarray *esa,
                                       const fid_Symbol *pattern, fid_Uint32 plen,
                                       fid_Suffixinterval_32 *si);
/*!\copydoc #fid_suffixarray_extend_interval
 * \brief 64 bit version of #fid_suffixarray_extend_interval(). */
#line 287
int fid_suffixarray_extend_interval_64(const fid_Suffixarray *esa,
                                       const fid_Symbol *pattern, fid_Uint64 plen,
                                       fid_Suffixinterval_64 *si);
/*!\copydoc #fid_suffixarray_find_interval
 * \brief 32 bit version of #fid_suffixarray_find_interval(). */
#line 290
int fid_suffixarray_find_interval_32(const fid_Suffixarray *esa,
                                     const fid_Symbol *pattern, fid_Uint32 plen,
                                     fid_Suffixinterval_32 *si);
/*!\copydoc #fid_suffixarray_find_interval
 * \brief 64 bit version of #fid_suffixarray_find_interval(). */
#line 290
int fid_suffixarray_find_interval_64(const fid_Suffixarray *esa,
                                     const fid_Symbol *pattern, fid_Uint64 plen,
                                     fid_Suffixinterval_64 *si);
/*!\copydoc #fid_suffixarray_find_large_lcp
 * \brief 32 bit version of #fid_suffixarray_find_large_lcp(). */
#line 293
fid_Uint32 fid_suffixarray_find_large_lcp_32(const fid_Suffixarray *esa,
                                       fid_Uint32 suffix);
/*!\copydoc #fid_suffixarray_find_large_lcp
 * \brief 64 bit version of #fid_suffixarray_find_large_lcp(). */
#line 293
fid_Uint64 fid_suffixarray_find_large_lcp_64(const fid_Suffixarray *esa,
                                       fid_Uint64 suffix);
/*!\copydoc #fid_suffixarray_suffix_length
 * \brief 32 bit version of #fid_suffixarray_suffix_length(). */
#line 295
fid_Uint32 fid_suffixarray_suffix_length_32(const fid_Suffixarray *esa,
                                      fid_Uint32 suffix);
/*!\copydoc #fid_suffixarray_suffix_length
 * \brief 64 bit version of #fid_suffixarray_suffix_length(). */
#line 295
fid_Uint64 fid_suffixarray_suffix_length_64(const fid_Suffixarray *esa,
                                      fid_Uint64 suffix);
/*!\copydoc #fid_suffixarray_compute_distribution
 * \brief 32 bit version of #fid_suffixarray_compute_distribution(). */
#line 297
void fid_suffixarray_compute_distribution_32(fid_Suffixarray *esa);
/*!\copydoc #fid_suffixarray_compute_distribution
 * \brief 64 bit version of #fid_suffixarray_compute_distribution(). */
#line 297
void fid_suffixarray_compute_distribution_64(fid_Suffixarray *esa);
#line 299
void fid_suffixarray_free(fid_Suffixarray *esa);
void fid_suffixarray_dump(const fid_Suffixarray *esa, FILE *stream);
/*!\copydoc #fid_suffixarray_dump_intervals
 * \brief 32 bit version of #fid_suffixarray_dump_intervals(). */
#line 302
void fid_suffixarray_dump_intervals_32(const fid_Suffixarray *esa,
                                       const fid_Uint32 intervals[fid_INTERVALBUFFERSIZE],
                                       FILE *stream);
/*!\copydoc #fid_suffixarray_dump_intervals
 * \brief 64 bit version of #fid_suffixarray_dump_intervals(). */
#line 302
void fid_suffixarray_dump_intervals_64(const fid_Suffixarray *esa,
                                       const fid_Uint64 intervals[fid_INTERVALBUFFERSIZE],
                                       FILE *stream);
/*!\copydoc #fid_suffixarray_dump_suffix
 * \brief 32 bit version of #fid_suffixarray_dump_suffix(). */
#line 305
void fid_suffixarray_dump_suffix_32(const fid_Suffixarray *esa, fid_Uint32 suffix,
                                    fid_Uint32 length, FILE *stream);
/*!\copydoc #fid_suffixarray_dump_suffix
 * \brief 64 bit version of #fid_suffixarray_dump_suffix(). */
#line 305
void fid_suffixarray_dump_suffix_64(const fid_Suffixarray *esa, fid_Uint64 suffix,
                                    fid_Uint64 length, FILE *stream);
/*!\copydoc #fid_suffixarray_print
 * \brief 32 bit version of #fid_suffixarray_print(). */
#line 307
void fid_suffixarray_print_32(const fid_Suffixarray *esa, 
                              fid_Tablerequest request, FILE *stream);
/*!\copydoc #fid_suffixarray_print
 * \brief 64 bit version of #fid_suffixarray_print(). */
#line 307
void fid_suffixarray_print_64(const fid_Suffixarray *esa, 
                              fid_Tablerequest request, FILE *stream);
/*!\copydoc #fid_suffixinterval_lcpvalue
 * \brief 32 bit version of #fid_suffixinterval_lcpvalue(). */
#line 309
fid_Uint32 fid_suffixinterval_lcpvalue_32(const fid_Suffixinterval_32 *si,
                                    const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_lcpvalue
 * \brief 64 bit version of #fid_suffixinterval_lcpvalue(). */
#line 309
fid_Uint64 fid_suffixinterval_lcpvalue_64(const fid_Suffixinterval_64 *si,
                                    const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_to_lcpinterval
 * \brief 32 bit version of #fid_suffixinterval_to_lcpinterval(). */
#line 311
void fid_suffixinterval_to_lcpinterval_32(fid_Suffixinterval_32 *si,
                                          const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_to_lcpinterval
 * \brief 64 bit version of #fid_suffixinterval_to_lcpinterval(). */
#line 311
void fid_suffixinterval_to_lcpinterval_64(fid_Suffixinterval_64 *si,
                                          const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_find_right
 * \brief 32 bit version of #fid_suffixinterval_find_right(). */
#line 313
void fid_suffixinterval_find_right_32(fid_Suffixinterval_32 *si,
                                      const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_find_right
 * \brief 64 bit version of #fid_suffixinterval_find_right(). */
#line 313
void fid_suffixinterval_find_right_64(fid_Suffixinterval_64 *si,
                                      const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_homepos
 * \brief 32 bit version of #fid_suffixinterval_homepos(). */
#line 315
fid_Uint32 fid_suffixinterval_homepos_32(const fid_Suffixinterval_32 *si,
                                   const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_homepos
 * \brief 64 bit version of #fid_suffixinterval_homepos(). */
#line 315
fid_Uint64 fid_suffixinterval_homepos_64(const fid_Suffixinterval_64 *si,
                                   const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_dump
 * \brief 32 bit version of #fid_suffixinterval_dump(). */
#line 317
void fid_suffixinterval_dump_32(const fid_Suffixinterval_32 *si,
                                const fid_Suffixarray *esa, FILE *stream);
/*!\copydoc #fid_suffixinterval_dump
 * \brief 64 bit version of #fid_suffixinterval_dump(). */
#line 317
void fid_suffixinterval_dump_64(const fid_Suffixinterval_64 *si,
                                const fid_Suffixarray *esa, FILE *stream);
#line 320
char *fid_tablerequest_to_string(fid_Tablerequest request, const char *sep);
#ifdef __cplusplus
}
#endif
/*@}*/

#endif /*! SUFFIXARRAY_H */
