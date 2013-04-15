/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2007  Robert Homann
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

#include <string.h>
#include <stdlib.h>

#include "libdefs.h"
#include "options.h"

int fid_options_prefetch=1;
int fid_options_smart_prefetch=1;

/*!
 * \brief Parse environment variable for library configuration.
 *
 * This function is called automatically by functions that can be influenced
 * by an environment variable whose name is defined by #FID_OPTIONS_VARNAME,
 * and is usually not called directly from client code. The function parses the
 * content of the environment variable once only.
 *
 * The environment variable is expected to contain a colon separated list of
 * option names. Currently, the following options are recognized (unknown
 * options are silently ignored).
 * - \c NOPREFETCH Usually every file opened for reading is prefetched
 *   automatically to fill the file cache in order to prevent excessive time
 *   consumption for searching within files. Setting this option disables
 *   automatic prefetching entirely.
 * - \c FORCEPREFETCH If prefetching is not disabled, then the default
 *   behavior is to prefetch a file only if it is very small, or if the
 *   kernel reports that it, or large parts thereof, are not present in file
 *   cache already. Setting this option causes the library to always read the
 *   whole file (still quite fast when cached already, but much slower than
 *   asking the kernel beforehand and relying on these information).
 *
 * \note Global variables #fid_options_prefetch and #fid_options_smart_prefetch
 * are set by this function according to the content of environment variable
 * #FID_OPTIONS_VARNAME. <em>Do not set these variables manually.</em>
 */
void fid_options_parse(void)
{
  static int initialized;
  char *string, *saveptr, *token;

  if(initialized)
  {
    return;
  }

  initialized=1;

  if((string=getenv(FID_OPTIONS_VARNAME)) == NULL)
  {
    return;
  }

  token=strtok_r(string,":",&saveptr);
  while(token != NULL)
  {
    if(strcmp(token,"NOPREFETCH") == 0)
    {
      fid_options_prefetch=0;
    }
    else if(strcmp(token,"FORCEPREFETCH") == 0)
    {
      fid_options_smart_prefetch=0;
    }
    token=strtok_r(NULL,":",&saveptr);
  }
}

