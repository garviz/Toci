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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "libdefs.h"
#include "error.h"
#include "arrays.h"
#include "alphabet.h"
#include "fileutils.h"
#include "sequences.h"
#include "dbfiles.h"
#include "utilities.h"
#include "projectfile.h"
#include "projectfileparser.h"

#define CANNOTPARSE "Cannot parse project file \"%s\", "

/*!
 * \brief Initialize a project file data structure.
 * 
 * \param project Data structure to be initialized.
 * \param prjbasename Base name of the enhanced suffix array project. The
 * function makes a private copy of this string, so be sure to call
 * fid_projectfile_free() when done.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_projectfile_init(fid_Projectfile *project, const char *prjbasename,
                         fid_Error *error) /*{{{*/
{
  assert(project != NULL);
  assert(prjbasename != NULL);
  assert(fid_WORDSIZE == 32 || fid_WORDSIZE == 64);

  project->integer_size_in_bits=fid_WORDSIZE;
  project->integer_size_in_bytes=(project->integer_size_in_bits >> 3);
  if(project->integer_size_in_bytes == 4)
  {
    project->uisize=fid_UINTSIZE_32;
  }
  else if(project->integer_size_in_bytes == 8)
  {
    project->uisize=fid_UINTSIZE_64;
  }
  else
  {
    abort();
  }

  if(*prjbasename == '\0')
  {
    fid_error_throw(error,"Project name empty.");
    return -1;
  }
  
  if((project->filename=fid_filename_create(prjbasename,"prj",error)) == NULL)
  {
    return -1;
  }
  
  if((project->prjbasename=strdup(prjbasename)) == NULL)
  {
    free(project->filename);
    project->filename=NULL;
    fid_OUTOFMEM(error);
    return -1;
  }
  
  fid_DYNARRAY_INIT(&project->dbfiles,fid_DBfileinfo);
  
  project->endianess_known=1;
#ifdef WORDS_BIGENDIAN
  project->littleendian=0;
#else /* !WORDS_BIGENDIAN */
  project->littleendian=1;
#endif /* WORDS_BIGENDIAN */
  project->totallength=0;
  project->num_of_sequences=0;
  project->num_of_query_sequences=0;
  project->prefixlength=0;
  project->large_lcp_values=0;
  project->max_branchdepth=0;
  project->num_of_specials=0;
  project->num_of_special_ranges=0;
  project->longest=0;
  project->len_of_special_prefix=0;
  project->len_of_special_suffix=0;

  return 0;
}

/*}}}*/

/*!
 * \brief Parse more of the project file, find next assignment.
 * 
 * This function is only a tokenizer, i.e., it finds strings followed by an
 * equals sign followed by a string. Empty lines and comment lines
 * introduced with '#' are skipped.
 * 
 * \param buffer Buffer that contains the project file as text.
 * \param bufsize Size of the input buffer.
 * \param lineno Line number is advanced for each skipped line, also used
 * for error messages.
 * \param offset Current offset into \p buffer. Will be set to the beginning
 * of the next line after a line has been successfully tokenized.
 * \param tokenoffset Offset of the found token.
 * \param tokenlen Length of the found token string.
 * \param valueoffset Offset of the assigned value.
 * \param valuelen Length of the assigned value string.
 * \param filename Name of the input file for error messages.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, 1 if no more lines could be found, -1 on error.
 */
