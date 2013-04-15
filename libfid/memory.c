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
#include <stdio.h>
#include <assert.h>

#ifdef fid_ENABLE_MEMORY_PROFILING
#undef fid_ENABLE_MEMORY_PROFILING
#endif /* fid_ENABLE_MEMORY_PROFILING */
#define fid_MEMORY_INTERNAL

#include "libdefs.h"

static size_t memory_peak=0;
static size_t memory_current_size=0;
static fid_Uint64 memory_calls=0;
static fid_Uint64 memory_allocations=0;
static fid_Uint64 memory_deallocations=0;

#define PTR_INC  ((size_t)8192)

#define PEAK\
  if(memory_current_size > memory_peak)\
  {\
    memory_peak=memory_current_size;\
  }

typedef struct
{
  void *ptr;
  size_t size;
} Ptrinfo;

/*@null@*/
static Ptrinfo *memory_pointers=NULL;
static size_t memory_pointers_count=0;

/*@null@*/
static Ptrinfo *find_pointer(void *ptr)
{
  size_t i;

  assert(ptr != NULL);

#ifdef S_SPLINT_S
  if(memory_pointers == NULL)
  {
    return NULL;
  }
#endif /* S_SPLINT_S */

  for(i=0; i < memory_pointers_count; ++i)
  {
    if(memory_pointers[i].ptr == ptr)
    {
      /*@-immediatetrans@*/
      return &memory_pointers[i];
      /*@+immediatetrans@*/
    }
  }
  return NULL;
}

static void add_pointer(void *ptr, size_t size)
{
  size_t i, j;

  assert(ptr != NULL);
  assert(size > 0);

  if(memory_pointers == NULL)
  {
    memory_pointers_count=PTR_INC;
    if((memory_pointers=
        (Ptrinfo *)malloc(memory_pointers_count*sizeof(Ptrinfo))) == NULL)
    {
      fprintf(stderr,"### %d: out of memory\n",__LINE__);
      abort();
    }
    for(i=0; i < memory_pointers_count; ++i)
    {
      memory_pointers[i].ptr=NULL;
      memory_pointers[i].size=0;
    }
  }

  /* find free slot */
  for(i=0; i < memory_pointers_count; ++i)
  {
    if(memory_pointers[i].ptr == NULL)
    {
      break;
    }
  }

  /* add more slots if no free one is available  */
  if(i == memory_pointers_count)
  {
    memory_pointers_count+=PTR_INC;
    /*@-statictrans@*/
    if((memory_pointers=
        (Ptrinfo *)realloc(memory_pointers,
                           memory_pointers_count*sizeof(Ptrinfo))) == NULL)
    {
      fprintf(stderr,"### %d: out of memory\n",__LINE__);
      abort();
    }
    /*@+statictrans@*/
    for(j=i; j < memory_pointers_count; ++j)
    {
      memory_pointers[j].ptr=NULL;
      memory_pointers[j].size=0;
    }
  }

  /* put pointer into list and update statistics */
  memory_pointers[i].ptr=ptr;
  memory_pointers[i].size=size;

  memory_current_size+=size;
  PEAK;
  ++memory_allocations;
}

static void remove_pointer(void *ptr)
{
  Ptrinfo *ptrinfo;

  if(ptr == NULL)
  {
    return;
  }

  if((ptrinfo=find_pointer(ptr)) != NULL)
  {
    memory_current_size-=ptrinfo->size;
    ++memory_deallocations;
    ptrinfo->ptr=NULL;
    ptrinfo->size=0;
  }
  else
  {
    fprintf(stderr,"### %d: pointer %p unknown\n",__LINE__,ptr);
    abort();
  }
}

/*!
 * \brief Simple wrapper for \c malloc().
 *
 * Calls of \c malloc() are wrapped by this function if compiled with
 * \c fid_ENABLE_MEMORY_PROFILING defined.
 */
void *fid_memory_malloc(size_t size)
{
  void *block;

  ++memory_calls;
  if((block=malloc(size)) != NULL)
  {
    add_pointer(block,size);
  }

  return block;
}

