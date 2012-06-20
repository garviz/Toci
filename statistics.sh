#!/bin/bash - 
#===============================================================================
#
#          FILE:  statistics.sh
# 
#         USAGE:  ./statistics.sh 
# 
#   DESCRIPTION:  Counting suffixes links 
#       OPTIONS:  ---
#  REQUIREMENTS:  ---
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR: YOUR NAME (), 
#       COMPANY: 
#       CREATED: 06/14/12 14:14:56 CEST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
for i in {0..50}
do
    echo -n "$i "
    #grep "($i) -->" $1 | gawk '{sum+=$1} END{print sum}'
    grep "($2) -->" $1|gawk -v x=$i -f program.awk
done
