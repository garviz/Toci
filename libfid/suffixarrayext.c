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

#include "libdefs.h"
#include "error.h"
#include "arrays.h"
#include "alphabet.h"
#include "fileutils.h"
#include "sequences.h"
#include "dbfiles.h"
#include "suffixarray.h"
#include "projectfile.h"
#include "suffixarrayext.h"
#include "verify.h"

#include "suffixarray.32"
#include "suffixarrayext.32"
#include "suffixarrayext.templ.c"

#include "suffixarray.64"
#include "suffixarrayext.64"
#include "suffixarrayext.templ.c"

#include "suffixarray.undef"
#include "suffixarrayext.undef"

/*!
 * \brief Load enhanced suffix array based on the given project data structure.
 * 
 * Given a parsed project file, open all requested tables and put these
 * into the passed enhanced suffix array data structure.
 * 
 * \param esa The enhanced suffix array to be initialized.
 * \param project Structure representing a parsed enhanced suffix array
 * project file. This is required to intialize several data fields of
 * \p esa and to perform some integrity checks.
 * \param tables Bitvector of requested tables.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_suffixarray_load_from_project(fid_Suffixarray *esa,
                                      fid_Projectfile *project,
                                      fid_Tablerequest tables,
                                      fid_Error *error) /*{{{*/
{
  int retval=1;

  assert(esa != NULL);
  assert(project != NULL);
  
  VERIFY_REQUEST(tables,error);
  
  if((retval=fid_suffixarray_load_from_files(esa,project->prjbasename,tables,
                                             project->uisize,error)) == 0)
  {
    fid_SWITCH48(project->uisize,
    {
      if(sanity_checks_32(esa,project,tables,error) == 0)
      {
        return 0;
      }
    },
    {
      if(sanity_checks_64(esa,project,tables,error) == 0)
      {
        return 0;
      }
    });
  }

  if(retval == 0)
  {
    /* throw away everything on integrity error */
    fid_suffixarray_free(esa);
  }

  return -1;
}

/*}}}*/

/*!
 * \brief Load enhanced suffix array project and initialize data structures.
 * 
 * Like fid_suffixarray_load(), this function opens and parses a project
 * file, opens all requested tables and put these into the passed enhanced
 * suffix array data structure. It takes two additional arguments, which,
 * however, are not needed in most cases.
 * 
 * \param esa The enhanced suffix array to be initialized.
 * \param prjbasename Base filename of the enhanced suffix array project.
 * \param tables Bitvector of requested tables.
 * \param uisize_request Loading fails if the integer size does not match this
 * value. Pass #fid_UINTSIZE_UNDEF to accept any integer size.
 * \param ignore_byteorder Ignore byte order as given in the project file if
 * set to True. If the project file does not contain a \c littleendian line,
 * then this option has no effect. This option is only useful for programs
 * that can either handle binary data in non-native byte order, or that just
 * need to check if all files needed can be read and have certain sizes.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_suffixarray_load_special(fid_Suffixarray *esa, const char *prjbasename,
                                 fid_Tablerequest tables,
                                 fid_Uintsize uisize_request,
                                 int ignore_byteorder, fid_Error *error) /*{{{*/
{
  fid_Projectfile project;
  int retcode;
  
  VERIFY_REQUEST(tables,error);
  
  if((retcode=fid_projectfile_parse_from_file(&project,prjbasename,error)) == 0)
  {
    if(ignore_byteorder ||
       (project.endianess_known &&
#ifdef WORDS_BIGENDIAN
        !project.littleendian
#else /* !WORDS_BIGENDIAN */
        project.littleendian
#endif /* WORDS_BIGENDIAN */
       ))
    {
      if(uisize_request == fid_UINTSIZE_UNDEF ||
         uisize_request == project.uisize)
      {
        retcode=fid_suffixarray_load_from_project(esa,&project,tables,error);
      }
      else
      {
        fid_error_throw(error,"Enhanced suffix array \"%s\" has integer size "
                        "%u, requested size %u.",
                        prjbasename,(project.uisize == fid_UINTSIZE_32)?32:64,
                        (uisize_request == fid_UINTSIZE_32)?32:64);
        retcode=-1;
      }
    }
    else
    {
      if(project.endianess_known)
      {
        fid_error_throw(error,"Enhanced suffix array \"%s\" has wrong "
                        "byte-order (%s endian), cannot use it on this "
                        "architecture.",
                        prjbasename,project.littleendian?"little":"big");
      }
      else
      {
        fid_error_throw(error,"Enhanced suffix array \"%s\" has unknown "
                        "byte-order, cannot use it. Set the littleendian "
                        "variable in file %s.prj to the correct value (i.e., "
                        "either 0 or 1), and try again.",
                        prjbasename,prjbasename);
      }
      retcode=-1;
    }
    fid_projectfile_free(&project);
  }
  
  return retcode;
}

/*}}}*/

/*!
 * \brief Load enhanced suffix array project and initialize data structures.
 * 
 * This is arguably the most important and high-level function of the whole
 * library. Given a base filename, open and parse the corresponding
 * project file, then open all requested tables and put these into the
 * passed enhanced suffix array data structure.
 * 
 * \param esa The enhanced suffix array to be initialized.
 * \param prjbasename Base filename of the enhanced suffix array project.
 * \param tables Bitvector of requested tables.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 *
 * \note If you need to ignore the byte order or need a special integer size,
 * use fid_suffixarray_load_special() instead. This function is just a wrapper
 * around that function.
 */
int fid_suffixarray_load(fid_Suffixarray *esa, const char *prjbasename,
                         fid_Tablerequest tables, fid_Error *error)/*{{{*/
{
  return fid_suffixarray_load_special(esa,prjbasename,tables,
                                      fid_UINTSIZE_UNDEF,0,error);
}
/*}}}*/

/*!
 * \brief Initialize fid_Projectfile structure from mapped enhanced suffix
 * array.
 * 
 * This function is useful for generating enhanced suffix array projects from
 * parsed sequence data. Generate enhanced suffix array tables first, then
 * call this function to fill the fid_Projectfile data structure from these.
 * 
 * \param project The profile file structure to be initialized.
 * \param dbfiles Names and lengths of the original input sequence files.
 * \param prjbasename Base filename of the enhanced suffix array project.
 * \param esa Structure that has some enhanced suffix array tables associated.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
int fid_projectfile_init_from_suffixarray(fid_Projectfile *project,
                                          const fid_DBfiles *dbfiles,
                                          const char *prjbasename,
                                          const fid_Suffixarray *esa,
                                          fid_Error *error) /*{{{*/
{
  if(fid_projectfile_init(project,prjbasename,error) == -1)
  {
    return -1;
  }
  
  if(fid_dbfiles_copy(&project->dbfiles,dbfiles,error) == -1)
  {
    return -1;
  }

  project->uisize=esa->uisize;
  
  fid_SWITCH48(project->uisize,
  {
    project->integer_size_in_bytes=4;
    init_from_esa_32(project,esa);
  },
  {
    project->integer_size_in_bytes=8;
    init_from_esa_64(project,esa);
  });

  project->integer_size_in_bits=8*project->integer_size_in_bytes;
  
  return 0;
}

/*}}}*/
