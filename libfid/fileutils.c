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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <assert.h>

#include "libdefs.h"
#include "vacopy.h"
#include "error.h"
#include "fileutils.h"
#include "options.h"

#if fid_WORDSIZE == 64
#define FILETOOLARGEERROR(ERR,FNAME)\
  fid_error_throw(ERR,"File \"%s\" is too large to be handled by "\
                  PACKAGE ". The maximum file size is limited to %lu bytes "\
                  "in this version.",(FNAME),(unsigned long)fid_FILE_MAXSIZE)
#else /* fid_WORDSIZE != 64 */
#define FILETOOLARGEERROR(ERR,FNAME)\
  fid_error_throw(ERR,"File \"%s\" is too large to be handled by "\
                  PACKAGE ". The maximum file size is limited to %lu bytes. "\
                  "Please consider compiling and using a 64 bit version.",\
                  (FNAME),(unsigned long)fid_FILE_MAXSIZE)
#endif /* fid_WORDSIZE */

/*!
 * \brief Map file into memory.
 * 
 * This function can operate in two basic modes, i.e., mapping an
 * existing file into memory, or creating a new memory mapped file.
 * If a file is created using this function, then a sparse one is created.
 * Values can easily be written to it since it is always mapped into
 * memory. Data is written to disk when the file is closed, when running
 * out of memory, or whenever the operating system thinks that writing to
 * disk is a good thing to do.
 * 
 * If this function fails and created a new file, then it will remove the
 * created file to leave no debris behind.
 *
 * This function, and fid_file_fake(), is the only initialization function for
 * fid_Mappedfile structures.
 * 
 * \param mfile The structure that represents a memory mapped file.
 * \param filename The name of the file to be opened.
 * \param writable Map file in read-only mode if 0, or writable if not 0.
 * \param create Create a new file if not 0. Note that an existing file
 * will be overritten.
 * \param initial_size When creating a new file, i.e., if \p create is not
 * 0, then make it this large. This value must be greater than 0. A sparse
 * file will be created (on systems that support it), so this can be set
 * to a rather large value without wasting time or disk space.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 otherwise.
 */
static int map_file(fid_Mappedfile *mfile, const char *filename,
                    int writable, int create, size_t initial_size,
                    fid_Error *error) /*{{{*/
{
  struct stat buf;
  int all_ok;
  
  assert(mfile != NULL);
  assert(filename != NULL);

  mfile->content=NULL;
  mfile->occupied=mfile->allocated=0;
  mfile->fd=0;
  mfile->filename=NULL;
  
  if(create)
  {
    assert(initial_size > 0);
    if(initial_size > fid_FILE_MAXSIZE)
    {
      FILETOOLARGEERROR(error,filename);
      return -1;
    }
    writable=1;
  }
  
  if((mfile->filename=strdup(filename)) == NULL)
  {
    fid_OUTOFMEM(error);
  }
  else
  {
    if(create)
    {
      mfile->fd=open(mfile->filename,O_CREAT|O_TRUNC|O_RDWR,
                     S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    }
    else
    {
      mfile->fd=open(mfile->filename,writable?O_RDWR:O_RDONLY,0);
    }
    
    if(mfile->fd == -1)
    {
      fid_error_throw(error,"Could not open file \"%s\".",mfile->filename);
    }
    else
    {
      all_ok=0;
      if(create)
      {
        mfile->occupied=0;
        mfile->allocated=initial_size;
        if(ftruncate(mfile->fd,(off_t)mfile->allocated) == 0)
        {
          all_ok=1;
        }
        else
        {
          fid_error_throw(error,"Could not create file \"%s\" of size "
                          "%lu bytes.",mfile->filename,
                          (unsigned long)mfile->allocated);
        }
      }
      else if(fstat(mfile->fd,&buf) == 0)
      {
        if(S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode))
        {
          if(buf.st_size <= (off_t)fid_FILE_MAXSIZE)
          {
            mfile->allocated=mfile->occupied=(size_t)buf.st_size;
            all_ok=1;
          }
          else
          {
            FILETOOLARGEERROR(error,filename);
          }
        }
        else
        {
          fid_error_throw(error,"Path \"%s\" is not a file.",mfile->filename);
        }
      }
      else
      {
        fid_error_throw(error,"Could not read file \"%s\".",mfile->filename);
      }
      
      if(all_ok)
      {
        mfile->mmap_prot=PROT_READ|(writable?PROT_WRITE:0);
        mfile->mmap_flags=writable?MAP_SHARED:MAP_PRIVATE;
        if((mfile->content=
            (unsigned char *)mmap(NULL,mfile->allocated,
                                  mfile->mmap_prot,mfile->mmap_flags,
                                  mfile->fd,(off_t)0)) != MAP_FAILED)
        {
          /* this is the only successful exit of this function */
          return 0;
        }
        else
        {
          fid_error_throw(error,"Could not map file \"%s\" into memory.",
                          mfile->filename);
          mfile->content=NULL;
        }
      }
      
      /* error occured, clean up */
      (void)close(mfile->fd);
      
      if(create)
      {
        /* it is safe to just remove the file here since we could only get
         * here if fid_Mappedfile::fd was valid \em and \c create is
         * non-zero, so a file must have been created by this function,
         * but failed later */
        (void)remove(mfile->filename);
      }
    }
    free(mfile->filename);
  }
  mfile->content=NULL;
  mfile->occupied=mfile->allocated=0;
  return -1;
}

