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
#include "arrays.h"
#include "alphabet.h"
#include "fileutils.h"

/*!
 * \brief Parse alphabet definition and fill alphabet structure.
 * 
 * The alphabet definition consists of multiple lines, each containing
 * characters to be considered as equal. Thus, each line defines a
 * character class. Symbols are assigned to character classes in
 * increasing order. Lines starting with a '#' character are treated as
 * comments.
 * 
 * \param alpha The structure to be filled according to the passed
 * definition.
 * \param str Alphabet definition file content.
 * \param len Length of \p str. If 0, then the length will be determined
 * using \c strlen(3).
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_alphabet_init_from_speclines(fid_Alphabet *alpha, const char *str,
                                     size_t len, fid_Error *error) /*{{{*/
{
  size_t pos=0, cindex;
  char c;

  assert(alpha != NULL);
  assert(str != NULL);
  
  if(len == 0)
  {
    len=strlen((const char *)str);
  }
  
  while(len > 0 && str[len-1] == '\n')
  {
    --len;
  }
  if(len == 0)
  {
    /* empty input */
    fid_error_throw(error,"Empty alphabet specification.");
    return -1;
  }
  
  alpha->num_of_chars=0;
  alpha->num_of_syms=0;
  memset(alpha->char_to_sym,(int)fid_UNDEF,sizeof(alpha->char_to_sym));
  memset(alpha->sym_to_char,'\0',sizeof(alpha->sym_to_char));
  
  while(pos < len)
  {
    c=str[pos];
    if(c == '#')
    {
      /* skip comment line */
      for(++pos; pos < len && str[pos] != '\n'; ++pos)
      {
        /* nothing */
      }
    }
    else if(c == '\n')
    {
      /* skip empty line */
      ++pos;
    }
    else
    {
      /* read something */
      alpha->sym_to_char[alpha->num_of_syms]=c;
      for(/* nothing */; pos < len && str[pos] != '\n'; ++pos)
      {
        c=str[pos];
        cindex=fid_CHAR_AS_INDEX(c);
        if(alpha->char_to_sym[cindex] != fid_UNDEF)
        {
          /* duplicate character */
          if(isprint((int)c))
          {
            fid_error_throw(error,"Duplicate character '%c' in alphabet "
                            "specification.",c);
          }
          else
          {
            fid_error_throw(error,"Duplicate character in alphabet "
                            "specification (non-printable 0x%02hhx).",
                            (unsigned char)c);
          }
          return -1;
        }
        ++alpha->num_of_chars;
        assert(alpha->num_of_syms <= UCHAR_MAX);
        alpha->char_to_sym[cindex]=(fid_Symbol)alpha->num_of_syms;
      }
      ++alpha->num_of_syms;
      ++pos;
    }
  }
  
  if(alpha->num_of_syms == (fid_Uint16)1)
  {
    /* parsed only one line, not enough */
    fid_error_throw(error,"Alphabet specification contains only one "
                    "line. Unary alphabets are not supported.");
    return -1;
  }
  
  /* fix wildcard mapping */
  for(pos=0; pos < sizeof(alpha->char_to_sym); ++pos)
  {
    if(alpha->char_to_sym[pos] == (fid_Symbol)(alpha->num_of_syms-1))
    {
      alpha->char_to_sym[pos]=fid_WILDCARD;
    }
  }
  alpha->sym_to_char[(size_t)fid_WILDCARD]=
    alpha->sym_to_char[alpha->num_of_syms-1];
  
  return 0;
}

/*}}}*/

/*!
 * \brief Parse alphabet definition file and fill alphabet structure.
 * 
 * The alphabet definition is read from file. See
 * fid_alphabet_init_from_speclines() for details.
 * 
 * \param alpha The structure to be filled according to the alphabet
 * definition from the file.
 * \param filename Name of the file containing an alphabet definition.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_alphabet_init_from_specfile(fid_Alphabet *alpha, const char *filename,
                                    fid_Error *error) /*{{{*/
{
  fid_Mappedfile file;
  int retcode;
  
  if((retcode=fid_file_map(&file,filename,0,0,error)) == 0)
  {
    retcode=fid_alphabet_init_from_speclines(alpha,(const char *)file.content,
                                             file.occupied,error);
    fid_file_unmap(&file);
  }
  if(retcode != 0)
  {
    fid_error_throw(error,"Could not read alphabet specification file.");
  }
  return retcode;
}

/*}}}*/

