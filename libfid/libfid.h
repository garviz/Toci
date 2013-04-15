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

#ifndef LIBFID_H
#define LIBFID_H
/*!\addtogroup integers Integers */
/*@{*/

/*!\def fid_WORDSIZE
 * \brief The native integer size.
 *
 * This is defined to either 32 or 64. Its value is figured via feature test
 * macros. Types like #fid_Uint64 are defined depending on the value of this
 * symbol.
 */

/*
 * Figure out architecture word size using feature test macros.
 */
#if defined __LP64__ || defined _LP64 || defined __64BIT__ || (defined __sparc && defined __sparcv9) || defined __x86_64 || defined __amd64
#define fid_WORDSIZE  64
#else /* no 64 bit symbol defined */
#ifdef __WORDSIZE
#define fid_WORDSIZE  __WORDSIZE
#else /* !__WORDSIZE */
#define fid_WORDSIZE  32
#endif /* __WORDSIZE */
#endif /* __LP64__ ||  _LP64 || __64BIT__ */

/*!\typedef fid_Uint64
 * \brief Unsigned 64 bit integer type. */
/*!\typedef fid_Uint32
 * \brief Unsigned 32 bit integer type. */
/*!\typedef fid_Uint16
 * \brief Unsigned 16 bit integer type. */
/*!\typedef fid_Sint64
 * \brief Signed 64 bit integer type. */
/*!\typedef fid_Sint32
 * \brief Signed 32 bit integer type. */
/*!\typedef fid_Sint16
 * \brief Signed 16 bit integer type. */
/*!\def fid_U64FMT
 * \brief Format string for printing a #fid_Uint64 value.
 *
 * Always use this for printing values of type #fid_Uint64. Never use
 * <code>%%lu</code> or <code>%%llu</code> directly for printing a #fid_Uint64
 * value. The reason is that #fid_Uint64 may be defined differently on
 * different platforms, so there can be no fixed format string that is correct
 * on all platforms,
 *
 * \see also fid_S64FMT. */
/*!\def fid_S64FMT
 * \brief Format string for printing a #fid_Sint64 value.
 *
 * Always use this for printing values of type #fid_Sint64.
 *
 * \see also fid_U64FMT. */

/*
 * Define 64 bit integer types fid_Uint64 and fid_Sint64, and format strings
 * fid_U64FMT and fid_S64FMT for printing 64 bit integers when cast to
 * fid_Uint64 or fid_Sint64, respectively.
 */
#if fid_WORDSIZE == 64
typedef unsigned long      fid_Uint64;
typedef signed long        fid_Sint64;
#define fid_U64FMT         "%lu"
#define fid_S64FMT         "%ld"
#elif fid_WORDSIZE == 32
typedef unsigned long long fid_Uint64;
typedef signed long long   fid_Sint64;
#define fid_U64FMT         "%llu"
#define fid_S64FMT         "%lld"
#else /* fid_WORDSIZE != 64 && fid_WORDSIZE != 32 */
#error Unknown word size.
#endif /* fid_WORDSIZE == 64 || fid_WORDSIZE == 32 */

/*
 * Define 32 bit and 16 bit integer types. These should be used whenever a
 * fixed size is required.
 */
typedef unsigned int       fid_Uint32;
typedef signed int         fid_Sint32;
typedef unsigned short     fid_Uint16;
typedef signed short       fid_Sint16;

/*! \brief Maximum value of a #fid_Uint64. */
#define fid_UINT64_MAX  ((fid_Uint64)~(fid_Uint64)0)

/*! \brief Maximum value of a #fid_Uint32. */
#define fid_UINT32_MAX  ((fid_Uint32)~(fid_Uint32)0)

/*! \brief Maximum value of a #fid_Uint16. */
#define fid_UINT16_MAX  ((fid_Uint16)~(fid_Uint16)0)

/*!
 * \brief Format string for printing a #fid_Uint32 value.
 *
 * This should be used for printing values of type #fid_Uint32. Raw int values
 * or other raw C type values, however, should be printed using their
 * respective standard format strings.
 *
 * \see also fid_S32FMT.
 */
#define fid_U32FMT         "%u"

/*!
 * \brief Format string for printing a fid_Sint32 value.
 *
 * This should be used for printing values of type #fid_Sint32.
 *
 * \see also fid_U32FMT.
 */
#define fid_S32FMT         "%d"

/*!
 * \brief Format string for printing a table request bitmask.
 *
 * This should be used for printing values of type #fid_Tablerequest.
 */
#define fid_TABREQFMT      "%08x"

/*!
 * \brief Type of integers used in some structure, i.e., fid_Uint48.
 *
 * Some data types, i.e., fid_Uint48 and fid_Suffixarray can store data
 * represented by either 32 or 64 bit values. In order to be able to decide
 * between these where it is necessary, a type identifier is stored along with
 * the data.
 *
 * Instead of using a simple boolean to switch between 32/64 bit code, we use
 * this enumeration. The enumeration begins at 1 in order to catch
 * uninitialized values at runtime (indicating a programming error).
 *
 * \see fid_SWITCH48().
 */
typedef enum
{
  fid_UINTSIZE_UNDEF=1,  /*!<\brief Undefined integer size. */
  fid_UINTSIZE_32,       /*!<\brief Integer size is 32 bits. */
  fid_UINTSIZE_64        /*!<\brief Integer size is 64 bits. */
} fid_Uintsize;

/*!
 * \brief A type that represents either 32 or 64 bit unsigned integer.
 *
 * \see fid_Uintsize, fid_SWITCH48(), fid_VALUE48(), fid_PRINT48().
 */
typedef union
{
  fid_Uint32 v_uint32;  /*!< 32 bit unsigned integer. */
  fid_Uint64 v_uint64;  /*!< 64 bit unsigned integer. */

#ifdef __cplusplus
  fid_Uint32 operator=(const fid_Uint32 &v) { return v_uint32=v; }
  fid_Uint64 operator=(const fid_Uint64 &v) { return v_uint64=v; }
  fid_Uint32 operator-=(const fid_Uint32 &v) { return v_uint32-=v; }
  fid_Uint64 operator-=(const fid_Uint64 &v) { return v_uint64-=v; }
  fid_Uint32 operator+=(const fid_Uint32 &v) { return v_uint32+=v; }
  fid_Uint64 operator+=(const fid_Uint64 &v) { return v_uint64+=v; }

  bool operator==(const fid_Uint32 &v) const { return v_uint32 == v; }
  bool operator==(const fid_Uint64 &v) const { return v_uint64 == v; }
  bool operator<(const fid_Uint32 &v) const { return v_uint32 < v; }
  bool operator<(const fid_Uint64 &v) const { return v_uint64 < v; }
  bool operator<=(const fid_Uint32 &v) const { return v_uint32 <= v; }
  bool operator<=(const fid_Uint64 &v) const { return v_uint64 <= v; }
  bool operator>(const fid_Uint32 &v) const { return v_uint32 > v; }
  bool operator>(const fid_Uint64 &v) const { return v_uint64 > v; }
  bool operator>=(const fid_Uint32 &v) const { return v_uint32 >= v; }
  bool operator>=(const fid_Uint64 &v) const { return v_uint64 >= v; }
#endif /* __cplusplus */
} fid_Uint48;

/*!
 * \brief A type that represents const pointers to either 32 or 64 bit
 * unsigned integers.
 *
 * \see fid_Uint48.
 */
typedef union
{
  const fid_Uint32 *v_uint32;  /*!< Pointer to 32 bit unsigned integer. */
  const fid_Uint64 *v_uint64;  /*!< Pointer to 64 bit unsigned integer. */
} fid_Uint48constptr;

/*!
 * \brief Branch code into 32 and 64 bit versions.
 *
 * If \p UISIZE contains an invalid value, then the program will be terminated
 * by calling \c abort().
 *
 * \param UISIZE A value of type #fid_Uintsize.
 * \param CODE32 32 bit code.
 * \param CODE64 64 bit code.
 */
#define fid_SWITCH48(UISIZE,CODE32,CODE64)\
  switch(UISIZE)\
  {\
   case fid_UINTSIZE_32:\
    CODE32\
    break;\
   case fid_UINTSIZE_64:\
    CODE64\
    break;\
   default:\
    abort();\
  }

/*!
 * \brief Assign value to a fid_Uint48 or fid_Uint48constptr type.
 *
 * \param VAR Variable of type #fid_Uint48 or #fid_Uint48constptr.
 * \param UISIZE A value of type #fid_Uintsize.
 * \param VAL An integer value. No type cast is performed.
 */
#define fid_ASSIGN48(VAR,UISIZE,VAL)\
  fid_SWITCH48(UISIZE,(VAR).v_uint32=(VAL);,(VAR).v_uint64=(VAL);)

/*!
 * \brief Fetch value from a fid_Uint48 or fid_Uint48constptr type.
 *
 * \param UISIZE A value of type #fid_Uintsize.
 * \param VAL A value of type fid_Uint48.
 *
 * \returns Either a fid_Uint32 or a #fid_Uint64 value, as stored in \p VAL.
 *
 * \note This macro does not abort if \p UISIZE contains an invalid value.
 */
