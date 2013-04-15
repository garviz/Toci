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

#ifndef LIBDEFS_H
#define LIBDEFS_H
/*!\addtogroup misc Miscellaneous stuff
 * It is possible to use a simple form of memory profiling by configuring
 * \em libfid with option \c --enable-memprof. If this option is given to the
 * configure script, then the \c fid_ENABLE_MEMORY_PROFILING proprocessor
 * symbols gets defined; the effect is that functions such as \c malloc() or
 * \c strdup() get replaced by macros that call internal functions such as
 * fid_memory_malloc() or fid_memory_strdup(). These replacement functions
 * count the number of bytes allocated and deallocated, the number of
 * (de-)allocations, and store the peak memory usage. The collected values can
 * be obtained via functions fid_memory_query() or printed to some \c FILE
 * pointer via fid_memory_show_usage(). These two names refer to real functions
 * only if \c fid_ENABLE_MEMORY_PROFILING is defined, otherwise they are macros
 * that expand to nothing.
 *
 * There are, however, a few issues with this kind of memory profiling:
 * - It is based on macros, i.e., based on source code substitution, and can be
 *   enabled or disabled at compile-time only. It is not possible to turn on
 *   internal profiling in a version not compiled to do profiling, and vice
 *   versa. As a consequence, all programs that link against a version of
 *   \em libfid configured with \c --enable-memprof \em must also define the
 *   \c fid_ENABLE_MEMORY_PROFILING symbol \em before including \c libfid.h
 *   somewhere; otherwise the program is likely to crash.
 * - It is not 100% accurate, also because the profiling is based on macros.
 *   Invocations of \c malloc() buried inside library functions are not
 *   redirected to fid_memory_malloc(), so these do not appear in the
 *   statistics collected there. Worse, trying to \c free() memory returned by
 *   such a function with \c fid_ENABLE_MEMORY_PROFILING enabled will lead to
 *   program termination since \c free() would have been redirected to
 *   fid_memory_free() then; but since \em libfid has never seen the pointer to
 *   be freed before, it assumes a program error, and terminates by calling
 *   \c abort().
 * - Memory used by the heap manager implementation (the memory overhead caused
 *   by using \c malloc()) cannot be account for. This overhead is, of course,
 *   an implementation detail of your platform, and cannot be estimated at all.
 *
 * Despite the drawbacks sketched above, the memory profiling facilities built
 * into \em libfid are advantageous if you \em don't want to see the memory
 * overhead imposed by your system's libraries and heap manager, but only the
 * amount of memory explicitly used by your program. Additionally, keeping
 * track of allocations inside \em libfid is usally much more accurate than
 * using sampling-based methods such as utilized by \c memtime for short
 * running programs.
 */
/*@{*/
#include "libfidinttypes.h"
#include "memory.h"

/*!\brief Print message to \c stderr, return with return value -1. */
#define fid_NOTIMPLEMENTED_STDERR\
  fprintf(stderr,"%s() [%s:%d]: not implemented yet\n",\
          __func__,__FILE__,__LINE__);\
  return -1

/*!\brief Put error into #fid_Error \p ERR, return with return value -1. */
#define fid_NOTIMPLEMENTED(ERR)\
  fid_error_throw(ERR,"%s() [%s:%d]: not implemented yet",\
                  __func__,__FILE__,__LINE__);\
  return -1
/*@}*/

#endif /* !LIBDEFS_H */