static int get_next_token_pair(const char *buffer, size_t bufsize,
                               size_t *lineno, size_t *offset, 
                               size_t *tokenoffset, size_t *tokenlen, 
                               size_t *valueoffset, size_t *valuelen,
                               const char *filename, fid_Error *error) /*{{{*/
{
  char c;
  
  if(*offset >= bufsize)
  {
    return 1;
  }
  
  /* skip empty lines and comments*/
  c='\n';
  while(c == '\n')
  {
    while(*offset < bufsize && (c=buffer[*offset]) == '\n')
    {
      ++*offset;
      ++*lineno;
    }
    if(*offset < bufsize)
    {
      assert(c != '\n');
      if(c == '#')
      {
        while(*offset < bufsize && (c=buffer[*offset]) != '\n')
        {
          ++*offset;
        }
        if(*offset >= bufsize)
        {
          return 1;
        }
        assert(c == '\n');
        ++*offset;
        ++*lineno;
      }
    }
    else
    {
      return 1;
    }
  }
  
  /* read until equals sign, end of line, or end of buffer */
  *tokenoffset=*offset;
  while(*offset < bufsize && (c=buffer[*offset]) != '\n')
  {
    if(c == '=')
    {
      if(*tokenoffset < *offset)
      {
        /* non-empty string from beginning of line until equals sign */
        *tokenlen=*offset-*tokenoffset;
        *valueoffset=++*offset;
        while(*offset < bufsize && buffer[*offset] != '\n')
        {
          ++*offset;
        }
        if(*valueoffset < *offset)
        {
          *valuelen=*offset-*valueoffset;
          
          ++*offset;
          return 0;
        }
        else
        {
          fid_error_throw(error,CANNOTPARSE "empty assignment in line %lu.",
                          filename,(unsigned long)*lineno);
          return -1;
        }
      }
      else
      {
        fid_error_throw(error,CANNOTPARSE "line %lu begins with '=' instead "
                        "of token.",filename,(unsigned long)*lineno);
        return -1;
      }
    }
    ++*offset;
  }
  
  return 1;
}

/*}}}*/

/*!
 * \brief Reset flags in token table that remember if tokens were encountered.
 * 
 * \param tokens An array of token descriptions.
 * \param size Number of entries in \p tokens.
 */
static void reset_tokentable(Tokentableentry *tokens, size_t size) /*{{{*/
{
  size_t i;
  
  for(i=0; i < size; ++i)
  {
    tokens[i].set=0;
  }
}

/*}}}*/

/*!
 * \brief Find given string in token table.
 * 
 * The token is searched in the table of token descriptions in a simplistic
 * one-by-one manner, i.e., no sophisticated search trees are used here.
 * If the given string is a valid token in the token table, return its
 * ID, that is its index in the array of token descriptions.
 * 
 * \param tokens An array of token descriptions.
 * \param size Number of entries in \p tokens.
 * \param token The string to be found in the table.
 * \param len Length of \p token.
 * 
 * \returns A TokenID pointing into \p tokens if the string is a valid
 * token and was not encountered before.
 * \retval TOKEN_UNKNOWN if the string is not a valid token.
 * \retval TOKEN_ALREADYSET if the token has been seen before.
 */
static TokenID get_token_id(Tokentableentry *tokens, size_t size,
                            const char *token, size_t len) /*{{{*/
{
  size_t i;
  
  for(i=0; i < size; ++i)
  {
    if(tokens[i].length == len && memcmp(token,tokens[i].name,len) == 0)
    {
      if(tokens[i].set && !tokens[i].allow_duplicate)
      {
        return TOKEN_ALREADYSET;
      }
      else
      {
        tokens[i].set=1;
        return (TokenID)i;
      }
    }
  }
  return TOKEN_UNKNOWN;
}

/*}}}*/

#ifndef HAVE_MEMRCHR
/*!
 * \brief Drop-in replacement for missing \c memrchr(3).
 * 
 * The \c memrchr(3) function is a GNU extension to libc. Non-GNU systems
 * like Sun Solaris don't have it, so here is the replacement. The return
 * type of the original implementation's prototype is <c>void *</c>,
 * without the \c const, but this should cause no problems.
 * 
 * \param s Pointer to a buffer of bytes.
 * \param c Character to search for in \p s.
 * \param n Size of \p s in bytes.
 * 
 * \returns A pointer to the first occurence of \p c in \p s when scanned
 * backwards. If \p c does not occur in \p s, then return \c NULL.
 * 
 * \see memchr(3).
 */
/*@null@*/
static const void *memrchr(const void *s, int c, size_t n) /*{{{*/
{
  const char *ptr;
  
  if(n > 0)
  {
    ptr=(const char *)s+n;
    do
    {
      if(*(--ptr) == (char)c)
      {
        return ptr;
      }
    } while(ptr > (const char *)s);
  }
  
  return NULL;
}

/*}}}*/
#endif /* !HAVE_MEMRCHR */

