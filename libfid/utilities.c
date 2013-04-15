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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include "libdefs.h"
#include "error.h"
#include "utilities.h"

/*!
 * \brief Parse a 32 bit unsigned integer from string.
 * 
 * This function performs good error checking.
 * 
 * \param string Textual representation of an integer.
 * \param ui The parsed value as binary 32 bit value.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_utils_parse_uint_32(const char *string, fid_Uint32 *ui,
                            fid_Error *error)
{
  int retval;
  fid_Uint64 ui64;

  if((retval=fid_utils_parse_uint_64(string,&ui64,error)) == 0)
  {
    if(ui64 <= (fid_Uint64)(~(fid_Uint32)0))
    {
      *ui=(fid_Uint32)ui64;
      return 0;
    }
    else
    {
      fid_error_throw(error,"Parsed value too large.");
    }
  }
  return retval;
}

#ifdef HAVE_STRTOULL
#define MAXVAL  ULLONG_MAX
/* Some header files to not define the prototype for strtoull(), even when the
 * function is linkable. */
extern unsigned long long strtoull(const char *, char **, int);
#elif defined HAVE_STRTOUL
#define MAXVAL  ULONG_MAX
#endif /* HAVE_STRTOULL || HAVE_STRTOUL */

/*!
 * \brief Parse a 64 bit unsigned integer from string.
 * 
 * This function performs good error checking.
 * 
 * \param string Textual representation of an integer.
 * \param ui The parsed value as binary 64 bit value.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_utils_parse_uint_64(const char *string, fid_Uint64 *ui,
                            fid_Error *error) /*{{{*/
{
  char *endptr=NULL;
#ifdef HAVE_STRTOULL
  unsigned long long parsed_value;
#else /* !HAVE_STRTOULL */
  unsigned long parsed_value;
#endif /* HAVE_STRTOULL*/

  if(*string != '-')
  {
    errno=0;
#ifdef HAVE_STRTOULL
    parsed_value=strtoull(string,&endptr,10);
#else /* !HAVE_STRTOULL */
    parsed_value=strtoul(string,&endptr,10);
#endif /* HAVE_STRTOULL*/
    if(endptr == NULL || *endptr != '\0')
    {
      fid_error_throw(error,"Non-negative integer value expected.");
    }
    else if((parsed_value == MAXVAL && errno == ERANGE)
#if defined HAVE_STRTOULL && SIZEOF_UNSIGNED_LONG_LONG > 8
            || parsed_value > (unsigned long long)(~(fid_Uint64)0)
#endif /* !(HAVE_STRTOULL && SIZEOF_UNSIGNED_LONG_LONG > 8) */
            )
    {
      fid_error_throw(error,"Parsed value too large.");
    }
    else
    {
      *ui=parsed_value;
      return 0;
    }
  }
  else
  {
    fid_error_throw(error,"Non-negative integer value expected.");
  }
  return -1;
}

/*}}}*/
