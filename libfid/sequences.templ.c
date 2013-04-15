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

/*!
 * \brief Callback function for parsing sequences, called for each description.
 *
 * For each complete sequence description, a function of this type will be
 * called. The function should handle that description in some way and may also
 * take care about sequence description separators.
 *
 * \param desc Pointer to the description text. The string pointed at is \e not
 * guaranteed to be zero-terminated. This pointer might be \c NULL. In this
 * case the whole input sequence has been parsed and a last separator may be
 * written at this point.
 * \param desc_length Length of the text pointed to by \p desc.
 * \param write_separator If this is non-zero, then a sequence description
 * separator must be written, i.e., another description (thus, sequence) will
 * follow.
 * \param user_data Pointer to any data to be used by the function.
 * \param error Error messages go here.
 * 
 * \returns 0 for success, anything else to indicate an error.
 */
typedef int (*Appenddesc)(const char *desc, UINT desc_length,
                          int write_separator, void *user_data,
                          fid_Error *error);

/*!
 * \brief Callback function for parsing sequences, called in-between sequences.
 *
 * This function is called whenever a sequence separator must be written. It is
 * not called after the last sequence has been parsed, but only in-between two
 * parsed sequences.
 *
 * \param seq_position Current sequence position.
 * \param user_data Pointer to any data to be used by the function.
 * \param error Error messages go here.
 *
 * \returns 0 for success, anything else to indicate an error.
 */
typedef int (*Nextsequence)(UINT seq_position, void *user_data,
                            fid_Error *error);

/*!
 * \brief A collection of function pointers for handling of parsed data.
 *
 * The sequence parsers call the functions pointed to by the function pointers
 * of this structure. See member documentation and function typedefs for
 * further details.
 */
typedef struct
{
  Appendline appendlinefun; /*!<\brief Called for each sequence line. */
  Appenddesc appenddescfun; /*!<\brief Called for each parsed description.
                             * This pointer is allowed to be \c NULL, in which
                             * case the descriptions will be parsed, but not
                             * handled. */
  Nextsequence nextseqfun;  /*!<\brief Called after a sequence has been parsed,
                             * and before the next sequence is being parsed. */
} Parserfuns;

/*!\addtogroup wordsize */
/*@{*/

/*!
 * \brief Update data fields in fid_Sequences structure according to mapped
 * files.
 * 
 * This is the word size dependent implementation.
 * 
 * \param seqs The structure to be updated.
 * \param tables Update only values derivable from the tables specified in
 * this request. To update all values, pass #fid_TABLES_ONLINE.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int sequences_realize(fid_Sequences *seqs, fid_Tablerequest tables,
                             fid_Error *error) /*{{{*/
{
#if defined SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64
  const char *filename=NULL;
#endif /* SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64 */

  WSIZECHECK(seqs);
  if((tables&(fid_TABLE_TIS|fid_TABLE_OIS)) != 0)
  {
    if((tables&fid_TABLE_TIS) != 0 && seqs->tisfile.content != NULL)
    {
      TO_UINT(seqs->total_length.VU,seqs->tisfile.occupied,error,
              filename=seqs->tisfile.filename;
              goto bail_out;);
    }
    else if((tables&fid_TABLE_OIS) != 0 && seqs->oisfile.content != NULL)
    {
      TO_UINT(seqs->total_length.VU,seqs->oisfile.occupied,error,
              filename=seqs->oisfile.filename;
              goto bail_out;);
    }
    else
    {
      seqs->total_length.VU=0;
    }
    
    if(seqs->total_length.VU > 0)
    {
      TO_UINT(seqs->num_of_sequences.VU,
              (size_t)(seqs->sspfile.occupied/sizeof(UINT))+1,error,
              filename=seqs->sspfile.filename;
              goto bail_out;);
    }
    else
    {
      seqs->num_of_sequences.VU=0;
    }
    
    seqs->separators.VU=fid_CAST_POINTER(seqs->sspfile.content,const UINT);
  }
  
  if((tables&fid_TABLE_DES) != 0)
  {
    seqs->descriptions.VU=fid_CAST_POINTER(seqs->sdsfile.content,const UINT);
  }
  return 0;

#if defined SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64
bail_out:
  fid_error_throw(error,"Size of file \"%s\" too large to be represented in "
                  "32 bits.\n",filename);
  return -1;
#endif /* SEQUENCES_32BIT_VERSION && fid_WORDSIZE == 64 */
}

