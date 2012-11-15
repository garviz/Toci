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

export OMP_NUM_THREADS=24
export length=20
export ref=`basename $1`
export qry=`basename $2`

for chunks in {1..128}
do
    export OMP_SCHEDULE="$3,$chunks"
    likwid-perfctr -C 0-23 -g FLOPS_DP mpirun -np 1 ./toci -C $chunks -mum -l $length $1 $2 1> results/$(date +"%m-%d-%Y_%T")_"$ref"_"$qry"_$OMP_SCHEDULE.dat 2>&1
    echo $chunks
done






















