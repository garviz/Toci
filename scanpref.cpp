/*
  Copyright (c) 2003 by Stefan Kurtz and The Institute for
  Genomic Research.  This is OSI Certified Open Source Software.
  Please see the file LICENSE for licensing information and
  the file ACKNOWLEDGEMENTS for names of contributors to the
  code base.
*/

#include <omp.h>
#include <likwid.h>
#include "streedef.h"
#include "streeacc.h"
#include "protodef.h"

//extern double tSPFNS,tSPS;

static Uint lcp(Uchar *start1,Uchar *end1,Uchar *start2,Uchar *end2)
{
  register Uchar *ptr1 = start1, 
                  *ptr2 = start2;
  while(ptr1 <= end1 && 
        ptr2 <= end2 &&
        *ptr1 == *ptr2)
  {
    ptr1++;
    ptr2++;
  }
  return (Uint) (ptr1-start1);
}

Uchar *scanprefixfromnodestree(Suffixtree *stree,Location *loc,Bref btptr,Uchar *left,Uchar *right,Uint rescanlength)
{
  Uint *nodeptr = NULL, *largeptr = NULL, leafindex, nodedepth, node, distance = 0, prefixlen, headposition, tmpnodedepth, edgelen, remainingtoskip;
  Uchar *lptr, *leftborder = (Uchar *) NULL, firstchar, edgechar = 0;
  //double sF, eF;
  //sF = omp_get_wtime();
  lptr = left;
  nodeptr = btptr;
  if(nodeptr == stree->branchtab)
  {
    nodedepth = 0;
    headposition = 0;
  } else
  {
    GETBOTH(nodedepth,headposition,nodeptr);
  }
  loc->nextnode.toleaf = false;
  loc->nextnode.address = nodeptr;
  loc->locstring.start = headposition;
  loc->locstring.length = nodedepth;
  loc->remain = 0;
  if(rescanlength <= nodedepth)
  {
    remainingtoskip = 0;
  } else
  {  
    remainingtoskip = rescanlength - nodedepth; 
  } 
  while(true) 
  { 
    if(lptr > right)   // check for empty word 
    { 
      return NULL; 
    }  
  {
    remainingtoskip = rescanlength - nodedepth;
  }
  while(true)
  {
    if(lptr > right)   // check for empty word
    {
      return NULL;
    }
    firstchar = *lptr;
    if(nodeptr == stree->branchtab)  // at the root
    {
      if((node = stree->rootchildren[(Uint) firstchar]) == UNDEFINEDREFERENCE)
      {
        return lptr;
      }
      if(ISLEAF(node))
      {
        leafindex = GETLEAFINDEX(node);
        loc->firstptr = stree->text + leafindex;
        if(remainingtoskip > 0)
        {
          prefixlen = remainingtoskip + lcp(lptr+remainingtoskip,right,loc->firstptr+remainingtoskip,stree->sentinel-1);
        } else
        {
          prefixlen = 1 + lcp(lptr+1,right,loc->firstptr+1,stree->sentinel-1);
        }
        loc->previousnode = stree->branchtab;
        loc->edgelen = stree->textlen - leafindex + 1;
        loc->remain = loc->edgelen - prefixlen;
        loc->nextnode.toleaf = true;
        loc->nextnode.address = stree->leaftab + leafindex;
        loc->locstring.start = leafindex;
        loc->locstring.length = prefixlen;
        if(prefixlen == (Uint) (right - lptr + 1))
        {
          return NULL;
        }
        return lptr + prefixlen;
      } 
      nodeptr = stree->branchtab + GETBRANCHINDEX(node);
      GETONLYHEADPOS(headposition,nodeptr);
      leftborder = stree->text + headposition;
    } else
    {
      node = GETCHILD(nodeptr);
      while(true)
      {
        if(NILPTR(node))
        {
          return lptr;
        }
        if(ISLEAF(node))
        {
          leafindex = GETLEAFINDEX(node);
          leftborder = stree->text + (nodedepth + leafindex);
          if(leftborder == stree->sentinel)
          {
            return lptr;
          }
          edgechar = *leftborder;
          if(edgechar > firstchar)
          {
            return lptr;
          }
          if(edgechar == firstchar)
          {
            if(remainingtoskip > 0)
            {
              prefixlen = remainingtoskip + lcp(lptr+remainingtoskip,right,leftborder+remainingtoskip,stree->sentinel-1);
             } else
            {
              prefixlen = 1 + lcp(lptr+1,right,leftborder+1,stree->sentinel-1);
            }
            loc->firstptr = leftborder;
            loc->previousnode = loc->nextnode.address;
            loc->edgelen = stree->textlen - (nodedepth + leafindex) + 1;
            loc->remain = loc->edgelen - prefixlen;
            loc->nextnode.toleaf = true;
            loc->nextnode.address = stree->leaftab + leafindex;
            loc->locstring.start = leafindex;
            loc->locstring.length = nodedepth + prefixlen;
            if(prefixlen == (Uint) (right - lptr + 1))
            {
              return NULL;
            }
            return lptr + prefixlen;
          }
          node = LEAFBROTHERVAL(stree->leaftab[leafindex]);
        } else
        {
          nodeptr = stree->branchtab + GETBRANCHINDEX(node);
          GETONLYHEADPOS(headposition,nodeptr);
          leftborder = stree->text + (nodedepth + headposition);
          edgechar = *leftborder;
          if (edgechar > firstchar)
          {
            return lptr;
          }
          if(edgechar == firstchar)
          {
            break;
          }
          node = GETBROTHER(nodeptr);
        }
      }
    }
    GETONLYDEPTH(tmpnodedepth,nodeptr);
    edgelen = tmpnodedepth - nodedepth;
    if(remainingtoskip > 0)
    {
      if(remainingtoskip >= edgelen)
      {
        prefixlen = edgelen;
        remainingtoskip -= prefixlen;
      } else
      {
        NOTSUPPOSEDTOBENULL(leftborder);
        prefixlen = remainingtoskip + lcp(lptr+remainingtoskip,right,leftborder+remainingtoskip,leftborder+edgelen-1);
        remainingtoskip = 0;
      }
    } else
    {
      NOTSUPPOSEDTOBENULL(leftborder);
      prefixlen = 1 + lcp(lptr+1,right,leftborder+1,leftborder+edgelen-1);
    }
    loc->nextnode.toleaf = false;
    loc->locstring.start = headposition;
    loc->locstring.length = nodedepth + prefixlen;
    if(prefixlen == edgelen)
    {
      lptr += edgelen;
      nodedepth += edgelen;
      loc->nextnode.address = nodeptr;
      loc->remain = 0;
    } else
    {
      loc->firstptr = leftborder;
      loc->previousnode = loc->nextnode.address;
      loc->nextnode.address = nodeptr;
      loc->edgelen = edgelen;
      loc->remain = loc->edgelen - prefixlen;
      if(prefixlen == (Uint) (right - lptr + 1))
      {
        return NULL;
      }
      return lptr + prefixlen;
    }
  }
}
}

