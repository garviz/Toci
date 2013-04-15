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
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "libdefs.h"
#include "vacopy.h"
#include "error.h"

#define STEP_ERROR_INDEX(I)\
  if(++(I) >= fid_ERROR_MAX)\
  {\
    (I)=0;\
  }

/*!
 * \brief Initialize an error structure for detailed error reporting.
 * 
 * \param error The structure to be initialized. This may also be \c NULL,
 * in which case nothing will happen.
 * \param mode Mode of overflow handling. See fid_Error documentation for
 * more details.
 */
void fid_error_init(fid_Error *error, fid_Errormode mode) /*{{{*/
{
  int i;
  
  if(error == NULL)
  {
    return;
  }
  
  assert(mode == fid_ERRORMODE_FIFO || mode == fid_ERRORMODE_DEEPEST || mode == fid_ERRORMODE_KEEP_LAST);
  
  error->num_of_errors=error->first_error=0;
  error->mode=mode;
  error->overflow=error->outofmem=0;
  for(i=0;i < fid_ERROR_MAX; ++i)
  {
    error->errors[i]=NULL;
  }
}

/*}}}*/

/*!
 * \brief Initialize an error structure for detailed error reporting.
 * 
 * Overflow handling mode will be set to the default mode, which is
 * \c fid_ERRORMODE_FIFO in this version.
 * 
 * \param error The structure to be initialized. This may also be \c NULL,
 * in which case nothing will happen.
 */
void fid_error_init_default(fid_Error *error) /*{{{*/
{
  fid_error_init(error,fid_ERRORMODE_FIFO);
}

/*}}}*/

/*!
 * \brief Free an error structure.
 * 
 * This function has reset characteristics, i.e., after this function has
 * been called for a previously initialized fid_Error structure, an extra
 * call of fid_error_init() is not necessary.
 * 
 * \param error The structure to be freed. This may also be \c NULL, in
 * which case nothing will happen.
 */
void fid_error_free(fid_Error *error) /*{{{*/
{
  int i, err;
  
  if(error == NULL)
  {
    return;
  }
  
  err=error->first_error;
  for(i=0; i < error->num_of_errors; ++i)
  {
    assert(error->errors[err] != NULL);
    free(error->errors[err]);
    error->errors[err]=NULL;
    STEP_ERROR_INDEX(err);
  }
  error->num_of_errors=error->first_error=0;
  error->overflow=error->outofmem=0;
}

/*}}}*/

/*!
 * \brief Construct error message from variable argument list.
 *
 * \param fmt Format string like that you would pass to printf(), followed
 * by values filled into the format.
 * \param ap Variable argument list.
 * \param error Modified only in out-of-memory condition, see below.
 * 
 * \returns A newly allocated string containing the formatted message, or
 * \c NULL on error. A return value of \c NULL does not necessarily indicate
 * an out of memory condition since the format string could have been malformed
 * or zero length. Check fid_Error::outofmem to find out.
 */
static char *make_message(const char *fmt, va_list ap, fid_Error *error)/*{{{*/
{
#ifdef ENABLE_VSNPRINTF_WORKAROUND
  char dummy;
#endif /* ENABLE_VSNPRINTF_WORKAROUND */
  va_list copy;
  int bufsize;
  char *buffer=NULL;

#ifndef S_SPLINT_S
  va_copy(copy,ap);
#endif /* !S_SPLINT_S */
  bufsize=vsnprintf(
#ifdef ENABLE_VSNPRINTF_WORKAROUND
                    &dummy,1,
#else /* !ENABLE_VSNPRINTF_WORKAROUND */
                    NULL,0,
#endif /* ENABLE_VSNPRINTF_WORKAROUND*/
                    fmt,copy)+1;
  va_end(copy);
  if(bufsize > 1)
  {
    if((buffer=(char *)malloc(sizeof(char)*bufsize)) != NULL)
    {
#ifndef S_SPLINT_S
      va_copy(copy,ap);
#endif /* !S_SPLINT_S */
      (void)vsnprintf(buffer,(size_t)bufsize,fmt,copy);
      va_end(copy);
    }
    else
    {
      error->outofmem=1;
    }
  }
  return buffer;
}
/*}}}*/

