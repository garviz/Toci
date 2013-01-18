#include "pomp_lib.h"


extern struct ompregdescr omp_rd_36;
extern struct ompregdescr omp_rd_37;
extern struct ompregdescr omp_rd_38;
extern struct ompregdescr omp_rd_39;
extern struct ompregdescr omp_rd_40;

int POMP_MAX_ID = 41;

struct ompregdescr* pomp_rd_table[41] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  &omp_rd_36,
  &omp_rd_37,
  &omp_rd_38,
  &omp_rd_39,
  &omp_rd_40,
};
