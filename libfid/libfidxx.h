#ifndef LIBFIDXX_H
#define LIBFIDXX_H
#include <libfid.h>

template <int WS> struct fid_traits;

template <>
struct fid_traits<32>
{
  typedef fid_Uint32                 uint;
  typedef fid_Sint32                 sint;
  typedef fid_ArrayUint32            arrayuint;
  typedef fid_ArrayUint32            arraysint;
  typedef fid_Suffixinterval_32      fid_Suffixinterval;
  typedef fid_Sequenceiterfun_32     fid_Sequenceiterfun;
  typedef fid_Esatraversecallback_32 fid_Esatraversecallback;

  static const fid_Uintsize uisize=fid_UINTSIZE_32;
  static const uint fid_SUFFIXINTERVAL_SINGLETON=fid_SUFFIXINTERVAL_SINGLETON_32;

  static inline uint getval(const fid_Uint48 &i) { return i.v_uint32; }
  static inline uint setval(fid_Uint48 &i, uint v) { return i.v_uint32=v; }
  static inline uint incval(fid_Uint48 &i) { return ++i.v_uint32; }
  static inline uint decval(fid_Uint48 &i) { return --i.v_uint32; }
  static inline const uint *getptr(const fid_Uint48constptr &p) { return p.v_uint32; }

  static inline void fid_suffixinterval_init_root(fid_Suffixinterval_32 *si,
                                                  const fid_Suffixarray *esa)
  {
    fid_suffixinterval_init_root_32(si,esa);
  }

  static inline uint fid_sequences_offset_to_index(const fid_Sequences *seqs, uint offset)
  {
    return fid_sequences_offset_to_index_32(seqs,offset);
  }
  static inline void fid_sequences_index_to_boundaries(const fid_Sequences *seqs, uint seqindex, uint *left, uint *right)
  {
    return fid_sequences_index_to_boundaries_32(seqs,seqindex,left,right);
  }
  static inline void fid_sequences_offset_to_boundaries(const fid_Sequences *seqs, uint offset, uint *left, uint *right)
  {
    return fid_sequences_offset_to_boundaries_32(seqs,offset,left,right);
  }
  static inline int fid_sequences_iterate_range(const fid_Sequences *seqs, uint from, uint to, const fid_Sequenceiterfun iterfun, void *user_data)
  {
    return fid_sequences_iterate_range_32(seqs,from,to,iterfun,user_data);
  }
  static inline int fid_sequences_iterate(const fid_Sequences *seqs, const fid_Sequenceiterfun iterfun, void *user_data)
  {
    return fid_sequences_iterate_32(seqs,iterfun,user_data);
  }
  static inline void fid_sequences_dump_range(const fid_Symbol *seq, uint length, const fid_Alphabet *alpha, const char *str, int stop_at_separator, FILE *stream)
  {
    return fid_sequences_dump_range_32(seq,length,alpha,str,stop_at_separator,stream);
  }
  static inline int fid_suffixarray_traverse(const fid_Suffixarray *esa, fid_Esatraversecallback callback, void *user_data, fid_Error *error)
  {
    return fid_suffixarray_traverse_32(esa,callback,user_data,error);
  }
  static inline uint fid_suffixarray_get_intervals(const fid_Suffixarray *esa, const fid_Suffixinterval *si, uint *intervals)
  {
    return fid_suffixarray_get_intervals_32(esa,si,intervals);
  }
  static inline int fid_suffixarray_find_embedded_interval(const fid_Suffixarray *esa, fid_Suffixinterval *si, fid_Symbol symbol)
  {
    return fid_suffixarray_find_embedded_interval_32(esa,si,symbol);
  }
  static inline int fid_suffixarray_extend_interval(const fid_Suffixarray *esa, const fid_Symbol *pattern, uint plen, fid_Suffixinterval *si)
  {
    return fid_suffixarray_extend_interval_32(esa,pattern,plen,si);
  }
  static inline int fid_suffixarray_find_interval(const fid_Suffixarray *esa, const fid_Symbol *pattern, uint plen, fid_Suffixinterval *si)
  {
    return fid_suffixarray_find_interval_32(esa,pattern,plen,si);
  }
  static inline uint fid_suffixarray_find_large_lcp(const fid_Suffixarray *esa, uint suffix)
  {
    return fid_suffixarray_find_large_lcp_32(esa,suffix);
  }
  static inline uint fid_suffixarray_suffix_length(const fid_Suffixarray *esa, uint suffix)
  {
    return fid_suffixarray_suffix_length_32(esa,suffix);
  }
  static inline void fid_suffixarray_compute_distribution(fid_Suffixarray *esa)
  {
    return fid_suffixarray_compute_distribution_32(esa);
  }
  static inline void fid_suffixarray_dump_intervals(const fid_Suffixarray *esa, const uint intervals[fid_INTERVALBUFFERSIZE], FILE *stream)
  {
    return fid_suffixarray_dump_intervals_32(esa,intervals,stream);
  }
  static inline void fid_suffixarray_dump_suffix(const fid_Suffixarray *esa, uint suffix, uint length, FILE *stream)
  {
    return fid_suffixarray_dump_suffix_32(esa,suffix,length,stream);
  }
  static inline void fid_suffixarray_print(const fid_Suffixarray *esa, fid_Tablerequest request, FILE *stream)
  {
    return fid_suffixarray_print_32(esa,request,stream);
  }
  static inline uint fid_suffixinterval_lcpvalue(const fid_Suffixinterval *si, const fid_Suffixarray *esa)
  {
    return fid_suffixinterval_lcpvalue_32(si,esa);
  }
  static inline void fid_suffixinterval_to_lcpinterval(fid_Suffixinterval *si, const fid_Suffixarray *esa)
  {
    return fid_suffixinterval_to_lcpinterval_32(si,esa);
  }
  static inline void fid_suffixinterval_find_right(fid_Suffixinterval *si, const fid_Suffixarray *esa)
  {
    return fid_suffixinterval_find_right_32(si,esa);
  }
  static inline uint fid_suffixinterval_homepos(const fid_Suffixinterval *si, const fid_Suffixarray *esa)
  {
    return fid_suffixinterval_homepos_32(si,esa);
  }
  static inline void fid_suffixinterval_dump(const fid_Suffixinterval *si, const fid_Suffixarray *esa, FILE *stream)
  {
    return fid_suffixinterval_dump_32(si,esa,stream);
  }
};

