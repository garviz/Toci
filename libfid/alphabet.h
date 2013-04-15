/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2006, 2007, 2009  Robert Homann
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

#ifndef ALPHABET_H
#define ALPHABET_H
/*!\addtogroup alphabets Alphabet handling
 * Conversion between printable characters and binary symbols.
 */
/*@{*/
#ifdef __cplusplus
#include <climits>
#else /* !__cplusplus */
#include <limits.h>
#endif /* __cplusplus */
#include "arrays.h"

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

#endif /* !ALPHABET_H */