/*}}}*/

/*!
 * \brief Map existing file into memory.
 * 
 * \param mfile The structure that represents a memory mapped file.
 * \param filename The name of the file to be opened.
 * \param writable Map file in read-only mode if 0, or writable if not 0.
 * \param may_prefetch Prefetch file automatically if non-zero, and if
 * allowed by global settings (controlled via environment variable, see
 * fid_options_parse()).
 * \param error Errors go here.
 * 
 * \returns 0 on success, -1 otherwise.
 */
int fid_file_map(fid_Mappedfile *mfile, const char *filename, int writable,
                 int may_prefetch, fid_Error *error) /*{{{*/
{
  int retval;

  if((retval=map_file(mfile,filename,writable,0,0,error)) == 0 && may_prefetch)
  {
    fid_options_parse();
    if(fid_options_prefetch)
    {
      /* prefetching allowed for this file, and enabled globally */
      fid_file_prefetch(mfile,fid_options_smart_prefetch);
    }
  }
  return retval;
}

/*}}}*/

/*!
 * \brief Create new file and map it writable into memory.
 * 
 * The initial file size is determined by the compile time constant
 * #fid_MAPPEDFILE_GROWSIZE. It must be truncated to the desired size after
 * it has been filled. Note that this is done automatically by functions
 * like fid_file_make_readonly() and fid_file_unmap().
 * 
 * \param mfile The structure that represents a memory mapped file.
 * \param filename The name of the file to be created.
 * \param error Errors go here.
 * 
 * \returns 0 on success, -1 otherwise.
 */
int fid_file_new(fid_Mappedfile *mfile, const char *filename, fid_Error *error) /*{{{*/
{
  return map_file(mfile,filename,1,1,fid_MAPPEDFILE_GROWSIZE,error);
}

/*}}}*/

/*!
 * \brief Create new file and map it writable into memory.
 * 
 * The same as fid_file_new(), but the initial file size can be set by
 * the caller.
 * 
 * \param mfile The structure that represents a memory mapped file.
 * \param filename The name of the file to be created.
 * \param size Initial size of the new file. This value must be greater
 * than 0.
 * \param error Errors go here.
 * 
 * \returns 0 on success, -1 otherwise.
 */
int fid_file_allocate(fid_Mappedfile *mfile, const char *filename,
                      size_t size, fid_Error *error) /*{{{*/
{
  return map_file(mfile,filename,1,1,size,error);
}

/*}}}*/

/*!
 * \brief Create a fake mapped file from dynamic array data.
 *
 * A fake mapped file is basically a dynamic array stored as mapped file. This
 * concept has been introduced to enable parsing sequence data to memory, using
 * dynamic arrays, and using that parsed data just like a mapped file. Please
 * note that most functions that do low-level stuff of mapped files do not, and
 * probably will never, work on fake mapped files.
 *
 * When calling fid_file_unmap() on a fake mapped file, pointer \p block passed
 * to this function will be \c free()'ed. This is for pure convenience so that
 * no special case distinction between real and fake files is required in
 * client code.
 *
 * This function, and the internal map_file(), is the only initialization
 * function for fid_Mappedfile structures.
 *
 * \param mfile The structure to be initialized.
 * \param block A pointer to allocated memory.
 * \param size Size of the data pointed to by \p block, in bytes.
 */
