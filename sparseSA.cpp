#include <math.h>
#include <pthread.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <assert.h>
#include <string.h>
#include <parallel/algorithm>
#include <papi.h>
#include <omp.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>

#include "sparseSA.hpp"

#define LCPCHILD(cLCP,cS,cE) \
    long long childcE = CHILD[cE];\
    if (cS < childcE && childcE <= cE)\
        cLCP = LCP[childcE];\
    else\
        cLCP = LCP[CHILD[cS]];
//#define PRINT fprintf(stderr,"%d %lld,%lld,%lld\n",__LINE__,cur.start,cur.end,cur.depth);cin.get();
#define PRINT
using namespace std;

// LS suffix sorter (integer alphabet). 
extern "C" { void suffixsort(int *x, int *p, int n, int k, int l); };

pthread_mutex_t cout_mutex = PTHREAD_MUTEX_INITIALIZER;

long memCount = 0;

sparseSA::sparseSA(string &R_, vector<string> &descr_, vector<long long> &startpos_, bool __4column, long long K_, bool suflink_, bool child_, int sparseMult_, bool printSubstring_) : 
  descr(descr_), startpos(startpos_) {
  _4column = __4column;
  hasChild = child_;
  hasSufLink = suflink_;
  sparseMult = sparseMult_;
  printSubstring = printSubstring_;
  string line;
  //long long sa=0, isa=0, child=0, lcp=0, i=0;

  K = K_;

  /*-----------------------------------------------------------------------------
   *  Load Reference genome file in R string.
   *-----------------------------------------------------------------------------*/
  string file(R_+".ois");
  ifstream ifs(file);
  if (ifs.is_open())
      getline(ifs, Reference);
  else
      cerr << R_ << " ERROR reading file\n";
  ifs.close();
  //transform(Reference.begin(), Reference.end(), Reference.begin(), ::tolower);
  N = Reference.length();
  cout << "# R.length=" << N;
  /*-----------------------------------------------------------------------------
   *  Load Suffix array from file
   *-----------------------------------------------------------------------------*/
  unsigned long size;
  file.assign(R_+".SA");
  ifs.open(file.c_str(), ios::binary);
  if (ifs.is_open()) {
      ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
      SA.resize(size);
      ifs.read(reinterpret_cast<char*>(&SA[0]),size*sizeof(SA[0]));
  }
  else
      cerr << file << " ERROR reading file\n";
  ifs.close();

  /*-----------------------------------------------------------------------------
   *  Load Inversed Suffix array from file
   *-----------------------------------------------------------------------------*/
  //vector<unsigned long> ISA;
  file.assign(R_+".ISA");
  ifs.open(file.c_str(), ios::binary);
  if (ifs.is_open()) {
      ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
//      /*ifs.seekg(10,ifs.cur);
//      cout << ifs.tellg() << endl;*/
      ISA.resize(size);
      ifs.read(reinterpret_cast<char*>(&ISA[0]),size*sizeof(ISA[0]));
  }
  else
      cerr << file << " ERROR reading file\n";
  ifs.close();

  /*-----------------------------------------------------------------------------
   *  Load Child table from file
   *-----------------------------------------------------------------------------*/
  //vector<unsigned long> CHILD;
  file.assign(R_+".CHILD");
  ifs.open(file.c_str(), ios::binary);
  if (ifs.is_open()) {
      ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
      CHILD.resize(size);
      ifs.read(reinterpret_cast<char*>(&CHILD[0]),size*sizeof(CHILD[0]));
  }
  else
      cerr << file << " ERROR reading file\n";
  ifs.close();
  /*-----------------------------------------------------------------------------
   *  Load LCP table from file
   *-----------------------------------------------------------------------------*/
  file.assign(R_+".LCP");
  ifs.open(file.c_str());
  if (ifs.is_open()) {
      getline(ifs,line);
      size = strtol(line.c_str(),NULL,0);
      LCP.resize(size);
      long lcp;
      for (long long i=0; i<(long long)size; ++i) {
          getline(ifs,line);
          lcp = strtol(line.c_str(),NULL,0);
          LCP.set(i, lcp); 
      }
  }
  else
      cerr << file << " ERROR reading file\n";
  ifs.close();

  /*-----------------------------------------------------------------------------
   *  Load offset table from file
   *-----------------------------------------------------------------------------*/
  file.assign(R_+".off");
  ifs.open(file.c_str());
  string dat;
  long long l, r;
  while (ifs >> dat >> l >> r) {
      interval_t t(l,r,8);
      offset[dat]=t;
  }
  ifs.close();
  //for (long i=0;i<SA.size();i++)
  //    fprintf(stderr,"%lld,%lld,%lld,%lld\n",SA[i],ISA[i],CHILD[i],LCP[i]);
  // Adjust to "sampled" size. 
  N = SA.size();
  logN = (long)ceil(log(N/K) / log(2.0));

  NKm1 = N;
}