template <>
struct fid_traits<64>
{
  typedef fid_Uint64                 uint;
  typedef fid_Sint64                 sint;
  typedef fid_ArrayUint64            arrayuint;
  typedef fid_ArrayUint64            arraysint;
  typedef fid_Suffixinterval_64      fid_Suffixinterval;
  typedef fid_Sequenceiterfun_64     fid_Sequenceiterfun;
  typedef fid_Esatraversecallback_64 fid_Esatraversecallback;

  static const fid_Uintsize uisize=fid_UINTSIZE_64;
  static const uint fid_SUFFIXINTERVAL_SINGLETON=fid_SUFFIXINTERVAL_SINGLETON_64;

  static inline uint getval(const fid_Uint48 &i) { return i.v_uint64; }
  static inline uint setval(fid_Uint48 &i, uint v) { return i.v_uint64=v; }
  static inline uint incval(fid_Uint48 &i) { return ++i.v_uint64; }
  static inline uint decval(fid_Uint48 &i) { return --i.v_uint64; }
  static inline const uint *getptr(const fid_Uint48constptr &p) { return p.v_uint64; }

  static inline void fid_suffixinterval_init_root(fid_Suffixinterval_64 *si,
                                                  const fid_Suffixarray *esa)
  {
    fid_suffixinterval_init_root_64(si,esa);
  }