/*!
 * \brief Append error message to error structure according to its mode.
 *
 * \param error The structure the message should be appended to.
 * \param buffer A string containing the error message.
 */
static void append_message(fid_Error *error, char *buffer)/*{{{*/
{
  if(buffer != NULL)
  {
#ifdef DEBUG
    char *bufptr, c;

    fprintf(stderr,"THROW ERROR: \"%s\"\n",buffer);

    for(bufptr=buffer; (c=*bufptr) != '\0'; ++bufptr)
    {
      if(c == '\n')
      {
        fprintf(stderr,"%s(): WARNING: error message contains newlines.\n",
                __func__);
        fprintf(stderr,"%s(): WARNING: ---> %s <---\n",__func__,buffer);
        break;
      }
    }
#endif /* DEBUG */
    
    /* so we have an error message now...*/
    if(error->num_of_errors < fid_ERROR_MAX)
    {
      /* easy... */
      error->errors[error->num_of_errors++]=buffer;
    }
    else
    {
      /* handle overflow */
      error->overflow=1;
      switch(error->mode)
      {
       case fid_ERRORMODE_FIFO:
        assert(error->errors[error->first_error] != NULL);
        free(error->errors[error->first_error]);
        error->errors[error->first_error]=buffer;
        STEP_ERROR_INDEX(error->first_error);
        break;
       case fid_ERRORMODE_KEEP_LAST:
        assert(error->errors[fid_ERROR_MAX-1] != NULL);
        free(error->errors[fid_ERROR_MAX-1]);
        error->errors[fid_ERROR_MAX-1]=buffer;
        break;
       default:
        abort();
      }
    }
  }
#ifdef DEBUG
  else if(error->outofmem)
  {
    fprintf(stderr,"THROW ERROR, OUT OF MEMORY (%s)\n",buffer);
  }
  else
  {
    fprintf(stderr,"THROW ERROR, EMPTY MESSAGE (%s)\n",buffer);
  }
#endif /* !DEBUG */
}
/*}}}*/

/*!
 * \brief Append an error message to the error structure.
 * 
 * The error structure can be filled with multiple error messages to provide
 * a more detailed picture of the error than a single error message could.
 * 
 * \param error The error structure that the new error should be appended
 * to. This may also be \c NULL to disable error reporting.
 * \param fmt Format string like that you would pass to printf(), followed
 * by values filled into the format.
 */
void fid_error_throw(fid_Error *error, const char *fmt, ...) /*{{{*/
{
  char *buffer;
  va_list ap;
  
  if(error == NULL || fmt == NULL)
  {
    /* nothing to store */
    return;
  }
  else if(error->num_of_errors == fid_ERROR_MAX &&
          error->mode == fid_ERRORMODE_DEEPEST)
  {
    /* too many errors */
#ifdef DEBUG
    fprintf(stderr,"THROW ERROR, OVERFLOW (%s)\n",fmt);
#endif /* !DEBUG */
    error->overflow=1;
    return;
  }
  
  va_start(ap,fmt);
  buffer=make_message(fmt,ap,error);
  va_end(ap);

  assert(buffer != NULL || error->outofmem == 1 || strlen(fmt) == 0);
  
  append_message(error,buffer);
}

/*}}}*/

/*!
 * \brief Append an error message generated from error code to error structure.
 * 
 * The generated error message will look very much like what \c perror() would
 * print. The error code is converted into an error string using \c strerror(),
 * which is appended to the passed error structure. Optionally, similar to
 * \c perror(), the error message is generated from the string passed in as
 * \p fmt, followed by <code>": "</code>, followed by the error string. As an
 * extension to plain perror(), \p fmt may be a format string.
 * 
 * \param error The error structure that the new error should be appended
 * to. This may also be \c NULL to disable error reporting.
 * \param errnum The error code, typically the value of the global \c errno
 * variable. If set to 0, then \p error will remain unchanged. This function
 * does not use \c errno directly.
 * \param fmt Optional format string containing an error description, followed
 * by a variable argument list. The message is formatted and appended to
 * \p error. Pass \c NULL to add just the raw error string as returned by
 * strerror() to the error structure.
 */
