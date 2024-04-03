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

    int case_i = 0;
    int case_o = 0;
    int case_n = 0;
    int case_v = 0;

    FILE *decrypt_input = stdin;
    FILE *decrypt_output = stdout;
    FILE *private_key;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            case_i = 1;
            decrypt_input = fopen(optarg, "r");
            break;
        case 'o':
            case_o = 1;
            decrypt_output = fopen(optarg, "w");
            break;
        case 'n':
            case_n = 1;
            private_key = fopen(optarg, "r");
            break;
        case 'h':
            printf("SYNOPSIS\n");
            printf("   Decrypts data using SS decryption.\n");
            printf("   Encrypted data is encrypted by the encrypt program.\n");
            printf("\n");
            printf("USAGE\n");
            printf("   ./decrypt [OPTIONS]\n");
            printf("\n");
            printf("OPTIONS\n");
            printf("   -h              Display program help and usage.\n");
            printf("   -v              Display verbose program output.\n");
            printf("   -i infile       Input file of data to decrypt (default: stdin).\n");
            printf("   -o outfile      Output file for decrypted data (default: stdout).\n");
            printf("   -n pvfile       Private key file (default: ss.priv).\n");
            return 0;
        case 'v': case_v = 1; break;
        default: break;
        }
    }

    if (case_n == 0) {
        private_key = fopen("ss.priv", "r");
    }

    if (decrypt_input == NULL) {
        printf("\nError Opening Input File\n");
        return 0;
    }
    if (decrypt_output == NULL) {
        printf("\nError Opening Output File\n");
        return 0;
    }
    if (private_key == NULL) {
        printf("\nError Opening Public Key File\n");
        return 0;
    }

    mpz_t pq, d;
    mpz_inits(pq, d, NULL);

    ss_read_priv(pq, d, private_key);

    if (case_v == 1) {
        gmp_printf("pq  (%d bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_printf("d  (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    ss_decrypt_file(decrypt_input, decrypt_output, d, pq);

    mpz_clears(d, pq, NULL);

    fclose(private_key);

    if (case_i == 1) {
        fclose(decrypt_input);
    }

    if (case_o == 1) {
        fclose(decrypt_output);
    }
    return 0;
}
