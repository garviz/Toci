#!/bin/bash - 
#===============================================================================
#
#          FILE:  check.sh
# 
#         USAGE:  ./check.sh ref.fa qry.fa R Q L
# 
#   DESCRIPTION:  Check if MUM is a real MUM
# 
#       OPTIONS:  ---
#  REQUIREMENTS:  ---
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR: Julio C\'esar Garc\'ia Vizca\'ino
#       COMPANY: 
#       CREATED: 11/10/12 19:50:03 CEST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

grep -v '>' $1 | tr -d '\n' | gawk -v R=$3 -v L=$5 '{print substr($0,R,L)}'|sha1sum -t
grep -v '>' $2 | tr -d '\n' | gawk -v Q=$4 -v L=$5 '{print substr($0,Q,L)}'|sha1sum -t
ref=`grep -v '>' $1 | tr -d '\n' | gawk -v R=$3 -v L=$5 '{print substr($0,R,L)}'`
qry=`grep -v '>' $2 | tr -d '\n' | gawk -v Q=$4 -v L=$5 '{print substr($0,Q,L)}'`

grep -v '>' $1 | tr -d '\n' | grep -o $ref | wc -l
grep -v '>' $2 | tr -d '\n' | grep -o $qry | wc -l