void fid_file_fake(fid_Mappedfile *mfile, void *block, size_t size)/*{{{*/
{
  assert(block != NULL);
  assert(size > 0);

  mfile->content=(unsigned char *)block;
  mfile->allocated=mfile->occupied=size;
  mfile->fd=-1;
  mfile->filename=NULL;
  mfile->mmap_prot=PROT_READ;
  mfile->mmap_flags=MAP_PRIVATE;
}
/*}}}*/

/*!
 * \brief Make memory mapped file larger.
 * 
 * This function enlarges the memory mapped file by \p size more bytes.
 * Note that this requires re-mapping the file and therefore the
 * fid_Mappedfile::content pointer might be changed by this call.
 * 
 * \param mfile The structure that represents a memory mapped file.
 * \param size Number of bytes to append to the file.
 * \param error Errors go here.
 * 
 * \note Repetitive invokation of this function for multiple simultaneously
 * open files may result in heavy fragmentation of virtual address space, and
 * thus unsuccessful calls to mmap() in this or other functions.
 *
 * \returns 0 on success, -1 otherwise.
 */
int fid_file_grow_by_size(fid_Mappedfile *mfile, size_t size,
                          fid_Error *error) /*{{{*/
{
  size_t oldsize;
  
  assert(mfile != NULL);
  assert(mfile->content != NULL);
  assert(mfile->occupied <= mfile->allocated);

  if(!fid_MAPPEDFILE_IS_WRITABLE(mfile))
  {
    fid_error_throw(error,"Cannot enlarge read-only file \"%s\".",
                    mfile->filename);
    return -1;
  }

  if(size > fid_FILE_MAXSIZE || mfile->allocated+size > fid_FILE_MAXSIZE)
  {
    fid_error_throw(error,"Cannot enlarge file \"%s\" by %lu "
                    "bytes, maximum file size of %lu exceeded.",
                    mfile->filename,(unsigned long)size,
                    (unsigned long)fid_FILE_MAXSIZE);
    return -1;
  }
  
  /* grow by given value */
  oldsize=mfile->allocated;
  mfile->allocated+=size;
  assert(mfile->allocated <= fid_FILE_MAXSIZE);
  
  if(ftruncate(mfile->fd,(off_t)mfile->allocated) == 0)
  {
#ifndef S_SPLINT_S
    (void)msync((caddr_t)mfile->content,oldsize,MS_ASYNC);
#endif /* !S_SPLINT_S */
    (void)munmap((caddr_t)mfile->content,oldsize);
    if((mfile->content=
        (unsigned char *)mmap(NULL,mfile->allocated,mfile->mmap_prot,
                              mfile->mmap_flags,mfile->fd,(off_t)0)) != MAP_FAILED)
    {
      return 0;
    }
    else
    {
      fid_error_throw(error,"Could not re-map file \"%s\".",mfile->filename);
      mfile->content=NULL;
    }
  }
  else
  {
    fid_error_throw(error,"Could not set size of file \"%s\" to %lu bytes.",
                    mfile->filename,(unsigned long)mfile->allocated);
  }
  
  return -1;
}

/*}}}*/

/*!
 * \brief Make sure that a certain number of bytes are free in the file.
 * 
 * If there are less than \p size bytes free in the file, make it larger
 * to make sure that at least \p size bytes will fit into it.
 * Note that this requires re-mapping the file, so the
 * fid_Mappedfile::content pointer may have changed after this call.
 * 
 * \param mfile The structure that represents a memory mapped file.
 * \param size Number of bytes required to be free in the file.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 otherwise.
 * 
 * \note Repetitive invokation of this function for multiple simultaneously
 * open files may result in heavy fragmentation of virtual address space, and
 * thus unsuccessful calls to mmap() in this or other functions.
 */
