/*
 * =====================================================================================
 *
 *       Filename:  STree.h
 *
 *    Description:  Class for suffix tree construction
 *
 *        Version:  1.0
 *        Created:  10/10/11 17:53:48
 *       Revision:  none
 *       Compiler:  mpic++
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx 
 *        Company:  CAOS
 *
 * =====================================================================================
 */

#ifndef STREE_H
#define STREE_H

#include "types.h"

class STree
{
    private:
        Uint textlen,               // the length of the input string
       *leaftab,              // stores the brother-references of the leafs
       *branchtab,            // table TBranch
       *rootchildren;         // references to successors of root
  SYMBOL *text,               // points to the input string
         *sentinel;           // points to the position of the \(\$\)-symbol

  Uint nextfreeleafnum,       // the number of the next leaf
       headnodedepth,         // the depth of the headnode
       insertnode,            // the node the split edge leads to 
       insertprev,            // the edge preceeding the split edge
       smallnotcompleted,     // the number of small nodes in the current chain
       nextfreebranchnum,     // the number of the next free branch node
       onsuccpath,            // refers to node on success path of headnode
       currentdepth,          // depth of the new branch node
       branchnodeoffset,      // number of leafs in tree
       alphasize,             // the number of different characters in t
       maxbranchdepth,        // maximal depth of branching node
       largenode,             // number of large nodes
       smallnode,             // number of small nodes
       *setlink,              // address of a nil-reference
       *nextfreeleafptr,      // points to next free entry in leaftab
       *chainstart,           // address of the node, current chains starts at
       *nextfreebranch,       // reference to next free base addr. in branchtab
       *headnode,             // left component of head location
       currentbranchtabsize,  // current number of cells in branchtab
       *firstnotallocated,    // refers to the last address, such that at
                              // least \texttt{LARGEINTS} integers are 
                              // available. So a large node can be stored in 
                              // the available amount of space.
       *nonmaximal,           // bit table: if node with headposition \(i\) is 
                              // not maximal, then \(nonmaximal[i]\) is set.
       *leafcounts;           // holds counts of the number of leafs in subtree
                              // indexed by headposition
  BOOL setatnewleaf;          // nil-reference is stored in new leaf
  SYMBOL *headstart,          // these references represent the right component
         *headend,            // of the head location \((\overline{u},v)\). 
                              // \emph{headstart} refers to the first character
                              // of \(v\), and \emph{headend} to the last
                              // character. In case, \(v=\varepsilon\),
                              // \(\emph{headend}=\emph{NULL}\).
         *tailptr;            // points to the tail

//#ifdef DEBUG
  char * (*showsymbolstree)(SYMBOL,Uchar *);
  Uchar *alphabet;
  Uint splitleafedge,
       splitinternaledge,
       artificial,
       insertleafcalls,
       largelinks,
       largelinkwork,
       largelinklinkwork,
       multiplications,
       nodecount,
       *maxset;
  void *generalcounter;

    public:
        STree(Uint t);
};

#endif