/*}}}*/

/*!
 * \brief Advance to the next sequence in output files.
 * 
 * This function appends a separator to the output sequence files and writes
 * the separator position into the file of sequence separator positions.
 * Since separators separate sequences from each others, no such symbol is
 * inserted before the first sequence.
 * 
 * \param seq_position The current sequence position that will be
 * incremented by this function if a separator has been added.
 * \param seq_count The current sequence number, incremented by the function.
 * \param nextseqfun Callback function that handles the current sequence
 * seperator.
 * \param user_data Pointer for \p nextseqfun.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int next_sequence(UINT *seq_position, UINT *seq_count,
                         Nextsequence nextseqfun, void *user_data,
                         fid_Error *error) /*{{{*/
{
  if(*seq_count > 0)
  {
    if(nextseqfun(*seq_position,user_data,error) != 0)
    {
      return -1;
    }
    ++*seq_position;
  }
  ++*seq_count;
  return 0;
}

/*}}}*/

/*!
 * \brief Parse a FASTA file from a buffer and write contents to file.
 * 
 * \param alpha The alphabet to be used for parsing sequence data.
 * \param filename The name of the input file (for error messages only).
 * \param buffer Biological sequences in FASTA format. The parse will bail
 * out on parsing errors, no attempt is made to recover.
 * \param bufsize The size of the buffer.
 * \param funs A bunch of functions that handle the parsed data, like writing
 * to files.
 * \param user_data Pointer passed to functions in \p funs.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int parse_fasta(const fid_Alphabet *alpha, const char *filename,
                       const char *buffer, size_t bufsize,
                       const Parserfuns *funs, void *user_data,
                       fid_Error *error) /*{{{*/
{
  const char *ptr, *maxptr, *tempptr;
  UINT desc_size, seq_position=0;
  UINT num_of_sequences=0;
  char c;
  int in_sequence=0;
  
  assert(buffer != NULL);
  assert(bufsize > 0);
  assert(buffer[0] == '>');
  
  maxptr=buffer+bufsize;
  for(ptr=buffer; ptr < maxptr; ++ptr)
  {
    if(*ptr == '>')
    {
      in_sequence=0;
      
      /* parse header */
      tempptr=++ptr;
      SKIP_CHARS(ptr,maxptr,'\n');
      if(ptr == maxptr)
      {
        fid_error_throw(error,"Cannot parse FASTA file \"%s\", last header "
                        "('>'-line) not followed by data.",filename);
        return -1;
      }
      else if(ptr < maxptr-1 &&
              alpha->char_to_sym[fid_CHAR_AS_INDEX(c=*(ptr+1))] == fid_UNDEF)
      {
        if(isprint((int)c))
        {
          fid_error_throw(error,"Cannot parse FASTA file \"%s\", invalid "
                          "character '%c' in sequence data.",filename,c);
        }
        else
        {
          fid_error_throw(error,"Cannot parse FASTA file \"%s\", invalid "
                          "character in sequence data (non-printable "
                          "0x%02x).",filename,(unsigned int)c);
        }
        return -1;
      }
      if(funs->appenddescfun != NULL)
      {
        TO_UINT(desc_size,ptr-tempptr+1,error,
                fid_error_throw(error,"Cannot parse FASTA file \"%s\", "
                                "description too long.",filename);
                return -1;);
        if(funs->appenddescfun(tempptr,desc_size,1,user_data,error) != 0)
        {
          return -1;
        }
      }
    }
    else if(*ptr != '\n')
    {
      if(!in_sequence)
      {
        if(next_sequence(&seq_position,&num_of_sequences,funs->nextseqfun,
                         user_data,error) == -1)
        {
          return -1;
        }
        in_sequence=1;
      }
      
      /* read and check sequence data */
      for(tempptr=ptr; ptr < maxptr && (c=*ptr) != '\n'; ++ptr)
      {
        if(alpha->char_to_sym[fid_CHAR_AS_INDEX(c)] == fid_UNDEF)
        {
          if(isprint((int)c))
          {
            fid_error_throw(error,"Cannot parse FASTA file \"%s\", invalid "
                            "character '%c' in sequence data.",filename,c);
          }
          else
          {
            fid_error_throw(error,"Cannot parse FASTA file \"%s\", invalid "
                            "character in sequence data (non-printable "
                            "0x%02x).",filename,(unsigned int)c);
          }
          return -1;
        }
      }

      assert(tempptr < ptr);
      if(funs->appendlinefun(tempptr,ptr-tempptr,alpha,user_data,error) != 0)
      {
        return -1;
      }
      seq_position+=ptr-tempptr;
    }
  }
  
  if(funs->appenddescfun != NULL)
  {
    return funs->appenddescfun(NULL,0,1,user_data,error);
  }

  return 0;
}

