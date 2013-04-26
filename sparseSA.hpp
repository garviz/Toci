#ifndef __sparseSA_hpp__
#define __sparseSA_hpp__

#include <vector>
#include <string>
#include <iostream>
#include <parallel/algorithm>
#include <limits>
#include <omp.h>
#include <unordered_map>
#include <algorithm>

using namespace std;

// Stores the LCP array in an long char (0-255).  Values larger
// than or equal to 255 are stored in a sorted array.
// Simulates a vector<int> LCP;
struct vec_uchar {
  struct item_t{
    item_t(size_t i, long long v) { idx = i; val = v; }
    size_t idx; long long val;
    bool operator < (item_t t) const { return idx < t.idx;  }
  };
  vector<unsigned char> vec;  // LCP values from 0-65534
  vector<item_t> M;
  void resize(size_t N) { vec.resize(N); }
  // Vector X[i] notation to get LCP values.
 int operator[] (size_t idx) {
    if(vec[idx] == numeric_limits<unsigned char>::max()) 
      return lower_bound(M.begin(), M.end(), item_t(idx,0))->val;
    else
      return vec[idx]; 
  }
  // Actually set LCP values, distinguishes large and small LCP
  // values.
  void set(size_t idx, int v) {
    if (v >= numeric_limits<unsigned char>::max()) {
      vec.at(idx) = numeric_limits<unsigned char>::max();
      M.push_back(item_t(idx, v));
    }
    else { 
        vec.at(idx) = (unsigned char)v; 
    }
  }
  // Once all the values are set, call init. This will assure the
  // values >= 255 are sorted by index for fast retrieval.
  void init() {
      __gnu_parallel::sort(M.begin(), M.end()); 
  }
};

// Match find by findMEM. 
struct match_t {
  match_t() { ref = 0; query = 0, len = 0; }
  match_t(long r, long q, long l) { ref = r; query = q; len = l; }
  long ref; // position in reference sequence
  long query; // position in query
  long len; // length of match
};

// depth : [start...end] 
struct interval_t {
  interval_t() { start = 1; end = 0; depth = -1; lb = start; rb = end; }
  interval_t(long long s, long long e, long long d, long long l, long long r) { start = s; end = e; depth = d; lb = l; rb = r; }
  void reset(long e) { start = 0; end = e; depth = 0; }
  long long depth, start, end, lb, rb;
  long size() { return end - start + 1; }
  bool inside() { if (lb <= start && start <= rb && lb <= end && end <= rb) return true; else return false; }
};

struct inter {
    long long l = -1, r = -2;
    inter() : l(-1), r(-2) {}
    inter(long long lv, long long rv) : l(lv), r(rv) {}
    void update(long long val) { 
        if (val<l) l=val;
        else if (val>r) r=val;
    }
    long size() {return r-l+1;}
}; 

struct sparseSA {
  vector<string> &descr; // Descriptions of concatenated sequences.
  vector<long long> &startpos; // Lengths of concatenated sequences.
  long maxdescrlen; // Maximum length of the sequence description, used for formatting.
  bool _4column; // Use 4 column output format.

  long long N; //!< Length of the sequence.
  long long logN; // ceil(log(N)) 
  long long NKm1; // N/K - 1
  string Reference; //!< Reference to sequence data.
  vector<long long> SA;  // Suffix array.
  vector<long long> ISA;  // Inverse suffix array.
  vec_uchar LCP; // Simulates a vector<int> LCP.
  vector<long long> CHILD; //child table
  unordered_map<string,inter> offset;

  long long K; // suffix sampling, K = 1 every suffix, K = 2 every other suffix, K = 3, every 3rd sffix
  bool hasChild;
  bool hasSufLink;
  long long sparseMult;
  bool printSubstring;

  // Maps a hit in the concatenated sequence set to a position in that sequence.
  void from_set(long hit, long &seq, long &seqpos) {
    // Use binary search to locate index of sequence and position
    // within sequence.
    vector<long long>::iterator it = upper_bound(startpos.begin(), startpos.end(), hit);   // SG: should use vector<long>::const_iterator
    seq = distance(startpos.begin(), it) - 1;
    it--;
    seqpos = hit - *it;
  } 