#define fid_VALUE48(UISIZE,VAL)\
  ((UISIZE) == fid_UINTSIZE_32?(VAL).v_uint32:(VAL).v_uint64)

/*!
 * \brief Fetch value from a fid_Uint48 type for printing.
 *
 * Use the fid_U64FMT format string to print values returned by this macro.
 *
 * \param UISIZE A value of type #fid_Uintsize.
 * \param VAL A value of type fid_Uint48.
 *
 * \returns The value stored in \p VAL as #fid_Uint64 value.
 *
 * \note This macro does not abort if \p UISIZE contains an invalid value.
 */
#define fid_PRINT48(UISIZE,VAL)\
  ((UISIZE) == fid_UINTSIZE_32\
   ?(fid_Uint64)(VAL).v_uint32\
   :(VAL).v_uint64)

/*@}*/
/*!\addtogroup misc */
/*@{*/
#ifdef __cplusplus
#include <cstddef>
#else /* !__cplusplus */
#include <stddef.h>
#endif /* __cplusplus */

#ifdef fid_ENABLE_MEMORY_PROFILING
#ifdef __cplusplus
#include <cstdio>
#else /* !__cplusplus */
#include <stdio.h>
#endif /* __cplusplus */
#define malloc(s)     fid_memory_malloc(s)
#define calloc(n,s)   fid_memory_calloc(n,s)
#define realloc(p,s)  fid_memory_realloc(p,s)
#define strdup(s)     fid_memory_strdup(s)
#define free(p)       fid_memory_free(p)
#endif /* fid_ENABLE_MEMORY_PROFILING */

#ifdef __cplusplus
extern "C" {
#endif
#if defined fid_ENABLE_MEMORY_PROFILING || defined fid_MEMORY_INTERNAL
/*@null@*/
void *fid_memory_malloc(size_t size);
/*@null@*/
void *fid_memory_calloc(size_t nmemb, size_t size);
/*@null@*/
void *fid_memory_realloc(void *ptr, size_t size);
/*@null@*/
char *fid_memory_strdup(const char *s);
void fid_memory_free(void *ptr);
void fid_memory_query(size_t *peak, size_t *current, fid_Uint64 *allocs,
                      fid_Uint64 *frees, fid_Uint64 *total);
void fid_memory_show_usage(FILE *stream);
#else
/*!\brief Macro that expands to nothing. */
#define fid_memory_query(P,C,A,F,T)
/*!\brief Macro that expands to nothing. */
#define fid_memory_show_usage(S)
#endif /* fid_ENABLE_MEMORY_PROFILING || fid_MEMORY_INTERNAL */
#ifdef __cplusplus
}
#endif
/*@}*/

/*!\addtogroup errors Error message handling
 * Error propagation is an important and often poorly solved issue in any
 * program. In this library, errors are propagated by employing a dedicated
 * data structure fid_Error plus supporting function.
 */
/*@{*/
#ifdef __cplusplus
#include <cstdio>
using std::FILE;
using std::size_t;
#else /* !__cplusplus */
#include <stdio.h>
#endif /* __cplusplus */

/*!\brief This is the maximum number of errors that can be stored in an
 * #fid_Error structure. */
#define fid_ERROR_MAX  ((int)16)

/*!
 * \brief Error message overflow handling mode.
 * 
 * \see fid_Error documentation for more details.
 */
typedef enum
{
  fid_ERRORMODE_FIFO=0,
  fid_ERRORMODE_DEEPEST,
  fid_ERRORMODE_KEEP_LAST
} fid_Errormode;

/*!
 * \brief Structure for error message handling.
 * 
 * This data structure stores multiple error message to keep track of more
 * than only one error to enable the user to obtain a more detailed view
 * on the cause of an error. The data structure supports three modes of
 * operation to handle overflows, i.e., the case when more than a certain
 * number of errors occurred:
 * - FIFO (first in, first out)
 * - Keep deepest (stop collecting errors if there are too many)
 * - Keep deepest but the last (replace the most recent errors by a new one)
 * 
 * The maximum number of errors is defined by a compile time constant,
 * fid_ERROR_MAX. What mode to choose depends on the kind of error, i.e.,
 * depends on the application context.
 */
typedef struct
{
  int num_of_errors;  /*!< \brief Number of errors collected. */
  int first_error;    /*!< \brief Index of the first error message in
                       * #fid_Error::errors. */
  fid_Errormode mode; /*!< \brief How to handle many errors. */
  int overflow;       /*!< \brief Set to \e true if error messages have been
                       * lost due to overflows. */
  int outofmem;       /*!< \brief Set to \e true if an error could not be
                       * handled properly due to out-of-memory condition. */
  char *errors[fid_ERROR_MAX];  /*!< \brief Error messages. */
} fid_Error;

/*!
 * \brief Check if an error is stored in fid_Error \p ERR.
 * 
 * \param ERR An fid_Error structure.
 * 
 * \returns True if an error is available, false otherwise.
 */
#define fid_ERROR_OCCURRED(ERR)  ((ERR)->num_of_errors > 0)

/*!
 * \brief Append an out-of-memory error to \p ERR.
 */
#define fid_OUTOFMEM(ERR)\
  fid_error_throw(ERR,"Out of memory (%s[%d])",__FILE__,__LINE__)

#ifdef __cplusplus
extern "C" {
#endif
void fid_error_init(fid_Error *error, fid_Errormode mode);
void fid_error_init_default(fid_Error *error);
void fid_error_free(fid_Error *error);
#ifdef __GNUC__
void fid_error_throw(fid_Error *error, const char *fmt, ...)
  __attribute__ ((format (printf, 2, 3)));
void fid_error_throw_errno(fid_Error *error, int errnum, const char *fmt, ...)
  __attribute__ ((format (printf, 3, 4)));
#else /* !__GNUC__ */
void fid_error_throw(fid_Error *error, const char *fmt, ...);
void fid_error_throw_errno(fid_Error *error, int errnum, const char *fmt, ...);
#endif /* __GNUC__ */
int fid_error_dump(const fid_Error *error, FILE *stream);
#ifdef __cplusplus
}
#endif
/*@}*/

/*!\addtogroup options Library configuration
 * The behavior of the library can be influencent by setting an environment
 * variable, whose name is defined by #FID_OPTIONS_VARNAME. See the
 * documentation for fid_options_parse() to find a list of values it may
 * take. */
/*@{*/

/*!\brief Name of environment variable parsed by \e libfid. */
#define FID_OPTIONS_VARNAME  "FID_OPTIONS"

/*!\internal */
extern int fid_options_prefetch;

/*!\internal */
extern int fid_options_smart_prefetch;

#ifdef __cplusplus
extern "C" {
#endif
void fid_options_parse(void);
#ifdef __cplusplus
}
#endif
/*@}*/

/*!\addtogroup arrays Dynamic arrays
 * Generic implementation of dynamic arrays. Here is a set of macros for
 * mamaging dynamic arrays of arbitrary types. To declare a new dynamic array
 * type, use fid_DYNARRAY_DECLARE(). Then use fid_DYNARRAY_INIT() or
 * fid_DYNARRAY_INIT_PREALLOC() for initialization, and one of the various
 * macros described in here to add or remove elements to/from your array. When
 * running out of space, the array will grow automatically (depending on the
 * macro used). Free up the space occupied by your array with
 * fid_DYNARRAY_FREE().
 *
 * Macro fid_DYNARRAY_DECLARE() declares a new dynamic array type of a given
 * name, storing elements of given type. This new type has several members,
 * where the "public" ones are
 * - \c dyndata,
 * - \c allocated, and
 * - \c occupied
 *
 * The actual data is stored in memory pointed to by \c dyndata. This is a
 * pointer of the type passed to fid_DYNARRAY_DECLARE(). The number of
 * allocated elements is stored in \c allocated. This is the total number of
 * elements of the stored type that the array can store, not the size of the
 * memory block in bytes. If the total size needs to be known in bytes, then
 * this can be obtained by multiplying \c allocated with the size of the stored
 * type. The number of elements currently in use is stored in \c occupied. The
 * dynamic arrays implemented here do not maintain a free-list, but assume a
 * stack-like organization. That is, the value \c occupied is always
 * interpreted in a way that elements <code>0, 1, ...,  occupied-1</code> are
 * in use, and that elements <code>occupied, occupied+1, ..., allocated-1</code>
 * are free. This makes the element at index <code>occupied-1</code> the last
 * used element in the array, which can be accessed easily using macro
 * fid_DYNARRAY_TOPELEM().
 *
 * \note When compiled for debugging (i.e., with preprocessor symbol \c DEBUG
 * defined), then the array structures are augmented with one extra member that
 * is used for additional checks. These checks are only performed in debug
 * mode. An implication is that array structure sizes are different in debug
 * and non-debug modes, so that applications compiled with \c DEBUG are
 * <em>binary incompatible</em> with a version of \c libfid compiled without
 * \c DEBUG (and vice versa). Please keep this in mind when observing strange
 * things happening.
 */
/*@{*/
#ifdef __cplusplus
#include <cassert>
#else /* !__cplusplus */
#include <assert.h>
#endif /* __cplusplus */

