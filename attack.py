# -*- coding: utf-8 -*-

import sys
import test_oracle

def main():
	if len(sys.argv) != 2:
		print 'Expected RSA modulus (N) as argument'
		exit(1)

	d = 0x1
	N = int(sys.arg[1], 16)
	for _ in xrange(16):
		k = test_oracle.next("custom_times_64.txt", N, d)
		print bin(d)

	print 'Discovered key is:', hex(d)

if __name__ == '__main__':
    main()