// Binary search for left boundry of interval.
long sparseSA::bsearch_left(char c, long long i, long long s, long long e) {
  if(c == Reference[SA[s]+i]) return s;
  long l = s, r = e;
  while (r - l > 1) {
    long m = (l+r) / 2;
    if (c <= Reference[SA[m] + i]) r = m;
    else l = m;
  }
  return r;
}

// Binary search for right boundry of interval.
long sparseSA::bsearch_right(char c, long long i, long long s, long long e) {
  if(c == Reference[SA[e]+i]) return e;
  long l = s, r = e;
  while (r - l > 1) {
    long m = (l+r) / 2;
    if (c < Reference[SA[m] + i]) r = m;
    else l = m;
  }
  return l;
}


// Simple top down traversal of a suffix array.
bool sparseSA::top_down(char c, long long i, long long &start, long long &end) {
  if(c < Reference[SA[start]+i]) return false;
  if(c > Reference[SA[end]+i]) return false;
  long l = bsearch_left(c, i, start, end);
  long l2 = bsearch_right(c, i, start, end);
  start = l; end = l2;
  return l <= l2;
}

// Top down traversal of the suffix array to match a pattern.  NOTE:
// NO childtab as in the enhanced suffix array (ESA).
bool sparseSA::search(string &P, long long &start, long long &end) {
  start = 0; end = N - 1;
  long i = 0;
  while(i < (long)P.length()) {
    if(top_down(P[i], i, start, end) == false) {
      return false;
    }
    i++;
  }
  return true;
}


// Traverse pattern P starting from a given prefix and interval
// until mismatch or min_len characters reached.
void sparseSA::traverse(string &P, long prefix, interval_t &cur, long long min_len) {
  long long lengthP = P.length();
  if(cur.depth >= min_len) return;

  while(prefix+cur.depth < lengthP) {
    long long start = cur.start; long long end = cur.end;
    // If we reach a mismatch, stop.
    if(top_down_faster(P[prefix+cur.depth], cur.depth, start, end) == false) return;

    // Advance to next interval.
    cur.depth += 1; cur.start = start; cur.end = end;
    
    // If we reach min_len, stop.
    if(cur.depth == min_len) return;
  }
}