/*
 * Here is some documentation for the macros defined inside the #ifdef, #else,
 * #endif blocks. No need for copy-and-paste...
 */

/*!\def fid_DYNARRAY_SIZECHECK
 * \brief Debugging.
 *
 * Only defined to something meaningful in debug mode (i.e., when \c DEBUG is
 * defined). Please ignore this.
 */ 

/*!\def fid_DYNARRAY_DECLARE
 * \brief Declare a dynamic array.
 *
 * \param NAME The name of the new structure.
 * \param TYPE The type of elements to be stored inside the dynamic array.
 */

/*!\def fid_DYNARRAY_INIT
 * \brief Initialize a dynamic array.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 */

/*!\def fid_DYNARRAY_FREE
 * \brief Free space occupied by a dynamic array.
 *
 * This macro does attempt not iterate over the array's elements and free them.
 * If the array elements need to be freed, maybe because they contain pointers
 * to allocated memory, then you must free them by yourself before calling this
 * macro.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 */

/*!\def fid_DYNARRAY_PUSH_UNSAFE
 * \brief Append element to dynamic array, without size check.
 *
 * This macro, like fid_DYNARRAY_PUSH(), does not store anything to the array.
 * It is just a convenience macro that increases the \c occupied member of the
 * array. After that, the element at index \c occupied is considered free and
 * can be written to. This macro is used to be make the code more readable, and
 * also to emphasize that a simple increment of the \c occupied variable is not
 * safe in general.
 *
 * Use fid_DYNARRAY_PUSH() if the dynamic array may need to be resized. Also
 * check if fid_DYNARRAY_APPEND() or fid_DYNARRAY_APPEND_UNSAFE() are more
 * appropriate for your application.
 *
 * Never use this macro unless you are absolutely sure that there is enough
 * free space in the array. The debug version may catch an attempt to write
 * beyond array bounds, but the non-debug version will \e not.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 */

/*!\def fid_DYNARRAY_POP_ELEM
 * \brief Assign last element of dynamic array to some variable, and remove it
 * from the array.
 *
 * This macro is unsafe. If the array is empty, your program will not do the
 * correct thing since it will try to read the non-existent element beyond the
 * lower array boundary. The debug version might catch such a mistake, the
 * non-debug version will \e not.
 *
 * Also think if the assignment performed by this macro is correct for your
 * application, or if the use of some copy function would be more approriate,
 * or if you could simply use the array element directly.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 * \param VAL A variable of type \p TYPE that the last element of the dynamic
 * array is assigned to.
 */

/*!\def fid_DYNARRAY_POP
 * \brief Remove last element from dynamic array.
 *
 * Like fid_DYNARRAY_PUSH_UNSAFE(), this macro is a convenience macro that
 * simply decrements the \c occupied member of the passed dynamic array. And
 * like fid_DYNARRAY_POP_ELEM(), it is also unsafe to use it on empty arrays.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 */

#ifdef DEBUG
#define fid_DYNARRAY_SIZECHECK(ARRAY,TYPE) assert((ARRAY)->size_of_type == sizeof(TYPE))

#define fid_DYNARRAY_DECLARE(NAME,TYPE)\
  typedef struct\
  {\
    size_t occupied;     /*!<\brief Number of elements in use. */\
    size_t allocated;    /*!<\brief Number of elements for which space has
                          * been allocated. */\
    TYPE *dyndata;       /*!<\brief Pointer to array data. */\
    TYPE *prev_dyndata;  /*!<\brief Temporary used before calling
                          * \c realloc() to prevent leaking memory. */\
    size_t size_of_type; /*!<\brief Size of stored type for additional checks
                          * (only available in debug mode). */\
  } NAME

#define fid_DYNARRAY_INIT(ARRAY,TYPE)\
  (ARRAY)->occupied=(ARRAY)->allocated=0;\
  (ARRAY)->dyndata=(TYPE *)NULL;\
  (ARRAY)->prev_dyndata=(TYPE *)NULL;\
  (ARRAY)->size_of_type=sizeof(TYPE)

#define fid_DYNARRAY_FREE(ARRAY,TYPE)\
  fid_DYNARRAY_SIZECHECK(ARRAY,TYPE);\
  if(((ARRAY)->dyndata) != NULL)\
  {\
    free((ARRAY)->dyndata);\
  }\
  (ARRAY)->size_of_type=0

#define fid_DYNARRAY_PUSH_UNSAFE(ARRAY,TYPE)\
  fid_DYNARRAY_SIZECHECK(ARRAY,TYPE);\
  assert((ARRAY)->occupied < (ARRAY)->allocated);\
  ++(ARRAY)->occupied

#define fid_DYNARRAY_POP_ELEM(ARRAY,TYPE,VAL)\
  fid_DYNARRAY_SIZECHECK(ARRAY,TYPE);\
  assert((ARRAY)->occupied > 0);\
  (VAL)=(ARRAY)->dyndata[--(ARRAY)->occupied]

#define fid_DYNARRAY_POP(ARRAY,TYPE)\
  fid_DYNARRAY_SIZECHECK(ARRAY,TYPE);\
  assert((ARRAY)->occupied > 0);\
  --(ARRAY)->occupied
#else /* !DEBUG */
#define fid_DYNARRAY_SIZECHECK(ARRAY,TYPE)

#define fid_DYNARRAY_DECLARE(NAME,TYPE)\
  typedef struct\
  {\
    size_t occupied;     /*!<\brief Number of elements in use. */\
    size_t allocated;    /*!<\brief Number of elements for which space has
                          * been allocated. */\
    TYPE *dyndata;       /*!<\brief Pointer to array data. */\
    TYPE *prev_dyndata;  /*!<\brief Temporary used before calling\
                          * \c realloc() to prevent leaking memory. */\
  } NAME

#define fid_DYNARRAY_INIT(ARRAY,TYPE)\
  (ARRAY)->occupied=(ARRAY)->allocated=0;\
  (ARRAY)->dyndata=(TYPE *)NULL;\
  (ARRAY)->prev_dyndata=(TYPE *)NULL

#define fid_DYNARRAY_FREE(ARRAY,TYPE)\
  fid_DYNARRAY_SIZECHECK(ARRAY,TYPE);\
  if(((ARRAY)->dyndata) != NULL)\
  {\
    free((ARRAY)->dyndata);\
  }

#define fid_DYNARRAY_PUSH_UNSAFE(ARRAY,TYPE)\
  ++(ARRAY)->occupied

#define fid_DYNARRAY_POP_ELEM(ARRAY,TYPE,VAL)\
  (VAL)=(ARRAY)->dyndata[--(ARRAY)->occupied]

#define fid_DYNARRAY_POP(ARRAY,TYPE)\
  --(ARRAY)->occupied
#endif /* DEBUG */

/*!
 * \brief Make sure that there are at least \e n free elements in dynamic array.
 *
 * If you know that you will store a certain number of elements inside a
 * dynamic array, then this macro can be used to pre-allocate this number of
 * elements. After a successful call of this macro, the next \p N elements,
 * starting at index \c occupied, can be used safely. This means that macros
 * like fid_DYNARRAY_PUSH_UNSAFE() or fid_DYNARRAY_APPEND_UNSAFE() can be used
 * safely up to \p N times in order to access the next \p N elements of the
 * array.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 * \param N The number of elements needed.
 * \param FAILCODE When there is not enough memory left, then this code is
 * executed.
 *
 * \note In case of failure, the \c dyndata pointer will still point to the old
 * memory block, which is left intact.
 */
#define fid_DYNARRAY_ENSURE_NFREE(ARRAY,TYPE,N,FAILCODE)\
  fid_DYNARRAY_SIZECHECK(ARRAY,TYPE);\
  if((ARRAY)->occupied+(N) > (ARRAY)->allocated)\
  {\
    (ARRAY)->allocated=(ARRAY)->occupied+(N);\
    (ARRAY)->prev_dyndata=(ARRAY)->dyndata;\
    (ARRAY)->dyndata=(TYPE *)realloc((ARRAY)->dyndata,sizeof(TYPE)*(ARRAY)->allocated);\
    if((ARRAY)->dyndata == NULL)\
    {\
      (ARRAY)->dyndata=(ARRAY)->prev_dyndata;\
      FAILCODE\
    }\
  }\
  assert((ARRAY)->dyndata != NULL)

/*!
 * \brief Make sure that the next element in a dynamic array can be written to.
 *
 * Before writing to the next element in a dynamic array, use this macro to
 * make sure that this next element can actually be used. When running out of
 * space, the macro resizes the dynamic array so that \p N more elements can be
 * stored. After a successful call of this macro, the element at index
 * \c occupied can safely be written to, and unsafe macros like
 * fid_DYNARRAY_PUSH_UNSAFE() may be used once (in that case, however, you may
 * use fid_DYNARRAY_PUSH() as well).
 *
 * Use fid_DYNARRAY_ENSURE_NFREE() to pre-allocate a specific number of free
 * elements.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 * \param N The number of elements to add to the array when running out of
 * space. This value should be an estimate on how many elements are likely to
 * be added while the application is running. The greater this value, the fewer
 * reallocations will happen, which is good for performance, but the more space
 * is likely to be wasted.
 * \param FAILCODE When there is not enough memory left, then this code is
 * executed.
 *
 * \note In case of failure, the \c dyndata pointer will still point to the old
 * memory block, which is left intact.
 */
