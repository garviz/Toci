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

#include "arrays.h"

typedef struct
{
  UINT sufindex;
  UINT lcp;
} Largelcpentry;

typedef struct
{
  UINT intervals[fid_INTERVALBUFFERSIZE];
  fid_Symbol sym;
  UINT depth;
} Stackelem;

fid_DYNARRAY_DECLARE(Stack,Stackelem);

/*!\addtogroup wordsize */
/*@{*/

/*!
 * \brief Update data fields in fid_Suffixarray structure according to mapped
 * files.
 * 
 * This is the word size dependent implementation.
 *
 * \param esa The structure to be updated.
 * \param tables Update only values derivable from the tables specified in
 * this request. To update all values, pass #fid_TABLES_ALL, or
 * #fid_TABLES_OFFLINE to update only values derivable from the index tables.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int suffixarray_realize(fid_Suffixarray *esa, fid_Tablerequest tables,
                               fid_Error *error) /*{{{*/
{
  if((tables&fid_TABLE_SUF) != 0)
  {
    esa->suftab.VU=fid_CAST_POINTER(esa->suffile.content,const UINT);
  }
  
  if((tables&fid_TABLE_LCP) != 0)
  {
    if(esa->llvfile.content == NULL)
    {
      esa->num_of_large_lcps.VU=0;
    }
    else
    {
      TO_UINT(esa->num_of_large_lcps.VU,
              (size_t)(esa->llvfile.occupied/(2*sizeof(UINT))),error,
              fid_error_throw(error,
                              "Size of file \"%s\" too large to be "
                              "represented in 32 bits.\n",
                              esa->llvfile.filename);
              return -1;);
    }
  }
  
  if((tables&fid_TABLE_SKP) != 0)
  {
    esa->skiptab.VU=fid_CAST_POINTER(esa->skpfile.content,const UINT);
  }

  if((tables&fid_TABLE_STI) != 0)
  {
    esa->stitab.VU=fid_CAST_POINTER(esa->stifile.content,const UINT);
  }

  return 0;
}

/*}}}*/

/*!
 * \brief Initialize stack used by fid_suffixarray_traverse().
 *
 * The stack will be filled with one entry corresponding to all
 * suffix-intervals at depth 1, or remain empty (but still initialized and
 * allocated) if there are no such intervals.
 *
 * \param stack The stack to be initialized.
 * \param esa An enhanced suffix array to be traversed.
 * \param error Error messages go here.
 *
 * \returns 0 on success, -1 when running out of memory.
 */
static int init_stack(Stack *stack, const fid_Suffixarray *esa,
                      fid_Error *error)/*{{{*/
{
  fid_Suffixinterval si;

  fid_DYNARRAY_INIT(stack,Stackelem);
  fid_DYNARRAY_PUSH(stack,Stackelem,128,
                    fid_DYNARRAY_FREE(stack,Stackelem);
                    fid_OUTOFMEM(error);
                    return -1;);
  assert(stack->dyndata != NULL);
  fid_suffixinterval_init_root(&si,esa);
  if(fid_suffixarray_get_intervals(esa,&si,
                                   fid_DYNARRAY_TOPELEM(stack,
                                                        Stackelem).intervals) > 0)
  {
    fid_DYNARRAY_TOPELEM(stack,Stackelem).sym=0;
    fid_DYNARRAY_TOPELEM(stack,Stackelem).depth=0;
  }

  return 0;
}
/*}}}*/

/*!
 * \brief Generic depth-first traversal of an enhanced suffix array.
 *
 * This function calls a callback function for each lcp-interval in an
 * enhanced suffix array. The callback function is free to limit the maximum
 * depth or to stop the traversal by returning special return values. See
 * #fid_Esatraversecallback_32 and #fid_Esatraversecallback_64 for more
 * details.
 *
 * While it might not be the most efficient way to perform a depth-first by
 * means of a callback driven generic function, it is for sure a very
 * convenient one to try out some ideas. Once an algorithm works when using
 * this function (leading to a reference implementation), it can be rewritten
 * with the traversal inlined to speed it up.
 *
 * \param esa An enhanced suffix array to be traversed.
 * \param callback A callback function that processes lcp-intervals.
 * \param user_data Pointer to data used by the callback function.
 * \param error Error messages go here.
 *
 * \returns -1 on error, 0 on success, or the non-zero return value returned by
 * the callback function.
 */