Uchar *scanprefixstree(Suffixtree *stree,Location *outloc,
                                   Location *inloc,Uchar *left,
                                   Uchar *right,Uint rescanlength)
{
  Uint prefixlen, remainingtoskip;

  if(inloc->remain == 0)
  {
    return scanprefixfromnodestree(stree,outloc,inloc->nextnode.address,left,right,rescanlength);
  }
  if(rescanlength <= inloc->locstring.length)
  {
    remainingtoskip = 0;
  } else
  {
    remainingtoskip = rescanlength - inloc->locstring.length;
  }
  if(inloc->nextnode.toleaf)
  {
    
    if(remainingtoskip > 0)
    {
      prefixlen = remainingtoskip + lcp(left+remainingtoskip,right,inloc->firstptr+(inloc->edgelen-inloc->remain)+remainingtoskip,stree->sentinel-1);
    } else
    {
      prefixlen = lcp(left,right,inloc->firstptr+(inloc->edgelen-inloc->remain),stree->sentinel-1);
    }
    outloc->firstptr = inloc->firstptr;
    outloc->edgelen = inloc->edgelen;
    outloc->remain = inloc->remain - prefixlen;
    outloc->previousnode = inloc->previousnode;
    outloc->nextnode.toleaf = true;
    outloc->nextnode.address = inloc->nextnode.address;
    outloc->locstring.start = LEAFADDR2NUM(stree,inloc->nextnode.address);
    outloc->locstring.length = inloc->locstring.length + prefixlen;
    return left + prefixlen;
  }
  if(remainingtoskip > 0)
  {
    if(remainingtoskip >= inloc->remain)
    {
      prefixlen = inloc->remain;
    } else
    {
      prefixlen = remainingtoskip + lcp(left+remainingtoskip,right,inloc->firstptr+(inloc->edgelen-inloc->remain)+remainingtoskip,inloc->firstptr+inloc->edgelen-1);
    }
  } else
  {
    prefixlen = lcp(left,right,inloc->firstptr+(inloc->edgelen-inloc->remain),inloc->firstptr+inloc->edgelen-1);
  }
  if(prefixlen < inloc->remain)
  {
    outloc->firstptr = inloc->firstptr;
    outloc->edgelen = inloc->edgelen;
    outloc->remain = inloc->remain - prefixlen;
    outloc->previousnode = inloc->previousnode;
    outloc->nextnode.toleaf = false;
    outloc->nextnode.address = inloc->nextnode.address;
    outloc->locstring.start = inloc->locstring.start;
    outloc->locstring.length = inloc->locstring.length + prefixlen;
    return left + prefixlen;
  }
  return scanprefixfromnodestree(stree,outloc,inloc->nextnode.address,left+prefixlen,right,rescanlength);
}