/*}}}*/

/*!
 * \brief Read content of ID tag of UniProt file and write to description file.
 * 
 * This function expects the current file position, indicated by \p maxptr,
 * to point directly to the "ID" string. The identifier itself is read over,
 * the succeeding string is used as the first part of the description of the
 * sequences parsed next (the other part is taken from the "DE" tag).
 * 
 * \param ptr_ptr This is set to point to the beginning of the line
 * following the "ID" line for the caller to continue at that point.
 * \param maxptr Length of the parse buffer, expressed as pointer (only
 * read up to and not including this pointer).
 * \param filename Name of the parsed file for generating error messages.
 * \param appenddescfun Parsed description strings are handled by this
 * function. This pointer is allowed to be \c NULL.
 * \param user_data Pointer for \p appenddescfun.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int read_uniprot_identifier(const char **ptr_ptr, const char *maxptr,
                                   const char *filename,
                                   Appenddesc appenddescfun, void *user_data,
                                   fid_Error *error) /*{{{*/
{
  const char *ptr=*ptr_ptr, *tempptr;
  char c=0;
  UINT desc_size;
  
  if(appenddescfun != NULL)
  {
    /* go to first non-whitespace character */
    ptr+=3;
    SKIP_GAP(ptr,maxptr,' ');
    if(ptr == maxptr)
    {
      goto error_no_data;
    }
    
    /* jump over next string */
    tempptr=ptr;
    while(ptr < maxptr && (c=*ptr) != '\n' && c != ' ')
    {
      ++ptr;
    }
    if(ptr == maxptr)
    {
      goto error_no_data;
    }
    
    /* write that string into description file */
    TO_UINT(desc_size,ptr-tempptr,error,
            fid_error_throw(error,"Cannot parse UniProt file \"%s\", "
                            "description too long.",filename);
            return -1;);
    if(appenddescfun(tempptr,desc_size,1,user_data,error) != 0)
    {
      return -1;
    }
  }
  
  /* go to next line */
  SKIP_CHARS(ptr,maxptr,'\n');
  ++ptr;
  
  *ptr_ptr=ptr;
  return 0;
  
 error_no_data:
  fid_error_throw(error,"Cannot parse UniProt file \"%s\", last "
                  "identifier ('ID'-line) not followed by sequence "
                  "data.",filename);
  return -1;
}

/*}}}*/

/*!
 * \brief Read content of DE tag of UniProt file and write to description file.
 * 
 * This function expects the current file position, indicated by \p maxptr,
 * to point directly to the first "DE" string. All consequtive "DE" lines
 * are read, but only the content of the first line is appended to the
 * description of the sequences parsed next (unless \p all_descriptions is
 * set unequal to 0, in which case all descriptions are appended).
 * 
 * \param ptr_ptr This is set to point to the beginning of the line
 * following the block of "DE" lines for the caller to continue at that point.
 * \param maxptr Length of the parse buffer, expressed as pointer (only
 * read up to and not including this pointer).
 * \param already_started If this is 0, then no new description will 
 * be started, but the current description is prolonged, otherwise start a
 * new description. The purpose is to be able to parse sequences that
 * contain no "ID" tag, which is expected to appear before the "DE" block.
 * \param all_descriptions If 0, then only put the content of the first "DE"
 * line into the sequence description, otherwise put all of them there.
 * \param filename Name of the parsed file for generating error messages.
 * \param appenddescfun Parsed description strings are handled by this
 * function. This pointer is allowed to be \c NULL.
 * \param user_data Pointer for \p appenddescfun.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int read_uniprot_descriptions(const char **ptr_ptr, const char *maxptr,
                                     int already_started, int all_descriptions,
                                     const char *filename,
                                     Appenddesc appenddescfun, void *user_data,
                                     fid_Error *error) /*{{{*/
{
  const char *ptr=*ptr_ptr, *tempptr;
  char c=0;
  UINT lines=0, desc_size;
  
  while(memcmp(ptr,"DE ",3) == 0)
  {
    /* find first non-space character */
    tempptr=ptr+3;
    SKIP_GAP(tempptr,maxptr,' ');
    if(tempptr == maxptr)
    {
      goto error_no_data;
    }
    else if(c == '\n')
    {
      /* empty DE line */
      ptr=tempptr+1;
      continue;
    }
    
    --tempptr;
    
    /* jump to end of line */
    ptr=tempptr+1;
    SKIP_CHARS(ptr,maxptr,'\n');
    if(ptr == maxptr)
    {
      goto error_no_data;
    }
    ++ptr;
    
    if(appenddescfun != NULL && (all_descriptions || lines == 0))
    {
      TO_UINT(desc_size,ptr-tempptr,error,
              fid_error_throw(error,"Cannot parse UniProt file \"%s\", "
                              "description too long.",filename);
              return -1;);
      if(appenddescfun(tempptr,desc_size,!already_started,user_data,error) != 0)
      {
        return -1;
      }
      already_started=1;
    }
    ++lines;
  }
  
  *ptr_ptr=ptr;
  return 0;
  
 error_no_data:
  fid_error_throw(error,"Cannot parse UniProt file \"%s\", last "
                  "description ('DE'-lines) not followed by sequence "
                  "data.",filename);
  return -1;
}