int fid_suffixarray_traverse(const fid_Suffixarray *esa,
                             fid_Esatraversecallback callback,
                             void *user_data, fid_Error *error)/*{{{*/
{
  fid_Suffixinterval si;
  Stack stack;
  Stackelem *elem;
  fid_Symbol wcsym;
  int retval=0;

  assert(esa != NULL);
  assert(callback != NULL);

  /* initialize stack and put first element there */
  if(init_stack(&stack,esa,error) == -1)
  {
    return -1;
  }

  wcsym=esa->alpha.num_of_syms-1;

  while(stack.occupied > 0)
  {
    elem=&fid_DYNARRAY_TOPELEM(&stack,Stackelem);
    while(elem->sym < wcsym)
    {
      assert(fid_REGULARSYMBOL(elem->sym));
      fid_suffixinterval_init(&si,elem->depth+1,
                              elem->intervals[(size_t)elem->sym],
                              elem->intervals[elem->sym+1]-1);
      ++elem->sym;
      if(si.left+1 < elem->intervals[(size_t)elem->sym])
      {
        /* suffix interval; note: do not compare si.left and si.right in the
         * condition above since si.right might have suffered an underflow */
        fid_suffixinterval_to_lcpinterval(&si,esa);
        if((retval=callback(esa,&si,elem->depth,error,user_data)) > 0)
        {
          goto stop_traversal;
        }
        else if(retval == fid_TRAVERSE_SKIP)
        {
          continue;
        }

        fid_DYNARRAY_PUSH(&stack,Stackelem,128,
                          fid_DYNARRAY_FREE(&stack,Stackelem);
                          fid_OUTOFMEM(error);
                          return -1;);
        if(fid_suffixarray_get_intervals(esa,&si,
                                         fid_DYNARRAY_TOPELEM(&stack,
                                                              Stackelem).intervals) > 0)
        {
          /* keep on processing sub-intervals */
          fid_DYNARRAY_TOPELEM(&stack,Stackelem).sym=0;
          fid_DYNARRAY_TOPELEM(&stack,Stackelem).depth=si.depth;
          break;
        }
        else
        {
          /* no sub-intervals, done */
          fid_DYNARRAY_POP(&stack,Stackelem);
        }
      }
      else if(si.left == si.right)
      {
        /* singleton/leaf */
        si.depth=fid_SUFFIXINTERVAL_SINGLETON;
        if((retval=callback(esa,&si,elem->depth,error,user_data)) > 0)
        {
          goto stop_traversal;
        }
      }
    }

    if(fid_DYNARRAY_TOPELEM(&stack,Stackelem).sym >= wcsym)
    {
      /* checked all symbols at current depth, remove element from stack */
      fid_DYNARRAY_POP(&stack,Stackelem);
    }
  }

stop_traversal:
  fid_DYNARRAY_FREE(&stack,Stackelem);
  return (retval <= 0)?0:retval;
}
/*}}}*/

#ifdef DEBUG
/*!
 * \brief Check if the given suffix-interval is a valid suffix-interval.
 * 
 * This function is intended to be used for debugging. All suffixes in a
 * suffix-interval must have a length of at least depth+1 and share a
 * common prefix of length depth+1. Since we can assume to have a valid
 * enhanced suffix array, this function just checks checks the suffixes at
 * the boundaries for these properties. Additionally, it checks if the
 * suffix left from the left boundary and the suffix right from the right
 * boundary does \em not fulfill these properties.
 * 
 * If any necessary suffix-interval property is violated, the program will
 * be aborted and may dump core.
 * 
 * \param esa An enhanced suffix array.
 * \param si A suffix-interval to be checked.
 * \param notspecial Set unequal to 0 if the symbols in the suffixes at
 * \p si's depth are a special symbol such that special care must be taken
 * in the checks.
 */
static void check_interval(const fid_Suffixarray *esa, const fid_Suffixinterval *si,
                           int notspecial) /*{{{*/
{
  assert(si->left <= si->right);
  assert(si->left < esa->sequences.total_length.VU);
  assert(si->right < esa->sequences.total_length.VU);
  
  if(notspecial)
  {
    /* all suffixes must have a length of depth+1 */
    if(esa->suftab.VU[si->left]+si->depth >= esa->sequences.total_length.VU)
    {
      fprintf(stderr,"Suffix " UINTFMT " too short in " UINTFMT
              "-[" UINTFMT "," UINTFMT "].\n",
              si->left,si->depth,si->left,si->right);
      abort();
    }
    if(esa->suftab.VU[si->right]+si->depth >= esa->sequences.total_length.VU)
    {
      fprintf(stderr,"Suffix " UINTFMT " too short in " UINTFMT
              "-[" UINTFMT "," UINTFMT "].\n",
              si->right,si->depth,si->left,si->right);
      abort();
    }
    
    /* compare prefixes of boundary suffixes */
    if(memcmp(&esa->sequences.tisfile.content[esa->suftab.VU[si->left]],
              &esa->sequences.tisfile.content[esa->suftab.VU[si->right]],
              (size_t)(si->depth+1)) != 0)
    {
      fprintf(stderr,"Prefixes differ within " UINTFMT "-[" UINTFMT
              "," UINTFMT "].\n",si->depth,si->left,si->right);
      fid_sequences_dump_range(&esa->sequences.tisfile.content[esa->suftab.VU[si->left]],
                               si->depth+1,&esa->alpha,"String 1: ",0,stderr);
      fid_sequences_dump_range(&esa->sequences.tisfile.content[esa->suftab.VU[si->right]],
                               si->depth+1,&esa->alpha,"String 2: ",0,stderr);
      abort();
    }
  }
  else
  {
    /* at least first and last suffixes must have with a special symbol at
     * the interval's depth */
    if((esa->suftab.VU[si->left]+si->depth < esa->sequences.total_length.VU && 
        fid_REGULARSYMBOL(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[si->left]+si->depth))) ||
       (si->left < si->right-1 &&
        fid_REGULARSYMBOL(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[si->right-1]+si->depth))))
    {
      fprintf(stderr,"Special interval " UINTFMT "-[" UINTFMT
              "," UINTFMT "] contains non-special prefixes.\n",
              si->depth,si->left,si->right);
      fid_sequences_dump_range(&esa->sequences.tisfile.content[esa->suftab.VU[si->left]],
                               si->depth+1,&esa->alpha,"String 1: ",0,stderr);
      if(si->left < si->right-1)
      {
        fid_sequences_dump_range(&esa->sequences.tisfile.content[esa->suftab.VU[si->right-1]],
                                 si->depth+1,&esa->alpha,"String 2: ",0,stderr);
      }
      abort();
    }
  }
  
  /* check left from left boundary */
  if(si->left > 0)
  {
    if(esa->sequences.total_length.VU-esa->suftab.VU[si->left-1] > si->depth &&
       memcmp(&esa->sequences.tisfile.content[esa->suftab.VU[si->left-1]],
              &esa->sequences.tisfile.content[esa->suftab.VU[si->left]],
              (size_t)si->depth+1) == 0)
    {
      fprintf(stderr,"Suffix boundary " UINTFMT " is not left-maximal "
              "in " UINTFMT "-[" UINTFMT "," UINTFMT "].\n",
              si->left,si->depth,si->left,si->right);
      abort();
    }
  }
  
  /* check right from right boundary */
  if(si->right < esa->sequences.total_length.VU)
  {
    if(esa->sequences.total_length.VU-esa->suftab.VU[si->right+1] > si->depth &&
       memcmp(&esa->sequences.tisfile.content[esa->suftab.VU[si->right]],
              &esa->sequences.tisfile.content[esa->suftab.VU[si->right+1]],
              (size_t)si->depth+1) == 0)
    {
      fprintf(stderr,"Suffix boundary " UINTFMT " is not right-maximal "
              "in " UINTFMT "-[" UINTFMT "," UINTFMT "].\n",
              si->right,si->depth,si->left,si->right);
      abort();
    }
  }
}