  static inline uint fid_sequences_offset_to_index(const fid_Sequences *seqs, uint offset)
  {
    return fid_sequences_offset_to_index_64(seqs,offset);
  }
  static inline void fid_sequences_index_to_boundaries(const fid_Sequences *seqs, uint seqindex, uint *left, uint *right)
  {
    return fid_sequences_index_to_boundaries_64(seqs,seqindex,left,right);
  }
  static inline void fid_sequences_offset_to_boundaries(const fid_Sequences *seqs, uint offset, uint *left, uint *right)
  {
    return fid_sequences_offset_to_boundaries_64(seqs,offset,left,right);
  }
  static inline int fid_sequences_iterate_range(const fid_Sequences *seqs, uint from, uint to, const fid_Sequenceiterfun iterfun, void *user_data)
  {
    return fid_sequences_iterate_range_64(seqs,from,to,iterfun,user_data);
  }
  static inline int fid_sequences_iterate(const fid_Sequences *seqs, const fid_Sequenceiterfun iterfun, void *user_data)
  {
    return fid_sequences_iterate_64(seqs,iterfun,user_data);
  }
  static inline void fid_sequences_dump_range(const fid_Symbol *seq, uint length, const fid_Alphabet *alpha, const char *str, int stop_at_separator, FILE *stream)
  {
    return fid_sequences_dump_range_64(seq,length,alpha,str,stop_at_separator,stream);
  }
  static inline int fid_suffixarray_traverse(const fid_Suffixarray *esa, fid_Esatraversecallback callback, void *user_data, fid_Error *error)
  {
    return fid_suffixarray_traverse_64(esa,callback,user_data,error);
  }
  static inline uint fid_suffixarray_get_intervals(const fid_Suffixarray *esa, const fid_Suffixinterval *si, uint *intervals)
  {
    return fid_suffixarray_get_intervals_64(esa,si,intervals);
  }
  static inline int fid_suffixarray_find_embedded_interval(const fid_Suffixarray *esa, fid_Suffixinterval *si, fid_Symbol symbol)
  {
    return fid_suffixarray_find_embedded_interval_64(esa,si,symbol);
  }
  static inline int fid_suffixarray_extend_interval(const fid_Suffixarray *esa, const fid_Symbol *pattern, uint plen, fid_Suffixinterval *si)
  {
    return fid_suffixarray_extend_interval_64(esa,pattern,plen,si);
  }
  static inline int fid_suffixarray_find_interval(const fid_Suffixarray *esa, const fid_Symbol *pattern, uint plen, fid_Suffixinterval *si)
  {
    return fid_suffixarray_find_interval_64(esa,pattern,plen,si);
  }
  static inline uint fid_suffixarray_find_large_lcp(const fid_Suffixarray *esa, uint suffix)
  {
    return fid_suffixarray_find_large_lcp_64(esa,suffix);
  }
  static inline uint fid_suffixarray_suffix_length(const fid_Suffixarray *esa, uint suffix)
  {
    return fid_suffixarray_suffix_length_64(esa,suffix);
  }
  static inline void fid_suffixarray_compute_distribution(fid_Suffixarray *esa)
  {
    return fid_suffixarray_compute_distribution_64(esa);
  }
  static inline void fid_suffixarray_dump_intervals(const fid_Suffixarray *esa, const uint intervals[fid_INTERVALBUFFERSIZE], FILE *stream)
  {
    return fid_suffixarray_dump_intervals_64(esa,intervals,stream);
  }
  static inline void fid_suffixarray_dump_suffix(const fid_Suffixarray *esa, uint suffix, uint length, FILE *stream)
  {
    return fid_suffixarray_dump_suffix_64(esa,suffix,length,stream);
  }
  static inline void fid_suffixarray_print(const fid_Suffixarray *esa, fid_Tablerequest request, FILE *stream)
  {
    return fid_suffixarray_print_64(esa,request,stream);
  }
  static inline uint fid_suffixinterval_lcpvalue(const fid_Suffixinterval *si, const fid_Suffixarray *esa)
  {
    return fid_suffixinterval_lcpvalue_64(si,esa);
  }
  static inline void fid_suffixinterval_to_lcpinterval(fid_Suffixinterval *si, const fid_Suffixarray *esa)
  {
    return fid_suffixinterval_to_lcpinterval_64(si,esa);
  }
  static inline void fid_suffixinterval_find_right(fid_Suffixinterval *si, const fid_Suffixarray *esa)
  {
    return fid_suffixinterval_find_right_64(si,esa);
  }
  static inline uint fid_suffixinterval_homepos(const fid_Suffixinterval *si, const fid_Suffixarray *esa)
  {
    return fid_suffixinterval_homepos_64(si,esa);
  }
  static inline void fid_suffixinterval_dump(const fid_Suffixinterval *si, const fid_Suffixarray *esa, FILE *stream)
  {
    return fid_suffixinterval_dump_64(si,esa,stream);
  }
};

#endif /* !LIBFIDXX_H */