/*!
 * \brief Determine alphabet from ASCII text.
 * 
 * Any character in the passed string is put into the alphabet as regular
 * symbol. No attempt is made to decode any other text format than ASCII
 * (like UTF-8), and no wildcards will be added by this function.
 * 
 * \param alpha The structure to be filled according to the text string.
 * \param str Arbitrary text string.
 * \param len Length of \p str. If 0, then the length will be determined
 * using \c strlen(3).
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_alphabet_init_from_string(fid_Alphabet *alpha, const char *str,
                                  size_t len, fid_Error *error) /*{{{*/
{
  size_t pos;
  char c;
  
  assert(alpha != NULL);
  assert(str != NULL);
  
  if(len == 0)
  {
    len=strlen((const char *)str);
  }
  if(len == 0)
  {
    /* empty input */
    fid_error_throw(error,"Cannot generate alphabet from empty string.");
    return -1;
  }
  
  alpha->num_of_chars=0;
  alpha->num_of_syms=0;
  memset(alpha->char_to_sym,(int)fid_UNDEF,sizeof(alpha->char_to_sym));
  memset(alpha->sym_to_char,'\0',sizeof(alpha->sym_to_char));
  
  for(pos=0; pos < len; ++pos)
  {
    c=str[pos];
    if(alpha->char_to_sym[fid_CHAR_AS_INDEX(c)] == fid_UNDEF)
    {
      if(alpha->num_of_syms > fid_SYMBOLMAX)
      {
        fid_error_throw(error,"Too many distinct symbols in string, "
                        "cannot generate alphabets with more than "
                        fid_SYMFMT " symbols.",fid_SYMBOLMAX);
        return -1;
      }
      alpha->sym_to_char[alpha->num_of_syms]=c;
      alpha->char_to_sym[fid_CHAR_AS_INDEX(c)]=
        (fid_Symbol)alpha->num_of_syms;
      ++alpha->num_of_syms;
      ++alpha->num_of_chars;
    }
  }
  
  return 0;
}

/*}}}*/

/*!
 * \brief Assign standard alphabet to alphabet structure.
 * 
 * Several commonly used alphabets are defined within this library. The
 * type of the desired alphabet is selected by an alphabet identifier.
 * 
 * \param alpha The structure to be filled.
 * \param type Identifier of a standard alphabet.
 */
void fid_alphabet_init_standard(fid_Alphabet *alpha, fid_Alphabettype type) /*{{{*/
{
  static const char *alphas[]=
  {
    "aA\ncC\ngG\ntT\nnsywrkvbdhmNSYWRKVBDHM",
    "aA\ncC\ngG\nuU\nnsywrkvbdhmNSYWRKVBDHM",
    "aA\ncC\ngG\ntTuU\nnsywrkvbdhmNSYWRKVBDHM",
    "L\nV\nI\nF\nK\nR\nE\nD\nA\nG\nS\nT\nN\nQ\nY\nW\nP\nH\nM\nC\nXUBZO*-"
  };
  
  assert(type == fid_ALPHABET_DNA || type == fid_ALPHABET_RNA || type == fid_ALPHABET_DNARNA || type == fid_ALPHABET_PROTEIN);
  (void)fid_alphabet_init_from_speclines(alpha,alphas[type],0,NULL);
}

/*}}}*/

/*!
 * \brief Add wildcard character to alphabet.
 * 
 * Note that this function is very useful when initializing alphabets via
 * fid_alphabet_init_from_string().
 * 
 * \param alpha The alphabet the wildcard should be added to.
 * \param wcchar ASCII representation of the wildcard. If \p wcchar is
 * already mapped to the wildcard symbol by the alphabet, then this
 * function does not change the alphabet. If \p wcchar is already mapped
 * to some regular symbol by the alphabet, then this function returns an
 * error. Note that \p wcchar must not be 0.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_alphabet_add_wildcard(fid_Alphabet *alpha, char wcchar,
                              fid_Error *error) /*{{{*/
{
  fid_Symbol sym;
  size_t wccharindex=fid_CHAR_AS_INDEX(wcchar);
  static const char *cannot_message="Cannot add wildcard character ";
  
  assert(alpha != NULL);
  
  if(wcchar == '\0')
  {
    fid_error_throw(error,"%s'\\0' to alphabet, invalid character.",
                    cannot_message);
    return -1;
  }
  if(alpha->num_of_chars >= UCHAR_MAX)
  {
    fid_error_throw(error,"%s'%c' to alphabet, too many characters in "
                    "alphabet.",cannot_message,wcchar);
    return -1;
  }
  else if((sym=alpha->char_to_sym[wccharindex]) == fid_WILDCARD)
  {
    /* character already mapped as wildcard, just ignore this... */
    return 0;
  }
  else if(sym != fid_UNDEF)
  {
    fid_error_throw(error,"%s'%c' to alphabet, character already mapped to "
                    "symbol " fid_SYMFMT ".",
                    cannot_message,wcchar,alpha->char_to_sym[wccharindex]);
    return -1;
  }
  
  if(alpha->sym_to_char[(size_t)fid_WILDCARD] == '\0')
  {
    /* no wildcard defined so far */
    if(alpha->num_of_syms > fid_SYMBOLMAX)
    {
      fid_error_throw(error,"%s'%c' to alphabet, too many symbols in alphabet.",
                      cannot_message,wcchar);
      return -1;
    }
    
    alpha->sym_to_char[(size_t)fid_WILDCARD]=wcchar;
    alpha->sym_to_char[alpha->num_of_syms]=wcchar;
    ++alpha->num_of_syms;
  }
  
  alpha->char_to_sym[wccharindex]=fid_WILDCARD;
  ++alpha->num_of_chars;
  
  return 0;
}

