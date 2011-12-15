/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

//\Ignore{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "optdesc.h"
#include "errordef.h"
#include "protodef.h"
#include "spacedef.h"

//}

/*
  This file implements functions for processing options according to
  some option declarations.
*/

/*EE
  The following function initializes the table \texttt{options} storeing the
  information for all options. \texttt{numofoptions} is the number of options.
*/

void initoptions(OptionDescription *options,Uint numofoptions)
{
  Uint i;

  for(i=0; i<numofoptions; i++)
  {
    options[i].declared = false;
  }
}

/*EE
  The following function adds an option to table 
  \texttt{options}. \texttt{numofoptions} is the number of options.
  \texttt{optnum} is the number of the option declared. \texttt{optname}
  is the option string to be used when calling the program. Finally,
  \texttt{optdesc} is the option description, i.e.\ a short explanation of
  the option. The function returns 0 if everything went fine. In 
  case of an error, a negative error code is returned. The
  function should be called via the macro \texttt{ADDOPTION} as defined
  in \texttt{optdesc.h}.
*/

Sint addoption(OptionDescription *options,Uint numofoptions,
               Uint optnum,char *optname,char *optdesc)
{
  Uint i;

  if(optnum>=numofoptions)
  {
    fprintf(stderr,"option number %ld out of range [0,%lu]",
            (long int) optnum,
            (long unsigned int) (numofoptions-1));
    return -1;
  }
  if(optnum > 0 && options[optnum].declared)
  {
    fprintf(stderr,"option %lu already declared",(long unsigned int) optnum);
    return -2;
  }
  options[optnum].optname = optname;
  options[optnum].description = optdesc;
  options[optnum].optval = optnum;
  options[optnum].isalreadyset = false;
  options[optnum].declared = true;
  if(optnum == numofoptions-1)
  {
    for(i=0; i<numofoptions; i++)
    {
      if(!options[i].declared)
      {
        fprintf(stderr,"option %lu not declared",(long unsigned int) i);
        return -3;
      }
    }
  }
  return 0;
}

/*EE
  The following function looks up a string \texttt{optstring} in a table of
  option descriptions. If it has found an option description whose
  \texttt{optname} is identical to \texttt{optstring}, it is checked if 
  this option has already been set. If so, then an error message is 
  thrown and the function returns with an error code.
  If the option was not set before, then the index of the option is returned. 
  If optstring is not found in the table, then this is an error and 
  the program throws an error message and returns with an exit code.
*/ 

Sint procoption(OptionDescription *opt,Uint numofopt,char *optstring)
{
  Uint i;

  for(i=0; i<numofopt; i++)
  {
    if(strcmp(optstring,opt[i].optname) == 0)
    {
      if(opt[i].isalreadyset)
      {
        fprintf(stderr,"option \"%s\" already set",optstring);
        return -1;
      }
      opt[i].isalreadyset = true;
      return (int) i;
    }
  }
  fprintf(stderr,"illegal option \"%s\"",optstring);
  return -2;
}

/*
  The following function checks if the number \texttt{i} occurrs in
  \texttt{list}.
*/

static bool occursinlist(Uint i,int *list)
{
  Uint j;

  for(j=0; list[j] != -1; j++)
  {
    if(((Sint) i) == list[j])
    {
      return true;
    }
  }
  return false;
}

/*
  The following function computes the maximal length of all options
  in the option description \texttt{opt}. If \texttt{excludetab} is not
  \texttt{NULL}, then the option numbers in \texttt{excludetab} are
  ignored.
*/

static Uint getoptindent(int *excludetab,OptionDescription *opt,Uint numofopt)
{
  Uint i, slen, indentlevel = 0;

  for(i=0; i<numofopt; i++)
  {
    if(excludetab == NULL || !occursinlist(i,excludetab))
    {
      slen = (Uint) strlen(opt[i].optname);
      if(indentlevel < slen)
      {
        indentlevel = slen;
      }
    }
  }
  return indentlevel + 2;
}

/*
  The following function shows the option description.
  An newline symbol in the option description is output with
  \texttt{indentlevel} many blanks.
*/

static void showoptdesc(FILE *fp,Uint indentlevel,char *desc)
{
  Uint j;

  for(j=0; desc[j] != '\0'; j++)
  {
    if(desc[j] == '\n')
    {
      fprintf(fp,"\n%-*s",(int) indentlevel," ");
    } else
    {
      (void) putc(desc[j],fp);
    }
  }
  (void) putc('\n',fp);
}

/*EE
  The following function shows the options as specified by the
  option description \texttt{opt}. For each option, it checks
  if the component \texttt{optval} is identical to the index of the option.
  The option description is output to a file pointer \texttt{outfp} with
  the indentation level delivered by \texttt{getoptindent}.
  \texttt{program} is the program which calls \texttt{showoptions}.
*/