#define fid_DYNARRAY_ENSURE_NEXT(ARRAY,TYPE,N,FAILCODE)\
  fid_DYNARRAY_SIZECHECK(ARRAY,TYPE);\
  if((ARRAY)->occupied >= (ARRAY)->allocated)\
  {\
    (ARRAY)->allocated=(ARRAY)->occupied+(N);\
    (ARRAY)->prev_dyndata=(ARRAY)->dyndata;\
    (ARRAY)->dyndata=(TYPE *)realloc((ARRAY)->dyndata,sizeof(TYPE)*(ARRAY)->allocated);\
    if((ARRAY)->dyndata == NULL)\
    {\
      (ARRAY)->dyndata=(ARRAY)->prev_dyndata;\
      FAILCODE\
    }\
  }\
  assert((ARRAY)->dyndata != NULL)

/*!
 * \brief Initialize a dynamic array, pre-allocate some elements.
 *
 * Like fid_DYNARRAY_INIT(), but also make sure that the first \p N elements
 * can be written to. The \p occupied member is initialized to point beyond the
 * last free element (i.e., set to the same value as \p allocated) so as to
 * mark all elements used.
 *
 * This macro makes using dynamic arrays convenient to use as fixed-size arrays
 * (that may, of course, be enlarged later), when the number of elements, or
 * initial number of elements, is known in advance.
 *
 * \note Unsafe macros that rely on the value of \c occupied being smaller than
 * \c allocated, like fid_DYNARRAY_APPEND_UNSAFE(), must \e not be used on
 * pre-allocated arrays initialized by this macro. Using macros like
 * fid_DYNARRAY_APPEND() may not work as expected since they will write
 * \e after the pre-allocated elements.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 * \param N The number of elements to allocate.
 * \param FAILCODE When there is not enough memory left, then this code is
 * executed.
 */
#define fid_DYNARRAY_INIT_PREALLOC(ARRAY,TYPE,N,FAILCODE)\
  fid_DYNARRAY_INIT(ARRAY,TYPE);\
  fid_DYNARRAY_ENSURE_NFREE(ARRAY,TYPE,N,FAILCODE);\
  (ARRAY)->occupied=(ARRAY)->allocated

/*!
 * \brief Append a value to a dynamic array.
 *
 * This is a shortcut for a call of fid_DYNARRAY_ENSURE_NEXT(), followed by an
 * assignment of a value to the last element of a dynamic array, followed by an
 * increment of \c occupied.
 *
 * The passed value \p VAL is stored via assigment. If this is not enough,
 * i.e., you need to call some copy function to make a real copy, then may
 * prefer to use fid_DYNARRAY_PUSH(), which does the same as this macro does,
 * but omits the assigment.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 * \param N The number of elements to add to the array when running out of
 * space. See fid_DYNARRAY_ENSURE_NEXT().
 * \param VAL The value of type \p TYPE to be stored.
 * \param FAILCODE When there is not enough memory left, then this code is
 * executed.
 *
 * \note In case of failure, the \c dyndata pointer will still point to the old
 * memory block, which is left intact.
 */
#define fid_DYNARRAY_APPEND(ARRAY,TYPE,N,VAL,FAILCODE)\
  fid_DYNARRAY_ENSURE_NEXT(ARRAY,TYPE,N,FAILCODE);\
  (ARRAY)->dyndata[(ARRAY)->occupied++]=(VAL)

/*!
 * \brief Append a value to a dynamic array, without size check.
 *
 * Just like fid_DYNARRAY_APPEND(), but omit the check if there is enough space
 * left.
 *
 * Never use this macro unless you are absolutely sure that there is enough
 * free space in the array. The debug version may catch an attempt to write
 * beyond array bounds, but the non-debug version will \e not.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 * space. See fid_DYNARRAY_ENSURE_NEXT().
 * \param VAL The value of type \p TYPE to be stored.
 */
#define fid_DYNARRAY_APPEND_UNSAFE(ARRAY,TYPE,VAL)\
  fid_DYNARRAY_SIZECHECK(ARRAY,TYPE);\
  (ARRAY)->dyndata[(ARRAY)->occupied++]=(VAL)

/*!
 * \brief Append element to dynamic array.
 *
 * Simpler alternative for fid_DYNARRAY_APPEND() when assignment is not good
 * enough. After a successful call of this macro, you may want to use
 * fid_DYNARRAY_TOPELEM() to initialize the new element.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 * \param N The number of elements to add to the array when running out of
 * space. See fid_DYNARRAY_ENSURE_NEXT().
 * \param FAILCODE When there is not enough memory left, then this code is
 * executed.
 *
 * \note In case of failure, the \c dyndata pointer will still point to the old
 * memory block, which is left intact.
 */
#define fid_DYNARRAY_PUSH(ARRAY,TYPE,N,FAILCODE)\
  fid_DYNARRAY_ENSURE_NEXT(ARRAY,TYPE,N,FAILCODE);\
  fid_DYNARRAY_PUSH_UNSAFE(ARRAY,TYPE)

/*!
 * \brief Access last/top element in dynamic array.
 *
 * Just a shortcut macro to make the code more readable. It is safe to put a
 * <code>&</code> in front of this macro to obtain a pointer to the last
 * element. (Bear in mind, however, that this pointer may quickly become
 * invalid when the array needs to be reallocated.)
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 */
#define fid_DYNARRAY_TOPELEM(ARRAY,TYPE)\
  ((ARRAY)->dyndata[(ARRAY)->occupied-1])

/*!
 * \brief Reallocate array so that it does not use up more space than necessary.
 *
 * If the array is empty, i.e., \c occupied is zero, then the array will be
 * freed and re-initialized.
 *
 * \param ARRAY A pointer to a dynamic array.
 * \param TYPE Type of the elements stored inside the dynamic array.
 * \param FAILCODE When reallocation failed, then this code is executed.
 *
 * \note In case of failure, the \c dyndata pointer will still point to the old
 * memory block, which is left intact. This means, it might be safe to ignore
 * a failure, and keep your program running even in case of failure since all
 * data will still be there, and the rest of the dynamic array will remain
 * unchanged.
 */
#define fid_DYNARRAY_SHRINK(ARRAY,TYPE,FAILCODE)\
  if((ARRAY)->occupied > 0)\
  {\
    if((ARRAY)->allocated > (ARRAY)->occupied)\
    {\
      (ARRAY)->prev_dyndata=(ARRAY)->dyndata;\
      (ARRAY)->dyndata=(TYPE *)realloc((ARRAY)->dyndata,sizeof(TYPE)*(ARRAY)->occupied);\
      if((ARRAY)->dyndata != NULL)\
      {\
        (ARRAY)->allocated=(ARRAY)->occupied;\
      }\
      else\
      {\
        (ARRAY)->dyndata=(ARRAY)->prev_dyndata;\
        FAILCODE\
      }\
    }\
  }\
  else\
  {\
    fid_DYNARRAY_FREE(ARRAY,TYPE);\
    fid_DYNARRAY_INIT(ARRAY,TYPE);\
  }

/*!
 * \brief Dynamic array of <code>char</code> values.
 */
fid_DYNARRAY_DECLARE(fid_Arraychar,char);

/*!
 * \brief Dynamic array of <code>unsigned char</code> values.
 */
fid_DYNARRAY_DECLARE(fid_Arrayuchar,unsigned char);

/*!
 * \brief Dynamic array of <code>fid_Uint32</code> values.
 */
fid_DYNARRAY_DECLARE(fid_ArrayUint32,fid_Uint32);

/*!
 * \brief Dynamic array of <code>fid_Uint64</code> values.
 */
fid_DYNARRAY_DECLARE(fid_ArrayUint64,fid_Uint64);

/*!
 * \brief Dynamic array of <code>fid_Sint32</code> values.
 */
fid_DYNARRAY_DECLARE(fid_ArraySint32,fid_Sint32);

/*!
 * \brief Dynamic array of <code>fid_Sint64</code> values.
 */
fid_DYNARRAY_DECLARE(fid_ArraySint64,fid_Sint64);
/*@}*/

/*!\addtogroup alphabets Alphabet handling
 * Conversion between printable characters and binary symbols.
 */
/*@{*/
#ifdef __cplusplus
#include <climits>
#else /* !__cplusplus */
#include <limits.h>
#endif /* __cplusplus */

/*!
 * \brief Use this type to denote a binary transformed sequence symbol.
 * 
 * This type has been introduced for pure documentary reasons. There is no,
 * and probably never will be, any form of wide character or unicode support.
 * It would be safe to use <code>unsigned char</code> all the time, but using
 * \c fid_Symbol instead makes the code much more readable and
 * understandable.
 */
typedef unsigned char fid_Symbol;

/*!
 * \brief Format string for printing the numeric value of a #fid_Symbol.
 */
#define fid_SYMFMT  "%hhu"

