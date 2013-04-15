/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2006, 2007  Robert Homann
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

#ifndef SKIPTREE_H
#define SKIPTREE_H
/*!\addtogroup suffixarrays */
/*@{*/
/*!
 * \brief The inner skip chain loop.
 * 
 * If the last character read from the start suffix was a special character
 * and depth is 0, then use SKIPTREE_DEPTH0_LOOP() instead to prevent
 * reading invalid memory.
 */
#define fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,REL)\
  while((NEXT) < (MAX) && (LCP) REL (DEPTH))\
  {\
    NEXT=(SKPTAB)[NEXT]+1;\
    fid_LCP(LCP,ESA,NEXT);\
  }

/*!
 * \brief The inner skip chain loop for special character at depth 0.
 */
#define fid_SKIPTREE_DEPTH0_LOOP(ESA,SKPTAB,NEXT,LCP,MAX)\
  while((NEXT) < (MAX))\
  {\
    NEXT=(SKPTAB)[NEXT]+1;\
    if((NEXT) < (MAX))\
    {\
      fid_LCP(LCP,ESA,NEXT);\
    }\
  }

/*!
 * \brief Skip tree if the last character read from the start suffix was a common character.
 */
#define fid_SKIPTREE_COMMON(ESA,SKPTAB,NEXT,START,LCP,DEPTH,MAX)\
  if((START) < (MAX))\
  {\
    (NEXT)=(START)+1;\
    fid_LCP(LCP,ESA,NEXT);\
    fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,>);\
  }\
  else\
  {\
    NEXT=(MAX);\
    fid_LCP(LCP,ESA,NEXT);\
  }

/*!
 * \brief Skip tree if the last character read from the start suffix was a special character.
 */
#define fid_SKIPTREE_SPECIAL(ESA,SKPTAB,NEXT,START,LCP,DEPTH,MAX)\
  if((START) < (MAX))\
  {\
    (NEXT)=(START)+1;\
    fid_LCP(LCP,ESA,NEXT);\
    if((DEPTH) > 0)\
    {\
      fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,>=);\
    }\
    else\
    {\
      fid_SKIPTREE_DEPTH0_LOOP(ESA,SKPTAB,NEXT,LCP,MAX);\
    }\
  }\
  else\
  {\
    NEXT=(MAX);\
    fid_LCP(LCP,ESA,NEXT);\
  }

/*!
 * \brief Skip a subtree of a virtual suffix tree via skip table.
 * 
 * The macro will always update \p NEXT and \p LCP, the other arguments are
 * used read-only.
 * 
 * \param ESA Pointer to an enhanced suffix array.
 * \param SKPTAB Pointer to the skip table (part of \p ESA). This can be
 * given separately because you may want to have a pointer around that is
 * directly pointing to the skip table instead of referencing it through
 * the fid_Suffixarray structure each time.
 * \param NEXT An fid_Uint that stores the next index in the suffix table,
 * that is the first suffix that does not belong to the skipped subtree.
 * \param START The current index into the suffix table, that is the index
 * of the suffix whose subtree should be skipped.
 * \param LCP An fid_Uint that stores the current index in the LCP table.
 * \param DEPTH The current depth in the suffix tree.
 * \param MAX The length of the sequence underlaying the suffix array.
 * The length could also be read directly from a fid_Suffixarray structure,
 * but this may take too much time, so it should have been stored in a
 * separate fid_Uint by the caller.
 * \param COMMON Boolean indicating if the last character of the start
 * suffix is a special (0) or a common (not 0) one as determined by the
 * fid_REGULARSYMBOL() and fid_SPECIALSYMBOL() macros.
 * 
*/
#define fid_SKIPTREE(ESA,SKPTAB,NEXT,START,LCP,DEPTH,MAX,COMMON)\
  if((START) < (MAX))\
  {\
    (NEXT)=(START)+1;\
    fid_LCP(LCP,ESA,NEXT);\
    if(COMMON)\
    {\
      fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,>);\
    }\
    else if((DEPTH) > 0)\
    {\
      fid_SKIPTREE_LOOP(ESA,SKPTAB,NEXT,LCP,DEPTH,MAX,>=);\
    }\
    else\
    {\
      fid_SKIPTREE_DEPTH0_LOOP(ESA,SKPTAB,NEXT,LCP,MAX);\
    }\
  }\
  else\
  {\
    NEXT=(MAX);\
    fid_LCP(LCP,ESA,NEXT);\
  }
/*@}*/

#endif /* !SKIPTREE_H */