/*!
 * \brief Copy string as zero-terminated string into buffer.
 * 
 * Note that, since this function is intended to be a simple shortcut for
 * parsing integers, the passed buffer is assumed to be of size
 * PARSEBUFFERSIZE, so \p length should always be smaller than this value.
 * 
 * \param parsebuf Buffer that should contain the zero-terminated string.
 * \param str The character array to be copied.
 * \param length Number of characters in \p str.
 */
static void to_parsebuf(char *parsebuf, const char *str, size_t length) /*{{{*/
{
  assert(length < PARSEBUFFERSIZE);
  memcpy(parsebuf,str,length);
  parsebuf[length]='\0';
}

/*}}}*/

/*!
 * \brief Parse the \c dbline line from a project file.
 * 
 * \param project Field fid_Projectfile::dbfiles will be modified by this
 * function.
 * \param value The tokenized string that is assigned to \c dbfile in the
 * project file.
 * \param valuelen Length of \p value.
 * \param parsebuf Buffer for parsing integers. This buffer must have a size
 * of at least PARSEBUFFERSIZE.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int parse_dbfile(fid_Projectfile *project, const char *value,
                        size_t valuelen, char *parsebuf, fid_Error *error) /*{{{*/
{
  const char *inputfilelen=NULL, *seqlen;
  size_t dbfile_length=0, inputfilelen_length=0, seqlen_length=0;
  fid_Uint64 parsed_seqlength, parsed_inputlength;
  char *parsed_name;
  int ok=0;
  
  /* the input line consists of a filename and two integers */
  if((seqlen=(const char *)memrchr(value,' ',valuelen)) != NULL &&
     seqlen > value+3)
  {
    seqlen_length=valuelen-1-(seqlen-value);
    if((inputfilelen=(const char *)memrchr(value,' ',valuelen-seqlen_length-1)) != NULL &&
       inputfilelen > value+1)
    {
      inputfilelen_length=valuelen-seqlen_length-2-(inputfilelen-value);
      dbfile_length=valuelen-inputfilelen_length-seqlen_length-2;
      ++seqlen;
      ++inputfilelen;
      if(inputfilelen_length >= PARSEBUFFERSIZE || seqlen_length >= PARSEBUFFERSIZE)
      {
        fid_error_throw(error,"Value too large.");
      }
      else
      {
        ok=1;
      }
    }
  }
  
  if(ok)
  {
    /* at this point we know the positions of the three line elements, now
     * parse them */
    to_parsebuf(parsebuf,seqlen,seqlen_length);
    if(fid_utils_parse_uint_64(parsebuf,&parsed_seqlength,error) == 0)
    {
      to_parsebuf(parsebuf,inputfilelen,inputfilelen_length);
      if(fid_utils_parse_uint_64(parsebuf,&parsed_inputlength,error) == 0)
      {
        if((parsed_name=(char *)malloc((dbfile_length+1)*sizeof(char))) != NULL)
        {
          /* finally! */
          memcpy(parsed_name,value,dbfile_length);
          parsed_name[dbfile_length]='\0';
          if(fid_dbfiles_append(&project->dbfiles,parsed_name,
                                parsed_inputlength,parsed_seqlength,error) == 0)
          {
            return 0;
          }
          free(parsed_name);
        }
        else
        {
          fid_OUTOFMEM(error);
        }
      }
    }
  }
  
  return -1;
}

/*}}}*/