/*@null@*/Uchar *findprefixpathfromnodestree(Suffixtree *stree,
                                              ArrayPathinfo *path,
                                              Location *loc,
                                              Bref btptr,
                                              Uchar *left,
                                              Uchar *right,
                                              Uint rescanlength)
{
  //fprintf(stderr,"%s Thread:%d ArrayPathinfo:%lu\n",__func__, omp_get_thread_num(),path);
  Uint *nodeptr = NULL, *largeptr = NULL, leafindex, nodedepth, 
       edgelen, node, distance = 0, prefixlen, headposition, 
       remainingtoskip, tmpnodedepth;
  Uchar *leftborder = (Uchar *) NULL, *lptr, firstchar, edgechar = 0;

  lptr = left;
  nodeptr = btptr;
  if(nodeptr == stree->branchtab)
  {
    nodedepth = 0;
    headposition = 0;
  } else
  {
    GETBOTH(nodedepth,headposition,nodeptr);
  }
  loc->nextnode.toleaf = false;
  loc->nextnode.address = nodeptr;
  loc->locstring.start = headposition;
  loc->locstring.length = nodedepth;
  loc->remain = 0;
  if(rescanlength <= nodedepth)
  {
    remainingtoskip = 0;
  } else
  {
    remainingtoskip = rescanlength - nodedepth;
  }
  while(true)
  {
    if(lptr > right)   // check for empty word
    {
      return NULL;
    }
    firstchar = *lptr;
    if(nodeptr == stree->branchtab)  // at the root
    {
      if((node = stree->rootchildren[(Uint) firstchar]) == UNDEFINEDREFERENCE)
      {
        return lptr;
      }
      if(ISLEAF(node))
      {
        leafindex = GETLEAFINDEX(node);
        loc->firstptr = stree->text + leafindex;
        if(remainingtoskip > 0)
        {
          prefixlen = remainingtoskip + lcp(lptr+remainingtoskip,right,loc->firstptr+remainingtoskip,stree->sentinel-1);
        } else
        {
          prefixlen = 1 + lcp(lptr+1,right,loc->firstptr+1,stree->sentinel-1);
        }
        loc->previousnode = stree->branchtab;
        loc->edgelen = stree->textlen - leafindex + 1;
        loc->remain = loc->edgelen - prefixlen;
        loc->nextnode.toleaf = true;
        loc->nextnode.address = stree->leaftab + leafindex;
        loc->locstring.start = leafindex;
        loc->locstring.length = prefixlen;
        if(prefixlen == (Uint) (right - lptr + 1))
        {
          return NULL;
        }
        return lptr + prefixlen;
      } 
      nodeptr = stree->branchtab + GETBRANCHINDEX(node);
      GETONLYHEADPOS(headposition,nodeptr);
      leftborder = stree->text + headposition;
    } else
    {
      node = GETCHILD(nodeptr);
      while(true)
      {
        if(NILPTR(node))
        {
          return lptr;
        }
        if(ISLEAF(node))
        {
          leafindex = GETLEAFINDEX(node);
          leftborder = stree->text + (nodedepth + leafindex);
          if(leftborder == stree->sentinel)
          {
            return lptr;
          }
          edgechar = *leftborder;
          if(edgechar > firstchar)
          {
            return lptr;
          }
          if(edgechar == firstchar)
          {
            if(remainingtoskip > 0)
            {
              prefixlen = remainingtoskip+lcp(lptr+remainingtoskip,right,leftborder+remainingtoskip,stree->sentinel-1);
            } else
            {
              prefixlen = 1 + lcp(lptr+1,right,leftborder+1,stree->sentinel-1);
            }
            loc->firstptr = leftborder;
            loc->previousnode = loc->nextnode.address;
            loc->edgelen = stree->textlen - (nodedepth + leafindex) + 1;
            loc->remain = loc->edgelen - prefixlen;
            loc->nextnode.toleaf = true;
            loc->nextnode.address = stree->leaftab + leafindex;
            loc->locstring.start = leafindex;
            loc->locstring.length = nodedepth + prefixlen;
            if(prefixlen == (Uint) (right - lptr + 1))
            {
              return NULL;
            }
            return lptr + prefixlen;
          }
          node = LEAFBROTHERVAL(stree->leaftab[leafindex]);
        } else
        {
          nodeptr = stree->branchtab + GETBRANCHINDEX(node);
          GETONLYHEADPOS(headposition,nodeptr);
          leftborder = stree->text + (nodedepth + headposition);
          edgechar = *leftborder;
          if (edgechar > firstchar)
          {
            return lptr;
          }
          if(edgechar == firstchar)
          {
            /*@innerbreak@*/ break;
          }
          node = GETBROTHER(nodeptr);
        }
      }
    }
    GETONLYDEPTH(tmpnodedepth,nodeptr);
    edgelen = tmpnodedepth - nodedepth;
    if(remainingtoskip > 0)
    {
      if(remainingtoskip >= edgelen)
      {
        prefixlen = edgelen;
        remainingtoskip -= prefixlen;
      } else
      {
        NOTSUPPOSEDTOBENULL(leftborder);
        prefixlen = remainingtoskip+lcp(lptr+remainingtoskip,right,leftborder+remainingtoskip,leftborder+edgelen-1);
        remainingtoskip = 0;
      }
    } else
    {
      NOTSUPPOSEDTOBENULL(leftborder);
      prefixlen = 1 + lcp(lptr+1,right,leftborder+1,leftborder+edgelen-1);
    }
    loc->nextnode.toleaf = false;
    loc->locstring.start = headposition;
    loc->locstring.length = nodedepth + prefixlen;
    if(prefixlen == edgelen)
    {
      lptr += edgelen;
      nodedepth += edgelen;
      loc->nextnode.address = nodeptr;
      loc->remain = 0;
    } else
    {
      loc->firstptr = leftborder;
      loc->previousnode = loc->nextnode.address;
      loc->nextnode.address = nodeptr;
      loc->edgelen = edgelen;
      loc->remain = loc->edgelen - prefixlen;
      if(prefixlen == (Uint) (right - lptr + 1))
      {
        return NULL;
      }
      return lptr + prefixlen;
    }
    CHECKARRAYSPACE(path,Pathinfo,128);
    path->spacePathinfo[path->nextfreePathinfo].ref = nodeptr;
    path->spacePathinfo[path->nextfreePathinfo].depth = tmpnodedepth;
    path->spacePathinfo[path->nextfreePathinfo].headposition = headposition;
    path->nextfreePathinfo++;
  }
}

