#!/bin/bash - 
#===============================================================================
#
#          FILE:  split.sh
# 
#         USAGE:  ./split.sh 
# 
#   DESCRIPTION:  
# 
#       OPTIONS:  ---
#  REQUIREMENTS:  ---
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR: YOUR NAME (), 
#       COMPANY: 
#       CREATED: 07/25/12 14:20:36 CEST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

LENGTH=`seqstat --quiet $1|grep Total|cut -d":" -f2`;

SIZE=`expr $LENGTH / $2`
SIZE=`expr $SIZE + $3`

splitter $1 fasta -size $SIZE -overlap $3 -ossingle2;

for i in `ls  *.fasta`
do
    mkdir -p $4/$3/$2
    mv $i $4/$3/$2/
done