/*}}}*/

/*!
 * \brief Check all suffix-intervals represented by an array of interval boundaries.
 * 
 * This function is intended to be helpful to debug function
 * fid_suffixarray_get_intervals() since it checks its return value.
 * 
 * \param esa An enhanced suffix array.
 * \param depth The depth of the suffix-intervals, i.e., their LCP minus 1.
 * \param intervals An array of left suffix-interval boundaries.
 * \param count The sought number of intervals represented by array
 * \p intervals.
 * \param dump_intervals If unequal to 0, then print the interval ranges
 * represented by \p intervals to \c stderr.
 *
 * \see check_interval() for more information.
 */
static void check_intervals(const fid_Suffixarray *esa, UINT depth,
                            UINT *intervals, UINT count,
                            int dump_intervals) /*{{{*/
{
  fid_Suffixinterval si;
  UINT counted;
  fid_Symbol sym;
  
  for(sym=0; sym <= esa->alpha.num_of_syms; ++sym)
  {
    if(dump_intervals && sym < esa->alpha.num_of_syms)
    {
      fprintf(stderr,"%c -> [" UINTFMT "," UINTFMT "] %c\n",
              fid_PRINT_SYMBOL(&esa->alpha,sym),
              intervals[(size_t)sym],intervals[sym+1]-1,
              intervals[(size_t)sym] < intervals[sym+1]?' ':'*');
    }
    assert(intervals[(size_t)sym] != ~(UINT)0);
  }
  for(/* nothing */; sym < fid_INTERVALBUFFERSIZE; ++sym)
  {
    assert(intervals[(size_t)sym] == ~(UINT)0);
  }
  
  counted=0;
  si.depth=depth;
  for(sym=0; sym < esa->alpha.num_of_syms; ++sym)
  {
    if(intervals[(size_t)sym] < intervals[sym+1])
    {
      si.left=intervals[(size_t)sym];
      si.right=intervals[sym+1]-1;
      check_interval(esa,&si,(sym+(UINT)1 < esa->alpha.num_of_syms));
      ++counted;
    }
  }
  assert(counted == count);
}

/*}}}*/
#endif /* DEBUG */

/*!
 * \brief Perform binary search for right boundary of an embedded suffix-interval.
 * 
 * It is an error to call this function for symbols that cannot be found,
 * i.e., the caller must make sure that an embedded suffix-interval exists
 * for the given symbol within suffix-interval \p si.
 * 
 * \param esa An enhanced suffix array.
 * \param sym The symbol the shall prolong the longest common prefix of
 * the given suffix-interval.
 * \param si Search within this suffix-interval for the child-intervals
 * for symbol \p sym.
 * 
 * \returns The right boundary of the embedded suffix-interval.
 */
static UINT binary_search(const fid_Suffixarray *esa, fid_Symbol sym,
                          const fid_Suffixinterval *si) /*{{{*/
{
  UINT left, right, pivot;
  
  assert(si->left < si->right);
  assert(si->left < esa->sequences.total_length.VU);
  assert(si->right < esa->sequences.total_length.VU);
  assert(fid_REGULARSYMBOL(sym));
  
  left=si->left;
  right=si->right;
  
  while(left+1 < right)
  {
    pivot=left+((right-left) >> 1);
    if(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[pivot]+si->depth) <= sym)
    {
      left=pivot;
    }
    else
    {
      right=pivot-1;
    }
  }
  
  /* we assume that the symbol must have been found since otherwise a call
   * of this function would be a program error */
  if(esa->suftab.VU[right]+si->depth < esa->sequences.total_length.VU &&
     fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[right]+si->depth) == sym)
  {
    return right;
  }
  else
  {
    assert(left+1 == right);
    assert(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[left]+si->depth) == sym);
    return left;
  }
}

/*}}}*/

/*!
 * \brief Return all embedded suffix-intervals embedded in a suffix-interval.
 * 
 * Given a suffix-interval, determine the suffix indices of all embedded
 * suffix-intervals. The result is written to \p intervals. Let \c s be a
 * symbol, then the embedded suffix-interval for symbol \c s starts at
 * suffix \c intervals[s], and its length is \c intervals[s+1]-intervals[s].
 * If the length is 0, then the interval does not exist.
 * 
 * \param esa The enhanced suffix array the interval are to be searched in.
 * \param si The suffix-interval whose child-intervals are to be determined.
 * \param intervals Array of suffix indices. This array must have
 * #fid_INTERVALBUFFERSIZE entries, all elements will be set.
 * 
 * \returns The number of suffix-intervals embedded in \p si.
 */
