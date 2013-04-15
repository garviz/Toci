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

#ifndef PROJECTFILEPARSER_H
#define PROJECTFILEPARSER_H

#define PARSEBUFFERSIZE  ((size_t)128)

#define TOKEN_INSERT(S,D) {S,sizeof(S)-1,0,((D) == 0)?0:1}
 
typedef enum
{
  TOKEN_DBFILE=0,
  TOKEN_TOTALLENGTH,
  TOKEN_NUMOFSEQUENCES,
  TOKEN_NUMOFDBSEQUENCES,
  TOKEN_NUMOFQUERYSEQUENCES,
  TOKEN_PREFIXLENGTH,
  TOKEN_LARGELCPVALUES,
  TOKEN_MAXBRANCHDEPTH,
  TOKEN_INTEGERSIZE,
  TOKEN_LITTLEENDIAN,
  TOKEN_SPECIALCHARS,
  TOKEN_SPECIALRANGES,
  TOKEN_LONGEST,
  TOKEN_LENOFSPECIALPFX,
  TOKEN_LENOFSPECIALSUF,
  TOKEN_NUMOFTOKENS,
  TOKEN_UNKNOWN=-1,
  TOKEN_ALREADYSET=-2
} TokenID;

/*!
 * \brief Structure for holding information about project file tokens.
 *
 * \internal
 */
typedef struct
{
  const char *name;
  size_t length;
  unsigned int set:1;
  unsigned int allow_duplicate:1;
} Tokentableentry;

#endif /* !PROJECTFILEPARSER_H */
