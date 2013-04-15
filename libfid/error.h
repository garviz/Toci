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

#ifndef ERROR_H
#define ERROR_H
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

#endif /* !ERROR_H */