/*}}}*/

/*!
 * \brief Parse a UniProt file from a buffer and write contents to file.
 * 
 * \param alpha The alphabet to be used for parsing sequence data.
 * \param filename The name of the input file (for error messages only).
 * \param buffer Biological sequences in UniProt/Swiss-Prot format. The
 * parse will bail out on parsing errors, no attempt is made to recover.
 * \param bufsize The size of the buffer.
 * \param funs A bunch of functions that handle the parsed data, like writing
 * to files.
 * \param user_data Pointer passed to functions in \p funs.
 * \param error Error messages go here.
 * 
 * \returns 0 on success, -1 on error.
 */
static int parse_uniprot(const fid_Alphabet *alpha, const char *filename,
                         const char *buffer, size_t bufsize,
                         const Parserfuns *funs, void *user_data,
                         fid_Error *error) /*{{{*/
{
  const char *ptr, *maxptr, *tempptr;
  UINT seq_position=0;
  UINT num_of_sequences=0;
  char c;
  int in_sequence=0, wrote_identifier=0;
  
  assert(buffer != NULL);
  assert(bufsize > 0);
  
  maxptr=buffer+bufsize-2;
  for(ptr=buffer; ptr < maxptr; ++ptr)
  {
    if(memcmp(ptr,"SQ ",3) == 0)
    {
      if(!in_sequence)
      {
        if(next_sequence(&seq_position,&num_of_sequences,funs->nextseqfun,
                         user_data,error) == -1)
        {
          return -1;
        }
        in_sequence=1;
        wrote_identifier=0;
      }
      
      /* skip rest of line */
      SKIP_CHARS(ptr,maxptr,'\n');
      
      /* read and check sequence data */
      for(++ptr; ptr < maxptr && *ptr == ' '; ++ptr)
      {
        ++ptr;
        SKIP_GAP(ptr,maxptr,' ');
        tempptr=ptr;
        while(ptr < maxptr && (c=*ptr) != '\n' && !(c >= '0' && c <= '9'))
        {
          if(c == ' ')
          {
            if(tempptr < ptr &&
              funs->appendlinefun(tempptr,ptr-tempptr,alpha,user_data,
                                  error) != 0)
            {
              return -1;
            }
            seq_position+=ptr-tempptr;
            SKIP_GAP(ptr,maxptr,' ');
            tempptr=ptr;
            continue;
          }
          
          if(alpha->char_to_sym[fid_CHAR_AS_INDEX(c)] == fid_UNDEF)
          {
            if(isprint((int)c))
            {
              fid_error_throw(error,"Cannot parse UniProt file \"%s\", invalid "
                              "character '%c' in sequence data.",filename,c);
            }
            else
            {
              fid_error_throw(error,"Cannot parse UniProt file \"%s\", invalid "
                              "character in sequence data (non-printable "
                              "0x%02x).",filename,(unsigned int)c);
            }
            return -1;
          }
          ++ptr;
        }
        SKIP_CHARS(ptr,maxptr,'\n');
        ++ptr;
      }
    }
    else if(memcmp(ptr,"ID ",3) == 0)
    {
      in_sequence=0;
      if(read_uniprot_identifier(&ptr,maxptr,filename,funs->appenddescfun,
                                 user_data,error) == -1)
      {
        return -1;
      }
      wrote_identifier=1;
    }
    else if(memcmp(ptr,"DE ",3) == 0)
    {
      in_sequence=0;
      if(read_uniprot_descriptions(&ptr,maxptr,wrote_identifier,0,filename,
                                   funs->appenddescfun,user_data,error) == -1)
      {
        return -1;
      }
      wrote_identifier=0;
    }
    else
    {
      /* just ignore the line, being too picky is bad in this field... */
      SKIP_CHARS(ptr,maxptr,'\n');
    }
  }
  
  if(funs->appenddescfun != NULL)
  {
    return funs->appenddescfun(NULL,0,1,user_data,error);
  }

  return 0;
}

