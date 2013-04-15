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

#ifndef ARRAYS_H
#define ARRAYS_H
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

#endif /* !ARRAYS_H */
