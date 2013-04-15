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

#ifndef OPTIONS_H
#define OPTIONS_H
/*!\addtogroup options Library configuration
 * The behavior of the library can be influencent by setting an environment
 * variable, whose name is defined by #FID_OPTIONS_VARNAME. See the
 * documentation for fid_options_parse() to find a list of values it may
 * take. */
/*@{*/

/*!\brief Name of environment variable parsed by \e libfid. */
#define FID_OPTIONS_VARNAME  "FID_OPTIONS"

/*!\internal */
extern int fid_options_prefetch;

/*!\internal */
extern int fid_options_smart_prefetch;

#ifdef __cplusplus
extern "C" {
#endif
void fid_options_parse(void);
#ifdef __cplusplus
}
#endif
/*@}*/

#endif /* !OPTIONS_H */
