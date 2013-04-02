#!/usr/bin/python

import sys
from random import randint

length = sys.argv[1]

dna = {1 : "a", 2 : "c", 3 : "g", 4 : "t"}

seq = []

for i in range(int(length)):
    base = randint(1,4)
    seq.append(dna[base])

print ('>sequence ', length)
print (''.join(seq))
