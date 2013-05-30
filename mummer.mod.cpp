#ifdef _POMP
#  undef _POMP
#endif
#define _POMP 200110

#include "mummer.cpp.opari.inc"
#line 1 "mummer.cpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <likwid.h>

#include "sparseSA.hpp"
#include "fasta.hpp"

#include <getopt.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <cctype>                                                  

// NOTE use of special characters ~, `, and $ !!!!!!!!

using namespace std;

void usage(string prog);

enum mum_t { MUM, MAM, MEM };

int min_len = 20;
int chunks = 1;
int sparseMult=1;
mum_t type = MAM;
bool rev_comp = false, _4column = false, nucleotides_only = false;
bool forwards = true;
bool setRevComp = false;
bool setBoth = false;
bool automatic = true;
bool automaticSkip = true;
bool suflink = true;
bool child = true;
bool print_length = false;
bool printSubstring = false;
int K = 1, num_threads = 1, query_threads = 1;
sparseSA *sa;
string query_fasta;

struct query_arg {
  int skip0;
  int skip;
};

void *query_thread(void *arg_) {
  query_arg *arg = (query_arg *)arg_;
  long long memCounter = 0;
  string meta, line;
  ifstream data(query_fasta.c_str());

  vector<match_t> matches;

  bool print = false;

  long seq_cnt = 0;

  if(!data.is_open()) { cerr << "unable to open " << query_fasta << endl; exit(1); }

  // Collect meta data.
  while(!data.eof()) {
    getline(data, line); // Load one line at a time.
    if(line.length() == 0) continue;
    if(line[0] == '>') {
      long start = 1, end = line.length() - 1;
      trim(line, start, end);
      for(long i = start; i <= end; i++) {
	if( line[i] == ' ') break; // Behave like MUMmer 3 cut off meta after first space.
	meta += line[i];
      }
      //cerr << "# " << meta << endl;
      break;
    }
  }
  string *P = new string;
  while(!data.eof()) {
    getline(data, line); // Load one line at a time.
    if(line.length() == 0) continue;
    long start = 0, end = line.length() - 1;
    // Meta tag line and start of a new sequence.
    // Collect meta data.
    if(line[0] == '>') {
      if(meta != "") {
	if(seq_cnt % arg->skip == arg->skip0) {
	  // Process P.
	  cout << ",Q.length=" << P->length() << endl;
      if(forwards){
        if(print){ 
            if(print_length) printf("> %s\tLen = %ld\n", meta.c_str(), P->length()); 
            else printf("> %s\n", meta.c_str());
        }
        if(type == MAM) sa->MAM(*P, 0, 1, matches, min_len, memCounter, print);
        else if(type == MUM) {
            sa->MUMParallel(*P, chunks, matches, min_len, memCounter, print);
        }

        else if(type == MEM) sa->MEM(*P, matches, min_len, print, memCounter, num_threads);
        if(!print) sa->print_match(meta, matches, false);
      }
	  if(rev_comp) {
	    reverse_complement(*P, nucleotides_only);
	    if(print){ 
            if(print_length) printf("> %s Reverse\tLen = %ld\n", meta.c_str(), P->length()); 
            else printf("> %s Reverse\n", meta.c_str());
        }
	    if(type == MAM) sa->MAM(*P, 0, 1, matches, min_len, memCounter, print);
	    else if(type == MUM) {
            sa->MUMParallel(*P, chunks, matches, min_len, memCounter, print);
        }
        else if(type == MEM) sa->MEM(*P, matches, min_len, print, memCounter, num_threads);
	    if(!print) sa->print_match(meta, matches, true);
	  }
	}
	seq_cnt++;
        delete P; P = new string; meta = ""; 
      }
      start = 1;
      trim(line, start, end);
      for(long i = start; i <= end; i++) {
	if(line[i] == ' ') break; // Behave like MUMmer 3 cut of meta after first space.
	meta += line[i];
      }
      cerr << "# " << meta << endl;
    }
    else { // Collect sequence data.
      trim(line, start,end);
      for(long i = start; i <= end; i++) {
		//char c = std::tolower(line[i]);        
		char c = line[i];        
		if(nucleotides_only) {
	  		switch(c) {
	  			case 'A': case 'T': case 'G': case 'C': break;
	  			default:
	    			c = '~';
	  		}
		}
		*P += c;
      }
    }
  }
  // Handle very last sequence.
  if(meta != "") {
    if(seq_cnt % arg->skip == arg->skip0) {
      cout << ",Q.length=" << P->length() << endl;
      if(forwards){
        if(print){ 
            if(print_length) printf("> %s\tLen = %ld\n", meta.c_str(), P->length()); 
            else printf("> %s\n", meta.c_str());
        }

        if(type == MAM) sa->MAM(*P, 0, 1, matches, min_len, memCounter, print);
        else if(type == MUM) {
            sa->MUMParallel(*P, chunks, matches, min_len, memCounter, print);
        }
        else if(type == MEM) sa->MEM(*P, matches, min_len, print, memCounter, num_threads);
        if(print) sa->print_match(meta, matches, false);
      }
      if(rev_comp) {
        reverse_complement(*P, nucleotides_only);
        if(print){ 
            if(print_length) printf("> %s Reverse\tLen = %ld\n", meta.c_str(), P->length()); 
            else printf("> %s Reverse\n", meta.c_str());
        }
        if(type == MAM) sa->MAM(*P, 0, 1, matches, min_len, memCounter, print);
        else if(type == MUM) sa->MUMParallel(*P, chunks, matches, min_len, memCounter, print);
        else if(type == MEM) sa->MEM(*P, matches, min_len, print, memCounter, num_threads);
        if(print) sa->print_match(meta, matches, true);
      }
    }
  }
  delete P;
  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
POMP_Init();
#line 180 "mummer.cpp"
  LIKWID_MARKER_INIT;
  LIKWID_MARKER_START("MUM");
  // Collect parameters from the command line.
    rusage memory;
  while (1) {
    static struct option long_options[] = { 
      {"l", 1, 0, 0}, // 0
      {"mumreference", 0, 0, 0}, // 1
      {"b", 0, 0, 0}, // 2
      {"maxmatch", 0, 0, 0}, // 3
      {"mum", 0, 0, 0}, // 4
      {"mumcand", 0, 0, 0},  // 5
      {"F", 0, 0, 0}, // 6
      {"k", 1, 0, 0}, // 7
      {"threads", 1, 0, 0}, // 8
      {"n", 0, 0, 0}, // 9
      {"qthreads", 1, 0, 0}, // 10
      {"suflink", 1, 0, 0}, // 11
      {"C", 1, 0, 0}, // 12
      {"skip", 1, 0, 0}, // 13
      {"L", 0, 0, 0}, // 14
      {"r", 0, 0, 0}, // 15
      {"s", 0, 0, 0}, // 16
      {0, 0, 0, 0} 
    };
    int longindex = -1;
    int c = getopt_long_only(argc, argv, "", long_options, &longindex);
    if(c == -1) break; // Done parsing flags.
    else if(c == '?') { // If the user entered junk, let him know. 
      cerr << "Invalid parameters." << endl;
      usage(argv[0]);
    }
    else {
      // Branch on long options.
      switch(longindex) { 
      case 0: min_len = atol(optarg); break;
      case 1: type = MAM; break;
      case 2: setBoth = true;	break;
      case 3: type = MEM; break;
      case 4: type = MUM; break;
      case 5: type = MAM; break;
      case 6: _4column = true; break;
      case 7: K = atoi(optarg); break;
      case 8: num_threads = atoi(optarg); break;
      case 9: nucleotides_only = true; break;
      case 10: query_threads = atoi(optarg) ; break;
      case 11: suflink = atoi(optarg) > 0;	automatic = false; break;
      case 12: chunks = atoi(optarg);	automatic = false; break;
      case 13: sparseMult = atoi(optarg); automaticSkip = false; break;
      case 14: print_length = true; break;
      case 15: setRevComp = true; break;
      case 16: printSubstring = true; break;
      default: break; 
      }
    }
  }
  if (argc - optind != 2) usage(argv[0]);

  if(K != 1 && type != MEM) { cerr << "-k option valid only for -maxmatch" << endl; exit(1); }
  if(num_threads <= 0) { cerr << "invalid number of threads specified" << endl; exit(1); }

  string ref_fasta = argv[optind]; 
  query_fasta = argv[optind+1];

  string ref;
  
  vector<string> refdescr; 
  vector<long long> startpos;

  LIKWID_MARKER_STOP("MUM");
  LIKWID_MARKER_CLOSE;
  //load_fasta(ref_fasta, ref, refdescr, startpos);

  // Automatically use 4 column format if there are multiple reference sequences.
  if(startpos.size() > 1) _4column = true;
  if(automatic) {
      suflink = K < 4;
      child = K >= 4;
  }
  if(automaticSkip) {
      if(suflink && !child) sparseMult = 1;
      else{
          if(K >= 4) sparseMult = (int) (min_len-10)/K;
          else sparseMult = (int) (min_len-12)/K;
      }
  }
  else{
      if(sparseMult*K > min_len){
        while(sparseMult*K > min_len)
            sparseMult--;
        cerr << "skip parameter was decreased to " << sparseMult << " because skip*K > minimum length" << endl;
      }
      if(sparseMult*K > min_len-10){
          cerr << "note that the skip parameter is very high, a value of " << ((int) (min_len-10)/K);
          cerr << " or " << ((int) (min_len-12)/K) << " would be more appropriate" << endl;
      }
  }
  
  if(setBoth && setRevComp){
      cerr << "ERROR -r and -b options are mutually exclusive" << endl;
      exit(1);
  }
  if(setBoth || setRevComp)
      rev_comp = true;
  if(setRevComp)
      forwards = false;
  double s, e;
  s = omp_get_wtime();
  sa = new sparseSA(ref_fasta, refdescr, startpos, _4column, K, suflink, true, sparseMult, printSubstring);
  e = omp_get_wtime();
  cout << ",sparseSA=" << e-s;
  pthread_attr_t attr;  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  vector<query_arg> args(query_threads);
  vector<pthread_t> thread_ids(query_threads);  

  // Initialize additional thread data.
  for(int i = 0; i < query_threads; i++) { 
    args[i].skip = query_threads;
    args[i].skip0 = i;
  }

  // Create joinable threads to find MEMs.
  for(int i = 0; i < query_threads; i++) 
    pthread_create(&thread_ids[i], &attr, query_thread, (void *)&args[i]);

  // Wait for all threads to terminate.
  for(int i = 0; i < query_threads; i++) 
    pthread_join(thread_ids[i], NULL);    

  getrusage(RUSAGE_SELF, &memory);
  cout << "# RSS=" << memory.ru_maxrss << endl;
  delete sa;
}


void usage(string prog) {
  cerr << "Usage: " << prog << " [options] <reference-file> <query-file>" << endl;
  cerr << "Implemented MUMmer v3 options:" << endl;
  cerr << "-mum           compute maximal matches that are unique in both sequences" << endl;
  cerr << "-mumreference  compute maximal matches that are unique in the reference-" << endl;
  cerr << "               sequence but not necessarily in the query-sequence (default)" << endl;
  cerr << "-mumcand       same as -mumreference" << endl;
  cerr << "-maxmatch      compute all maximal matches regardless of their uniqueness" << endl;
  cerr << "-l             set the minimum length of a match" << endl;
  cerr << "               if not set, the default value is 20" << endl;
  cerr << "-b             compute forward and reverse complement matches" << endl;
  cerr << "-F             force 4 column output format regardless of the number of" << endl;
  cerr << "               reference sequence inputs"  << endl;
  cerr << "-n             match only the characters a, c, g, or t" << endl;
  cerr << "-L             print length of query sequence in header of matches" << endl;
  cerr << "-r             compute only reverse complement matches" << endl;
  cerr << "-s             print first 53 characters of the matching substring" << endl;
  cerr << "-C        number of chunks to split query" << endl;
  cerr << endl;
  
  exit(1);
}
