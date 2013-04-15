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

#ifndef LIBFIDINTTYPES_H
#define LIBFIDINTTYPES_H
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
#endif /* !LIBFIDINTTYPES_H */
