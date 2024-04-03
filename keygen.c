#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "randstate.h"
#include "ss.h"

#include <sys/stat.h>
#include <fcntl.h>

#define OPTIONS "b:i:n:d:s:vh"

int main(int argc, char **argv) {

    int opt = 0;

    uint64_t min_bits = 256;
    uint64_t iters = 50;
    uint64_t seed;
    char *user;

    //int case_b = 0;
    //int case_i = 0;
    int case_n = 0;
    int case_d = 0;
    int case_s = 0;
    int case_v = 0;

    FILE *public_key;
    FILE *private_key;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b':
            //case_b = 1;
            min_bits = (uint64_t) strtoul(optarg, NULL, 10);
            break;
        case 'i':
            //case_i = 1;
            iters = (uint64_t) strtoul(optarg, NULL, 10);
            break;
        case 'n':
            case_n = 1;
            //iters = (uint64_t) strtoul(optarg, NULL, 10);
            public_key = fopen(optarg, "w");
            break;
        case 'd':
            case_d = 1;
            //iters = (uint64_t) strtoul(optarg, NULL, 10);
            private_key = fopen(optarg, "w");
            break;
        case 's':
            case_s = 1;
            seed = (uint64_t) strtoul(optarg, NULL, 10);
            srandom(seed);
            randstate_init(seed);
            printf("seed is %d", (int) seed);
            break;
        case 'v': case_v = 1; break;
        case 'h':
            printf("SYNOPSIS\n");
            printf("   Generates an SS public/private key pair.\n");
            printf("\n");
            printf("USAGE\n");
            printf("   ./keygen [OPTIONS]\n");
            printf("\n");
            printf("OPTIONS\n");
            printf("   -h              Display program help and usage.\n");
            printf("   -v              Display verbose program output.\n");
            printf("   -b bits         Minimum bits needed for public key n (default: 256).\n");
            printf(
                "   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n");
            printf("   -n pbfile       Public key file (default: ss.pub).\n");
            printf("   -d pvfile       Private key file (default: ss.priv).\n");
            printf("   -s seed         Random seed for testing.\n");
            return 0;
            //break;
        default: break;
        }
    }

    if (case_n == 0) {
        //This means no public key is specified so we default
        public_key = fopen("ss.pub", "w");
    }

    if (case_d == 0) {
        //This means no private key is specified so we default
        private_key = fopen("ss.priv", "w");
    }

    if (public_key == NULL || private_key == NULL) {
        printf("Error Opening File");
        return 0;
    }

    if (case_s == 0) {
        srandom(time(NULL));
        randstate_init(time(NULL));
    }

    fchmod(fileno(private_key), 0600);

    mpz_t p, q, n, d, pq;
    mpz_inits(p, q, n, d, pq, NULL);

    ss_make_pub(p, q, n, min_bits, iters);
    ss_make_priv(d, pq, p, q);

    user = getenv("USER");

    ss_write_pub(n, user, public_key);

    ss_write_priv(pq, d, private_key);

    //printf("minbits %" PRIu64 "\n", min_bits);
    //printf("iters %" PRIu64 "\n", iters);

    if (case_v == 1) {
        gmp_printf("user = %s\n", user);
        gmp_printf("p  (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q  (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n  (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("pq (%d bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_printf("d  (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    fclose(public_key);
    fclose(private_key);

    randstate_clear();

    mpz_clears(p, q, n, d, pq, NULL);
    return 0;
}
