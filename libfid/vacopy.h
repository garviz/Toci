/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2007, 2008  Robert Homann
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

#ifndef VACOPY_H
#define VACOPY_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#else /* !HAVE_STDARG_H */
#ifdef HAVE_SYS_VARARGS_H
#include <sys/varargs.h>
#endif /* HAVE_SYS_VARARGS_H */
#endif /* HAVE_STDARG_H */

#ifndef HAVE_VA_COPY
#ifdef HAVE___VA_COPY
#define va_copy(D,S)  __va_copy(D,S)
#else /* !HAVE___VA_COPY */
#include <string.h>
#define va_copy(D,S)  memcpy (&(D),&(S),sizeof(va_list))
#endif /* HAVE___VA_COPY */
#endif /* !HAVE_VA_COPY */

#endif /* !VACOPY_H */
