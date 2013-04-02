#include "pomp_lib.h"

struct ompregdescr omp_rd_2 = {
  "parallelfor", "", 0, "sparseSA.cpp", 155, 155, 158, 158
};

#define POMP_DLIST_00002 shared(omp_rd_2)

struct ompregdescr omp_rd_3 = {
  "region", "MUM", 0, "sparseSA.cpp", 737, 737, 749, 749
};

struct ompregdescr omp_rd_4 = {
  "parallel", "", 0, "sparseSA.cpp", 738, 738, 748, 748
};

#define POMP_DLIST_00004 shared(omp_rd_4,omp_rd_5,omp_rd_6)

struct ompregdescr omp_rd_5 = {
  "for", "", 0, "sparseSA.cpp", 740, 740, 745, 745
};

struct ompregdescr omp_rd_6 = {
  "critical", "", 0, "sparseSA.cpp", 746, 746, 747, 747
};