UINT fid_suffixarray_get_intervals(const fid_Suffixarray *esa,
                                   const fid_Suffixinterval *si,
                                   UINT *intervals) /*{{{*/
{
  fid_Symbol sym, nextsym, wcsym;
  fid_Suffixinterval current_interval;
  UINT count=0;
#ifdef DEBUG
  UINT debugindex;
#endif /* DEBUG */
  
  assert(esa != NULL);
  assert(esa->sequences.tisfile.content != NULL && esa->suffile.content != NULL && esa->suftab.VU != NULL);
  assert(esa->alpha.num_of_syms > 0);
  assert(si != NULL);
  assert(si->left <= si->right);
  assert(si->left < esa->sequences.total_length.VU);
  assert(si->right < esa->sequences.total_length.VU);
  assert(intervals != NULL);
  
#ifdef DEBUG
  memset(intervals,0xff,fid_INTERVALBUFFERSIZE*sizeof(UINT));
  for(debugindex=si->left; debugindex < si->right; ++debugindex)
  {
    if(esa->suftab.VU[debugindex]+si->depth >= esa->sequences.total_length.VU)
    {
      abort();
    }
  }
#endif /* DEBUG */
  
  wcsym=(fid_Symbol)(esa->alpha.num_of_syms-1);
  current_interval=*si;
  intervals[0]=current_interval.left;
  nextsym=fid_READ_SYMBOL(&esa->sequences,
                          esa->suftab.VU[current_interval.left]+si->depth);
  
  for(sym=0; current_interval.left <= current_interval.right; ++sym)
  {
    assert(esa->suftab.VU[current_interval.left]+si->depth <= esa->sequences.total_length.VU);
    if(esa->suftab.VU[current_interval.left]+si->depth == esa->sequences.total_length.VU)
    {
      /* suffix too short for comparisons, treat as special symbol */
      nextsym=(fid_Symbol)(wcsym+1);
      break;
    }
    
    nextsym=fid_READ_SYMBOL(&esa->sequences,
                            esa->suftab.VU[current_interval.left]+si->depth);
    
    if(fid_SPECIALSYMBOL(nextsym))
    {
      /* special symbol encountered, put them all into their own interval */
      nextsym=wcsym;
      break;
    }
    
    for(/* nothing */; sym < nextsym; ++sym)
    {
      /* symbols in this range don't occur at this depth in the given interval */
      intervals[sym+1]=current_interval.left;
    }
    
    assert(fid_REGULARSYMBOL(sym));
    
    if(current_interval.left < current_interval.right)
    {
      intervals[sym+1]=binary_search(esa,sym,&current_interval)+1;
    }
    else
    {
      intervals[sym+1]=current_interval.left+1;
    }
    assert(intervals[sym+1] <= esa->sequences.total_length.VU);
    assert(intervals[sym+1] >= current_interval.left);
    current_interval.left=intervals[sym+1];
    ++count;
  }
  
  for(/* nothing */; sym < wcsym; ++sym)
  {
    /* symbols in this range don't occur at this depth in the given interval */
    intervals[sym+1]=intervals[(size_t)sym];
  }
  if(nextsym <= wcsym)
  {
    intervals[wcsym+1]=si->right+1;
    if(nextsym == wcsym)
    {
      ++count;
    }
  }
  else
  {
    intervals[wcsym+1]=si->right;
  }
  
  assert(count > 0);
  
#ifdef DEBUG
  check_intervals(esa,si->depth,intervals,count,0);
#endif /* DEBUG */
  
  return count;
}

/*}}}*/

/*!
 * \brief Determine the embedded suffix-interval for a given symbol.
 * 
 * The child-interval is returned in argument \p si, i.e., \p si will be
 * modified if the requested child-interval exists. If no such interval
 * exists, then \p si will remain unchanged.
 * 
 * \param esa The enhanced suffix array the interval is to be searched in.
 * \param si The suffix-interval whose child-interval is to be determined.
 * \param symbol The embedded child-interval shall be the suffix-interval
 * for the longest common prefix of \p si concatenated with this symbol.
 * 
 * \returns 0 if found, -1 if no such interval exists.
 */
int fid_suffixarray_find_embedded_interval(const fid_Suffixarray *esa,
                                           fid_Suffixinterval *si,
                                           fid_Symbol symbol) /*{{{*/
{
  UINT left, right, pivot;
#ifdef DEBUG
  UINT i;
#endif /* DEBUG */
  
  assert(esa != NULL);
  assert(esa->sequences.tisfile.content != NULL && esa->suftab.VU != NULL);
  assert(si != NULL);
  assert(si->left <= si->right);
  assert(si->left < esa->sequences.total_length.VU);
  assert(si->right < esa->sequences.total_length.VU);
  assert(symbol < esa->alpha.num_of_syms-1 || fid_SPECIALSYMBOL(symbol));
  
#ifdef DEBUG
  for(i=si->left; i < si->right; ++i)
  {
    if(esa->suftab.VU[i]+si->depth >= esa->sequences.total_length.VU)
    {
      abort();
    }
  }
#endif /* DEBUG */
  
  if(fid_REGULARSYMBOL(symbol))
  {
    /* find left boundary */
    left=si->left;
    right=si->right;
    while(left+1 < right)
    {
      pivot=left+((right-left) >> 1);
      if(esa->suftab.VU[pivot]+si->depth < esa->sequences.total_length.VU &&
         fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[pivot]+si->depth) < symbol)
      {
        left=pivot+1;
      }
      else
      {
        right=pivot;
      }
    }
    
    if(esa->suftab.VU[left]+si->depth < esa->sequences.total_length.VU &&
       fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[left]+si->depth) == symbol)
    {
      assert(left < esa->sequences.total_length.VU);
      si->left=left;
    }
    else
    {
      if(left == right ||
         esa->suftab.VU[right]+si->depth == esa->sequences.total_length.VU ||
         fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[right]+si->depth) != symbol)
      {
        return -1;
      }
      assert(right < esa->sequences.total_length.VU);
      si->left=right;
    }
    
    /* find right boundary */
    left=si->left;
    right=si->right;
    while(left+1 < right)
    {
      pivot=left+((right-left) >> 1);
      if(esa->suftab.VU[pivot]+si->depth < esa->sequences.total_length.VU &&
         fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[pivot]+si->depth) <= symbol)
      {
        left=pivot;
      }
      else
      {
        right=pivot-1;
      }
    }
    
    if(right < esa->sequences.total_length.VU &&
       esa->suftab.VU[right]+si->depth < esa->sequences.total_length.VU &&
       fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[right]+si->depth) == symbol)
    {
      si->right=right;
    }
    else
    {
      assert(left+1 == right);
      assert(left < esa->sequences.total_length.VU);
      assert(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[left]+si->depth) == symbol);
      si->right=left;
    }
  }
  else
  {
    /* find left boundary of interval of special symbols, right boundary
     * remains unchanged */
    left=si->left;
    right=si->right;
    while(left+1 < right)
    {
      pivot=left+((right-left) >> 1);
      if(fid_REGULARSYMBOL(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[pivot]+si->depth)))
      {
        left=pivot+1;
      }
      else
      {
        right=pivot;
      }
    }
    
    if(esa->suftab.VU[left]+si->depth < esa->sequences.total_length.VU &&
       fid_SPECIALSYMBOL(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[left]+si->depth)))
    {
      assert(esa->suftab.VU[left-1]+si->depth == esa->sequences.total_length.VU || left == si->left || fid_REGULARSYMBOL(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[left-1]+si->depth)));
      si->left=left;
    }
    else
    {
      if(left == right ||
         esa->suftab.VU[right]+si->depth == esa->sequences.total_length.VU ||
         fid_REGULARSYMBOL(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[right]+si->depth)))
      {
        return -1;
      }
      assert(fid_REGULARSYMBOL(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[right-1]+si->depth)));
      assert(esa->suftab.VU[right]+si->depth == esa->sequences.total_length.VU || fid_SPECIALSYMBOL(fid_READ_SYMBOL(&esa->sequences,esa->suftab.VU[right]+si->depth)));
      si->left=right;
    }
  }