// Traverse pattern P starting from a given prefix and interval
// until mismatch or min_len characters reached.
// Uses the child table for faster traversal
void sparseSA::traverse_faster(const string &P,const long long prefix, interval_t &cur, long long min_len){
    if(cur.depth >= min_len) return;
    long long c = prefix + cur.depth;
    register long long lengthP = P.length();
    bool intervalFound = c < lengthP;
    long long curLCP;//check if this is correct for root interval (unlikely case)
    register long long sa_cS;
    if(cur.start < CHILD[cur.end] && CHILD[cur.end] <= cur.end)
        curLCP = LCP[CHILD[cur.end]];
    else
        curLCP = LCP[CHILD[cur.start]];
    if(intervalFound && cur.size() > 1 && curLCP == cur.depth)
        intervalFound = top_down_child(P[c], cur);
    else if(intervalFound)
        intervalFound = P[c] == Reference[SA[cur.start]+cur.depth]; 
  PRINT
    bool mismatchFound = false;
    while(intervalFound && !mismatchFound && c < lengthP && cur.depth < min_len){
        c++;
        cur.depth++;
        sa_cS = SA[cur.start];
        if(cur.start != cur.end){
            long long childLCP;
            //calculate LCP of child node, which is now cur. the LCP value
            //of the parent is currently c - prefix
            if(cur.start < CHILD[cur.end] && CHILD[cur.end] <= cur.end)
                childLCP = LCP[CHILD[cur.end]];
            else
                childLCP = LCP[CHILD[cur.start]];
            long long minimum = min(childLCP,min_len);
            //match along branch
            while(!mismatchFound && c < lengthP && cur.depth < minimum){
                mismatchFound = Reference[sa_cS+cur.depth] != P[c++];
                cur.depth += !mismatchFound;
            }
            intervalFound = c < lengthP && !mismatchFound && cur.depth < min_len && top_down_child(P[c], cur);
        }
        else{
            register long long lengthR = Reference.length();
            while(!mismatchFound && c < lengthP && cur.depth < min_len){
                mismatchFound = sa_cS+cur.depth >= lengthR || Reference[sa_cS+cur.depth] != P[c++];
                cur.depth += !mismatchFound;
            }
        }
    }
}

//finds the child interval of cur that starts with character c
//updates left and right bounds of cur to child interval if found, or returns
//cur if not found (also returns true/false if found or not)
bool sparseSA::top_down_child(char c, interval_t &cur){
    long long left = cur.start;
    long long right = CHILD[cur.end];
    PRINT
    if(cur.start >= right || right > cur.end)
        right = CHILD[cur.start];
    //now left and right point to first child
    if(Reference[SA[cur.start]+cur.depth] == c){
        cur.end = right-1;
        PRINT
        return true;
    }
    left = right;
    //while has next L-index
    while(CHILD[right] > right && LCP[right] == LCP[CHILD[right]]){
        right = CHILD[right];
        if(Reference[SA[left]+cur.depth] == c){
            cur.start = left; cur.end = right - 1;
            PRINT
            return true;
        }
        left = right;
    }
    //last interval
    if(Reference[SA[left]+cur.depth] == c){
            cur.start = left;
            PRINT
            return true;
    }
    PRINT
    return false;
}

// Given SA interval apply binary search to match character c at
// position i in the search string. Adapted from the C++ source code
// for the wordSA implementation from the following paper: Ferragina
// and Fischer. Suffix Arrays on Words. CPM 2007.
bool sparseSA::top_down_faster(char c, long long i, long long &start, long long &end) {
  long l, r, m, r2=end, l2=start, vgl;
  bool found = false;
  long cmp_with_first = (long)c - (long)Reference[SA[start]+i];
  long cmp_with_last = (long)c - (long)Reference[SA[end]+i];
  if(cmp_with_first < 0) { 
    l = start+1; l2 = start; // pattern doesn't occur!  
  }
  else if(cmp_with_last > 0) { 
    l = end+1; l2 = end; 
    // pattern doesn't occur!  
  }
  else {
    // search for left border:
    l = start; r = end;
    if (cmp_with_first == 0) {
      found = true; r2 = r;
    }
    else {
      while (r - l > 1) {
	m = (l+r) / 2;
	vgl = (long)c - (long)Reference[SA[m] + i];
	if (vgl <= 0) {
	  if (!found && vgl == 0) {
	    found = true;
	    l2 = m; r2 = r; // search interval for right border
	  }
	  r = m;
	}
	else l = m;
      }
      l = r;
    }
    // search for right border (in the range [l2:r2])
    if (!found) { 
      l2 = l - 1; // pattern not found => right border to the left of 'l' 
    } 
    if (cmp_with_last == 0) { 
      l2 = end; // right border is the end of the array 
    }
    else {
      while (r2 - l2 > 1) {
	m = (l2 + r2) / 2;
	vgl = (long)c - (long)Reference[SA[m] + i];
	if (vgl < 0) r2 = m;
	else l2 = m;
      }
    }
  }
  start = l;
  end = l2;
  return l <= l2;
}