int fid_file_ensure_free_space(fid_Mappedfile *mfile, size_t size,
                               fid_Error *error) /*{{{*/
{
  size_t reqsize;
  
  assert(mfile != NULL);
  assert(mfile->content != NULL);
  assert(mfile->occupied <= mfile->allocated);
  
  if(!fid_MAPPEDFILE_IS_WRITABLE(mfile))
  {
    fid_error_throw(error,"Cannot enlarge read-only file \"%s\".",
                    mfile->filename);
    return -1;
  }

  if(size <= fid_FILE_MAXSIZE &&
     (reqsize=mfile->occupied+size) <= fid_FILE_MAXSIZE)
  {
    if(reqsize > mfile->allocated)
    {
      /* we allocate some more than requested to decrease the number of
       * resizes required by future allocations */
      return fid_file_grow_by_size(mfile,size+fid_MAPPEDFILE_GROWSIZE,error);
    }
    else
    {
      return 0;
    }
  }
  fid_error_throw(error,"Need %lu more bytes in file \"%s\", "
                  "but maximum file size of %lu would be "
                  "exceeded then.",(unsigned long)size,mfile->filename,
                  (unsigned long)fid_FILE_MAXSIZE);
  return -1;
}

/*}}}*/

/*!
 * \brief Truncate file to real size.
 * 
 * The real size of the file is defined by fid_Mappedfile::occupied. If the
 * file is read-only, then nothing happens.
 * 
 * \param mfile The structure that represents a memory mapped file.
 */
static void truncate_file(fid_Mappedfile *mfile) /*{{{*/
{
  int dummy;

  assert(mfile != NULL);
  assert(mfile->content != NULL);
  
  if(fid_MAPPEDFILE_IS_WRITABLE(mfile) && mfile->allocated > mfile->occupied)
  {
    dummy=ftruncate(mfile->fd,(off_t)mfile->occupied);
    (void)lseek(mfile->fd,(off_t)0,SEEK_END);
  }
}

/*}}}*/

/*!
 * \brief Write formatted string to mapped file.
 * 
 * This function acts like printf(3) and writes directly to a mapped file.
 * It enlarges the file as necessary, but note that since internally
 * vsnprintf(3) is used, this function has to allocate one byte more than
 * actually necessary because vsnprintf(3) always appends a '\\0' character
 * to its output. The fid_Mappedfile::occupied field is updated to not count
 * this extra byte, so it does not need to be adjusted externally.
 * 
 * Additionally, if the file size has to be increased, then it is increased
 * by a bit more than is actually required to decrease the chance of needing
 * more resizes in subsequent calls.
 * 
 * \param mfile The mapped file that should be written to.
 * \param error Error messages go here.
 * \param fmt Format string as required by vsnprintf(3), followed by data
 * to be formatted. If this is \c NULL, then this function does nothing.
 * 
 * \returns 0 on success, -1 otherwise.
 */
int fid_file_write(fid_Mappedfile *mfile, fid_Error *error, const char *fmt,
                   ...) /*{{{*/
{
  int bufsize, retval=0;
  va_list ap, copy;
#ifdef ENABLE_VSNPRINTF_WORKAROUND
  char dummy;
#endif /* ENABLE_VSNPRINTF_WORKAROUND */
  
  assert(mfile != NULL);
  
  if(fmt == NULL)
  {
    /* nothing to write */
    return 0;
  }

  va_start(ap,fmt);
#ifndef S_SPLINT_S
  va_copy(copy,ap);
#endif /* !S_SPLINT_S */
  bufsize=vsnprintf(
#ifdef ENABLE_VSNPRINTF_WORKAROUND
                    &dummy,1,
#else /* !ENABLE_VSNPRINTF_WORKAROUND */
                    NULL,0,
#endif /* ENABLE_VSNPRINTF_WORKAROUND */
                    fmt,copy)+1;
  va_end(copy);
  if(bufsize > 1)
  {
    /* print directly to file */
    fid_MAPPEDFILE_CHECKSPACE(mfile,bufsize,bufsize+1024,error,retval=-1;);
    if(retval == 0)
    {
#ifndef S_SPLINT_S
      va_copy(copy,ap);
#endif /* !S_SPLINT_S */
      (void)vsnprintf((char *)mfile->content+mfile->occupied,(size_t)bufsize,
                      fmt,copy);
      va_end(copy);
      mfile->occupied+=bufsize-1;
    }
  }
  va_end(ap);
  return retval;
}