/*}}}*/

/*!
 * \brief Transform string according to given alphabet.
 * 
 * \param alpha Alphabet used for transformation.
 * \param string The input string to be transformed.
 * \param length The number of characters in \p string. If 0, then the length
 * of \p string will be determined within this function. In this case the
 * string must be zero terminated.
 * \param transformed Buffer the transformed string is written to. This must
 * be of size at least the length of \p string.
 * \param no_special_symbols If unequal to 0, then stop transformation and
 * return an error if a character in \p string is transformed into a special
 * symbol.
 * 
 * \returns 0 on success, or the index+1 of the first special symbol when
 * special symbols were not allowed. Note that the transformation will be
 * incomplete if a positive value is returned.
 */
size_t fid_alphabet_transform_string(const fid_Alphabet *alpha,
                                     const char *string, size_t length,
                                     fid_Symbol *transformed,
                                     int no_special_symbols) /*{{{*/
{
  size_t i;
  
  assert(alpha != NULL);
  assert(string != NULL);
  assert(transformed != NULL);
  
  if(length == 0)
  {
    if((length=strlen(string)) == 0)
    {
      return 0;
    }
  }
  
  if(no_special_symbols)
  {
    for(i=0; i < length; ++i)
    {
      if((transformed[i]=
          alpha->char_to_sym[fid_CHAR_AS_INDEX(string[i])]) > fid_SYMBOLMAX)
      {
        return i+1;
      }
    }
  }
  else
  {
    for(i=0; i < length; ++i)
    {
      transformed[i]=alpha->char_to_sym[fid_CHAR_AS_INDEX(string[i])];
    }
  }
  
  return 0;
}

/*}}}*/

/*!
 * \brief Transform string according to given alphabet.
 * 
 * This function replaces the original string by the transformed string in
 * the same buffer.
 * 
 * \param alpha Alphabet used for transformation.
 * \param string The string to be transformed in place.
 * \param length The number of characters in \p string. If 0, then the length
 * of \p string will be determined within this function. In this case the
 * string must be zero terminated.
 * \param no_special_symbols If unequal to 0, then stop transformation and
 * return an error if a character in \p string is transformed into a special
 * symbol.
 * 
 * \returns 0 on success, or the index+1 of the first special symbol when
 * special symbols were not allowed. Note that the transformation will be
 * incomplete if a positive value is returned.
 */
size_t fid_alphabet_transform_string_inplace(const fid_Alphabet *alpha,
                                             char *string, size_t length,
                                             int no_special_symbols) /*{{{*/
{
  return fid_alphabet_transform_string(alpha,string,length,(fid_Symbol *)string,
                                       no_special_symbols);
}

/*}}}*/

/*!
 * \brief Transform string according to given alphabet into new buffer.
 * 
 * This function allocates a buffer large enough to hold the transformed
 * string and returns that to the caller.
 * 
 * \param alpha Alphabet used for transformation.
 * \param string The input string to be transformed.
 * \param length The number of characters in \p string. If 0, then the length
 * of \p string will be determined within this function. In this case the
 * string must be zero terminated.
 * \param no_special_symbols If unequal to 0, then stop transformation and
 * return an error if a character in \p string is transformed into a special
 * symbol.
 * \param error Error messages go here.
 * 
 * \returns A pointer to an allocated buffer holding the transformed string.
 * A \c NULL pointer may be returned in the following cases:
 * - The length of \p string was found to be 0.
 * - The input string contains at least one character that is transformed
 * into a special symbol be the alphabet. An appropriate error message will
 * be added to \p error in this case.
 * - No memory could be allocated. An appropriate error message will be
 * added to \p error in this case (if possible at all in this condition).
 */
fid_Symbol *fid_alphabet_transform_string_new(const fid_Alphabet *alpha,
                                              const char *string,
                                              size_t length,
                                              int no_special_symbols,
                                              fid_Error *error) /*{{{*/
{
  fid_Symbol *transformed;
  size_t errpos;
  
  assert(alpha != NULL);
  assert(string != NULL);
  
  if(length == 0)
  {
    if((length=strlen(string)) == 0)
    {
      return NULL;
    }
  }
  
  if((transformed=(fid_Symbol *)malloc(length*sizeof(fid_Symbol))) != NULL)
  {
    if((errpos=fid_alphabet_transform_string(alpha,string,length,transformed,
                                             no_special_symbols)) > 0)
    {
      free(transformed);
      fid_error_throw(error,"Cannot transform string, character '%c' is "
                      "mapped to a non-regular symbol.",string[errpos-1]);
      return NULL;
    }
  }
  else
  {
    fid_OUTOFMEM(error);
  }
  
  return transformed;
}

