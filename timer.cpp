#include <iostream>
#include <cstdlib>
#include <ctime>
#include <ttmath/ttmath.h>
#include <openssl/rsa.h>

long long rsaEncryptTime(ttmath::UInt<2> m) {
	clock_t startTime = clock();
	// send message for rsa decryption
	for (int i = 0; i < 100000000; ++i) {
	}
	long long result = (long long) clock() - startTime;
	return result;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Expected message as argument" << std::endl;
		exit(1);
	}
	ttmath::UInt<2> m = argv[1];
	FILE* f = fopen("time.txt", "w");
	fprintf(f, "%d\n", rsaEncryptTime(m));
	return 0;
}