/*}}}*/

/*!
 * \brief Call format-dependent parser.
 *
 * \param infilename, buffer, bufsize, alpha, error See
 * fid_sequences_parse_from_memory_to_file() and
 * fid_sequences_parse_from_memory_to_memory().
 * \param format This argument decides upon which parser gets called. It is
 * also the only argument actually interpreted by this function, all the others
 * are just passed on the to real parsers.
 * \param funs Pointers to functions that handle the parsed data.
 * \param user_data Data for the data handler routines.
 *
 * \returns Whatever the parsing routine returned.
 *
 * \todo Implement parsers for more formats.
 */
static int parse_sequences(const char *infilename, const char *buffer,
                           size_t bufsize, fid_Fileformat format,
                           const fid_Alphabet *alpha, const Parserfuns *funs,
                           void *user_data, fid_Error *error)/*{{{*/
{
  switch(format)
  {
   case fid_FORMAT_FASTA:
    return parse_fasta(alpha,infilename,buffer,bufsize,funs,user_data,error);
   case fid_FORMAT_UNIPROT:
    return parse_uniprot(alpha,infilename,buffer,bufsize,funs,user_data,error);
   default:
    abort();
  }
}
/*}}}*/

/*!
 * \brief Turn the dynamic arrays into a fid_Sequences structure.
 *
 * When parsing sequences into memory, we can just use the fid_Sequences
 * structure to return the parsed data. This is convenient for client code
 * since there is no real special case for sequences parsed to memory.
 * Internally, however, the fid_Sequences structure needs to know about this
 * issue for it must not try to unmap the faked mapped files. Thus, this
 * function sets the fid_Sequences::fakefiles flag to 1.
 *
 * \param seqs The structure to be initialized.
 * \param arrays The data that has just been parsed from file. The memory
 * blocks occupied by the arrays are resized to the smallest possible value by
 * this function.
 * \param alpha An alphabet, needed to initialize \p seqs correctly.
 * \param tables The tables that are used, passed to fid_sequences_realize().
 * \param padding Number of padding characters appended to the sequence. The
 * number of occupied bytes in fid_Sequences::tisfile will be corrected by this
 * parameter if necessary.
 * \param error Error messages go here.
 *
 * \returns The return value of fid_sequences_realize().
 */
static int arrays_to_sequences(fid_Sequences *seqs, Sequencearrays *arrays,
                               const fid_Alphabet *alpha,
                               fid_Tablerequest tables, size_t padding,
                               fid_Error *error)/*{{{*/
{ 
  fid_sequences_init(seqs,fid_UINTSIZE,alpha);
  FAKEFILE_FROM_ARRAY(seqs->tisfile,arrays->tis,fid_Symbol);
  FAKEFILE_FROM_ARRAY(seqs->oisfile,arrays->ois,char);
  FAKEFILE_FROM_ARRAY(seqs->desfile,arrays->des,char);
  FAKEFILE_FROM_ARRAY(seqs->sspfile,arrays->ssp.VU,UINT);
  FAKEFILE_FROM_ARRAY(seqs->sdsfile,arrays->sds.VU,UINT);
  if(seqs->tisfile.occupied >= padding) seqs->tisfile.occupied-=padding;
  if(seqs->oisfile.occupied >= padding) seqs->oisfile.occupied-=padding;
  return fid_sequences_realize(seqs,tables,error);
}
/*}}}*/