/*!
 * \brief Parse project file content from buffer.
 * 
 * \param project Content of the file is parsed into this structure. The
 * fid_Projectfile::filename field is assumed to be filled already.
 * \param buffer Buffer that contains the project file as text.
 * \param bufsize Size of the input buffer.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int projectfile_parse(fid_Projectfile *project, const char *buffer,
                             size_t bufsize, fid_Error *error) /*{{{*/
{
  size_t lineno=(size_t)1, offset=0;
  size_t tokenoffset=0, tokenlen=0, valueoffset=0, valuelen=0;
  fid_Uint64 temp;
  fid_Uint64 num_of_db_sequences=0;
  int retcode;
  TokenID tid;
  char parsebuf[PARSEBUFFERSIZE];
  static Tokentableentry tokens[TOKEN_NUMOFTOKENS]=
  {
    TOKEN_INSERT("dbfile",1),
    TOKEN_INSERT("totallength",0),
    TOKEN_INSERT("numofsequences",0),
    TOKEN_INSERT("numofdbsequences",0),
    TOKEN_INSERT("numofquerysequences",0),
    TOKEN_INSERT("prefixlength",0),
    TOKEN_INSERT("largelcpvalues",0),
    TOKEN_INSERT("maxbranchdepth",0),
    TOKEN_INSERT("integersize",0),
    TOKEN_INSERT("littleendian",0),
    TOKEN_INSERT("specialcharacters",0),
    TOKEN_INSERT("specialranges",0),
    TOKEN_INSERT("longest",0),
    TOKEN_INSERT("lengthofspecialprefix",0),
    TOKEN_INSERT("lengthofspecialsuffix",0)
  };

  assert(project != NULL);
  assert(project->filename != NULL);
  assert(buffer != NULL);
  assert(bufsize > 0);
  
  reset_tokentable(tokens,(size_t)TOKEN_NUMOFTOKENS);
  project->endianess_known=0;
  while((retcode=get_next_token_pair(buffer,bufsize,&lineno,&offset,
                                     &tokenoffset,&tokenlen,&valueoffset,
                                     &valuelen,project->filename,error)) == 0)
  {
    assert(tokenlen > 0);
    assert(valuelen > 0);
    
    tid=get_token_id(tokens,(size_t)TOKEN_NUMOFTOKENS,buffer+tokenoffset,
                     tokenlen);
    if(tid != TOKEN_UNKNOWN)
    {
      if(tid == TOKEN_ALREADYSET)
      {
        fid_error_throw(error,CANNOTPARSE "duplicate specification of token in "
                       "line %lu.",project->filename,(unsigned long)lineno);
        retcode=-1;
        break;
      }
      else if(tid != TOKEN_DBFILE)
      {
        to_parsebuf(parsebuf,buffer+valueoffset,valuelen);
      }
    }
    
    switch(tid)
    {
     case TOKEN_DBFILE:
      retcode=parse_dbfile(project,buffer+valueoffset,valuelen,parsebuf,error);
      break;
     case TOKEN_TOTALLENGTH:
      retcode=fid_utils_parse_uint_64(parsebuf,&project->totallength,error);
      break;
     case TOKEN_NUMOFSEQUENCES:
      retcode=fid_utils_parse_uint_64(parsebuf,&project->num_of_sequences,
                                      error);
      break;
     case TOKEN_NUMOFDBSEQUENCES:
      retcode=fid_utils_parse_uint_64(parsebuf,&num_of_db_sequences,error);
      break;
     case TOKEN_NUMOFQUERYSEQUENCES:
      retcode=fid_utils_parse_uint_64(parsebuf,
                                      &project->num_of_query_sequences,error);
      break;
     case TOKEN_PREFIXLENGTH:
      retcode=fid_utils_parse_uint_64(parsebuf,&project->prefixlength,error);
      break;
     case TOKEN_LARGELCPVALUES:
      retcode=fid_utils_parse_uint_64(parsebuf,&project->large_lcp_values,
                                      error);
      break;
     case TOKEN_MAXBRANCHDEPTH:
      retcode=fid_utils_parse_uint_64(parsebuf,&project->max_branchdepth,error);
      break;
     case TOKEN_INTEGERSIZE:
      if((retcode=fid_utils_parse_uint_64(parsebuf,&temp,error)) == 0)
      {
        if(temp != (fid_Uint64)32 && temp != (fid_Uint64)64)
        {
          fid_error_throw(error,CANNOTPARSE "unsupported integer size.",
                          project->filename);
          retcode=-1;
        }
        else
        {
          project->integer_size_in_bits=(unsigned char)temp;
          project->integer_size_in_bytes=
            (unsigned char)(project->integer_size_in_bits >> 3);
          if(temp == 32)
          {
            project->uisize=fid_UINTSIZE_32;
          }
          else
          {
            project->uisize=fid_UINTSIZE_64;
          }
        }
      }
      break;
     case TOKEN_LITTLEENDIAN:
      if((retcode=fid_utils_parse_uint_64(parsebuf,&temp,error)) == 0)
      {
        if(temp == 0 || temp == 1)
        {
          project->littleendian=(char)temp;
          project->endianess_known=1;
        }
        else
        {
          fid_error_throw(error,CANNOTPARSE "token \"%s\" can only be set "
                          "to 0 or 1.",project->filename,tokens[tid].name);
          retcode=-1;
        }
      }
      break;
     case TOKEN_SPECIALCHARS:
      retcode=fid_utils_parse_uint_64(parsebuf,&project->num_of_specials,error);
      break;
     case TOKEN_SPECIALRANGES:
      retcode=
        fid_utils_parse_uint_64(parsebuf,&project->num_of_special_ranges,error);
      break;
     case TOKEN_LONGEST:
      retcode=fid_utils_parse_uint_64(parsebuf,&project->longest,error);
      break;
     case TOKEN_LENOFSPECIALPFX:
      retcode=
        fid_utils_parse_uint_64(parsebuf,&project->len_of_special_prefix,error);
      break;
     case TOKEN_LENOFSPECIALSUF:
      retcode=
        fid_utils_parse_uint_64(parsebuf,&project->len_of_special_suffix,error);
      break;
     case TOKEN_UNKNOWN:
      fprintf(stderr,"ignoring unknown token in line %lu of %s\n",
              (unsigned long)lineno,project->filename);
      break;
     default:
      abort();
    }
    if(retcode != 0)
    {
      fid_error_throw(error,CANNOTPARSE "invalid value for \"%s\" in "
                      "line %lu.",project->filename,tokens[tid].name,
                      (unsigned long)lineno);
      break;
    }
    ++lineno;
  }
 
  assert(retcode != 0);

  if(retcode > 0)
  {
    if(!tokens[TOKEN_INTEGERSIZE].set)
    {
      fid_error_throw(error,CANNOTPARSE "integer size not specified "
                      "(need \"%s=<32|64>\").",project->filename,
                      tokens[TOKEN_INTEGERSIZE].name);
      retcode=-1;
    }
  }
  
  if(retcode > 0)
  {
    return 0;
  }
  else
  {
    fid_projectfile_free(project);
    return retcode;
  }
}

