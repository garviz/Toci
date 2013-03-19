#include "pomp_lib.h"

struct ompregdescr omp_rd_3 = {
  "parallelfor", "", 0, "sparseSA.cpp", 155, 155, 158, 158
};

#define POMP_DLIST_00003 shared(omp_rd_3)

struct ompregdescr omp_rd_4 = {
  "region", "mum", 0, "sparseSA.cpp", 733, 733, 783, 783
};

struct ompregdescr omp_rd_5 = {
  "parallel", "", 0, "sparseSA.cpp", 740, 740, 750, 750
};

#define POMP_DLIST_00005 shared(omp_rd_5,omp_rd_6,omp_rd_7)

struct ompregdescr omp_rd_6 = {
  "for", "", 0, "sparseSA.cpp", 742, 742, 747, 747
};

struct ompregdescr omp_rd_7 = {
  "critical", "", 0, "sparseSA.cpp", 748, 748, 749, 749
};