/*!
 * \brief An array of symbols, i.e., a sequence of dynamic size.
 */
fid_DYNARRAY_DECLARE(fid_ArraySymbol,fid_Symbol);

/*!
 * \brief Special symbol: sequence separator.
 */
#define fid_SEPARATOR  ((fid_Symbol)UCHAR_MAX)

/*!
 * \brief Special symbol: wildcard character.
 */
#define fid_WILDCARD   ((fid_Symbol)(UCHAR_MAX-1))

/*!
 * \brief Special symbol: undefined symbol.
 */
#define fid_UNDEF      ((fid_Symbol)(UCHAR_MAX-2))

/*!
 * \brief Maximum allowed value for a symbol.
 * 
 * Note that this is not the maximum \em number of symbols, but the maximum
 * allowed \em value of a symbol.
 */
#define fid_SYMBOLMAX  ((fid_Symbol)(UCHAR_MAX-3))

/*!
 * \brief Check whether symbol \p S is a sequence symbol or not.
 *
 * Note that undefined characters are considered regular symbols, but wildcards
 * and sequence separators are not.
 *
 * \param S A symbol of type #fid_Symbol.
 */
#define fid_REGULARSYMBOL(S)  ((S) <= fid_UNDEF)

/*!
 * \brief The opposite of fid_REGULARSYMBOL().
 */
#define fid_SPECIALSYMBOL(S)  ((S) > fid_UNDEF)

/*!
 * \brief Definition of an alphabet.
 * 
 * The structure basically consists of two arrays, one for mapping
 * characters to symbols, and one for mapping symbols to characters. Its
 * purpose is to map printable characters to binary symbols, which are
 * consecutive numbers. There are always as many or more characters than
 * there are symbols. At each position in fid_Alphabet::char_to_sym the
 * corresponding symbol is stored, and at each position in
 * fid_Alphabet::sym_to_char one of the corresponding character is stored.
 * The set of characters that are mapped to the same symbol are called a
 * character class. The latter is only useful for printing out sequences
 * to present them to the user, all algorithms should work on the binary
 * representation.
 * 
 * Note that there is no entry for fid_SEPARATOR in the array of symbols.
 * Also note that up to fid_SYMBOLMAX symbols can be supported. The reason
 * to have UCHAR_MAX+1 entries in the array of characters is that there are
 * only 256 characters in the ASCII character set. It could easily be
 * extended to contain much more entries to support larger character sets,
 * but the array of symbols cannot grow as easily unless one would accept
 * that a symbol occupies more than one byte. A larger range of characters
 * would hence imply that they form up to fid_SYMBOLMAX character classes.
 */
typedef struct
{
  fid_Uint16 num_of_chars;  /*!<\brief Number of printable characters defined by
                             * this alphabet. */
  fid_Uint16 num_of_syms;   /*!<\brief Number of symbols defined by this
                             * alphabet. */
  fid_Symbol char_to_sym[UCHAR_MAX+1]; /*!<\brief Mapping from printable
                                        * characters to binary symbols. */
  char sym_to_char[fid_WILDCARD+1];    /*!<\brief Mapping from binary
                                        * symbols to printable characters. */
} fid_Alphabet;

/*!
 * \brief Transform binary symbol into its printable form, honoring specials.
 *
 * Use this macro whenver presenting alphabet encoded sequences to human
 * beings. The given symbol is transformed into its printable form, so that
 * undefined symbols and sequence separators are also printed correctly.
 *
 * \param ALPHA A pointer to a fid_Alphabet structure.
 * \param S A symbol encoded by and to be decoded via alphabet \p ALPHA.
 *
 * \returns A printable character.
 */
#define fid_PRINT_SYMBOL(ALPHA,S)\
  ((S) == fid_UNDEF\
     ?'~'\
     :((S) == fid_SEPARATOR\
         ?'|'\
         :(ALPHA)->sym_to_char[(size_t)(S)]))

/*!
 * \brief Type cast printable character into unsigned array index.
 *
 * Converting a signed char into some bigger unsigned type can go very wrong
 * if not done carefully. This macro is careful. Use it for accessing
 * fid_Alphabet::char_to_sym.
 *
 * \param C A printable character, type \c char.
 * 
 * \returns An array index.
 */
#define fid_CHAR_AS_INDEX(C)  ((size_t)((unsigned char)(C)))

/*!
 * \brief Identifiers of built-in alphabets.
 * 
 * An alphabet structure can be initialized by a library function to
 * define one of the standard alphabets supported by the library.
 */
typedef enum
{
  fid_ALPHABET_DNA, /*!<\brief Standard DNA alphabet with common wildcards. */
  fid_ALPHABET_RNA, /*!<\brief Standard RNA alphabet with common wildcards. */
  fid_ALPHABET_DNARNA,  /*!<\brief Mixed DNA and RNA alphabet, with \e T and 
                         * \e U being defined equivalent. */
  fid_ALPHABET_PROTEIN  /*!<\brief Standard amino acid alphabet. */
} fid_Alphabettype;

#ifdef __cplusplus
extern "C" {
#endif
int fid_alphabet_init_from_speclines(fid_Alphabet *alpha, const char *str,
                                     size_t len, fid_Error *error);
int fid_alphabet_init_from_specfile(fid_Alphabet *alpha, const char *filename,
                                     fid_Error *error);
int fid_alphabet_init_from_string(fid_Alphabet *alpha, const char *string,
                                  size_t length, fid_Error *error);
void fid_alphabet_init_standard(fid_Alphabet *alpha, fid_Alphabettype type);
int fid_alphabet_add_wildcard(fid_Alphabet *alpha, char wcchar,
                              fid_Error *error);
size_t fid_alphabet_transform_string(const fid_Alphabet *alpha,
                                     const char *string, size_t length,
                                     fid_Symbol *transformed,
                                     int no_special_symbols);
size_t fid_alphabet_transform_string_inplace(const fid_Alphabet *alpha,
                                             char *string, size_t length,
                                             int no_special_symbols);
/*@null@*/
fid_Symbol *fid_alphabet_transform_string_new(const fid_Alphabet *alpha,
                                              const char *string,
                                              size_t length,
                                              int no_special_symbols,
                                              fid_Error *error);
int fid_alphabet_write_to_file(const fid_Alphabet *alpha,
                               const char *basefilename,
                               fid_Error *error);
void fid_alphabet_dump(const fid_Alphabet *alpha, FILE *stream);
#ifdef __cplusplus
}
#endif
/*@}*/

/*!
 * \brief Information about database file.
 *
 * This structure is used in conjunction with #fid_Projectfile. It stores
 * information about the input files that were used to produce the enhanced
 * suffix array project.
 */
typedef struct
{
  char *name;                  /*!<\brief Name of the database file. */
  fid_Uint64 inputfile_length; /*!<\brief Length of the database file. */
  fid_Uint64 sequence_length;  /*!<\brief Length of the sequence portion in
                                * the database file. */
} fid_DBfileinfo;

/*!\brief An array of database file information structures. */
fid_DYNARRAY_DECLARE(fid_DBfiles,fid_DBfileinfo);

void fid_dbfiles_free(fid_DBfiles *dbfiles);
int fid_dbfiles_append(fid_DBfiles *dbfiles, char *name, fid_Uint64 inputlen,
                       fid_Uint64 seqlen, fid_Error *error);
int fid_dbfiles_append_copy(fid_DBfiles *dbfiles, char *name,
                            fid_Uint64 inputlen, fid_Uint64 seqlen,
                            fid_Error *error);
int fid_dbfiles_copy(fid_DBfiles *dest, const fid_DBfiles *src,
                     fid_Error *error);
void fid_dbfiles_dump(const fid_DBfiles *dbfiles, FILE *stream);
/*!\addtogroup files File handling routines
 * This group contains various low-level and high-level routines for
 * operating on files.
 */
/*@{*/
/*!
 * \brief Representation of a memory mapped file.
 * 
 * Among others, this structure bundles the file name, a file descriptor,
 * and the pointer returned by mmap() to the file content. The protection
 * and flags argument passed to mmap() are also stored for re-mapping.
 * 
 * Additionally, this structure allows the use of a memory mapped file as
 * a dynamically growing array. The current size of real data is stored in
 * fid_Mappedfile::occupied, whereas the allocated disk space is stored in
 * fid_Mappedfile::allocated. Whenever fid_Mappedfile::occupied exceeds
 * fid_Mappedfile::allocated, enlarge the file, re-map, and continue.
 *
 * To support the use of allocated dynamic arrays as mapped files, the concept
 * of fake mapped files has been implemented. Fake mapped files do not have a
 * valid file descriptor, but point to allocated memory. Most functions defined
 * on the fid_Mappedfile structure, however, do not work for fake mapped files,
 * so their use is somewhat limited. Their main purpose is to enable existing
 * code written for reading from memory mapped files to read from allocated
 * memory. A fake mapped file can be identified by using the macro
 * fid_MAPPEDFILE_IS_FAKED() on a fid_Mappedfile structure pointer.
 */