// Suffix link simulation using ISA/LCP heuristic.
bool sparseSA::suffixlink(interval_t &m) {
  m.depth -= K;
  if( m.depth <= 0) return false;
  m.start = ISA[SA[m.start] / K + 1];  
  m.end = ISA[SA[m.end] / K + 1]; 
  return expand_link(m);
}

// For a given offset in the prefix k, find all MEMs.
void sparseSA::findMEM(long long k, string &P, vector<match_t> &matches, long long min_len, bool print) {
  if(k < 0 || k >= K) { cerr << "Invalid k." << endl; return; }
  // Offset all intervals at different start points.
  long long prefix = k;
  interval_t mli(0,N/K-1,0); // min length interval
  interval_t xmi(0,N/K-1,0); // max match interval
  
  // Right-most match used to terminate search.
  long long min_lenK = min_len - (sparseMult*K-1);

  while( prefix <= (long long)P.length() - (K-k)) {
#ifndef NDEBUG
//      interval_t mliCopy(mli.start,mli.end,mli.depth);
//      traverse(P, prefix, mliCopy, min_lenK);    // Traverse until minimum length matched.
#endif
    if(hasChild)
        traverse_faster(P, prefix, mli, min_lenK);    // Traverse until minimum length matched.
    else
        traverse(P, prefix, mli, min_lenK);    // Traverse until minimum length matched.
#ifndef NDEBUG
//      assert(mli.start == mliCopy.start);
//      assert(mli.end == mliCopy.end);
//      assert(mli.depth == mliCopy.depth);
#endif
    if(mli.depth > xmi.depth) xmi = mli;
    if(mli.depth <= 1) { mli.reset(N/K-1); xmi.reset(N/K-1); prefix+=sparseMult*K; continue; }

    if(mli.depth >= min_lenK) {
#ifndef NDEBUG
//      interval_t xmiCopy(xmi.start,xmi.end,xmi.depth);
//      traverse(P, prefix, xmiCopy, P.length());    // Traverse until mismatch.
#endif
      if(hasChild)
        traverse_faster(P, prefix, xmi, P.length()); // Traverse until mismatch.
      else
        traverse(P, prefix, xmi, P.length()); // Traverse until mismatch.
#ifndef NDEBUG
//      assert(xmi.start == xmiCopy.start);
//      assert(xmi.end == xmiCopy.end);
//      assert(xmi.depth == xmiCopy.depth);
#endif
      collectMEMs(P, prefix, mli, xmi, matches, min_len, print); // Using LCP info to find MEM length.
      // When using ISA/LCP trick, depth = depth - K. prefix += K. 
      prefix+=sparseMult*K;	
      if( !hasSufLink ){ mli.reset(N/K-1); xmi.reset(N/K-1); continue; }
      else{
          long long i = 0;
          bool succes  = true;
          while(i < sparseMult && (succes = suffixlink(mli))){
              suffixlink(xmi);
              i++;
          }
          if(!succes){
              mli.reset(N/K-1); xmi.reset(N/K-1); continue;
          }
      }
    }
    else {
      // When using ISA/LCP trick, depth = depth - K. prefix += K. 
      prefix+=sparseMult*K;	
      if( !hasSufLink) { mli.reset(N/K-1); xmi.reset(N/K-1); continue; }
      else{
          long long i = 0;
          bool succes  = true;
          while(i < sparseMult && (succes = suffixlink(mli))){
              i++;
          }
          if(!succes){
              mli.reset(N/K-1); xmi.reset(N/K-1); continue;
          }
          xmi = mli;
      }
    }
  }
  if(print) print_match(match_t(), matches);   // Clear buffered matches.
}