/*}}}*/

/*!
 * \brief Make memory mapped file read-only.
 * 
 * This function truncates the file to its real length first (as indicated by
 * fid_Mappedfile::occupied), and then makes the file and memory map read-only.
 * The function will return immediately with no error if \p mfile is read-only
 * already.
 * 
 * \param mfile The structure that represents a memory mapped file.
 * \param error Errors go here.
 * 
 * \returns 0 on success, -1 otherwise.
 */
int fid_file_make_readonly(fid_Mappedfile *mfile, fid_Error *error) /*{{{*/
{
  struct stat buf;

  assert(mfile != NULL);
  assert(mfile->content != NULL);

  if(!fid_MAPPEDFILE_IS_WRITABLE(mfile))
  {
    /* don't try anything on read-only files */
#ifdef DEBUG
    fprintf(stderr,"%s(): file is read-only already, doing nothing.\n",
            __func__);
#endif /* DEBUG */
    return 0;
  }

  /* sync, unmap, truncate, re-open read-only, map read-only */
#ifndef S_SPLINT_S
  (void)msync((caddr_t)mfile->content,mfile->allocated,MS_ASYNC);
#endif /* !S_SPLINT_S */
  (void)munmap((caddr_t)mfile->content,mfile->allocated);
  truncate_file(mfile);
  mfile->allocated=mfile->occupied;
  mfile->mmap_prot=PROT_READ;
  mfile->mmap_flags=MAP_PRIVATE;
  mfile->content=NULL;

  if(close(mfile->fd) == 0)
  {
    if((mfile->fd=open(mfile->filename,O_RDONLY)) != -1)
    {
      if(fstat(mfile->fd,&buf) == 0)
      {
        if(buf.st_size == (off_t)mfile->allocated)
        {
          if(buf.st_size == 0)
          {
            /* files of size 0 cannot be mapped */
            return 0;
          }

          if((mfile->content=
              (unsigned char *)mmap(NULL,mfile->allocated,
                                    mfile->mmap_prot,mfile->mmap_flags,
                                    mfile->fd,(off_t)0)) != MAP_FAILED)
          {
            return 0;
          }
          fid_error_throw(error,
                          "Could not make file \"%s\" read-only (mmap failed).",
                          mfile->filename);
          mfile->content=NULL;
        }
        else
        {
          fid_error_throw(error,
                          "Could not make file \"%s\" read-only (file size "
                          "changed unexpectedly).",mfile->filename);
        }
      }
      else
      {
        fid_error_throw(error,
                        "Could not make file \"%s\" read-only (fstat failed).",
                        mfile->filename);
      }
      (void)close(mfile->fd);
    }
    else
    {
      fid_error_throw(error,
                      "Could not make file \"%s\" read-only (open failed).",
                      mfile->filename);
    }
  }
  else
  {
    fid_error_throw(error,
                    "Could not make file \"%s\" read-only (close failed).",
                    mfile->filename);
  }

  return -1;
}

/*}}}*/

/*!
 * \brief Write content of a (fake) mapped file to new file.
 *
 * This function is mainly used for writing fake mapped files to real files.
 * Fake mapped files refer to allocated memory, which can be written to real
 * files using this function. A new file is created by this function, or
 * overwritten if a file of given name existed already, and the data the file
 * structure refers to is written to it.
 *
 * The destination file is deleted if writing fails, i.e., this function leaves
 * no partial files around. Interrupted system calls are handled correctly.
 *
 * This function does not restrict the input to fake mapped files. Thus, the
 * effect of applying this function to a real mapped file is that the input
 * file is copied to a new file of the given name.
 *
 * \param mfile The structure that represents a (fake) memory mapped file.
 * \param filename Name of the output file.
 * \param error Errors go here.
 *
 * \returns 0 on success, -1 otherwise.
 */
