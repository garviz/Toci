#!/bin/sh
VSTREESRC=${DIRVSTREE}/src/vstree/src
vcopy.sh `cat Importfiles`
grep -v -f Excludemulti ${VSTREESRC}/include/multidef.h > multidef.h
gawk -f Cuthere.awk ${VSTREESRC}/kurtz/multiseq.cpp | grep -v -f Excludemulti > multiseq.cpp
Mkprotodef.sh `ls *.cpp` > protodef.h
for filename in `ls *.[cpp|h]`
do
  cat ../Copyright > tmp
  Skipfirstcom.pl ${filename} >> tmp
  mv tmp ${filename}
done
