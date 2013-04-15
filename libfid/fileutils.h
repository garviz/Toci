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

#ifndef FILEUTILS_H
#define FILEUTILS_H
/*!\addtogroup files File handling routines
 * This group contains various low-level and high-level routines for
 * operating on files.
 */
/*@{*/
/*!
 * \brief Representation of a memory mapped file.
 * 
 * Among others, this structure bundles the file name, a file descriptor,
 * and the pointer returned by mmap() to the file content. The protection
 * and flags argument passed to mmap() are also stored for re-mapping.
 * 
 * Additionally, this structure allows the use of a memory mapped file as
 * a dynamically growing array. The current size of real data is stored in
 * fid_Mappedfile::occupied, whereas the allocated disk space is stored in
 * fid_Mappedfile::allocated. Whenever fid_Mappedfile::occupied exceeds
 * fid_Mappedfile::allocated, enlarge the file, re-map, and continue.
 *
 * To support the use of allocated dynamic arrays as mapped files, the concept
 * of fake mapped files has been implemented. Fake mapped files do not have a
 * valid file descriptor, but point to allocated memory. Most functions defined
 * on the fid_Mappedfile structure, however, do not work for fake mapped files,
 * so their use is somewhat limited. Their main purpose is to enable existing
 * code written for reading from memory mapped files to read from allocated
 * memory. A fake mapped file can be identified by using the macro
 * fid_MAPPEDFILE_IS_FAKED() on a fid_Mappedfile structure pointer.
 */
typedef struct
{
  unsigned char *content;  /*!<\brief Memory mapped file content. */
  size_t occupied;     /*!<\brief Number of bytes used in file. */
  size_t allocated;    /*!<\brief Allocated number of bytes in file. */
  int fd;              /*!<\brief File descriptor as returned by open(). */
  char *filename;      /*!<\brief Name of the file in file system, mainly used
                        * for diagnostic messages. */
  int mmap_prot;       /*!<\brief Protection flags as passed to last call of
                        * mmap() for this file; used or modified for mapping
                        * the file again at some later point. */
  int mmap_flags;      /*!<\brief Other flags as passed to last call of mmap()
                        * for this file. \see fid_Mappedfile::mmap_prot. */
} fid_Mappedfile;

/*!
 * \brief Buffer for creating filenames from basename and extension.
 * 
 * The allocated buffer pointed to by fid_Filenamebuffer::buffer contains a
 * basename, like "/tmp/myindex", followed by a trailing dot, ".". The
 * buffer's size corresponds to the length of the basename plus 5, one for
 * the appended dot, 3 for the extension, and 1 for the terminator. Pointer
 * fid_Filenamebuffer::bufptr points two bytes after the end of the basename
 * and hence right after the dot, into fid_Filenamebuffer::buffer.
 * 
 * This construction is meant to enable easy and fast filename construction.
 * Given a filename extension with a length of three characters, this
 * extension can simply be copied to fid_Filenamebuffer::bufptr to create a
 * full filename.
 */
typedef struct
{
  char *buffer; /*!<\brief An allocated buffer that holds a filename. */
  char *bufptr; /*!<\brief A pointer into fid_Filenamebuffer::buffer that
                 * points right after the "." character to enable fast copying
                 * of file name extensions to the end of the base name. */
} fid_Filenamebuffer;

/*!
 * \brief Number of bytes to append to a file when running out of space.
 */
#define fid_MAPPEDFILE_GROWSIZE  ((size_t)524288)

/*!
 * \brief Cast pointer into pointer of different type.
 * 
 * With gcc 3.1, a bare cast of unsigned chars to unsigned integers yields a
 * warning (cast increases required alignment of target type), hence we add
 * a prior cast to \c void to shut up the compiler in these cases.
 */
#define fid_CAST_POINTER(P,T)  ((T *)((void *)(P)))

/*!
 * \brief Check if the mapped file is writable.
 *
 * The check is performed by checking the fid_Mappedfile::mmap_prot field.
 *
 * \param MF A pointer to a fid_Mappedfile structure.
 *
 * \returns True if the file is writable, false otherwise.
 */
#define fid_MAPPEDFILE_IS_WRITABLE(MF)  (((MF)->mmap_prot&PROT_WRITE) != 0)

/*!
 * \brief Check if the mapped file is really a file or not.
 *
 * \param MF A pointer to a fid_Mappedfile structure.
 * 
 * \returns True if the file is not a real file, but a faked one. This means,
 * the content pointer really points to allocated memory that has been obtained
 * via \c malloc(), not via \c mmap().
 */
#define fid_MAPPEDFILE_IS_FAKED(MF) ((MF)->content != NULL && (MF)->fd == -1)

/*!
 * \brief Make sure that the mapped file has space to store \p N bytes.
 * 
 * When running out of space, i.e., if the file is too short to store \p N
 * bytes, the file length will be increased by \p GROW bytes.
 * 
 * \param MF A pointer to a fid_Mappedfile structure.
 * \param N Requested size.
 * \param GROW Number of bytes to add to mapped file if too short.
 * \param ERR Pointer to a fid_Error structure.
 * \param ERRCODE Code to be executed in case of an error condition.
 *
 * \see For this macro makes use of fid_file_grow_by_size(), you may consider
 * reading the notes attached to that function.
 */
