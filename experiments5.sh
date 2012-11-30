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

for GROUP in BRANCH CACHE L2 L2CACHE L3 L3CACHE MEM TLB
do
for thrd in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
do
    export OMP_SCHEDULE="$3,$4"
    export OMP_NUM_THREADS=$thrd
    export GOMP_CPU_AFFINITY="0 6 1 7 2 8 3 9 4 10 5 11 12 18 13 19 14 20 15 21 16 22 17 23"
    if [ $thrd -eq 1 ]
    then
        likwid-perfctr -C 0 -g $GROUP -m ./toci -C $thrd -mum -l $length $1 $2 >> results/$file_name-$thrd-$GROUP.aff
        echo 1
    else
        likwid-perfctr -C 0-$(($thrd-1)) -g $GROUP -m ./toci -C $thrd -mum -l $length $1 $2 >> results/$file_name-$thrd-$GROUP.aff
        echo 2
    fi
    echo -n "$thrd "
done
    echo $GROUP
done