void showoptions(FILE *outfp,char *program,OptionDescription *opt,
                 Uint numofopt)
{
  Uint i, indentlevel;
  FILE *fp;

  if(outfp == NULL)
  {
    fp = stdout;
  } else
  {
    fp = outfp;
  }
  indentlevel = getoptindent(NULL,opt,numofopt);
  for(i=0; i<numofopt; i++)
  {
    if(i != opt[i].optval)
    {
      fprintf(stderr,"%s: optvalue %lu for option \"%s\" must be %lu\n",
                     program,
                     (long unsigned int) opt[i].optval,
                     opt[i].optname,
                     (long unsigned int) i);
      exit(EXIT_FAILURE);
    }
    fprintf(fp,"%-*s",(int) indentlevel,opt[i].optname);
    showoptdesc(fp,indentlevel,opt[i].description);
  }
}

/*EE
  The following function shows the options as specified by the
  option description \texttt{opt}, except for the
  options whose numbers are specified in list \texttt{exludetab}.
  The option description is output to file pointer \texttt{outfp}.
  \texttt{program} is the program which calls \texttt{showoptions}.
*/

void showoptionswithoutexclude(FILE *outfp,char *program,
                               OptionDescription *opt,
                               int *excludetab,Uint numofopt)
{
  Uint i, indentlevel;
  FILE *fp;

  if(outfp == NULL)
  {
    fp = stdout;
  } else
  {
    fp = stdout;
  }
  indentlevel = getoptindent(excludetab,opt,numofopt);
  for(i=0; i<numofopt; i++)
  {
    if(!occursinlist(i,excludetab))
    {
      if(i != opt[i].optval)
      {
        fprintf(stderr,"%s: optvalue %lu for option \"%s\" must be %lu\n",
                       program,
                       (long unsigned int) opt[i].optval,
                       opt[i].optname,
                       (long unsigned int) i);
        exit(EXIT_FAILURE);
      }
      fprintf(fp,"%-*s",(int) indentlevel,opt[i].optname);
      showoptdesc(fp,indentlevel,opt[i].description);
    }
  }
}

/*EE
  The following function checks for all pairs of options
  if they have already been declared to be excluding each other.
*/

int checkdoubleexclude(Uint numofopts,OptionDescription *opt,
                        int *excludetab,Uint len)
{
  Uint i, j;
  Sint indi, indj;
  bool *excludepairs = ALLOCSPACE(NULL,bool,numofopts*numofopts);

  for(i=0; i < numofopts * numofopts; i++)
  {
    excludepairs[i] = false;
  }
  for(i=0; i < len; i = j+1)
  {
    indi = excludetab[i];
    for(j=i+1; excludetab[j] != -1; j++)
    {
      indj = excludetab[j];
      if(excludepairs[indi*numofopts+indj])
      {
        fprintf(stderr,"option %s and option %s already specfied as excluded",
               opt[indi].optname,opt[indj].optname);
        return -1;
      }
      excludepairs[indi*numofopts+indj] = true;
      excludepairs[indj*numofopts+indi] = true;
    }
  }
  FREESPACE(excludepairs);
  return 0;
}

/*EE
  To specify pairs of options which exclude each other we
  use an exclude table. An exclude table is just an array of 
  signed integers. Each block of this table consists of a maximal 
  subsequence not beginning with \texttt{-1}, but ending with 
  \texttt{-1}. Let \(i_{0}, i_{1}, \ldots, l_{r}\) be a block.
  Each number is an option number.
  For \(j\in[1,r]\), option number \(i_{0}\) and option number 
  \(i_{j}\) exclude each other.

  The following function takes a table of option descriptions and an
  exclude table of length \texttt{len}. It checks if
  two options which exclude each other are set. If there are no
  such options, then the function returns -1 and throws an error message.
  The function must be called after parsing the options.
*/

int checkexclude(OptionDescription *opt,Sint *excludetab,Uint len)
{
  Uint i, j;

  for(i=0; i < len; i = j+1)
  {
    for(j=i+1; excludetab[j] != -1; j++)
    {
      if(opt[excludetab[i]].isalreadyset && opt[excludetab[j]].isalreadyset)
      {
        fprintf(stderr,"option %s and option %s exclude each other",
               opt[excludetab[i]].optname,opt[excludetab[j]].optname);
        return -1;
      }
    }
  }
  return 0;
}

/*EE
  The following function takes a table of option descriptions and an
  exclude table of length \texttt{len}. It prints the exclude table
  in \LaTeX\ format. We use this function to produce a table in
  the manual of a program showing which options exclude each other.
*/

void showexclude(OptionDescription *opt,Sint *excludetab,Uint len)
{
  Uint offset, i, j;

  printf("\\begin{tabular}{|l|p{125mm}|}\\hline\n");
  printf("Option&cannot be combined with\\\\\\hline\n");
  for(i=0; i < len; i = j+1)
  {
    if(i>0)
    {
      printf("\\\\\\hline\n");
    }
    if(opt[excludetab[i]].optname[0] == '-')
    {
      offset = UintConst(1);
    } else
    {
      offset = 0;
    }
    printf("\\Showoption{%s}& ",opt[excludetab[i]].optname+offset);
    for(j=i+1; excludetab[j] != -1; j++)
    {
      if(opt[excludetab[j]].optname[0] == '-')
      {
        offset = UintConst(1);
      } else
      {
        offset = 0;
      }
      printf("\\Showoption{%s} ",opt[excludetab[j]].optname+offset);
    }
  }
  printf("\\\\\\hline");
  printf("\n\\end{tabular}\n");
}