/*}}}*/

/*!
 * \brief Parse project file content from file.
 * 
 * This function calls fid_projectfile_init(), so the caller should not do
 * so.
 * 
 * \param project Content of the file is parsed into this structure.
 * \param prjbasename Base name of the enhanced suffix array project.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_projectfile_parse_from_file(fid_Projectfile *project,
                                    const char *prjbasename, fid_Error *error) /*{{{*/
{
  fid_Mappedfile prjfile;
  int retcode;
  
  assert(project != NULL);
  assert(prjbasename != NULL);
  
  if((retcode=fid_projectfile_init(project,prjbasename,error)) == 0)
  {
    if((retcode=fid_file_map(&prjfile,project->filename,0,0,error)) == 0)
    {
      retcode=projectfile_parse(project,(const char *)prjfile.content,
                                prjfile.occupied,error);
      fid_file_unmap(&prjfile);
    }
    else
    {
      fid_projectfile_free(project);
    }
  }
  return retcode;
}

/*}}}*/

/*!
 * \brief Write textual representation of project file data structure to file.
 * 
 * A new file is created and the content of \p project is written to it in
 * textual form.
 * 
 * \param project The project file data structure to be written. The filename
 * is taken from fid_Projectfile::filename.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_projectfile_write(const fid_Projectfile *project, fid_Error *error) /*{{{*/
{
  fid_Mappedfile prjfile;
  const fid_DBfileinfo *entry;
  size_t i;
  int retcode;
  
  assert(project != NULL);
  assert(project->filename != NULL);
  
  if((retcode=fid_file_new(&prjfile,project->filename,error)) == 0)
  {
    for(i=0; retcode == 0 && i < project->dbfiles.occupied; ++i)
    {
      entry=&project->dbfiles.dyndata[i];
      retcode=
        fid_file_write(&prjfile,error,
                       "dbfile=%s " fid_U64FMT " " fid_U64FMT "\n",
                       entry->name,entry->inputfile_length,
                       entry->sequence_length);
    }
    
    if(retcode == 0)
    {
      retcode=
        fid_file_write(&prjfile,error,
                       "totallength=" fid_U64FMT "\n"
                       "specialcharacters=" fid_U64FMT "\n"
                       "specialranges=" fid_U64FMT "\n"
                       "lengthofspecialprefix=" fid_U64FMT "\n"
                       "lengthofspecialsuffix=" fid_U64FMT "\n"
                       "numofsequences=" fid_U64FMT "\n"
                       "numofdbsequences=" fid_U64FMT "\n"
                       "numofquerysequences=" fid_U64FMT "\n"
                       "longest=" fid_U64FMT "\n"
                       "prefixlength=" fid_U64FMT "\n"
                       "largelcpvalues=" fid_U64FMT "\n"
                       "maxbranchdepth=" fid_U64FMT "\n"
                       "integersize=%hhu\n",
                       project->totallength,project->num_of_specials,
                       project->num_of_special_ranges,
                       project->len_of_special_prefix,
                       project->len_of_special_suffix,
                       project->num_of_sequences,
                       project->num_of_sequences-project->num_of_query_sequences,
                       project->num_of_query_sequences,project->longest,
                       project->prefixlength,project->large_lcp_values,
                       project->max_branchdepth,project->integer_size_in_bits);
      if(retcode == 0 && project->endianess_known)
      {
        retcode=fid_file_write(&prjfile,error,"littleendian=%d\n",
                               project->littleendian?1:0);
      }
    }
    
    fid_file_unmap(&prjfile);
  }
  return retcode;
}

