#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <gmp.h>

#include "randstate.h"
#include "ss.h"

#define OPTIONS "i:o:n:hv"

int main(int argc, char **argv) {

    int opt = 0;

    //char *user = NULL;
    char *user = (char *) calloc(99999, sizeof(char));

    int case_i = 0;
    int case_o = 0;
    int case_n = 0;
    int case_v = 0;

    FILE *encrypt_input = stdin;
    FILE *encrypt_output = stdout;
    FILE *public_key;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            case_i = 1;
            encrypt_input = fopen(optarg, "r");
            break;
        case 'o':
            case_o = 1;
            encrypt_output = fopen(optarg, "w");
            break;
        case 'n':
            case_n = 1;
            public_key = fopen(optarg, "r");
            break;
        case 'h':
            printf("SYNOPSIS\n");
            printf("   Encrypts data using SS encryption.\n");
            printf("   Encrypted data is decrypted by the decrypt program.\n");
            printf("\n");
            printf("USAGE\n");
            printf("   ./encrypt [OPTIONS]\n");
            printf("\n");
            printf("OPTIONS\n");
            printf("   -h              Display program help and usage.\n");
            printf("   -v              Display verbose program output.\n");
            printf("   -i infile       Input file of data to encrypt (default: stdin).\n");
            printf("   -o outfile      Output file for encrypted data (default: stdout).\n");
            printf("   -n pbfile       Public key file (default: ss.pub).\n");
            return 0;
        case 'v': case_v = 1; break;
        default: break;
        }
    }

    if (case_n == 0) {
        public_key = fopen("ss.pub", "r");
    }

    if (encrypt_input == NULL) {
        printf("\nError Opening Input File\n");
        return 0;
    }
    if (encrypt_output == NULL) {
        printf("\nError Opening Output File\n");
        return 0;
    }
    if (public_key == NULL) {
        printf("\nError Opening Public Key File\n");
        return 0;
    }

    mpz_t n;
    mpz_inits(n, NULL);

    ss_read_pub(n, user, public_key);

    if (case_v == 1) {
        gmp_printf("user = %s\n", user);
        gmp_printf("n  (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
    }

    ss_encrypt_file(encrypt_input, encrypt_output, n);

    mpz_clears(n, NULL);

    free(user);
    user = NULL;

    fclose(public_key);

    if (case_i == 1) {
        fclose(encrypt_input);
    }

    if (case_o == 1) {
        fclose(encrypt_output);
    }
    return 0;
}
