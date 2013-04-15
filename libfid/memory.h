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

#ifndef MEMORY_H
#define MEMORY_H
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

#endif /* !MEMORY_H */
