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
  fprintf(stderr, "P=");
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

long long mont_mult_min(RSA* key, BIGNUM *a, BIGNUM *b) {
  BIGNUM *r = BN_new();
  BIGNUM *ma = BN_new();
  BIGNUM *mb = BN_new();
  BN_CTX *ctx = BN_CTX_new();
  BN_MONT_CTX *mont = BN_MONT_CTX_new();
  BN_MONT_CTX_set(mont, key->n, ctx);
  BN_to_montgomery(ma, a, mont, ctx);
  BN_to_montgomery(mb, b, mont, ctx);
  timespec tbegin, tend;
  long long result = 1LL<<62;
  for (int i=0; i<1000; i++) {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tbegin);
    BN_mod_mul_montgomery(r, ma, mb, mont, ctx);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);
    long long start_time = tbegin.tv_sec*1000000000LL+tbegin.tv_nsec;
    long long end_time = tend.tv_sec*1000000000LL+tend.tv_nsec;
    result = min(result, end_time-start_time);
  }
  return result;
}

long long mont_mult_sum(RSA* key, BIGNUM *a, BIGNUM *b) {
  BIGNUM *r = BN_new();
  BN_CTX *ctx = BN_CTX_new();
  BIGNUM *ma = BN_new();
  BIGNUM *mb = BN_new();
  BN_MONT_CTX *mont = BN_MONT_CTX_new();
  BN_MONT_CTX_set(mont, key->n, ctx);
  BN_to_montgomery(ma, a, mont, ctx);
  BN_to_montgomery(mb, b, mont, ctx);
  timespec tbegin, tend;
  long long result = 1LL<<62;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tbegin);
  for (int i=0; i<1000; i++) {
    BN_mod_mul_montgomery(r, ma, mb, mont, ctx);
  }
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tend);
  long long start_time = tbegin.tv_sec*1000000000LL+tbegin.tv_nsec;
  long long end_time = tend.tv_sec*1000000000LL+tend.tv_nsec;
  result = end_time-start_time;
  return result;
}

int main() {
  RSA *rsa = read_key("private64.pem");
  BIGNUM *a = BN_new();
  BIGNUM *b = BN_new();
  char s[1024];
  char q[1024];
  int cnt = 0;
  while (scanf("%s %s", s, q) == 2) {
    BN_dec2bn(&a, s);
    BN_dec2bn(&b, q);
    long long time = mont_mult_min(rsa, a, b);
    printf("%s %s %lld\n", s, q, time);
    cnt++;
    if (cnt % 1000 == 0)
      fprintf(stderr, "Done with %d pairs.\n", cnt);
  }
  return 0;
}