/*!
 * \brief Parse sequence data into dynamic arrays.
 *
 * This function just initializes the dynamic arrays, and then calls
 * parse_sequences().
 *
 * \param infilename, format, tables, alpha, padding, error See
 * fid_sequences_parse_from_file_to_memory() and
 * fid_sequences_parse_from_memory_to_memory().
 * \param buffer, bufsize, seqs See
 * fid_sequences_parse_from_memory_to_memory().
 * 
 * \returns 0 on success, -1 on failure.
 *
 * \todo The initial memory block size should be estimated from the input file
 * size, and not simply set to #fid_MAPPEDFILE_GROWSIZE.
 */
static int parse_from_mem_to_mem(const char *infilename, const char *buffer,
                                 size_t bufsize, fid_Fileformat format,
                                 fid_Tablerequest tables, fid_Sequences *seqs,
                                 const fid_Alphabet *alpha, size_t padding,
                                 fid_Error *error)/*{{{*/
{
  Parserfuns funs;
  Sequencearrays arrays;

  funs.appendlinefun=append_seq_to_dynarrays;
  funs.appenddescfun=(tables&fid_TABLE_DES)?append_desc_to_dynarrays:NULL;
  funs.nextseqfun=next_sequence_to_dynarrays;

  fid_DYNARRAY_INIT(&arrays.tis,fid_Symbol);
  fid_DYNARRAY_INIT(&arrays.ois,char);
  fid_DYNARRAY_INIT(&arrays.des,char);
  fid_DYNARRAY_INIT(&arrays.ssp.VU,UINT);
  fid_DYNARRAY_INIT(&arrays.sds.VU,UINT);
  arrays.filename=infilename;

  if((tables&fid_TABLE_TIS) != 0)
  {
    fid_DYNARRAY_ENSURE_NEXT(&arrays.tis,fid_Symbol,fid_MAPPEDFILE_GROWSIZE,
                             goto cleanup;);
  }
  if((tables&fid_TABLE_OIS) != 0)
  {
    fid_DYNARRAY_ENSURE_NEXT(&arrays.ois,char,fid_MAPPEDFILE_GROWSIZE,
                             goto cleanup;);
  }
  if((tables&(fid_TABLE_TIS|fid_TABLE_OIS)) != 0)
  {
    fid_DYNARRAY_ENSURE_NEXT(&arrays.ssp.VU,UINT,fid_MAPPEDFILE_GROWSIZE,
                             goto cleanup;);
  }
  if((tables&fid_TABLE_DES) != 0)
  {
    fid_DYNARRAY_ENSURE_NEXT(&arrays.des,char,fid_MAPPEDFILE_GROWSIZE,
                             goto cleanup;);
    fid_DYNARRAY_ENSURE_NEXT(&arrays.sds.VU,UINT,fid_MAPPEDFILE_GROWSIZE,
                             goto cleanup;);
  }

  if(parse_sequences(infilename,buffer,bufsize,format,alpha,&funs,&arrays,
                     error) == 0)
  {
    /* convert to (fake) files if no error occurred */
    if(append_padding(&arrays,padding,error) == 0 &&
       arrays_to_sequences(seqs,&arrays,alpha,tables,padding,error) == 0)
    {
      return 0;
    }
  }

  if(0)
  {
 cleanup:
    fid_OUTOFMEM(error);
  }

  fid_DYNARRAY_FREE(&arrays.tis,fid_Symbol);
  fid_DYNARRAY_FREE(&arrays.ois,char);
  fid_DYNARRAY_FREE(&arrays.des,char);
  fid_DYNARRAY_FREE(&arrays.ssp.VU,UINT);
  fid_DYNARRAY_FREE(&arrays.sds.VU,UINT);
  return -1;
}
/*}}}*/

/*!
 * \brief Determine sequence index from sequence offset.
 * 
 * It is an error to pass the position of a sequence separator as offset.
 * 
 * \param seqs Set of sequences.
 * \param offset Offset into the concatenated set of sequences.
 * 
 * \returns The sequence index corresponding to the given offset.
 */