typedef struct
{
  unsigned char *content;  /*!<\brief Memory mapped file content. */
  size_t occupied;     /*!<\brief Number of bytes used in file. */
  size_t allocated;    /*!<\brief Allocated number of bytes in file. */
  int fd;              /*!<\brief File descriptor as returned by open(). */
  char *filename;      /*!<\brief Name of the file in file system, mainly used
                        * for diagnostic messages. */
  int mmap_prot;       /*!<\brief Protection flags as passed to last call of
                        * mmap() for this file; used or modified for mapping
                        * the file again at some later point. */
  int mmap_flags;      /*!<\brief Other flags as passed to last call of mmap()
                        * for this file. \see fid_Mappedfile::mmap_prot. */
} fid_Mappedfile;

/*!
 * \brief Buffer for creating filenames from basename and extension.
 * 
 * The allocated buffer pointed to by fid_Filenamebuffer::buffer contains a
 * basename, like "/tmp/myindex", followed by a trailing dot, ".". The
 * buffer's size corresponds to the length of the basename plus 5, one for
 * the appended dot, 3 for the extension, and 1 for the terminator. Pointer
 * fid_Filenamebuffer::bufptr points two bytes after the end of the basename
 * and hence right after the dot, into fid_Filenamebuffer::buffer.
 * 
 * This construction is meant to enable easy and fast filename construction.
 * Given a filename extension with a length of three characters, this
 * extension can simply be copied to fid_Filenamebuffer::bufptr to create a
 * full filename.
 */
typedef struct
{
  char *buffer; /*!<\brief An allocated buffer that holds a filename. */
  char *bufptr; /*!<\brief A pointer into fid_Filenamebuffer::buffer that
                 * points right after the "." character to enable fast copying
                 * of file name extensions to the end of the base name. */
} fid_Filenamebuffer;

/*!
 * \brief Number of bytes to append to a file when running out of space.
 */
#define fid_MAPPEDFILE_GROWSIZE  ((size_t)524288)

/*!
 * \brief Cast pointer into pointer of different type.
 * 
 * With gcc 3.1, a bare cast of unsigned chars to unsigned integers yields a
 * warning (cast increases required alignment of target type), hence we add
 * a prior cast to \c void to shut up the compiler in these cases.
 */
#define fid_CAST_POINTER(P,T)  ((T *)((void *)(P)))

/*!
 * \brief Check if the mapped file is writable.
 *
 * The check is performed by checking the fid_Mappedfile::mmap_prot field.
 *
 * \param MF A pointer to a fid_Mappedfile structure.
 *
 * \returns True if the file is writable, false otherwise.
 */
#define fid_MAPPEDFILE_IS_WRITABLE(MF)  (((MF)->mmap_prot&PROT_WRITE) != 0)

/*!
 * \brief Check if the mapped file is really a file or not.
 *
 * \param MF A pointer to a fid_Mappedfile structure.
 * 
 * \returns True if the file is not a real file, but a faked one. This means,
 * the content pointer really points to allocated memory that has been obtained
 * via \c malloc(), not via \c mmap().
 */
#define fid_MAPPEDFILE_IS_FAKED(MF) ((MF)->content != NULL && (MF)->fd == -1)

/*!
 * \brief Make sure that the mapped file has space to store \p N bytes.
 * 
 * When running out of space, i.e., if the file is too short to store \p N
 * bytes, the file length will be increased by \p GROW bytes.
 * 
 * \param MF A pointer to a fid_Mappedfile structure.
 * \param N Requested size.
 * \param GROW Number of bytes to add to mapped file if too short.
 * \param ERR Pointer to a fid_Error structure.
 * \param ERRCODE Code to be executed in case of an error condition.
 *
 * \see For this macro makes use of fid_file_grow_by_size(), you may consider
 * reading the notes attached to that function.
 */
#define fid_MAPPEDFILE_CHECKSPACE(MF,N,GROW,ERR,ERRCODE)\
  if((MF)->occupied+(N) > (MF)->allocated)\
  {\
    if(fid_file_grow_by_size(MF,GROW,ERR) == -1)\
    {\
      ERRCODE;\
    }\
  }

/*!
 * \brief Append value of given type to end of file.
 * 
 * Use this macro only if you are absolutely sure that the file's allocated
 * size is not exceeded by appending the value to it. Use
 * fid_MAPPEDFILE_APPEND_GROW() or fid_MAPPEDFILE_APPEND_TRY() otherwise.
 * 
 * \param MF A pointer to a fid_Mappedfile structure.
 * \param TYPE Type of the value to be stored. This should be a basic C
 * type, no structure.
 * \param VAL The value to be stored. It will be casted to \p TYPE by this
 * macro.
 */
#define fid_MAPPEDFILE_APPEND_UNSAFE(MF,TYPE,VAL)\
  *fid_CAST_POINTER(&(MF)->content[(MF)->occupied],TYPE)=(TYPE)(VAL);\
  (MF)->occupied+=sizeof(TYPE)

/*!
 * \brief Append value of given type to end of file, increase size when needed.
 * 
 * When running out of space, i.e., if the file is too short to store the
 * given value, the file length will be increased by #fid_MAPPEDFILE_GROWSIZE
 * bytes.
 * 
 * \param MF A pointer to a fid_Mappedfile structure.
 * \param TYPE Type of the value to be stored. This should be a basic C
 * type, no structure.
 * \param VAL The value to be stored. It will be casted to \p TYPE by this
 * macro.
 * \param ERR Pointer to a fid_Error structure.
 * \param ERRCODE Code to be executed is case of an error condition.
 */
#define fid_MAPPEDFILE_APPEND_GROW(MF,TYPE,VAL,ERR,ERRCODE)\
  fid_MAPPEDFILE_CHECKSPACE(MF,sizeof(TYPE),fid_MAPPEDFILE_GROWSIZE,ERR,ERRCODE)\
  fid_MAPPEDFILE_APPEND_UNSAFE(MF,TYPE,VAL);\

/*!
 * \brief Append value of given type to end of file, increase size when needed.
 * 
 * When running out of space, i.e., if the file is too short to store the
 * given value, the file length will be increased by #fid_MAPPEDFILE_GROWSIZE
 * bytes. The macro does nothing if the file content is \c NULL, i.e., if no
 * open file is associated with \p MF, hence this macro is safe to be used at
 * any time.
 * 
 * \param MF A pointer to a fid_Mappedfile structure.
 * \param TYPE Type of the value to be stored. This should be a basic C
 * type, no structure.
 * \param VAL The value to be stored. It will be casted to \p TYPE by this
 * macro.
 * \param ERR Pointer to a fid_Error structure.
 * \param ERRCODE Code to be executed is case of an error condition.
 */
#define fid_MAPPEDFILE_APPEND_TRY(MF,TYPE,VAL,ERR,ERRCODE)\
  if((MF)->content != NULL)\
  {\
    fid_MAPPEDFILE_APPEND_GROW(MF,TYPE,VAL,ERR,ERRCODE);\
  }

#ifdef __cplusplus
extern "C" {
#endif
int fid_file_map(fid_Mappedfile *mfile, const char *filename, int writable,
                 int may_prefetch, fid_Error *error);
int fid_file_new(fid_Mappedfile *mfile, const char *filename, fid_Error *error);
int fid_file_allocate(fid_Mappedfile *mfile, const char *filename,
                      size_t size, fid_Error *error);
void fid_file_fake(fid_Mappedfile *mfile, void *block, size_t size);
int fid_file_grow_by_size(fid_Mappedfile *mfile, size_t size,
                          fid_Error *error);
int fid_file_ensure_free_space(fid_Mappedfile *mfile, size_t size,
                               fid_Error *error);
#ifdef __GNUC__
int fid_file_write(fid_Mappedfile *mfile, fid_Error *error,
                   const char *fmt, ...)
  __attribute__ ((format (printf, 3, 4)));
#else /* !__GNUC__ */
int fid_file_write(fid_Mappedfile *mfile, fid_Error *error,
                   const char *fmt, ...);
#endif /* __GNUC__ */
int fid_file_dump_to_file(const fid_Mappedfile *mfile, const char *filename,
                          fid_Error *error);
int fid_file_make_readonly(fid_Mappedfile *mfile, fid_Error *error);
void fid_file_cleanup(fid_Mappedfile *mfile);
void fid_file_prefetch(const fid_Mappedfile *mfile, int smart);
void fid_file_unmap(fid_Mappedfile *mfile);
int fid_filenamebuffer_init(fid_Filenamebuffer *fnamebuf, const char *filename,
                            fid_Error *error);
int fid_filenamebuffer_init_local(fid_Filenamebuffer *fnamebuf,
                                  fid_Filenamebuffer **extbuffer,
                                  const char *filename, fid_Error *error);
void fid_filenamebuffer_free(fid_Filenamebuffer *fnamebuf);
/*@null@*/
char *fid_filename_create(const char *basefilename, const char *fileext,
                          fid_Error *error);
#ifdef __cplusplus
}
#endif
/*@}*/

