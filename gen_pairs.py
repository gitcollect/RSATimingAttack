#!/usr/bin/python

import random
import sys

n = int(sys.argv[1])
m = int(sys.argv[2])

print>>sys.stderr, "Generating %d pairs of size %d bits..." % (m, n)
for _ in xrange(m):
    print random.randint(1, 2**n-1), random.randint(1, 2**n-1)