// Use LCP information to locate right maximal matches. Test each for
// left maximality.
void sparseSA::collectMEMs(string &P, long long prefix, interval_t mli, interval_t xmi, vector<match_t> &matches, long long min_len, bool print) {
  // All of the suffixes in xmi's interval are right maximal.
  for(long long i = xmi.start; i <= xmi.end; i++) find_Lmaximal(P, prefix, SA[i], xmi.depth, matches, min_len, print);

  if(mli.start == xmi.start && mli.end == xmi.end) return;

  while(xmi.depth >= mli.depth) {
    // Attempt to "unmatch" xmi using LCP information.
    if(xmi.end+1 < N) xmi.depth = max(LCP[xmi.start], LCP[xmi.end+1]);
    else xmi.depth = LCP[xmi.start];

    // If unmatched XMI is > matched depth from mli, then examine rmems.
    if(xmi.depth >= mli.depth) {
      // Scan RMEMs to the left, check their left maximality..
      while(LCP[xmi.start] >= xmi.depth) { 
	xmi.start--; 
	find_Lmaximal(P, prefix, SA[xmi.start], xmi.depth, matches, min_len, print);
      }
      // Find RMEMs to the right, check their left maximality.
      while((long long) (xmi.end+1) < N/K && (long long) LCP[xmi.end+1] >= xmi.depth) { 
	xmi.end++;  
	find_Lmaximal(P, prefix, SA[xmi.end], xmi.depth, matches, min_len, print);
      }
    }
  }
}


// Finds left maximal matches given a right maximal match at position i.
void sparseSA::find_Lmaximal(string &P, long long prefix, long long i, long long len, vector<match_t> &matches, long long min_len, bool print) {
  // Advance to the left up to K steps.
  for(long long k = 0; k < sparseMult*K; k++) {
    // If we reach the end and the match is long enough, print.
    if(prefix == 0 || i == 0) {
      if(len >= min_len) {
	if(print) print_match(match_t(i, prefix, len), matches);
	else matches.push_back(match_t(i, prefix, len));
      }
      return; // Reached mismatch, done.
    }
    else if(P[prefix-1] != Reference[i-1]){
      // If we reached a mismatch, print the match if it is long enough.
      if(len >= min_len) {
	if(print) print_match(match_t(i, prefix, len), matches);
	else matches.push_back(match_t(i, prefix, len));
      }
      return; // Reached mismatch, done.
    }
    prefix--; i--; len++; // Continue matching.
  }
}


// Print results in format used by MUMmer v3.  Prints results
// 1-indexed, instead of 0-indexed.
void sparseSA::print_match(match_t m) {
  memCount++;
  if(_4column == false) { 
    printf("%8ld  %8ld  %8ld\n", m.ref + 1, m.query + 1, m.len);
  }
  else {
    long refseq=0, refpos=0;
    from_set(m.ref, refseq, refpos); // from_set is slow!!!
    // printf works faster than count... why? I don't know!!
    printf("  %s", descr[refseq].c_str());
    for(long j = 0; j < maxdescrlen - (long)descr[refseq].length() + 1; j++) putchar(' '); 
    printf(" %8ld  %8ld  %8ld\n", refpos + 1L, m.query + 1L, m.len);
  }
//  if(printSubstring){
//      if(m.len > 53) printf("%s . . .\n", S.substr(m.ref, 53).c_str());
//      else printf("%s\n", S.substr(m.ref, m.len).c_str());
//  }
}

// This version of print match places m_new in a buffer. The buffer is
// flushed if m_new.len <= 0 or it reaches 1000 entries.  Buffering
// limits the number of locks on cout.
void sparseSA::print_match(match_t m_new, vector<match_t> &buf) {
  if(m_new.len > 0)  buf.push_back(m_new);
  if(buf.size() > 1000 || m_new.len <= 0) {
    pthread_mutex_lock(&cout_mutex);
    for(long i = 0; i < (long)buf.size(); i++) print_match(buf[i]);
    pthread_mutex_unlock(&cout_mutex);
    buf.clear();
  }
}

void sparseSA::print_match(string meta, vector<match_t> &buf, bool rc) {
  pthread_mutex_lock(&cout_mutex);
  if(!rc) printf("> %s\n", meta.c_str());
  else printf("> %s Reverse\n", meta.c_str());
  for(long i = 0; i < (long)buf.size(); i++) print_match(buf[i]);
  pthread_mutex_unlock(&cout_mutex);
  buf.clear();
}

