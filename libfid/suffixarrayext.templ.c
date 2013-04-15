/*
 * libfid - Full-text Index Data structure library
 * Copyright (C) 2006, 2007, 2009  Robert Homann
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

/*!
 * \brief Check if variables shared by \p esa and \p project conincide.
 * 
 * This test should never fail for correct combinations of project files and
 * enhanced suffix array tables. On error, a meaningful error message is
 * generated.
 * 
 * \param esa, project The structures to be checked.
 * \param tables Test only values specified by this request. Pass
 * fid_TABLES_ALL for a full check.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int sanity_checks(const fid_Suffixarray *esa,
                         const fid_Projectfile *project,
                         fid_Tablerequest tables, fid_Error *error) /*{{{*/
{
  int retcode=0;
  
  if((tables&fid_TABLE_TIS) != 0)
  {
    if(esa->sequences.tisfile.content != NULL &&
       esa->sequences.tisfile.occupied != esa->sequences.total_length.VU)
    {
      fid_error_throw(error,"Size of \"%s.tis\" (%lu) does not "
                      "match size stored in project file (" UINTFMT ").",
                      project->prjbasename,
                      (unsigned long)esa->sequences.tisfile.occupied,
                      esa->sequences.total_length.VU);
      retcode=-1;
    }
  }
  
  if((tables&fid_TABLE_OIS) != 0)
  {
    if(esa->sequences.oisfile.content != NULL &&
       esa->sequences.oisfile.occupied != esa->sequences.total_length.VU)
    {
      fid_error_throw(error,"Size of \"%s.ois\" (%lu) does not "
                      "match size stored in project file (" UINTFMT ").",
                      project->prjbasename,
                      (unsigned long)esa->sequences.oisfile.occupied,
                      esa->sequences.total_length.VU);
      retcode=-1;
    }
  }
  
  if((tables&(fid_TABLE_TIS|fid_TABLE_OIS)) != 0)
  {
    if(esa->sequences.num_of_sequences.VU != project->num_of_sequences)
    {
      fid_error_throw(error,"Number of sequences stored in project \"%s\" "
                      "(" fid_U64FMT ") does not match real number of "
                      "sequences (" UINTFMT ").",
                      project->prjbasename,project->num_of_sequences,
                      esa->sequences.num_of_sequences.VU);
      retcode=-1;
    }
    
    if(esa->sequences.total_length.VU != project->totallength)
    {
      fid_error_throw(error,"Size of sequences stored in project \"%s\" "
                      "(" fid_U64FMT ") does not match real sequence size "
                      "(" UINTFMT ").",
                      project->prjbasename,project->totallength,
                      esa->sequences.total_length.VU);
      retcode=-1;
    }
  }
  
  if((tables&fid_TABLE_LCP) != 0)
  {
    if(esa->llvfile.content == NULL)
    {
      if(esa->num_of_large_lcps.VU > 0)
      {
        fid_error_throw(error,"Number of large LCP values stored in "
                        "project file is " UINTFMT ", but file "
                        "\"%s.llv\" not found.",
                        esa->num_of_large_lcps.VU,project->prjbasename);
        retcode=-1;
      }
    }
    else if(esa->llvfile.occupied != esa->num_of_large_lcps.VU*2*project->integer_size_in_bytes ||
            esa->num_of_large_lcps.VU != project->large_lcp_values)
    {
      fid_error_throw(error,"Size of \"%s.llv\", %lu, does "
                      "not match the number of large LCP values, "
                      UINTFMT ", stored in project file.",
                      project->prjbasename,
                      (unsigned long)esa->llvfile.occupied,
                      esa->num_of_large_lcps.VU);
      retcode=-1;
    }
  }
  
  return retcode;
}

/*}}}*/

static void init_from_esa(fid_Projectfile *project, const fid_Suffixarray *esa)
{
  const fid_Symbol *seq, *ptr, *max;
  fid_Suffixinterval si;

  project->totallength=esa->sequences.total_length.VU;
  project->num_of_sequences=esa->sequences.num_of_sequences.VU;
  project->large_lcp_values=esa->num_of_large_lcps.VU;

  /* we need to access the sequence now; just return if not available */
  if((seq=esa->sequences.tisfile.content) == NULL) return;

  for(ptr=seq, max=ptr+esa->sequences.total_length.VU;
      ptr < max && fid_SPECIALSYMBOL(*ptr);
      ++ptr)
  {
    ++project->len_of_special_prefix;
  }
  for(ptr=max; ptr > seq && fid_SPECIALSYMBOL(*(ptr-1)); --ptr)
  {
    ++project->len_of_special_suffix;
  }

  /* now we also need the suffix array */
  if(esa->suftab.VU == NULL) return;

  fid_suffixinterval_init_root(&si,esa);
  if(fid_suffixarray_find_embedded_interval(esa,&si,fid_WILDCARD) == 0)
  {
    project->num_of_specials=si.right-si.left+1;
    while(si.left <= si.right)
    {
      if(esa->suftab.VU[si.left] == 0 ||
         fid_REGULARSYMBOL(seq[esa->suftab.VU[si.left]-1]))
      {
        ++project->num_of_special_ranges;
      }
      ++si.left;
    }
  }
}
