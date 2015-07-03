#!/usr/bin/python

from Crypto.Util import number
from matplotlib import pyplot as plt
import math

N64 = 0xF4AD592111F07CC5


def inv(x, n):
    r = 1
    while r < n:
        r = r*2
    return (x*number.inverse(r, n)) % n


def mmult(x, y, n):
    """Montgomery multiplication.

    Returns: (True if extra reduction needed, x*y mod n).
    """
    r = 1
    while r < n:
        r = r*2
    ax = (x*r) % n
    ay = (y*r) % n
    t = ax*ay
    np = r-number.inverse(n, r)
    assert (n*np+1) % r == 0
    m = ((t % r)*np) % r
    t = (t+m*n) / r
    if t >= n:
        return (1, t-n)
    return (0, t)


def oracle1(x, y, n):
    """Oracle for one montgomery multiplication."""
    return mmult(x, y, n)[0]


def pravi_oracle1(x, k, n):
    m = x
    m_temp = pow(m, k, n)**2 % n
    t = (m*m_temp) % n
    return oracle1(t, t, n)


def pravi_oracle2(x, k, n):
    m = x
    m_temp = pow(m, k, n)**2 % n
    t = (m_temp) % n
    return oracle1(t, t, n)


def mmod_exp(x, y, n):
    """Repeated squaring with montgomery multiplication.

    Returns: (list of booleans S, list of booleans M, x^y mod n).

    Boolean values represent of extra reduction is needed during squaring
    (S) and multiplication steps (M).
    """
    M = []
    S = []
    bits = int(math.log(y, 2)+1)
    m = x
    for i in xrange(bits-1):
        b, x = mmult(x, x, n)
        x = inv(x, n)
        S.append(b)
        if (y >> (bits-2-i)) & 1:
            b, x = mmult(x, m, n)
            x = inv(x, n)
            M.append(b)
        else:
            M.append(0)
    return (S, M, x)


def test_oracle1(filename, N):
    data = [tuple(map(int, x.split())) for x in file(filename).readlines()[1:]]
    m0 = [c[2] for c in data if not oracle1(c[0], c[1], N)]
    m1 = [c[2] for c in data if oracle1(c[0], c[1], N)]
    print 'Oracle says false: ', len(m0)
    print 'Oracle says true: ', len(m1)
    plt.hist(m1, bins=50, color="red", range=(250, 300))
    plt.hist(m0, bins=50, color="blue", range=(250, 300))
    plt.show()


def test_poracle1(filename, N, k):
    data = [tuple(map(int, x.split())) for x in file(filename).readlines()[1:]]
    m0 = [c[1] for c in data if not pravi_oracle1(c[0], k, N)]
    m1 = [c[1] for c in data if pravi_oracle1(c[0], k, N)]
    print 'POracle1 says false: ', len(m0)
    print 'POracle1 says true: ', len(m1)
    plt.hist(m1, bins=50, color="red")
    plt.savefig("po1-red.png")
    plt.hist(m0, bins=50, color="blue")
    plt.savefig("po1-blue.png")
    a = [float(sum(m0))/len(m0), float(sum(m1))/len(m1)]
    print "O1", a, a[0]-a[1]


def test_poracle2(filename, N, k):
    data = [tuple(map(int, x.split())) for x in file(filename).readlines()[1:]]
    m0 = [c[1] for c in data if not pravi_oracle2(c[0], k, N)]
    m1 = [c[1] for c in data if pravi_oracle2(c[0], k, N)]
    print 'POracle2 says false: ', len(m0)
    print 'POracle2 says true: ', len(m1)
    plt.hist(m1, bins=50, color="red")
    plt.savefig("po2-red.png")
    plt.hist(m0, bins=50, color="blue")
    plt.savefig("po2-blue.png")
    a = [float(sum(m0))/len(m0), float(sum(m1))/len(m1)]
    print "O2", a, a[0]-a[1]

def test_both(filename, N, k):
    data = [tuple(map(int, x.split())) for x in file(filename).readlines()[1:]]
    m1 = [c[1] for c in data if pravi_oracle1(c[0], k, N)]
    m2 = [c[1] for c in data if not pravi_oracle1(c[0], k, N)]
    m3 = [c[1] for c in data if pravi_oracle2(c[0], k, N)]
    m4 = [c[1] for c in data if not pravi_oracle2(c[0], k, N)]
    p1 = sum(m1)/float(len(m1))
    p2 = sum(m2)/float(len(m2))
    p3 = sum(m3)/float(len(m3))
    p4 = sum(m4)/float(len(m4))
    print [p1, p2, p3, p4]

data = None
time_diff = []
last_changed_bit = 0

def next(filename, N, k):
	global time_diff
	error_coef = 1
	global last_changed_bit	
	global data
	if not data:
		data = [tuple(map(int, x.split())) for x in file(filename).readlines()]
	m1 = [c[1] for c in data if pravi_oracle1(c[0], k, N)]
	m2 = [c[1] for c in data if not pravi_oracle1(c[0], k, N)]
	m3 = [c[1] for c in data if pravi_oracle2(c[0], k, N)]
	m4 = [c[1] for c in data if not pravi_oracle2(c[0], k, N)]
	p1 = sum(m1)/float(len(m1))
	p2 = sum(m2)/float(len(m2))
	p3 = sum(m3)/float(len(m3))
	p4 = sum(m4)/float(len(m4))
	
	
	print abs(p1 - p2), ' vs. ', abs(p3 - p4)	
	diff = abs(abs(p1 - p2) - abs(p3 - p4))
	time_diff.append(diff) 
	if (len(time_diff) > error_coef): #simple error correction
		avg = sum(time_diff[:-error_coef]) / float(len(time_diff) - error_coef)
		print 'avg =', avg
		if(len([x for x in time_diff[-error_coef:] if 1.5 * x < avg]) == error_coef): 
			# if bit should be changed
			if (last_changed_bit + error_coef - 1 != int(math.log(k, 2))): 
				# and is not already changed
				print 'Error correction..'
				time_diff[-error_coef:] = [avg]
				k >>= (error_coef - 1)			
				k ^= 1						
				last_changed_bit = int(math.log(k, 2))			
				return k			
 
	del m1; del m2; del m3; del m4

	if (p1 - p2) > (p3 - p4):
		return k*2+1
	return k*2


# test_oracle1("custom_mont_times_64_min.txt", N64)
# print inv(mmult(332, 33, N64)[1], N64)
#test_poracle1("custom_times_64a.txt", N64, 1)
#test_poracle2("custom_times_64a.txt", N64, 16)
#D = 0x8685C83A1325B11

k = 0x1
for _ in xrange(64):
    k = next("custom_times_64.txt", N64, k)
    print bin(k)