// Finds maximal almost-unique matches (MAMs) These can repeat in the
// given query pattern P, but occur uniquely in the indexed reference S.
void sparseSA::findMAM(string &P, int chunk, int chunks, vector<match_t> &matches, long long min_len, long long& currentCount, bool print) {
  double start,finish;
  start = omp_get_wtime();
  memCount = 0;
  inter t;
  long lborder = 0, rborder = N-1;
  interval_t cur(lborder, rborder, 0);
  unordered_map<string,interval_t>::const_iterator got;
  long prefix = P.length()/chunks*chunk;
  const long end = (long) (P.length()/chunks*(chunk+1));
  got = offset.find(P.substr(prefix,8));
  if (got == offset.end()) {
      cur.depth = 0; cur.start = 0; cur.end = N;
  } else
      cur = got->second;
  __builtin_prefetch(LCP.vec.data()); 
  while(prefix < end) {
      PRINT
    traverse_faster(P, prefix, cur, end);
    if (cur.depth <= 1) { 
        /*got = offset.find(P.substr(prefix,8));
        if (got == offset.end()) {*/
            cur.depth = 0; cur.start = lborder; cur.end = rborder;
        /*} else
            cur = got->second;*/
        prefix++;
        continue;
    }
    if(cur.size() == 1 && cur.depth >= min_len) {//unique match 
  PRINT
      if(is_leftmaximal(P, prefix, SA[cur.start])) {
  PRINT
	    match_t m; m.ref = SA[cur.start]; m.query = prefix; m.len = cur.depth;
	    if(print) print_match(m);
	    else  matches.push_back(m); 
      }
    }
    do {
      cur.depth = cur.depth-1;
      cur.start = ISA[SA[cur.start] + 1];  
      cur.end = ISA[SA[cur.end] + 1]; 
      prefix++;
  PRINT
      if( cur.depth == 0 || expand_link(cur) == false ) { cur.depth = 0; cur.start = lborder; cur.end = rborder;PRINT break; }
    } while(cur.depth > 0 && cur.size() == 1);
  }
  currentCount = memCount;
  finish = omp_get_wtime();
  fprintf(stdout,"# Search=%f\n", (double) (finish-start));
}

// Returns true if the position p1 in the query pattern and p2 in the
// reference is left maximal.
bool sparseSA::is_leftmaximal(string &P, long p1, long p2) {
  if(p1 == 0 || p2 == 0) return true;
  else return P[p1-1] != Reference[p2-1];
}


struct by_ref { bool operator() (const match_t &a, const match_t &b) const { if(a.ref == b.ref) return a.len > b.len; else return a.ref < b.ref; }  };

// Maximal Unique Match (MUM) 
void sparseSA::MUM(string &P, vector<match_t> &unique, long long min_len, long long& currentCount, bool print) {
  // Find unique MEMs.
  vector<match_t> matches;
  MAM(P, 0, 1, matches, min_len, currentCount, false);
  memCount=0;

  // Adapted from Stephan Kurtz's code in cleanMUMcand.c in MUMMer v3.20. 
  long currentright, dbright = 0;
  bool ignorecurrent, ignoreprevious = false;
  __gnu_parallel::sort(matches.begin(), matches.end(), by_ref());
  for(long i = 0; i < (long)matches.size(); i++) {
    ignorecurrent = false;
    currentright = matches[i].ref + matches[i].len - 1;
    if(dbright > currentright) 
      ignorecurrent = true;
    else {
      if(dbright == currentright) {
	ignorecurrent = true;
	if(!ignoreprevious && matches[i-1].ref == matches[i].ref) 
	  ignoreprevious = true;
      }
      else {
	dbright = currentright;
      }
    }
    if(i > 0 && !ignoreprevious) {
      if(print)	print_match(matches[i-1]);
      else unique.push_back(matches[i-1]);
    }
    ignoreprevious = ignorecurrent;
  }
  if(!ignoreprevious) {
    if(print) print_match(matches[matches.size()-1]);
    else unique.push_back(matches[matches.size()-1]);
  }
  currentCount = memCount;
}

struct thread_data {
  vector<long> Kvalues; // Values of K this thread should process.
  sparseSA *sa; // Suffix array + aux informaton
  int min_len; // Minimum length of match.
  string *P; // Query string.
};

