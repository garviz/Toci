/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#ifndef STREEDEF_H
#define STREEDEF_H
#include "types.h"
#include "streemac.h"
#include "streetyp.h"

Sint constructprogressstree(Suffixtree *stree,Uchar *text,Uint textlen,
                            void (*progress)(Uint,void *),
                            void (*finalprogress)(void *),void *info);
void freestree(Suffixtree *stree);
void getbranchinfostree(Suffixtree *stree,Uint whichinfo,
                                Branchinfo *branchinfo,Bref btptr);
void linklocstree(Suffixtree *stree,Location *outloc,Location *inloc);
void rescanstree(Suffixtree *stree,Location *loc,
                         Bref btptr,Uchar *left,Uchar *right);
Uchar *scanprefixfromnodestree(Suffixtree *stree,Location *loc,
                         Bref btptr,Uchar *left,
                         Uchar *right,Uint rescanlength);
Uchar *scanprefixstree(Suffixtree *stree,Location *outloc,
                         Location *inloc,Uchar *left,
                         Uchar *right,Uint rescanlength);
Sint depthfirststree(Suffixtree *stree,Reference *startnode,
                     Sint (*processleaf)(Uint,Bref,void *),
                     bool (*processbranch1)(Bref,void *),
                     Sint (*processbranch2)(Bref,void *),
                     bool (*stoptraversal)(void *),void *stopinfo,void *info);
Uchar *findprefixpathstree(Suffixtree *stree,
                                       ArrayPathinfo *path,
                                       Location *outloc,
                                       Location *inloc,
                                       Uchar *left,
                                       Uchar *right,
                                       Uint rescanlength);
#endif