void fid_error_throw_errno(fid_Error *error, int errnum, const char *fmt, ...)/*{{{*/
{
  char *buffer=NULL, *str;
  va_list ap;

  if(error == NULL || errnum == 0)
  {
    /* nothing to do */
    return;
  }

  str=strerror(errnum);

  if(fmt != NULL)
  {
    va_start(ap,fmt);
    buffer=make_message(fmt,ap,error);
    va_end(ap);
  }
  else if(str != NULL)
  {
    /* no message, just an error string -- better than nothing */
    append_message(error,str);
  }
  else
  {
    /* great -- no message, no error string -- nothing known at all... */
    fid_error_throw(error,"unknown error %d",errnum);
  }

  if(buffer != NULL)
  {
    /* so we have parsed the format string... */
    if(str != NULL)
    {
      /* also got an error message -- this should be the regular case */
      fid_error_throw(error,"%s: %s",buffer,str);
    }
    else
    {
      /* strerror() returned NULL */
      fid_error_throw(error,"%s: unknown error %d",buffer,errnum);
    }
    free(buffer);
  }
  else
  {
    /* out of memory -- line below is basically a no-op for now, but
     * append_message() might see extensions in the future */
    append_message(error,NULL);
  }
}
/*}}}*/

/*!
 * \brief Simple error reporting function.
 * 
 * This function prints all errors collected in \p error to \p stream and
 * tries to visualize them according to overflow handling mode in case too
 * many errors have occurred.
 * 
 * \param error The error structure containing error messages. It is safe
 * to call this function without checking if errors are available
 * beforehand. This may also be \c NULL to disable error reporting.
 * \param stream The output stream the errors should be written to. If
 * \c NULL, then the errors are not printed.
 * 
 * \returns The return value of fid_ERROR_OCCURRED(error) is returned to
 * make this function suitable for use in conditionals. If \p error is
 * \c NULL, then the return value is 0.
 */
int fid_error_dump(const fid_Error *error, FILE *stream) /*{{{*/
{
  int i, err, last;
  int retval;
  
  if(error == NULL)
  {
    return 0;
  }
  else if(!(retval=fid_ERROR_OCCURRED(error)) || stream == NULL)
  {
    return retval;
  }
  
  if(error->num_of_errors == (int)1)
  {
    assert(error->first_error == 0);
    assert(error->errors[0] != NULL);
    fprintf(stream,"Error: %s\n",error->errors[0]);
  }
  else
  {
    assert(error->first_error < fid_ERROR_MAX);
    fprintf(stream,"-----\nErrors:\n");
    if(error->overflow && error->mode == fid_ERRORMODE_KEEP_LAST)
    {
      last=error->num_of_errors-1;
    }
    else
    {
      last=error->num_of_errors;
    }
    
    if(error->overflow && error->mode == fid_ERRORMODE_FIFO)
    {
      fprintf(stream,"??: ...\n");
    }
    
    err=error->first_error;
    for(i=0; i < last; ++i)
    {
      assert(err < fid_ERROR_MAX);
      assert(error->errors[err] != NULL);
      fprintf(stream,"%2d: %s\n",i+1,error->errors[err]);
      STEP_ERROR_INDEX(err);
    }
    
    if(error->overflow)
    {
      switch(error->mode)
      {
       case fid_ERRORMODE_KEEP_LAST:
        fprintf(stream,"    ...\n");
        assert(error->errors[fid_ERROR_MAX-1] != NULL);
        fprintf(stream,"%02u: %s\n",(unsigned int)fid_ERROR_MAX,
                error->errors[fid_ERROR_MAX-1]);
        break;
       case fid_ERRORMODE_DEEPEST:
        fprintf(stream,"    ...\n");
        break;
       default:
        break;
      }
    }
    
    if(error->outofmem)
    {
      fprintf(stream,"xx: at least one error message got lost due to "
              "out-of-memory condition.\n");
    }
    fprintf(stream,"-----\n");
  }
  
  return retval;
}

/*}}}*/
