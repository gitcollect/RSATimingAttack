# -*- coding: utf-8 -*-

import sys
import random
from subprocess import call

def oracle0(m_temp, m, N):
	return m_temp ** 2 >= N

def oracle1(m_temp, m, N):
	return m * m_temp ** 2 >= N

def generate_messages(n, N):
	msgs = []
	for i in range(n):
		msgs.append(random.randint(0, N-1))
	return msgs

def main():
	bits = "1"
	key = [1];
	keylen = 32
	N = 10000000000000000000000000000000

	msgs = generate_messages(100, 100007)
	T = []
	m_temp = []
	for msg in msgs:
		call(['timer', str(msg)], shell=True)
		T.append(int(open('time.txt', 'r').read()))
		m_temp.append(msg) 

	for i in range(1, keylen):
		sum00 = 0; sum01 = 0; sum10 = 0; sum11 = 0;
		n00 = 0; n01 = 0; n10 = 0; n11 = 0
		for j in range(len(msgs)):
			if (oracle0(m_temp[j], msgs[j], N)): sum01 += T[j]; n01 += 1
			else: sum00 += T[j]; n00 += 1
			if (oracle1(m_temp[j], msgs[j], N)): sum11 += T[j]; n11 += 1
			else: sum10 += T[j]; n10 += 1
		t00 = sum00 / max(n00, 1)
		t01 = sum01 / max(n01, 1)
		t10 = sum10 / max(n10, 1)
		t11 = sum11 / max(n11, 1)
		if abs(t00 - t01) > abs(t10 - t11):
			key.append(0)
			bits += "0"
		else:
			key.append(1)
			bits += "1"
		for j in range(len(msgs)):
			if key[-1] == 0: m_temp[j] = (m_temp[j] * 2) % N
			else: m_temp[j] = (msgs[j] * (m_temp[j] ** 2)) % N

	print 'Discovered key is:', bits

if __name__ == '__main__':
    main()
