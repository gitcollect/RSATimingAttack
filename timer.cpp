#include <iostream>
#include <cstdlib>
#include <ctime>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace std;

RSA* read_key() {
  RSA *rsa = RSA_new();
  const char *filename = "private.pem";
  FILE *fp = fopen(filename, "r");
  if((PEM_read_RSAPrivateKey(fp, &rsa, NULL, NULL)) == NULL) {
    printf("\n%s\n", "Error Reading private key");
    return NULL ;
  }
/*
  printf("Read RSA key from %s\n", filename);
  */printf("N=");
  BN_print_fp(stdout, rsa->n);
  printf("\n");
  /*printf("P=");
  BN_print_fp(stdout, rsa->p);
  printf("\n");
  printf("Q=");
  BN_print_fp(stdout, rsa->q);
  printf("\n");
  printf("E=");
  BN_print_fp(stdout, rsa->e);
  printf("\n");*/
  printf("D=");
  BN_print_fp(stdout, rsa->d);
  printf("\n");
  return rsa;
}

long long rsa_decrypt(RSA* key, BIGNUM *m) {
  BIGNUM *r = BN_new();
  BN_CTX *ctx = BN_CTX_new();
  timespec tbegin, tend;
  cerr << "Flag " << BN_get_flags(key->d, BN_FLG_CONSTTIME) << endl;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tbegin);

  for (int i=0; i<10000; i++) {
    // BN_mod_exp(r, m, key->d, key->n, ctx);
    // BN_mod_exp_simple(r, m, key->d, key->n, ctx);
    BN_mod_exp_mont(r, m, key->d, key->n, ctx, NULL);
  }

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);

  long long start_time = tbegin.tv_sec*1000000000LL+tbegin.tv_nsec;
  long long end_time = tend.tv_sec*1000000000LL+tend.tv_nsec;
  return end_time-start_time;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Expected message as argument" << std::endl;
    exit(1);
  }
  RSA *rsa = read_key();
  BIGNUM *a = BN_new();
  BN_dec2bn(&a, argv[1]);
  /*
  printf("M=");
  BN_print_fp(stdout, a);
  printf("\n");
  */
  FILE* f = fopen("time.txt", "w");
  long long time = rsa_decrypt(rsa, a);
  fprintf(f, "%lld\n", time);
  //fprintf(stderr, "Time to decrypt = %lf\n", double(time)/CLOCKS_PER_SEC);
  return 0;
}
