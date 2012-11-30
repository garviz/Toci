#!/bin/bash - 
#===============================================================================
#
#          FILE:  experiments.sh
# 
#         USAGE:  ./experiments.sh 
# 
#   DESCRIPTION:  Run experiments with OpenMP for Toci
# 
#       OPTIONS:  ---
#  REQUIREMENTS:  Reference and query sequences
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR: Julio Cesar Garcia Vizcaino
#       COMPANY: 
#       CREATED: 13/11/12 17:17:16 CET
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

export length=20
export ref=`basename $1`
export qry=`basename $2`
export file_name=$(date +"%m-%d-%Y_%T")

for i in {1..3}
do
    for thrd in 1 2 4 8 16 24 32
    do
        export OMP_SCHEDULE="$3,$4"
        export OMP_NUM_THREADS=$thrd
        #export GOMP_CPU_AFFINITY="0 6 1 7 2 8 3 9 4 10 5 11"
        likwid-perfctr -C 0-23 -g CACHE mpirun -np 1 ./toci -C $4 -mum -l $length $1 $2 1>> results/$file_name.aff 2>&1
        echo -n "$thrd "
    done
    echo $i
done