/*}}}*/

/*!
 * \brief Free memory associated with a project file structure.
 * 
 * \param project The structure to be freed.
 */
void fid_projectfile_free(fid_Projectfile *project) /*{{{*/
{
  assert(project != NULL);
  free(project->filename);
  free(project->prjbasename);
  fid_dbfiles_free(&project->dbfiles);
#ifdef DEBUG
  memset(project,0,sizeof(fid_Projectfile));
#endif /* DEBUG */
}

/*}}}*/

/*!
 * \brief Print contents of fid_Projectfile structure to output stream.
 * 
 * \param project Structure to be printed out.
 * \param stream An output stream to which the structure is printed. If
 * \c NULL, nothing will be printed.
 */
void fid_projectfile_dump(const fid_Projectfile *project, FILE *stream) /*{{{*/
{
  size_t i;
  const fid_DBfileinfo *entry;
  
  assert(project != NULL);
  assert(project->filename != NULL);
  
  if(stream == NULL)
  {
    return;
  }
  
  fprintf(stream,"----------\nProject file \"%s\"\n",project->filename);
  fprintf(stream,"base name \"%s\"\n"
          "input databases %lu\n",project->prjbasename,
          (unsigned long)project->dbfiles.occupied);
  for(i=0; i < project->dbfiles.occupied; ++i)
  {
    entry=&project->dbfiles.dyndata[i];
    fprintf(stream,"   \"%s\"\n"
            "   input file size %lu, sequence size %lu\n",
            entry->name,
            (unsigned long)entry->inputfile_length,
            (unsigned long)entry->sequence_length);
  }
  fprintf(stream,"integer size %hhu (%hhu bytes)\n"
          "total length " fid_U64FMT "\n"
          "number of sequences " fid_U64FMT "\n"
          "number of database sequences %lu\n"
          "number of query sequences " fid_U64FMT "\n"
          "bucket table prefix length " fid_U64FMT "\n"
          "number of large LCP values " fid_U64FMT "\n"
          "maximum branching depth " fid_U64FMT "\n",
          project->integer_size_in_bits,project->integer_size_in_bytes,
          project->totallength,project->num_of_sequences,
          (unsigned long)project->dbfiles.occupied,
          project->num_of_query_sequences,project->prefixlength,
          project->large_lcp_values,project->max_branchdepth);
  fprintf(stream,"----------\n");
}

/*}}}*/