/*@null@*/ Uchar *findprefixpathstree(Suffixtree *stree,
                                       ArrayPathinfo *path,
                                       Location *outloc,
                                       Location *inloc,
                                       Uchar *left,
                                       Uchar *right,
                                       Uint rescanlength)
{
  Uint prefixlen, remainingtoskip;

  if(inloc->remain == 0)
  {
    CHECKARRAYSPACE(path,Pathinfo,128);
    path->spacePathinfo[path->nextfreePathinfo].ref = inloc->nextnode.address;
    path->spacePathinfo[path->nextfreePathinfo].depth = inloc->locstring.length;
    path->spacePathinfo[path->nextfreePathinfo].headposition = inloc->locstring.start;
    path->nextfreePathinfo++;
    return findprefixpathfromnodestree(stree,path,outloc,inloc->nextnode.address,left,right,rescanlength);
  } 
  if(rescanlength <= inloc->locstring.length)
  {
    remainingtoskip = 0;
  } else
  {
    remainingtoskip = rescanlength - inloc->locstring.length;
  }
  if(inloc->nextnode.toleaf)
  {
    if(remainingtoskip > 0)
    {
      prefixlen = remainingtoskip+lcp(left+remainingtoskip,right,inloc->firstptr+(inloc->edgelen-inloc->remain)+remainingtoskip,stree->sentinel-1);
    } else
    {
      prefixlen = lcp(left,right,inloc->firstptr+(inloc->edgelen-inloc->remain),stree->sentinel-1);
    }
    outloc->firstptr = inloc->firstptr;
    outloc->edgelen = inloc->edgelen;
    outloc->remain = inloc->remain - prefixlen;
    outloc->previousnode = inloc->previousnode;
    outloc->nextnode.toleaf = true;
    outloc->nextnode.address = inloc->nextnode.address;
    outloc->locstring.start = LEAFADDR2NUM(stree,inloc->nextnode.address);
    outloc->locstring.length = inloc->locstring.length + prefixlen;
    return left + prefixlen;
  }
  if(remainingtoskip > 0)
  {
    if(remainingtoskip >= inloc->remain)
    {
      prefixlen = inloc->remain;
    } else
    {
      prefixlen = remainingtoskip+lcp(left+remainingtoskip,right,inloc->firstptr+(inloc->edgelen-inloc->remain)+remainingtoskip,inloc->firstptr+inloc->edgelen-1);
    }
  } else
  {
    prefixlen = lcp(left,right,inloc->firstptr+(inloc->edgelen-inloc->remain),inloc->firstptr+inloc->edgelen-1);
  }
  if(prefixlen < inloc->remain)
  {
    outloc->firstptr = inloc->firstptr;
    outloc->edgelen = inloc->edgelen;
    outloc->remain = inloc->remain - prefixlen;
    outloc->previousnode = inloc->previousnode;
    outloc->nextnode.toleaf = false;
    outloc->nextnode.address = inloc->nextnode.address;
    outloc->locstring.start = inloc->locstring.start;
    outloc->locstring.length = inloc->locstring.length + prefixlen;
    return left + prefixlen;
  }
  CHECKARRAYSPACE(path,Pathinfo,128);
  path->spacePathinfo[path->nextfreePathinfo].ref = inloc->nextnode.address;
  path->spacePathinfo[path->nextfreePathinfo].depth = inloc->locstring.length + prefixlen;
  path->spacePathinfo[path->nextfreePathinfo].headposition = inloc->locstring.start;
  path->nextfreePathinfo++;
  return findprefixpathfromnodestree(stree,path,outloc,inloc->nextnode.address,left+prefixlen,right,rescanlength);
}