/*!\addtogroup misc */
/*@{*/
#ifdef __cplusplus
extern "C" {
#endif
int fid_utils_parse_uint_32(const char *string, fid_Uint32 *ui,
                            fid_Error *error);
int fid_utils_parse_uint_64(const char *string, fid_Uint64 *ui,
                            fid_Error *error);
#ifdef __cplusplus
}
#endif
/*@}*/

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
/*!\copydoc #fid_sequences_offset_to_index
 * \brief 32 bit version of #fid_sequences_offset_to_index(). */
fid_Uint32 fid_sequences_offset_to_index_32(const fid_Sequences *seqs,
                                      fid_Uint32 offset);
/*!\copydoc #fid_sequences_offset_to_index
 * \brief 64 bit version of #fid_sequences_offset_to_index(). */
fid_Uint64 fid_sequences_offset_to_index_64(const fid_Sequences *seqs,
                                      fid_Uint64 offset);
/*!\copydoc #fid_sequences_index_to_boundaries
 * \brief 32 bit version of #fid_sequences_index_to_boundaries(). */
void fid_sequences_index_to_boundaries_32(const fid_Sequences *seqs,
                                          fid_Uint32 seqindex,
                                          fid_Uint32 *left, fid_Uint32 *right);
/*!\copydoc #fid_sequences_index_to_boundaries
 * \brief 64 bit version of #fid_sequences_index_to_boundaries(). */
void fid_sequences_index_to_boundaries_64(const fid_Sequences *seqs,
                                          fid_Uint64 seqindex,
                                          fid_Uint64 *left, fid_Uint64 *right);
/*!\copydoc #fid_sequences_offset_to_boundaries
 * \brief 32 bit version of #fid_sequences_offset_to_boundaries(). */
void fid_sequences_offset_to_boundaries_32(const fid_Sequences *seqs,
                                           fid_Uint32 offset,
                                           fid_Uint32 *left, fid_Uint32 *right);
/*!\copydoc #fid_sequences_offset_to_boundaries
 * \brief 64 bit version of #fid_sequences_offset_to_boundaries(). */
void fid_sequences_offset_to_boundaries_64(const fid_Sequences *seqs,
                                           fid_Uint64 offset,
                                           fid_Uint64 *left, fid_Uint64 *right);
/*!\copydoc #fid_sequences_iterate_range
 * \brief 32 bit version of #fid_sequences_iterate_range(). */
int fid_sequences_iterate_range_32(const fid_Sequences *seqs,
                                   fid_Uint32 from, fid_Uint32 to,
                                   const fid_Sequenceiterfun_32 iterfun,
                                   void *user_data);
/*!\copydoc #fid_sequences_iterate_range
 * \brief 64 bit version of #fid_sequences_iterate_range(). */
int fid_sequences_iterate_range_64(const fid_Sequences *seqs,
                                   fid_Uint64 from, fid_Uint64 to,
                                   const fid_Sequenceiterfun_64 iterfun,
                                   void *user_data);
/*!\copydoc #fid_sequences_iterate
 * \brief 32 bit version of #fid_sequences_iterate(). */
int fid_sequences_iterate_32(const fid_Sequences *seqs,
                             const fid_Sequenceiterfun_32 iterfun, void *user_data);
/*!\copydoc #fid_sequences_iterate
 * \brief 64 bit version of #fid_sequences_iterate(). */
int fid_sequences_iterate_64(const fid_Sequences *seqs,
                             const fid_Sequenceiterfun_64 iterfun, void *user_data);
void fid_sequences_compute_distribution(fid_Sequences *seqs);
void fid_sequences_free(fid_Sequences *seqs);
/*!\copydoc #fid_sequences_dump_range
 * \brief 32 bit version of #fid_sequences_dump_range(). */
void fid_sequences_dump_range_32(const fid_Symbol *seq, fid_Uint32 length,
                                 const fid_Alphabet *alpha, const char *str,
                                 int stop_at_separator, FILE *stream);
/*!\copydoc #fid_sequences_dump_range
 * \brief 64 bit version of #fid_sequences_dump_range(). */
void fid_sequences_dump_range_64(const fid_Symbol *seq, fid_Uint64 length,
                                 const fid_Alphabet *alpha, const char *str,
                                 int stop_at_separator, FILE *stream);
#ifdef __cplusplus
}
#endif

/*@}*/
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
int fid_suffixarray_traverse_32(const fid_Suffixarray *esa,
                                fid_Esatraversecallback_32 callback,
                                void *user_data, fid_Error *error);
/*!\copydoc #fid_suffixarray_traverse
 * \brief 64 bit version of #fid_suffixarray_traverse(). */
int fid_suffixarray_traverse_64(const fid_Suffixarray *esa,
                                fid_Esatraversecallback_64 callback,
                                void *user_data, fid_Error *error);
/*!\copydoc #fid_suffixarray_get_intervals
 * \brief 32 bit version of #fid_suffixarray_get_intervals(). */
fid_Uint32 fid_suffixarray_get_intervals_32(const fid_Suffixarray *esa,
                                      const fid_Suffixinterval_32 *si,
                                      fid_Uint32 *intervals);
/*!\copydoc #fid_suffixarray_get_intervals
 * \brief 64 bit version of #fid_suffixarray_get_intervals(). */
fid_Uint64 fid_suffixarray_get_intervals_64(const fid_Suffixarray *esa,
                                      const fid_Suffixinterval_64 *si,
                                      fid_Uint64 *intervals);
/*!\copydoc #fid_suffixarray_find_embedded_interval
 * \brief 32 bit version of #fid_suffixarray_find_embedded_interval(). */
int fid_suffixarray_find_embedded_interval_32(const fid_Suffixarray *esa,
                                              fid_Suffixinterval_32 *si,
                                              fid_Symbol symbol);
/*!\copydoc #fid_suffixarray_find_embedded_interval
 * \brief 64 bit version of #fid_suffixarray_find_embedded_interval(). */
int fid_suffixarray_find_embedded_interval_64(const fid_Suffixarray *esa,
                                              fid_Suffixinterval_64 *si,
                                              fid_Symbol symbol);
/*!\copydoc #fid_suffixarray_extend_interval
 * \brief 32 bit version of #fid_suffixarray_extend_interval(). */
int fid_suffixarray_extend_interval_32(const fid_Suffixarray *esa,
                                       const fid_Symbol *pattern, fid_Uint32 plen,
                                       fid_Suffixinterval_32 *si);
/*!\copydoc #fid_suffixarray_extend_interval
 * \brief 64 bit version of #fid_suffixarray_extend_interval(). */
int fid_suffixarray_extend_interval_64(const fid_Suffixarray *esa,
                                       const fid_Symbol *pattern, fid_Uint64 plen,
                                       fid_Suffixinterval_64 *si);
/*!\copydoc #fid_suffixarray_find_interval
 * \brief 32 bit version of #fid_suffixarray_find_interval(). */
int fid_suffixarray_find_interval_32(const fid_Suffixarray *esa,
                                     const fid_Symbol *pattern, fid_Uint32 plen,
                                     fid_Suffixinterval_32 *si);
/*!\copydoc #fid_suffixarray_find_interval
 * \brief 64 bit version of #fid_suffixarray_find_interval(). */
int fid_suffixarray_find_interval_64(const fid_Suffixarray *esa,
                                     const fid_Symbol *pattern, fid_Uint64 plen,
                                     fid_Suffixinterval_64 *si);
/*!\copydoc #fid_suffixarray_find_large_lcp
 * \brief 32 bit version of #fid_suffixarray_find_large_lcp(). */
fid_Uint32 fid_suffixarray_find_large_lcp_32(const fid_Suffixarray *esa,
                                       fid_Uint32 suffix);
/*!\copydoc #fid_suffixarray_find_large_lcp
 * \brief 64 bit version of #fid_suffixarray_find_large_lcp(). */
fid_Uint64 fid_suffixarray_find_large_lcp_64(const fid_Suffixarray *esa,
                                       fid_Uint64 suffix);
/*!\copydoc #fid_suffixarray_suffix_length
 * \brief 32 bit version of #fid_suffixarray_suffix_length(). */
fid_Uint32 fid_suffixarray_suffix_length_32(const fid_Suffixarray *esa,
                                      fid_Uint32 suffix);
/*!\copydoc #fid_suffixarray_suffix_length
 * \brief 64 bit version of #fid_suffixarray_suffix_length(). */
fid_Uint64 fid_suffixarray_suffix_length_64(const fid_Suffixarray *esa,
                                      fid_Uint64 suffix);
/*!\copydoc #fid_suffixarray_compute_distribution
 * \brief 32 bit version of #fid_suffixarray_compute_distribution(). */
void fid_suffixarray_compute_distribution_32(fid_Suffixarray *esa);
/*!\copydoc #fid_suffixarray_compute_distribution
 * \brief 64 bit version of #fid_suffixarray_compute_distribution(). */
void fid_suffixarray_compute_distribution_64(fid_Suffixarray *esa);
void fid_suffixarray_free(fid_Suffixarray *esa);
void fid_suffixarray_dump(const fid_Suffixarray *esa, FILE *stream);
/*!\copydoc #fid_suffixarray_dump_intervals
 * \brief 32 bit version of #fid_suffixarray_dump_intervals(). */
void fid_suffixarray_dump_intervals_32(const fid_Suffixarray *esa,
                                       const fid_Uint32 intervals[fid_INTERVALBUFFERSIZE],
                                       FILE *stream);