#ifdef DEBUG
  check_interval(esa,si,fid_REGULARSYMBOL(symbol));
#endif /* DEBUG */
  
  ++si->depth;
  
  return 0;
}

/*}}}*/

/*!
 * \brief Extend the common prefix of a suffix-interval by some pattern.
 *
 * This function can be used to continue searching inside a suffix-interval by
 * prolonging the common prefix of length fid_Suffixinterval::depth by a some
 * string, possibly further narrowing down the suffix-interval.
 *
 * \param esa An enhanced suffix array to search in.
 * \param pattern Extension of the common prefix of the given suffix-interval,
 * alphabet encoded.
 * \param plen Length of \p pattern.
 * \param si A valid suffix-interval to start with, will be modified due to the
 * extension with \p pattern. If the common prefix cannot be extended by
 * \p pattern, then the content of this structure will be undefined.
 *
 * \returns 0 on success with \p si set to the requested suffix-interval, or
 * -1 if the suffix-interval could not be extended by \p pattern.
 */
int fid_suffixarray_extend_interval(const fid_Suffixarray *esa,
                                    const fid_Symbol *pattern, UINT plen,
                                    fid_Suffixinterval *si)/*{{{*/
{
  assert(esa != NULL);
  assert(si != NULL);
  assert(si->left <= si->right);
  assert(si->left < esa->sequences.total_length.VU);
  assert(si->right < esa->sequences.total_length.VU);
  assert(pattern != NULL);
  assert(plen > 0);

  while(si->depth < plen)
  {
    if(fid_suffixarray_find_embedded_interval(esa,si,pattern[si->depth]) != 0)
    {
      return -1;
    }
  }

  return 0;
}
/*}}}*/

/*!
 * \brief Find suffix-interval with suffixes sharing a common given prefix.
 *
 * Simply put, this function returns all occurrences of \p pattern in the
 * suffix array as suffix-interval (which is not necessarily an lcp-interval).
 *
 * \param esa An enhanced suffix array to search in.
 * \param pattern Common prefix of searched suffix-interval, alphabet encoded.
 * \param plen Length of \p pattern.
 * \param si The suffix-interval corresponding to \p pattern if it exists.
 *
 * \returns 0 on success with \p si set to the requested suffix-interval, or
 * -1 if \p pattern does not occur in the text.
 */
int fid_suffixarray_find_interval(const fid_Suffixarray *esa,
                                  const fid_Symbol *pattern, UINT plen,
                                  fid_Suffixinterval *si)/*{{{*/
{
  assert(esa != NULL);
  assert(esa->sequences.total_length.VU > 0);
  assert(si != NULL);

  fid_suffixinterval_init_root(si,esa);
  return fid_suffixarray_extend_interval(esa,pattern,plen,si);
}
/*}}}*/

/*!
 * \brief Find large LCP value in LCP exception table.
 * 
 * \param esa An enhanced suffix array.
 * \param suffix The suffix index whose large LCP value should be retrieved.
 * 
 * \returns The LCP value for the given suffix.
 */
UINT fid_suffixarray_find_large_lcp(const fid_Suffixarray *esa,
                                    UINT suffix) /*{{{*/
{
  UINT left, right, pivot;
  
  assert(esa != NULL);
  assert(esa->llvfile.content != NULL);
  assert(esa->num_of_large_lcps.VU > 0);
  
  left=0;
  right=esa->num_of_large_lcps.VU-1;
  while(left+1 < right)
  {
    pivot=left+((right-left) >> 1);
    if(fid_CAST_POINTER(esa->llvfile.content,Largelcpentry)[pivot].sufindex < suffix)
    {
      left=pivot;
    }
    else
    {
      right=pivot;
    }
  }
  
  if(left == right ||
     fid_CAST_POINTER(esa->llvfile.content,Largelcpentry)[left].sufindex == suffix)
  {
    assert(fid_CAST_POINTER(esa->llvfile.content,Largelcpentry)[left].lcp >= (UINT)UCHAR_MAX);
    return fid_CAST_POINTER(esa->llvfile.content,Largelcpentry)[left].lcp;
  }
  else
  {
    assert(fid_CAST_POINTER(esa->llvfile.content,Largelcpentry)[right].lcp >= (UINT)UCHAR_MAX);
    return fid_CAST_POINTER(esa->llvfile.content,Largelcpentry)[right].lcp;
  }
}