/*}}}*/

/*!
 * \brief Write textual representation of alphabet to file.
 * 
 * The function creates a new file and writes an alphabet definition file
 * based on \p alpha to that file. The filename will have the extension "al1"
 * appended to \p basefilename.
 * 
 * \param alpha Alphabet structure whole content shall be written to file.
 * \param basefilename The base filename of the enhanced suffix array.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_alphabet_write_to_file(const fid_Alphabet *alpha,
                               const char *basefilename, fid_Error *error) /*{{{*/
{
  char *filename;
  fid_Mappedfile file;
  fid_Symbol sym;
  unsigned int c;
  
  assert(alpha != NULL);
  assert(alpha->num_of_syms > 0);
  assert(alpha->num_of_chars > 0);
  assert(alpha->num_of_chars >= alpha->num_of_syms);
  assert(basefilename != NULL);
  
  if((filename=fid_filename_create(basefilename,"al1",error)) == NULL)
  {
    return -1;
  }
  
  if(fid_file_allocate(&file,filename,(size_t)alpha->num_of_chars*2,error) == 0)
  {
    assert(alpha->num_of_syms <= fid_SYMBOLMAX);
    for(sym=0; sym < alpha->num_of_syms-1; ++sym)
    {
      for(c=0; c <= UCHAR_MAX; ++c)
      {
        if(alpha->char_to_sym[c] == sym)
        {
          file.content[file.occupied++]=(unsigned char)c;
          assert(file.occupied <= file.allocated);
        }
      }
      file.content[file.occupied++]='\n';
      assert(file.occupied <= file.allocated);
    }
    
    for(c=0; c <= UCHAR_MAX; ++c)
    {
      if(alpha->char_to_sym[c] == fid_WILDCARD)
      {
        file.content[file.occupied++]=(unsigned char)c;
        assert(file.occupied <= file.allocated);
      }
    }
    file.content[file.occupied++]='\n';
    assert(file.occupied <= file.allocated);
    
    fid_file_unmap(&file);
  }
  
  free(filename);
  return 0;
}

/*}}}*/

/*!
 * \brief Print alphabet to output stream.
 * 
 * \param alpha The alphabet structure to be printed out.
 * \param stream An output stream to which the alphabet is printed. If
 * \c NULL, nothing will be printed.
 */
void fid_alphabet_dump(const fid_Alphabet *alpha, FILE *stream) /*{{{*/
{
  int i;
  fid_Symbol cs;
  
  assert(alpha != NULL);
  assert(alpha->num_of_chars <= UCHAR_MAX);
  assert(alpha->num_of_syms <= fid_SYMBOLMAX);
  
  if(stream == NULL)
  {
    return;
  }
  
  fprintf(stream,"----------\nAlphabet characters (%hu):\n",
          alpha->num_of_chars);
  for(i=0; i < (int)(UCHAR_MAX+1); ++i)
  {
    cs=alpha->char_to_sym[i];
    assert(cs < fid_SEPARATOR);
    if(cs != fid_UNDEF && cs != fid_WILDCARD)
    {
      if(isprint(i))
      {
        fprintf(stream," %c",(char)i);
      }
      else
      {
        fprintf(stream," [%c]",i);
      }
      fprintf(stream," -> " fid_SYMFMT "\n",cs);
    }
  }
  fprintf(stream," {");
  for(i=0; i < (int)sizeof(alpha->char_to_sym); ++i)
  {
    if(alpha->char_to_sym[i] == fid_WILDCARD)
    {
      if(isprint(i))
      {
        fprintf(stream,"%c",(char)i);
      }
      else
      {
        fprintf(stream,"[%d]",i);
      }
    }
  }
  fprintf(stream,"}\n");
  
  fprintf(stream,"Alphabet symbols (%hu):\n",alpha->num_of_syms);
  for(i=0; i < (int)sizeof(alpha->sym_to_char); ++i)
  {
    cs=alpha->sym_to_char[i];
    if(cs != '\0')
    {
      fprintf(stream," %d -> ",i);
      if(isprint(cs))
      {
        fprintf(stream," %c\n",(char)cs);
      }
      else
      {
        fprintf(stream," [" fid_SYMFMT "]\n",cs);
      }
    }
  }
  fprintf(stream,"----------\n");
}

/*}}}*/