/*!\copydoc #fid_suffixarray_dump_intervals
 * \brief 64 bit version of #fid_suffixarray_dump_intervals(). */
void fid_suffixarray_dump_intervals_64(const fid_Suffixarray *esa,
                                       const fid_Uint64 intervals[fid_INTERVALBUFFERSIZE],
                                       FILE *stream);
/*!\copydoc #fid_suffixarray_dump_suffix
 * \brief 32 bit version of #fid_suffixarray_dump_suffix(). */
void fid_suffixarray_dump_suffix_32(const fid_Suffixarray *esa, fid_Uint32 suffix,
                                    fid_Uint32 length, FILE *stream);
/*!\copydoc #fid_suffixarray_dump_suffix
 * \brief 64 bit version of #fid_suffixarray_dump_suffix(). */
void fid_suffixarray_dump_suffix_64(const fid_Suffixarray *esa, fid_Uint64 suffix,
                                    fid_Uint64 length, FILE *stream);
/*!\copydoc #fid_suffixarray_print
 * \brief 32 bit version of #fid_suffixarray_print(). */
void fid_suffixarray_print_32(const fid_Suffixarray *esa, 
                              fid_Tablerequest request, FILE *stream);
/*!\copydoc #fid_suffixarray_print
 * \brief 64 bit version of #fid_suffixarray_print(). */
void fid_suffixarray_print_64(const fid_Suffixarray *esa, 
                              fid_Tablerequest request, FILE *stream);
/*!\copydoc #fid_suffixinterval_lcpvalue
 * \brief 32 bit version of #fid_suffixinterval_lcpvalue(). */
fid_Uint32 fid_suffixinterval_lcpvalue_32(const fid_Suffixinterval_32 *si,
                                    const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_lcpvalue
 * \brief 64 bit version of #fid_suffixinterval_lcpvalue(). */
fid_Uint64 fid_suffixinterval_lcpvalue_64(const fid_Suffixinterval_64 *si,
                                    const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_to_lcpinterval
 * \brief 32 bit version of #fid_suffixinterval_to_lcpinterval(). */
void fid_suffixinterval_to_lcpinterval_32(fid_Suffixinterval_32 *si,
                                          const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_to_lcpinterval
 * \brief 64 bit version of #fid_suffixinterval_to_lcpinterval(). */
void fid_suffixinterval_to_lcpinterval_64(fid_Suffixinterval_64 *si,
                                          const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_find_right
 * \brief 32 bit version of #fid_suffixinterval_find_right(). */
void fid_suffixinterval_find_right_32(fid_Suffixinterval_32 *si,
                                      const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_find_right
 * \brief 64 bit version of #fid_suffixinterval_find_right(). */
void fid_suffixinterval_find_right_64(fid_Suffixinterval_64 *si,
                                      const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_homepos
 * \brief 32 bit version of #fid_suffixinterval_homepos(). */
fid_Uint32 fid_suffixinterval_homepos_32(const fid_Suffixinterval_32 *si,
                                   const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_homepos
 * \brief 64 bit version of #fid_suffixinterval_homepos(). */
fid_Uint64 fid_suffixinterval_homepos_64(const fid_Suffixinterval_64 *si,
                                   const fid_Suffixarray *esa);
/*!\copydoc #fid_suffixinterval_dump
 * \brief 32 bit version of #fid_suffixinterval_dump(). */
void fid_suffixinterval_dump_32(const fid_Suffixinterval_32 *si,
                                const fid_Suffixarray *esa, FILE *stream);
/*!\copydoc #fid_suffixinterval_dump
 * \brief 64 bit version of #fid_suffixinterval_dump(). */
void fid_suffixinterval_dump_64(const fid_Suffixinterval_64 *si,
                                const fid_Suffixarray *esa, FILE *stream);
char *fid_tablerequest_to_string(fid_Tablerequest request, const char *sep);
#ifdef __cplusplus
}
#endif
/*@}*/

/*!\addtogroup suffixarrays */
/*@{*/
#ifdef __cplusplus
extern "C" {
#endif
int fid_suffixarray_load_from_project(fid_Suffixarray *esa,
                                      fid_Projectfile *project,
                                      fid_Tablerequest tables,
                                      fid_Error *error);
int fid_suffixarray_load_special(fid_Suffixarray *esa, const char *prjbasename,
                                 fid_Tablerequest tables,
                                 fid_Uintsize uisize_request,
                                 int ignore_byteorder, fid_Error *error);
int fid_suffixarray_load(fid_Suffixarray *esa, const char *prjbasename,
                         fid_Tablerequest tables, fid_Error *error);
int fid_projectfile_init_from_suffixarray(fid_Projectfile *project,
                                          const fid_DBfiles *dbfiles,
                                          const char *prjbasename,
                                          const fid_Suffixarray *esa,
                                          fid_Error *error);
#ifdef __cplusplus
}
#endif
/*@}*/

/*!\addtogroup suffixarrays */
/*@{*/
/*!
 * \brief The inner skip chain loop.
 * 
 * If the last character read from the start suffix was a special character
 * and depth is 0, then use SKIPTREE_DEPTH0_LOOP() instead to prevent
 * reading invalid memory.
 */
#define fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,REL)\
  while((NEXT) < (MAX) && (LCP) REL (DEPTH))\
  {\
    NEXT=(SKPTAB)[NEXT]+1;\
    fid_LCP(LCP,ESA,NEXT);\
  }

/*!
 * \brief The inner skip chain loop for special character at depth 0.
 */
#define fid_SKIPTREE_DEPTH0_LOOP(ESA,SKPTAB,NEXT,LCP,MAX)\
  while((NEXT) < (MAX))\
  {\
    NEXT=(SKPTAB)[NEXT]+1;\
    if((NEXT) < (MAX))\
    {\
      fid_LCP(LCP,ESA,NEXT);\
    }\
  }

/*!
 * \brief Skip tree if the last character read from the start suffix was a common character.
 */
#define fid_SKIPTREE_COMMON(ESA,SKPTAB,NEXT,START,LCP,DEPTH,MAX)\
  if((START) < (MAX))\
  {\
    (NEXT)=(START)+1;\
    fid_LCP(LCP,ESA,NEXT);\
    fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,>);\
  }\
  else\
  {\
    NEXT=(MAX);\
    fid_LCP(LCP,ESA,NEXT);\
  }

/*!
 * \brief Skip tree if the last character read from the start suffix was a special character.
 */
#define fid_SKIPTREE_SPECIAL(ESA,SKPTAB,NEXT,START,LCP,DEPTH,MAX)\
  if((START) < (MAX))\
  {\
    (NEXT)=(START)+1;\
    fid_LCP(LCP,ESA,NEXT);\
    if((DEPTH) > 0)\
    {\
      fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,>=);\
    }\
    else\
    {\
      fid_SKIPTREE_DEPTH0_LOOP(ESA,SKPTAB,NEXT,LCP,MAX);\
    }\
  }\
  else\
  {\
    NEXT=(MAX);\
    fid_LCP(LCP,ESA,NEXT);\
  }

/*!
 * \brief Skip a subtree of a virtual suffix tree via skip table.
 * 
 * The macro will always update \p NEXT and \p LCP, the other arguments are
 * used read-only.
 * 
 * \param ESA Pointer to an enhanced suffix array.
 * \param SKPTAB Pointer to the skip table (part of \p ESA). This can be
 * given separately because you may want to have a pointer around that is
 * directly pointing to the skip table instead of referencing it through
 * the fid_Suffixarray structure each time.
 * \param NEXT An fid_Uint that stores the next index in the suffix table,
 * that is the first suffix that does not belong to the skipped subtree.
 * \param START The current index into the suffix table, that is the index
 * of the suffix whose subtree should be skipped.
 * \param LCP An fid_Uint that stores the current index in the LCP table.
 * \param DEPTH The current depth in the suffix tree.
 * \param MAX The length of the sequence underlaying the suffix array.
 * The length could also be read directly from a fid_Suffixarray structure,
 * but this may take too much time, so it should have been stored in a
 * separate fid_Uint by the caller.
 * \param COMMON Boolean indicating if the last character of the start
 * suffix is a special (0) or a common (not 0) one as determined by the
 * fid_REGULARSYMBOL() and fid_SPECIALSYMBOL() macros.
 * 
*/
#define fid_SKIPTREE(ESA,SKPTAB,NEXT,START,LCP,DEPTH,MAX,COMMON)\
  if((START) < (MAX))\
  {\
    (NEXT)=(START)+1;\
    fid_LCP(LCP,ESA,NEXT);\
    if(COMMON)\
    {\
      fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,>);\
    }\
    else if((DEPTH) > 0)\
    {\
      fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,>=);\
    }\
    else\
    {\
      fid_SKIPTREE_DEPTH0_LOOP(ESA,SKPTAB,NEXT,LCP,MAX);\
    }\
  }\
  else\
  {\
    NEXT=(MAX);\
    fid_LCP(LCP,ESA,NEXT);\
  }
/*@}*/

#endif /* !LIBFID_H */