/*}}}*/

/*!
 * \brief Determine length of suffix up to next sequence separator.
 *
 * It is easy to determine the length of a suffix in an enhanced suffix array
 * when there is only one sequence inside. This function also takes sequence
 * separators into account and returns the length of the sequence in the given
 * suffix up to the next sequence separator, or to the end of the input.
 *
 * \param esa An enhanced suffix array.
 * \param suffix The suffix whose lengths is to be found.
 *
 * \returns The length of the sequence in the given suffix.
 */
UINT fid_suffixarray_suffix_length(const fid_Suffixarray *esa,
                                   UINT suffix)/*{{{*/
{
  UINT offset, left, right;

  assert(esa != NULL);
  assert(esa->sequences.tisfile.content != NULL);
  assert(esa->suftab.VU != NULL);
  assert(suffix <= esa->sequences.total_length.VU);

  offset=esa->suftab.VU[suffix];
  fid_sequences_offset_to_boundaries(&esa->sequences,offset,&left,&right);

  return right-offset;
}
/*}}}*/

/*!
 * \brief Compute character distribution of given enhanced suffix array.
 *
 * The fid_Sequences::distribution of fid_Suffixarray::sequences will be
 * filled by this function, such that each entry \em s contains to the
 * relative frequency of symbol \em s.
 *
 * The length of the sequence is corrected by the number of sequence
 * separators present in the input sequence, so these will not be taken
 * into account. Note that the entry for wildcards will be filled twice,
 * once at index #fid_WILDCARD, and once at the first index after the
 * last normal symbol. The entries for separators and undefined characters
 * will both be set to 0.0.
 *
 * Note that this function is faster than fid_sequences_compute_distribution()
 * for it makes use of the suffix array.
 *
 * \param esa The enhanced suffix array whose character distribution should be
 * determined.
 */
void fid_suffixarray_compute_distribution(fid_Suffixarray *esa)/*{{{*/
{
  fid_Suffixinterval root;
  UINT intervals[fid_INTERVALBUFFERSIZE];
  UINT sym, wcsym;
  double dbsize;

  assert(esa != NULL);
  assert(esa->suftab.VU != NULL);
  assert(esa->sequences.alpha != NULL);
  assert(esa->sequences.tisfile.content != NULL);
  assert(esa->sequences.tisfile.occupied > 0);

  fid_suffixinterval_init_root(&root,esa);
  (void)fid_suffixarray_get_intervals(esa,&root,intervals);

  memset(esa->sequences.distribution,0,sizeof(esa->sequences.distribution));
  dbsize=(double)(esa->sequences.tisfile.occupied+1-
                  esa->sequences.num_of_sequences.VU);
  wcsym=esa->alpha.num_of_syms-1;
  for(sym=0; sym < wcsym; ++sym)
  {
    esa->sequences.distribution[sym]=
      (double)(intervals[sym+1]-intervals[sym])/dbsize;
  }
  esa->sequences.distribution[wcsym]=
    (double)(intervals[wcsym+1]-intervals[wcsym]+1-
             esa->sequences.num_of_sequences.VU)/dbsize;
  esa->sequences.distribution[(size_t)fid_WILDCARD]=
    esa->sequences.distribution[wcsym];
}
/*}}}*/

/*!
 * \brief Show information about an enhanced suffix array.
 * 
 * \param esa The enhanced suffix array structure to be printed out.
 * \param stream An output stream to which the structure is printed. If
 * \c NULL, nothing will be printed.
 */
static void suffixarray_dump(const fid_Suffixarray *esa, FILE *stream) /*{{{*/
{
  UINT suffixes;
  int show_suffixes=1;

  if(esa->sequences.tisfile.content != NULL)
  {
    suffixes=(UINT)esa->sequences.tisfile.occupied;
  }
  else if(esa->suffile.content != NULL)
  {
    suffixes=(UINT)(esa->suffile.occupied/sizeof(UINT));
  }
  else if(esa->suffile.content != NULL)
  {
    suffixes=(UINT)(esa->suffile.occupied/sizeof(UINT));
  }
  else
  {
    suffixes=0;
    show_suffixes=0;
  } 

  if(show_suffixes)
  {
    fprintf(stream,"Suffixes: " UINTFMT "\n",suffixes);
  }

  if(esa->lcpfile.content != NULL)
  {
    fprintf(stream,"Large LCPs: " UINTFMT "\n",esa->num_of_large_lcps.VU);
  }
}

/*}}}*/

/*!
 * \brief Dump array of suffix-intervals as returned by
 * fid_suffixarray_get_intervals().
 *
 * Each printed suffix-interval is preceded by its distinguishing character.
 * Empty intervals are left out and appear in a summary below the list of
 * dumped intervals.
 *
 * \param esa The enhanced suffix array the intervals belong to.
 * \param intervals An array of suffix-intervals encoded by integers.
 * \param stream An output stream to which the suffix-intervals are printed.
 * If NULL, nothing will be printed.
 */