UINT fid_sequences_offset_to_index(const fid_Sequences *seqs,
                                   UINT offset) /*{{{*/
{
  UINT left, right, pivot;
  UINT entry;
  
  assert(seqs != NULL);
  assert(seqs->num_of_sequences.VU > 0);
  assert(seqs->num_of_sequences.VU == (UINT)1 || seqs->separators.VU != NULL);
  WSIZECHECK(seqs);
  
  if(seqs->num_of_sequences.VU == (UINT)1)
  {
    return 0;
  }
  
  left=0;
  right=seqs->num_of_sequences.VU-1;
  while(left+1 < right)
  {
    pivot=left+((right-left) >> 1);
    entry=seqs->separators.VU[pivot];
    if(entry < offset)
    {
      left=pivot;
    }
    else
    {
      assert(entry != offset);
      right=pivot;
    }
  }
  
  if(left == 0 && seqs->separators.VU[left] > offset)
  {
    return 0;
  }
  else if(left == right || seqs->separators.VU[left] < offset)
  {
    return left+1;
  }
  else
  {
    return right+1;
  }
}

/*}}}*/

/*!
 * \brief Determine sequence boundaries from sequence index.
 * 
 * \param seqs Set of sequences.
 * \param seqindex Sequence index; the first sequence is identified by 0.
 * \param left,right The sequence boundaries will be written to these
 * integers. Note that sequence separators are excluded from the returned
 * sequence.
 */
void fid_sequences_index_to_boundaries(const fid_Sequences *seqs,
                                       UINT seqindex,
                                       UINT *left, UINT *right) /*{{{*/
{
  assert(seqs != NULL);
  assert(seqs->num_of_sequences.VU > 0);
  assert(seqs->num_of_sequences.VU == (UINT)1 || seqs->separators.VU != NULL);
  assert(seqindex < seqs->num_of_sequences.VU);
  assert(left != NULL);
  assert(right != NULL);
  WSIZECHECK(seqs);

  if(seqindex == 0)
  {
    *left=0;
  }
  else
  {
    *left=seqs->separators.VU[seqindex-1]+1;
  }

  if(seqindex+1 < seqs->num_of_sequences.VU)
  {
    *right=seqs->separators.VU[seqindex]-1;
  }
  else
  {
    *right=seqs->total_length.VU-1;
  }

  assert(*left <= *right);
}

/*}}}*/

/*!
 * \brief Determine sequence boundaries from sequence offset.
 * 
 * This function performs a binary search to find the sequence index first,
 * so use fid_sequences_index_to_boundaries() instead if you already know
 * the sequence index. It is an error to pass the position of a sequence
 * separator as offset.
 * 
 * \param seqs Set of sequences.
 * \param offset Offset into the concatenated set of sequences.
 * \param left,right The sequence boundaries will be written to these
 * integers. Note that sequence separators are excluded from the returned
 * sequence.
 */
void fid_sequences_offset_to_boundaries(const fid_Sequences *seqs,
                                        UINT offset,
                                        UINT *left, UINT *right) /*{{{*/
{
  WSIZECHECK(seqs);
  fid_sequences_index_to_boundaries(seqs,
                                    fid_sequences_offset_to_index(seqs,offset),
                                    left,right);
}

/*}}}*/

/*!
 * \brief Call a callback function for each sequence in given range.
 * 
 * For each sequence stored in \p seqs in the range specified by sequences
 * indices \p from and \p to, call the callback function \p iterfun. The
 * iteration can be interrupted by the callback function returning a
 * non-zero value.
 * 
 * \param seqs The sequences to be iterated over.
 * \param from, to The range of sequences to be processed. These values must
 * be valid, i.e., \p from must not be greater than \p to, and none of them
 * must be greater than fid_Sequences::num_of_sequences-1, behavior is
 * undefined in these cases.
 * \param iterfun The callback function.
 * \param user_data Arbitrary pointer passed through to \p iterfun.
 * 
 * \returns 0 on success, or the return value of \p iterfun otherwise.
 */
