#include <iostream>
#include <cstdlib>
#include <ctime>
// #include <ttmath/ttmath.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

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
  clock_t startTime = clock();
  for (int i=0; i<10000; i++) 
    BN_mod_exp(r, m, key->d, key->n, ctx);
  long long result = (long long) clock() - startTime;
  return result / 10000;
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