void fid_suffixarray_dump_intervals(const fid_Suffixarray *esa,
                                    const UINT intervals[fid_INTERVALBUFFERSIZE],
                                    FILE *stream)/*{{{*/
{
  fid_Suffixinterval si;
  fid_Symbol sym;
  int have_empty_ivs=0;

  assert(intervals != NULL);

  if(stream == NULL)
  {
    return;
  }
 
  for(sym=0; sym < esa->alpha.num_of_syms; ++sym)
  {
    fid_suffixinterval_init(&si,1,intervals[(size_t)sym],intervals[sym+1]-1);
    if(si.left < si.right)
    {
      fid_suffixinterval_to_lcpinterval(&si,esa);
      fprintf(stream,"%c: " UINTFMT "-[" UINTFMT ","
              UINTFMT "]\n",fid_PRINT_SYMBOL(&esa->alpha,sym),
              si.depth,si.left,si.right);
    }
    else if(si.left == si.right)
    {
      /* singleton */
      fprintf(stream,"%c: --[" UINTFMT "," UINTFMT "]\n",
              fid_PRINT_SYMBOL(&esa->alpha,sym),si.left,si.right);
    }
    else
    {
      have_empty_ivs=1;
    }
  }

  if(have_empty_ivs)
  {
    fprintf(stream,"No suffix-intervals for:");
    for(sym=0; sym < esa->alpha.num_of_syms; ++sym)
    {
      if(intervals[(size_t)sym] == intervals[sym+1])
      {
        fprintf(stream," %c",fid_PRINT_SYMBOL(&esa->alpha,sym));
      }
    }
    (void)fputc('\n',stream);
  }
}
/*}}}*/

/*!
 * \brief Dump prefix of a suffix to given stream.
 *
 * This function will not print more than \p length characters, and will stop
 * at sequence separators.
 *
 * \param esa The enhanced suffix array containing the suffix to be dumped.
 * \param suffix The index of the suffix to be dumped.
 * \param length Maximum number of characters to be printed. Dump stops at end
 * of the sequence.
 * \param stream An output stream to which the suffix is printed. If NULL,
 * nothing will be printed.
 */
void fid_suffixarray_dump_suffix(const fid_Suffixarray *esa, UINT suffix,
                                 UINT length, FILE *stream)/*{{{*/
{
  assert(esa != NULL);
  assert(esa->sequences.tisfile.content != NULL);
  assert(esa->suftab.VU != NULL);

  if(stream == NULL)
  {
    return;
  }

  if(esa->suftab.VU[suffix]+length > esa->sequences.total_length.VU)
  {
    length=esa->sequences.total_length.VU-esa->suftab.VU[suffix];
  }

  fid_sequences_dump_range(&esa->sequences.tisfile.content[esa->suftab.VU[suffix]],
                           length,&esa->alpha,NULL,1,stream);
}
/*}}}*/

/*!
 * \brief Print suffix array tables to given stream.
 *
 * Suffixes are not printed to full length since this would not be practical
 * even for tiny suffix arrays. Instead, only the first few characters
 * corresponding to a suffix' LCP-value are printed. Thus, when printing the
 * suffixes, the LCP-table is usually also of interest.
 *
 * \param esa The enhanced suffix array to be printed.
 * \param request Which tables to print. Note that suffixes themselves are
 * printed by adding #fid_TABLE_TIS to the request, in which case the LCP-table
 * must be mapped, too (even if it is not to be printed).
 * \param stream An output stream to which the suffix is printed. If NULL,
 * nothing will be printed.
 */
void fid_suffixarray_print(const fid_Suffixarray *esa,
                           fid_Tablerequest request, FILE *stream)/*{{{*/
{
  UINT lcp=0, i;

  if(stream == NULL || (request&fid_TABLES_ALL) == 0)
  {
    return;
  }

  fprintf(stream,"     i ");
  if((request&fid_TABLE_SUF) != 0) fprintf(stream," SUF[i]");
  if((request&fid_TABLE_LCP) != 0) fprintf(stream," LCP[i]");
  if((request&fid_TABLE_STI) != 0) fprintf(stream," STI[i]");
  if((request&fid_TABLE_SKP) != 0) fprintf(stream," SKP[i]");
  if((request&fid_TABLE_TIS) != 0) fprintf(stream,"  Suffixes");
  (void)fputc('\n',stream);
  for(i=0; i < esa->sequences.total_length.VU; ++i)
  {
    fprintf(stream,TABVALFMT ":",(fid_Uint64)i);
    if((request&fid_TABLE_SUF) != 0)
      fprintf(stream," " TABVALFMT,(fid_Uint64)esa->suftab.VU[i]);
    if((request&fid_TABLE_LCP) != 0)
      fprintf(stream," " TABVALFMT,(fid_Uint64)lcp);
    if((request&fid_TABLE_STI) != 0)
      fprintf(stream," " TABVALFMT,(fid_Uint64)esa->stitab.VU[i]);
    if((request&fid_TABLE_SKP) != 0)
      fprintf(stream," " TABVALFMT,(fid_Uint64)esa->skiptab.VU[i]);
    if((request&fid_TABLE_TIS) != 0)
    {
      fprintf(stream,"  ");
      fid_suffixarray_dump_suffix(esa,i,lcp+1,stream);
    }
    else
    {
      (void)fputc('\n',stream);
    }
    if((request&(fid_TABLE_LCP|fid_TABLE_TIS)) != 0)
    {
      fid_LCP(lcp,esa,i+1);
    }
  }
}
/*}}}*/

/*!
 * \brief Determine LCP value of suffix-interval.
 * 
 * This function computes the length of the longest common prefix of the
 * suffixes represented by a given suffix-interval by sequence comparisons.
 * Setting field fid_Suffixinterval::depth of \p si to the return value of this
 * functions yields an lcp-interval. Note that longest common prefixes never
 * include special symbols like wildcards or sequence separators since
 * these symbols are not equal to others, not even to themselves.
 * 
 * \param si A suffix-interval.
 * \param esa The enhanced suffix array the suffix-interval refers to.
 * 
 * \returns The LCP value of \p si, or fid_SUFFIXINTERVAL_SINGLETON if
 * \p si is a singleton.
 */