int fid_sequences_iterate_range(const fid_Sequences *seqs,
                                UINT from, UINT to,
                                const fid_Sequenceiterfun iterfun,
                                void *user_data) /*{{{*/
{
  UINT seqnum, length, offset, last;
  int retval;
  
  assert(seqs != NULL);
  assert(seqs->num_of_sequences.VU <= (UINT)1 ||  seqs->separators.VU != NULL);
  assert(from <= to);
  assert(to < seqs->num_of_sequences.VU);
  assert(iterfun != NULL);
  WSIZECHECK(seqs);
  
  if(seqs->num_of_sequences.VU > (UINT)1)
  {
    seqnum=from;
    if(seqnum < seqs->num_of_sequences.VU-1)
    {
      if(seqnum == 0)
      {
        offset=0;
        length=seqs->separators.VU[0];
      }
      else
      {
        offset=seqs->separators.VU[seqnum-1]+1;
        length=seqs->separators.VU[seqnum]-offset;
      }
      
      if(to == seqs->num_of_sequences.VU-1)
      {
        last=seqs->num_of_sequences.VU-2;
      }
      else
      {
        last=to;
      }
      
      retval=iterfun(seqs,seqnum,seqs->tisfile.content+offset,length,user_data);
      
      for(++seqnum; retval == 0 && seqnum <= last; ++seqnum)
      {
        offset+=length+1;
        length=seqs->separators.VU[seqnum]-offset;
        retval=iterfun(seqs,seqnum,seqs->tisfile.content+offset,length,user_data);
      }
      
      if(retval == 0 && last != to)
      {
        /* caller wants to see the last sequence, too */
        assert(to == seqs->num_of_sequences.VU-1);
        offset+=length+1;
        length=seqs->total_length.VU-offset;
        retval=iterfun(seqs,seqnum,seqs->tisfile.content+offset,length,user_data);
      }
    }
    else
    {
      /* caller wants to see the last sequence only */
      assert(to == seqs->num_of_sequences.VU-1);
      offset=seqs->separators.VU[seqnum-1]+1;
      length=seqs->total_length.VU-offset;
      retval=iterfun(seqs,seqnum,seqs->tisfile.content+offset,length,user_data);
    }
  }
  else
  {
    assert(seqs->num_of_sequences.VU == (UINT)1);
    assert(from == 0 && to == 0);
    retval=iterfun(seqs,0,seqs->tisfile.content,seqs->total_length.VU,user_data);
  }
  
  return retval;
}

/*}}}*/

/*!
 * \brief Call a callback function for each sequence.
 * 
 * For each sequence stored in \p seqs, call the callback function
 * \p iterfun. The iteration can be interrupted by the callback function
 * returning a non-zero value.
 * 
 * \param seqs The sequences to be iterated over.
 * \param iterfun The callback function.
 * \param user_data Arbitrary pointer passed through to \p iterfun.
 * 
 * \returns 0 on success, or the return value of \p iterfun otherwise.
 */
int fid_sequences_iterate(const fid_Sequences *seqs,
                          const fid_Sequenceiterfun iterfun, void *user_data) /*{{{*/
{
  assert(seqs != NULL);
  assert(seqs->num_of_sequences.VU <= (UINT)1 ||  seqs->separators.VU != NULL);
  assert(iterfun != NULL);
  WSIZECHECK(seqs);
  
  if(seqs->num_of_sequences.VU > 0)
  {
    return fid_sequences_iterate_range(seqs,0,seqs->num_of_sequences.VU-1,
                                       iterfun,user_data);
  }
  else
  {
    return 0;
  }
}

/*}}}*/

/*!
 * \brief Print piece of sequence to stream.
 * 
 * \param seq Some binary encoded sequence of symbols.
 * \param length The number of symbols to be read from \p seq and printed
 * to \p stream.
 * \param alpha Alphabet used to transform symbols to printable characters.
 * \param str If not \c NULL, then print this ASCII string in front of
 * the transformed sequence (useful to distinguish multiple sequences).
 * \param stop_at_separator If true, then stop printing when a sequence
 * separator is encountered. If false, print some special character for each
 * sequence separator encountered and continue.
 * \param stream An output stream to which the sequence is printed. If
 * \c NULL, nothing will be printed.
 */
void fid_sequences_dump_range(const fid_Symbol *seq, UINT length,
                              const fid_Alphabet *alpha, const char *str,
                              int stop_at_separator, FILE *stream) /*{{{*/
{
  UINT i;
  
  assert(seq != NULL);
  assert(alpha != NULL);
  
  if(stream == NULL)
  {
    return;
  }
  
  if(str != NULL)
  {
    fprintf(stream,"%s",str);
  }
  if(stop_at_separator)
  {
    for(i=0; i < length && seq[i] != fid_SEPARATOR; ++i)
    {
      (void)fputc(fid_PRINT_SYMBOL(alpha,seq[i]),stream);
    }
  }
  else
  {
    for(i=0; i < length; ++i)
    {
      (void)fputc(fid_PRINT_SYMBOL(alpha,seq[i]),stream);
    }
  }
  (void)fputc('\n',stream);
}

/*}}}*/
/*@}*/