void sparseSA::MUMParallel(string &P, int chunks, vector<match_t> &unique, long long min_len, long long& currentCount, bool print) {
  vector<match_t> matches_p;
  vector<match_t> matches;
  double start1, finish1;
  _mm_prefetch(LCP.vec.data(), _MM_HINT_NTA);
#pragma omp parallel default(none) shared(P, min_len, chunks, stderr, cout, cerr, matches) private(matches_p)
  { 
#pragma omp for schedule(static,1) nowait 
  for (int i=0; i<chunks; ++i)
  {
    long long memCount = 0;
    findMAM(P, i, chunks, matches_p, min_len, memCount, false);
  }
#pragma omp critical
  {
  matches.insert(matches.end(),matches_p.begin(),matches_p.end());
  }
  }
  long currentright, dbright = 0;
  bool ignorecurrent, ignoreprevious = false;
  start1 = omp_get_wtime();
  __gnu_parallel::sort(matches.begin(), matches.end(), by_ref());
  if (matches.size()>0) {
  for(long long i = 0; i < (long long)matches.size(); i++) { 
    ignorecurrent = false;
    currentright = matches[i].ref + matches[i].len - 1;
    if(dbright > currentright) 
      ignorecurrent = true;
    else { 
      if(dbright == currentright) {
	ignorecurrent = true;
	if(!ignoreprevious && matches[i-1].ref == matches[i].ref) 
	  ignoreprevious = true;
      } 
      else {
	dbright = currentright;
      } 
    }
    if(i  > 0 && !ignoreprevious) {
      if(print)	print_match(matches[i-1]);
      else unique.push_back(matches[i-1]);
    }
    ignoreprevious = ignorecurrent;
  }
  if(!ignoreprevious) {
    if(print) print_match(matches[matches.size()-1]);
    else unique.push_back(matches[matches.size()-1]);
  }
  }
  finish1 = omp_get_wtime();
  currentCount = unique.size();
  //fprintf(stderr,",OMP=%lf,Merge=%lf,LCPAcc=%ld,",(double) (finish-start), (double) (finish1-start1), LCP.access);
  fprintf(stdout,"# Merge=%lf,Matches=%lld,Thrd=%d\n", (double) (finish1-start1), currentCount, chunks);
}

void *MEMthread(void *arg) {
  thread_data *data = (thread_data*)arg;
  vector<long> &K = data->Kvalues;
  sparseSA *sa = data->sa;

  // Find MEMs for all assigned offsets to this thread.

  vector<match_t> matches; // place-holder
  matches.reserve(2000);   // TODO: Use this as a buffer again!!!!!! 

  for(long k = 0; k < (long)K.size(); k++) {  sa->findMEM(K[k], *(data->P), matches, data->min_len, true); }

  pthread_exit(NULL);
}

void sparseSA::MEM(string &P, vector<match_t> &matches, long long min_len, bool print, long long& currentCount, int num_threads) {
  if(min_len < K) return;
  memCount=0;
  if(num_threads == 1) {
    for(long long k = 0; k < K; k++) { findMEM(k, P, matches, min_len, print); }
    currentCount += memCount;
  }
  else if(num_threads > 1) {
    vector<pthread_t> thread_ids(num_threads);  
    vector<thread_data> data(num_threads);

    // Make sure all num_threads are joinable.
    pthread_attr_t attr;  pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Distribute K-values evenly between num_threads.
    int t = 0;
    for(long long k = 0; k < K; k++) {
      data[t].Kvalues.push_back(k);
      t++;
      if(t == num_threads) t = 0;
    }
    // Initialize additional thread data.
    for(int i = 0; i < num_threads; i++) { data[i].sa = this; data[i].min_len = min_len;  data[i].P = &P; }
    // Create joinable threads to find MEMs.
    for(int i = 0; i < num_threads; i++) pthread_create(&thread_ids[i], &attr, MEMthread, (void *)&data[i]);
    // Wait for all threads to terminate.
    for(int i = 0; i < num_threads; i++) pthread_join(thread_ids[i], NULL);    
  }
}