UINT fid_suffixinterval_lcpvalue(const fid_Suffixinterval *si,
                                 const fid_Suffixarray *esa) /*{{{*/
{
  UINT lcpvalue, maxlcp;
  UINT left_suffix, right_suffix;
  fid_Symbol symbol;
  
  assert(si != NULL);
  assert(si->depth > 0);
  assert(si->left < esa->sequences.total_length.VU);
  assert(si->right < esa->sequences.total_length.VU);
  assert(esa != NULL);
  
  if(si->left == si->right)
  {
    return fid_SUFFIXINTERVAL_SINGLETON;
  }
  
  left_suffix=esa->suftab.VU[si->left];
  if(fid_SPECIALSYMBOL(fid_READ_SYMBOL(&esa->sequences,left_suffix+si->depth-1)))
  {
    return si->depth;
  }
  right_suffix=esa->suftab.VU[si->right];
  maxlcp=esa->sequences.total_length.VU-
    ((left_suffix < right_suffix)?right_suffix:left_suffix);
  
  assert(maxlcp >= si->depth);
  assert(fid_READ_SYMBOL(&esa->sequences,left_suffix+si->depth-1) == fid_READ_SYMBOL(&esa->sequences,right_suffix+si->depth-1));
  
  for(lcpvalue=si->depth;
      lcpvalue < maxlcp &&
      (symbol=fid_READ_SYMBOL(&esa->sequences,left_suffix+lcpvalue)) ==
      fid_READ_SYMBOL(&esa->sequences,right_suffix+lcpvalue) &&
      fid_REGULARSYMBOL(symbol);
      ++lcpvalue)
  {
    /* nothing */
  }
  
  return lcpvalue;
}

/*}}}*/

/*!
 * \brief Turn suffix-interval into lcp-interval.
 * 
 * This function determines the LCP value of the given suffix-interval and
 * assigns that value to its fid_Suffixinterval::depth field.
 * 
 * \param si A suffix-interval.
 * \param esa The enhanced suffix array the suffix-interval refers to.
 */
void fid_suffixinterval_to_lcpinterval(fid_Suffixinterval *si,
                                       const fid_Suffixarray *esa) /*{{{*/
{
  si->depth=fid_suffixinterval_lcpvalue(si,esa);
}

/*}}}*/

/*!
 * \brief Find right boundary of suffix-interval from left boundary and depth.
 *
 * This function assumes that the left boundary and the depth of the passed
 * suffix-interval are correctly set up. From these two values the right
 * boundary is determined and assigned to fid_Suffixinterval::right of \p si.
 *
 * \param si An incomplete suffix-interval.
 * \param esa The enhanced suffix array the suffix-interval refers to.
 *
 * \bug This is a naive implementation and should be improved.
 */
void fid_suffixinterval_find_right(fid_Suffixinterval *si,
                                   const fid_Suffixarray *esa)/*{{{*/
{
  UINT suffix, lcp;

  assert(si != NULL);
  assert(si->left < esa->sequences.total_length.VU);
  assert(esa != NULL);

  suffix=si->left; 

  do
  {
    ++suffix;
    fid_LCP(lcp,esa,suffix);
  } while(lcp >= si->depth);

  si->right=suffix-1;
}
/*}}}*/

/*!
 * \brief Determine home-position of given lcp-interval.
 *
 * \param si A suffix-interval; stricty spoken, home-positions are defined for
 * lcp-intervals only, but since home-positions are derived just from interval
 * boundaries, you may safely pass in suffix-intervals.
 * \param esa An enhanced suffix array.
 *
 * \returns The home-position of lcp-interval \p si, i.e., either its left or
 * right boundary.
 */
UINT fid_suffixinterval_homepos(const fid_Suffixinterval *si,
                                const fid_Suffixarray *esa) /*{{{*/
{
  UINT leftlcp, rightlcp;
  
  assert(si != NULL);
  assert(si->left < esa->sequences.total_length.VU);
  assert(si->right < esa->sequences.total_length.VU);
  assert(si->left < si->right);
  assert(esa != NULL);
  assert(esa->lcpfile.content != NULL);
  
  fid_LCP(leftlcp,esa,si->left);
  fid_LCP(rightlcp,esa,si->right+1);
  if(leftlcp >= rightlcp)
  {
    return si->left;
  }
  else
  {
    return si->right;
  }
}

/*}}}*/

/*!
 * \brief Dump suffix-interval to given stream.
 *
 * \param si The suffix-interval to be dumped.
 * \param esa The enhanced suffix array containing the suffix-interval.
 * \param stream An output stream to which the suffix-interval is printed.
 * If NULL, nothing will be printed.
 */
void fid_suffixinterval_dump(const fid_Suffixinterval *si,
                             const fid_Suffixarray *esa, FILE *stream)/*{{{*/
{
  const fid_Symbol *seq;
#ifdef DEBUG
  const fid_Symbol *seqright;
#endif /* DEBUG */
  UINT i;

  assert(esa != NULL);
  assert(esa->sequences.tisfile.content != NULL);
  assert(esa->suftab.VU != NULL);

  if(stream == NULL)
  {
    return;
  }

  seq=&esa->sequences.tisfile.content[esa->suftab.VU[si->left]];
#ifdef DEBUG
  seqright=&esa->sequences.tisfile.content[esa->suftab.VU[si->right]];
#endif /* DEBUG */

  for(i=0; i < si->depth; ++i)
  {
#ifdef DEBUG
    assert(seq[i] == seqright[i]);
#endif /* DEBUG */
    (void)fputc(fid_PRINT_SYMBOL(&esa->alpha,seq[i]),stream);
  }

  fprintf(stream,"-[" UINTFMT "," UINTFMT "]\n",si->left,si->right);
}
/*}}}*/
/*@}*/
