#include <iostream>
#include <cstdlib>
#include <ctime>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace std;

RSA* read_key(const char *filename) {
  RSA *rsa = RSA_new();
  FILE *fp = fopen(filename, "r");
  if((PEM_read_RSAPrivateKey(fp, &rsa, NULL, NULL)) == NULL) {
    printf("\n%s\n", "Error Reading private key");
    return NULL ;
  }
  fprintf(stderr, "Read RSA key from %s\n", filename);
  fprintf(stderr, "N=");
  BN_print_fp(stderr, rsa->n);
  fprintf(stderr, "\n");
  printf("P=");
  BN_print_fp(stderr, rsa->p);
  fprintf(stderr, "\n");
  fprintf(stderr, "Q=");
  BN_print_fp(stderr, rsa->q);
  fprintf(stderr, "\n");
  fprintf(stderr, "E=");
  BN_print_fp(stderr, rsa->e);
  fprintf(stderr, "\n");
  fprintf(stderr, "D=");
  BN_print_fp(stderr, rsa->d);
  fprintf(stderr, "\n");
  return rsa;
}

long long rsa_decrypt_min(RSA* key, BIGNUM *m) {
  BIGNUM *r = BN_new();
  BN_CTX *ctx = BN_CTX_new();
  timespec tbegin, tend;
  long long result = 1LL<<62;

  for (int i=0; i<1000; i++) {
    // BN_mod_exp(r, m, key->d, key->n, ctx);
    // BN_mod_exp_simple(r, m, key->d, key->n, ctx);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tbegin);
    BN_mod_exp_mont(r, m, key->d, key->n, ctx, NULL);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);
    long long start_time = tbegin.tv_sec*1000000000LL+tbegin.tv_nsec;
    long long end_time = tend.tv_sec*1000000000LL+tend.tv_nsec;
    result = min(result, end_time-start_time);
  }

  return result;
}

long long rsa_decrypt_sum(RSA* key, BIGNUM *m) {
  BIGNUM *r = BN_new();
  BN_CTX *ctx = BN_CTX_new();
  timespec tbegin, tend;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tbegin);

  for (int i=0; i<1000; i++) {
    // BN_mod_exp(r, m, key->d, key->n, ctx);
    // BN_mod_exp_simple(r, m, key->d, key->n, ctx);
    BN_mod_exp_mont(r, m, key->d, key->n, ctx, NULL);

  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);
  long long start_time = tbegin.tv_sec*1000000000LL+tbegin.tv_nsec;
  long long end_time = tend.tv_sec*1000000000LL+tend.tv_nsec;
  long long result = end_time-start_time;
  return result;
}

int main() {
  RSA *rsa = read_key("private512.pem");
  BIGNUM *a = BN_new();
  char s[1024];
  int cnt = 0;
  while (scanf("%s", s) == 1) {
    BN_dec2bn(&a, s);
    // fprintf(stderr, "M=");
    // BN_print_fp(stderr, a);
    // fprintf(stderr, "\n");
    long long time = rsa_decrypt_sum(rsa, a);
    printf("%s %lld\n", s, time);
    cnt++;
    if (cnt % 1000 == 0)
      fprintf(stderr, "Done with %d messages.\n", cnt);
  }
  return 0;
}