#define fid_MAPPEDFILE_CHECKSPACE(MF,N,GROW,ERR,ERRCODE)\
  if((MF)->occupied+(N) > (MF)->allocated)\
  {\
    if(fid_file_grow_by_size(MF,GROW,ERR) == -1)\
    {\
      ERRCODE;\
    }\
  }

/*!
 * \brief Append value of given type to end of file.
 * 
 * Use this macro only if you are absolutely sure that the file's allocated
 * size is not exceeded by appending the value to it. Use
 * fid_MAPPEDFILE_APPEND_GROW() or fid_MAPPEDFILE_APPEND_TRY() otherwise.
 * 
 * \param MF A pointer to a fid_Mappedfile structure.
 * \param TYPE Type of the value to be stored. This should be a basic C
 * type, no structure.
 * \param VAL The value to be stored. It will be casted to \p TYPE by this
 * macro.
 */
#define fid_MAPPEDFILE_APPEND_UNSAFE(MF,TYPE,VAL)\
  *fid_CAST_POINTER(&(MF)->content[(MF)->occupied],TYPE)=(TYPE)(VAL);\
  (MF)->occupied+=sizeof(TYPE)

/*!
 * \brief Append value of given type to end of file, increase size when needed.
 * 
 * When running out of space, i.e., if the file is too short to store the
 * given value, the file length will be increased by #fid_MAPPEDFILE_GROWSIZE
 * bytes.
 * 
 * \param MF A pointer to a fid_Mappedfile structure.
 * \param TYPE Type of the value to be stored. This should be a basic C
 * type, no structure.
 * \param VAL The value to be stored. It will be casted to \p TYPE by this
 * macro.
 * \param ERR Pointer to a fid_Error structure.
 * \param ERRCODE Code to be executed is case of an error condition.
 */
#define fid_MAPPEDFILE_APPEND_GROW(MF,TYPE,VAL,ERR,ERRCODE)\
  fid_MAPPEDFILE_CHECKSPACE(MF,sizeof(TYPE),fid_MAPPEDFILE_GROWSIZE,ERR,ERRCODE)\
  fid_MAPPEDFILE_APPEND_UNSAFE(MF,TYPE,VAL);\

/*!
 * \brief Append value of given type to end of file, increase size when needed.
 * 
 * When running out of space, i.e., if the file is too short to store the
 * given value, the file length will be increased by #fid_MAPPEDFILE_GROWSIZE
 * bytes. The macro does nothing if the file content is \c NULL, i.e., if no
 * open file is associated with \p MF, hence this macro is safe to be used at
 * any time.
 * 
 * \param MF A pointer to a fid_Mappedfile structure.
 * \param TYPE Type of the value to be stored. This should be a basic C
 * type, no structure.
 * \param VAL The value to be stored. It will be casted to \p TYPE by this
 * macro.
 * \param ERR Pointer to a fid_Error structure.
 * \param ERRCODE Code to be executed is case of an error condition.
 */
#define fid_MAPPEDFILE_APPEND_TRY(MF,TYPE,VAL,ERR,ERRCODE)\
  if((MF)->content != NULL)\
  {\
    fid_MAPPEDFILE_APPEND_GROW(MF,TYPE,VAL,ERR,ERRCODE);\
  }

#ifdef __cplusplus
extern "C" {
#endif
int fid_file_map(fid_Mappedfile *mfile, const char *filename, int writable,
                 int may_prefetch, fid_Error *error);
int fid_file_new(fid_Mappedfile *mfile, const char *filename, fid_Error *error);
int fid_file_allocate(fid_Mappedfile *mfile, const char *filename,
                      size_t size, fid_Error *error);
void fid_file_fake(fid_Mappedfile *mfile, void *block, size_t size);
int fid_file_grow_by_size(fid_Mappedfile *mfile, size_t size,
                          fid_Error *error);
int fid_file_ensure_free_space(fid_Mappedfile *mfile, size_t size,
                               fid_Error *error);
#ifdef __GNUC__
int fid_file_write(fid_Mappedfile *mfile, fid_Error *error,
                   const char *fmt, ...)
  __attribute__ ((format (printf, 3, 4)));
#else /* !__GNUC__ */
int fid_file_write(fid_Mappedfile *mfile, fid_Error *error,
                   const char *fmt, ...);
#endif /* __GNUC__ */
int fid_file_dump_to_file(const fid_Mappedfile *mfile, const char *filename,
                          fid_Error *error);
int fid_file_make_readonly(fid_Mappedfile *mfile, fid_Error *error);
void fid_file_cleanup(fid_Mappedfile *mfile);
void fid_file_prefetch(const fid_Mappedfile *mfile, int smart);
void fid_file_unmap(fid_Mappedfile *mfile);
int fid_filenamebuffer_init(fid_Filenamebuffer *fnamebuf, const char *filename,
                            fid_Error *error);
int fid_filenamebuffer_init_local(fid_Filenamebuffer *fnamebuf,
                                  fid_Filenamebuffer **extbuffer,
                                  const char *filename, fid_Error *error);
void fid_filenamebuffer_free(fid_Filenamebuffer *fnamebuf);
/*@null@*/
char *fid_filename_create(const char *basefilename, const char *fileext,
                          fid_Error *error);
#ifdef __cplusplus
}
#endif
/*@}*/

#endif /* !FILEUTILS_H */
