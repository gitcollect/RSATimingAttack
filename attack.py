# -*- coding: utf-8 -*-

import sys
import random
from subprocess import call

def oracle0(m_temp, m, N):
	return m_temp ** 2 >= N ** 1.9

def oracle1(m_temp, m, N):
	m1 = (m * m_temp) % N
	return (m1 ** 2) >= N ** 1.9


#def generate_messages(n, N):
#	msgs = []
#	for i in range(n):
#		msgs.append(random.randint(0, N-1))
#	return msgs

def main():
	if len(sys.argv) != 2:
		print 'Expected RSA modulus (N) as argument'
		exit(1)

	key = [1];
	keylen = 64
	d = 1
	N = int(sys.argv[1], 16)
#	msgs = generate_messages(100, N)
	T = []
	M = []
	m_temp = []

	m = open('messages.txt', 'r')
	for line in m:
		msg = int(line)
		M.append(msg)	
		m1 = (msg ** 2) % N
		m_temp.append(m1)

	t = open('times.txt', 'r')
	for line in t:
		time = int(line)
		T.append(time)

	for i in range(1, keylen):
		sum00 = 0; sum01 = 0; sum10 = 0; sum11 = 0;
		n00 = 0; n01 = 0; n10 = 0; n11 = 0
		for j in range(len(M)):
			if (oracle0(m_temp[j], M[j], N)): sum01 += T[j]; n01 += 1
			else: sum00 += T[j]; n00 += 1
			if (oracle1(m_temp[j], M[j], N)): sum11 += T[j]; n11 += 1
			else: sum10 += T[j]; n10 += 1
		t00 = float(sum00) / max(n00, 1)
		t01 = float(sum01) / max(n01, 1)
		t10 = float(sum10) / max(n10, 1)
		t11 = float(sum11) / max(n11, 1)
		print abs(t00 - t01), 'vs', abs(t10 - t11)
		if abs(t00 - t01) > abs(t10 - t11): key.append(0); d = d * 2
		else: key.append(1); d = d * 2 + 1
		for j in range(len(M)):
			if key[-1] == 0: m_temp[j] = (m_temp[j] ** 2) % N
			else: m_temp[j] = ((M[j] * m_temp[j]) ** 2) % N
		#print m_temp[0]

	print 'Discovered key is:', hex(d)

if __name__ == '__main__':
    main()