/*!
 * \brief Simple wrapper for \c calloc().
 *
 * Calls of \c calloc() are wrapped by this function if compiled with
 * \c fid_ENABLE_MEMORY_PROFILING defined.
 */
void *fid_memory_calloc(size_t nmemb, size_t size)
{
  void *block;

  ++memory_calls;
  if((block=calloc(nmemb,size)) != NULL)
  {
    add_pointer(block,nmemb*size);
  }

  return block;
}

/*!
 * \brief Simple wrapper for \c realloc().
 *
 * Calls of \c realloc() are wrapped by this function if compiled with
 * \c fid_ENABLE_MEMORY_PROFILING defined.
 */
void *fid_memory_realloc(void *ptr, size_t size)
{
  void *block;
  Ptrinfo *ptrinfo;

  ++memory_calls;
  if((block=realloc(ptr,size)) != NULL)
  {
    if(ptr == NULL)
    {
      /* equivalent to malloc() */
      add_pointer(block,size);
    }
    else if((ptrinfo=find_pointer(ptr)) != NULL)
    {
      /* update info */
      memory_current_size-=ptrinfo->size;
      memory_current_size+=size;
      PEAK;
      ptrinfo->ptr=block;
      ptrinfo->size=size;
    }
    else
    {
      fprintf(stderr,"### %d: pointer %p unknown\n",__LINE__,ptr);
      abort();
    }
  }
  else if(ptr != NULL && size == 0)
  {
    /* returned NULL, passed 0 size -> free() */
    remove_pointer(ptr);
  }

  return block;
}

/*!
 * \brief Simple wrapper for \c strdup().
 *
 * Calls of \c strdup() are wrapped by this function if compiled with
 * \c fid_ENABLE_MEMORY_PROFILING defined.
 */
char *fid_memory_strdup(const char *s)
{
  char *block;

  ++memory_calls;
  if((block=strdup(s)) != NULL)
  {
    add_pointer(block,strlen(s));
  }

  return block;
}

/*!
 * \brief Simple wrapper for \c free().
 *
 * Calls of \c free() are wrapped by this function if compiled with
 * \c fid_ENABLE_MEMORY_PROFILING defined.
 */
void fid_memory_free(void *ptr)
{
  ++memory_calls;
  remove_pointer(ptr);
  free(ptr);
}

/*!
 * \brief Query memory usage statistics.
 *
 * This function can only give meaningful results when compiled with
 * \c fid_ENABLE_MEMORY_PROFILING defined. Then, all calls of \c malloc() and
 * related functions are wrapped by functions that count the number of
 * allocations, and keep track of current and peak memory usage. These values
 * can be read by calling this function.
 *
 * \param peak,current Peak and current memory usage in bytes. This is, of
 * course, not absolutely accurate since the overhead caused by the heap
 * management routines cannot be accounted for.
 * \param allocs,frees,total Number of allocations, deallocations, and total
 * number of calls of heap management routines. Note that the latter of these
 * values does not necessarily equal the sum of the former two.
 */
void fid_memory_query(size_t *peak, size_t *current, fid_Uint64 *allocs,
                      fid_Uint64 *frees, fid_Uint64 *total)
{
  *peak=memory_peak;
  *current=memory_current_size;
  *allocs=memory_allocations;
  *frees=memory_deallocations;
  *total=memory_calls;
}

#define MB(S)  (((double)(S))/1024.0/1024.0)

/*!
 * \brief Print memory usage statistics to stream.
 *
 * \param stream Write to this file pointer.
 */
void fid_memory_show_usage(FILE *stream)
{
  fprintf(stream,
          "\n"
          "###########################\n"
          "###  MEMORY STATISTICS  ###\n"
          "###########################\n"
          "Memory peak               : %lu bytes (%.2f MB)\n"
          "Current memory consumption: %lu bytes (%.2f MB)\n"
          "Number of allocations     : " fid_U64FMT "\n"
          "Number of deallocations   : " fid_U64FMT "\n"
          "Total number of calls     : " fid_U64FMT "\n"
          "###########################\n",
          (unsigned long)memory_peak,MB(memory_peak),
          (unsigned long)memory_current_size,MB(memory_current_size),
          memory_allocations,memory_deallocations,memory_calls);
}