int fid_file_dump_to_file(const fid_Mappedfile *mfile, const char *filename,
                          fid_Error *error)/*{{{*/
{
  int fd, errnum;
  const unsigned char *data;
  size_t remaining;
  ssize_t written;
#ifdef FID_MAX_WRITE_SIZE
  size_t write_now;
#endif /* FID_MAX_WRITE_SIZE */

  assert(mfile != NULL);
  assert(mfile->content != NULL);
  assert(filename != NULL);

  if((fd=open(filename,O_CREAT|O_TRUNC|O_RDWR,
              S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) == -1)
  {
    fid_error_throw(error,"Could not create file \"%s\".",filename);
    return -1;
  }

  data=mfile->content;
  remaining=mfile->occupied;
  while(remaining > 0)
  {
#ifdef FID_MAX_WRITE_SIZE
    if((write_now=remaining) > FID_MAX_WRITE_SIZE) write_now=FID_MAX_WRITE_SIZE;
#endif /* FID_MAX_WRITE_SIZE */

    errno=0;
    written=write(fd,data,
#ifdef FID_MAX_WRITE_SIZE
                  write_now
#else /* !FID_MAX_WRITE_SIZE */
                  remaining
#endif /* FID_MAX_WRITE_SIZE */
                 );
    errnum=errno;

    if(written != -1)
    {
      data+=written;
      assert(remaining >= (size_t)written);
      remaining-=written;
      assert(data <= mfile->content+mfile->occupied);
    }

    if(errnum != 0 && errnum != EINTR)
    {
      /* error occurred, and it was not an interrupted system call */
      fid_error_throw_errno(error,errnum,"write()");
      fid_error_throw(error,"Could not write to file \"%s\".",filename);
      break;
    }
  }

  (void)close(fd);

  if(remaining > 0)
  {
    /* error occured, remove file */
    (void)remove(filename);
    return -1;
  }

  return 0;
}
/*}}}*/

/*!
 * \brief Close memory mapped file and remove it from disk.
 * 
 * This function comes handy in error conditions. It closes the associated
 * file and removes it from disk afterwards. It does not work for fake mapped
 * files.
 * 
 * \param mfile The structure that represents a memory mapped file.
 */
void fid_file_cleanup(fid_Mappedfile *mfile) /*{{{*/
{
  assert(mfile != NULL);
  if(mfile->content == NULL || mfile->fd == -1 || mfile->filename == NULL)
  {
    return;
  }
  
  (void)munmap((caddr_t)mfile->content,mfile->allocated);
  (void)close(mfile->fd);
  (void)remove(mfile->filename);
  free(mfile->filename);
  mfile->content=NULL;
  mfile->occupied=mfile->allocated=0;
}

/*}}}*/

/*!
 * \brief Ask the kernel if the file is in cache already.
 *
 * At 100 different offsets spread across the file, the kernel is asked,
 * using the \c mincore() call, whether the corresponding page is in cache
 * or not. If 90 or more of these are in cache already, then there is
 * probably enough of the file in cache (roughly 90%) for not to cause a
 * big performance hit.
 *
 * \param mfile The file to be checked.
 * \param minsize Files smaller than this size are always considered \em not
 * to be in cache, This is because prefetching small files is not expensive,
 * and it is not worthwhile to check whether they are in cache or not.
 * \param pgsize The page size as returned by \c getpagesize().
 *
 * \returns 1 if the file is in cache, 0 if it is not.
 *
 * \note This function always returns 0 on systems that do not have a
 * \c mincore() implementation. Thus, we always assume pessimistically that
 * no file is in cache on these systems.
 *
 * \todo It seems as if either 100 sample points are not enough, or that 90%
 * cache coverage is not enough on Solaris. Often files are not prefetched,
 * leading to a major performance hit. Edit: seems like many times it is also
 * the other way around on Solaris, probably for large files. Forcing
 * prefetching off is better then.
 */
static int file_already_prefetched(const fid_Mappedfile *mfile,
                                   size_t minsize, int pgsize)/*{{{*/
{
#ifdef HAVE_MINCORE
#ifdef MINCORE_SIGNED_CHAR
  char vec;
#else /* !MINCORE_SIGNED_CHAR */
  unsigned char vec;
#endif /* MINCORE_SIGNED_CHAR */
  double frac;
  size_t offset, i, count;

  if(mfile->occupied < minsize)
  {
    /* we only try to act smart if the file is big enough */
    return 0;
  }

  count=0;
  frac=(double)mfile->occupied/100.0;
  for(i=0; i < 100; ++i)
  {
    offset=pgsize*(((size_t)((double)i*frac))/pgsize);
    assert(offset < mfile->occupied);
    assert((offset%(size_t)pgsize) == 0);
    if(mincore((caddr_t)mfile->content+offset,(size_t)1,&vec) != 0)
    {
      fprintf(stderr,"%s(): mincore() failed\n",__func__);
      return 0;
    }
    if((vec&MINCORE_INCORE) != 0)
    {
      if(++count >= 90)
      {
        return 1;
      }
    }
  }
#endif /* HAVE_MINCORE */

  return 0;
}
/*}}}*/

/*!
 * \brief Read whole mapped file to fill the operating system's file cache.
 *
 * When searching in an enhanced suffix array, some tables are accessed in a
 * more or less random order, e.g., the sequence data itself, or the inverse
 * suffix array. The operating system must read values from file if not read
 * before (and present in the file cache) already, resulting in an extremely
 * expensive search operation on the harddisk. While reading files
 * sequentially is usually reasonably fast, accessing files in random order
 * can be slower by orders of magnitude. Hence, for reducing the negative
 * effects of slow harddisk searches, this function can be called to read the
 * whole file in order to fill the operating system's file cache.
 *
 * Assuming that the operating system always reads and caches whole memory
 * pages when reading single values from a page, this function calls
 * \c getpagesize() exactly one time, and reads only the first byte of every
 * page from the file. The function will fail silently every time it is called
 * should \c getpagesize() have returned a non-positive value (a warning will
 * be printed to \c stderr in this case).
 *
 * \param mfile The mapped file to be prefetched. If \p mfile or
 * fid_Mappedfile::content is NULL, then this function will do nothing.
 * \param smart Try to be smart if non-zero. If non-zero, and, by some
 * probability, at least 90% of the file is in cache already, then the file
 * is not read again, and thus this function returns quickly. Otherwise, the
 * whole file is read.
 */
void fid_file_prefetch(const fid_Mappedfile *mfile, int smart)/*{{{*/
{
  const unsigned char *ptr, *maxptr;
  volatile unsigned char value;
  static int increment=0;
  static size_t minsize=0;

  if(mfile == NULL || mfile->content == NULL)
  {
    return;
  } 

  if(increment <= 0)
  {
    if(increment == 0)
    {
#ifdef HAVE_GETPAGESIZE
      increment=getpagesize();
#elif defined HAVE_SYSCONF
      increment=(int)sysconf(_SC_PAGE_SIZE);
#else /* !(HAVE_GETPAGESIZE || HAVE_SYSCONF) */
      /* This case should have been catched by configure. */
      abort();
#endif /* HAVE_GETPAGESIZE || HAVE_SYSCONF */
      if(increment <= 0)
      {
        fprintf(stderr,
#ifdef HAVE_GETPAGESIZE
                "getpagesize()"
#elif defined HAVE_SYSCONF
                "sysconf(_SC_PAGE_SIZE)"
#endif /* HAVE_GETPAGESIZE || HAVE_SYSCONF */
                " returned %d\n",increment);
        increment=-1;
        return;
      }
      minsize=(size_t)64*(size_t)increment;
    }
    else
    {
      return;
    }
  }

  if(smart && file_already_prefetched(mfile,minsize,increment))
  {
    return;
  }

  ptr=mfile->content;
  maxptr=ptr+mfile->occupied;
  while(ptr < maxptr)
  {
    value=*ptr;
    ptr+=increment;
  } 
}
/*}}}*/

/*!
 * \brief Close memory mapped file.
 * 
 * If the file is writable and its allocated size does not match its real
 * size, then it is truncated to its real size before it gets closed. A file
 * of zero length will not be deleted automatically.
 *
 * For fake mapped files, the fid_Mappedfile::content pointer will be
 * \c free()'ed.
 * 
 * \param mfile The structure that represents a memory mapped file.
 */
void fid_file_unmap(fid_Mappedfile *mfile) /*{{{*/
{
  assert(mfile != NULL);
  if(mfile->content == NULL)
  {
    return;
  }
  assert(mfile->content != MAP_FAILED || mfile->fd == -1);
  assert(mfile->allocated >= mfile->occupied);

  if(mfile->fd != -1)
  {
    /* real file */
    assert(mfile->filename != NULL);
  
#ifndef S_SPLINT_S
    (void)msync((caddr_t)mfile->content,mfile->allocated,MS_ASYNC);
#endif /* !S_SPLINT_S */
    (void)munmap((caddr_t)mfile->content,mfile->allocated);
    truncate_file(mfile);
    (void)close(mfile->fd);
    free(mfile->filename);
  }
  else
  {
    /* fake mapped file */
    free(mfile->content);
  }

  mfile->content=NULL;
  mfile->occupied=mfile->allocated=0;
}

/*}}}*/

/*!
 * \brief Allocate some memory to hold the generated filenames.
 * 
 * \param fnamebuf The structure to be initialized.
 * \param filename The base file name from which names are generated.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 if out of memory. Results are return in
 * arguments.
 */
int fid_filenamebuffer_init(fid_Filenamebuffer *fnamebuf, const char *filename,
                            fid_Error *error) /*{{{*/
{
  size_t baselength;
  
  assert(fnamebuf != NULL);
  assert(filename != NULL);
  
  baselength=strlen(filename);
  if((fnamebuf->buffer=(char *)malloc(sizeof(char)*(baselength+5))) == NULL)
  {
    fid_OUTOFMEM(error);
    return -1;
  }
  memcpy(fnamebuf->buffer,filename,baselength);
  fnamebuf->bufptr=fnamebuf->buffer+baselength;
  *fnamebuf->bufptr='.';
  ++fnamebuf->bufptr;
  return 0;
}

/*}}}*/

/*!
 * \brief Allocate file name buffer if necessary.
 * 
 * \param fnamebuf The file name buffer to be possibly initialized.
 * \param extbuffer Pointer to a file name buffer pointer. If the pointed-to
 * pointer is NULL, then \p fnamebuf will be initialized and the pointer is
 * set to \p fnamebuf, otherwise nothing will happen.
 * \param filename The base file name from which names are generated.
 * \param error Error messages go here.
 * 
 * \retval 0 if no memory has been allocated.
 * \retval 1 if a new file name buffer has been allocated.
 * \retval -1 on error.
 */
int fid_filenamebuffer_init_local(fid_Filenamebuffer *fnamebuf,
                                  fid_Filenamebuffer **extbuffer,
                                  const char *filename, fid_Error *error) /*{{{*/
{
  assert(fnamebuf != NULL);
  assert(extbuffer != NULL);
  assert(filename != NULL);
  
  if(*extbuffer == NULL)
  {
    *extbuffer=fnamebuf;
    return (fid_filenamebuffer_init(fnamebuf,filename,error) == -1)?-1:1;
  }
  return 0;
}

/*}}}*/

/*!
 * \brief Free file name buffer.
 * 
 * \param fnamebuf Structure to be freed.
 */
void fid_filenamebuffer_free(fid_Filenamebuffer *fnamebuf) /*{{{*/
{
  assert(fnamebuf != NULL);
  assert(fnamebuf->buffer != NULL);
  assert(fnamebuf->bufptr != NULL);
  free(fnamebuf->buffer);
}

/*}}}*/

/*!
 * \brief Construct filename from base name and extension.
 * 
 * \param basefilename Base filename.
 * \param fileext File name extension, such like "tis". Note that this
 * absolutely \e must consist of three printable zero-terminated characters,
 * and should not include a leading dot.
 * \param error Error messages go here.
 * 
 * \returns A pointer to an allocated string, or NULL on error.
 */
char *fid_filename_create(const char *basefilename, const char *fileext,
                          fid_Error *error) /*{{{*/
{
  fid_Filenamebuffer fnamebuf;
  
  assert(basefilename != NULL);
  assert(fileext != NULL);
  assert(strlen(fileext) == (size_t)3);
  
  if(fid_filenamebuffer_init(&fnamebuf,basefilename,error) == 0)
  {
    memcpy(fnamebuf.bufptr,fileext,(size_t)4);
    return fnamebuf.buffer;
  }
  else
  {
    return NULL;
  }
}

/*}}}*/