  // Constructor builds sparse suffix array. 
  sparseSA(string &S_, vector<string> &descr_, vector<long long> &startpos_, bool __4column, long long K_, bool suflink_, bool child_, int sparseMult_, bool printSubstring_);

  // Radix sort required to construct transformed text for sparse SA construction.
  void radixStep(int *t_new, int *SA, long &bucketNr, long *BucketBegin, long l, long r, long h);

  // Prints match to cout.
  void print_match(match_t m);
  void print_match(match_t m, vector<match_t> &buf); // buffered version
  void print_match(string meta, vector<match_t> &buf, bool rc); // buffered version

  // Binary search for left boundry of interval.
  inline long bsearch_left(char c, long long i, long long s, long long e);
  // Binary search for right boundry of interval.
  inline long bsearch_right(char c, long long i, long long s, long long e);

  // Simple suffix array search.
  inline bool search(string &P, long long &start, long long &end);

  // Simple top down traversal of a suffix array.
  inline bool top_down(char c, long long i, long long &start, long long &end);
  inline bool top_down_faster(char c, long long i, long long &start, long long &end);
  inline bool top_down_child(char c, interval_t &cur);

  // Traverse pattern P starting from a given prefix and interval
  // until mismatch or min_len characters reached.
  inline void traverse(string &P, long prefix, interval_t &cur, long long min_len);
  inline void traverse_faster(const string &P,const long long prefix, interval_t &cur, long long min_len);

  // Simulate a suffix link.
  inline bool suffixlink(interval_t &m);

  // Expand ISA/LCP interval. Used to simulate suffix links.
  inline bool expand_link(interval_t &link) {
    long long thresh = 2 * link.depth * logN, exp = 0; // Threshold link expansion.
    long long start = link.start;
    long long end = link.end;
    while(LCP[start] >= (long) link.depth) { 
      exp++; 
      if(exp >= thresh) {
          return false;
      }
      start--; 
    }
    while(end < NKm1 && LCP[end+1] >= link.depth) { 
      exp++; 
      if(exp >= thresh)  {
          return false;
      }
      end++; 
    }
    link.start = start; link.end = end;
    //fprintf(stdout,"%d| el s:%ld,e:%ld,d:%ld\n",__LINE__,link.start,link.end,link.depth);
    return true;
  }

  // Given a position i in S, finds a left maximal match of minimum
  // length within K steps.
  inline void find_Lmaximal(string &P, long long prefix, long long i, long long len, vector<match_t> &matches, long long min_len, bool print);

  // Given an interval where the given prefix is matched up to a
  // mismatch, find all MEMs up to a minimum match depth.
  void collectMEMs(string &P, long long prefix, interval_t mli, interval_t xmi, vector<match_t> &matches, long long min_len, bool print);

  // Find all MEMs given a prefix pattern offset k.
  void findMEM(long long k, string &P, vector<match_t> &matches, long long min_len, bool print);

  // NOTE: min_len must be > 1
  void findMAM(string &P, int chunk, int chunks, vector<match_t> &matches, long long min_len, long long& memCount, bool print);
  inline bool is_leftmaximal(string &P, long p1, long p2);

  // Maximal Almost-Unique Match (MAM). Match is unique in the indexed
  // sequence S. as computed by MUMmer version 2 by Salzberg
  // et. al. Note this is a "one-sided" query. It "streams" the query
  // P throught he index.  Consequently, repeats can occur in the
  // pattern P.
  void MAM(string &P, int chunk, int chunks, vector<match_t> &matches, long long min_len, long long& memCount, bool print) { 
    if(K != 1) return;  // Only valid for full suffix array.
    findMAM(P, chunk, chunks, matches, min_len, memCount, print);  
  }

  // Find Maximal Exact Matches (MEMs) 
  void MEM(string &P, vector<match_t> &matches, long long min_len, bool print, long long& memCount, int num_threads = 1);

  // Maximal Unique Match (MUM) 
  void MUM(string &P, vector<match_t> &unique, long long min_len, long long& memCount, bool print);  

  // Maximal Unique Match (MUM) with OpenMP
  void MUMParallel(string &P, int chunks, vector<match_t> &unique, long long min_len, long long& currentCount, bool print);
};


#endif // __sparseSA_hpp__

