#include <stdio.h>
#include "randstate.h"
#include "numtheory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    // p * p * q
    // Bits for p is between nbits/5 and (2*nbits)/5
    // Bits for p
    // Bits for p
    // remaining bits to q

    mpz_t calc1, calc2;
    mpz_inits(calc1, calc2, NULL);

    uint64_t lower_limit, upper_limit, bit_use;
    uint64_t total_bits = nbits;

    do {

        lower_limit = nbits / 5;

        upper_limit = (2 * nbits) / 5;

        upper_limit -= lower_limit;

        bit_use = random() % upper_limit;

        bit_use += lower_limit;

        make_prime(p, bit_use, iters);

        total_bits -= (bit_use * 2);

        make_prime(q, total_bits, iters);

        mpz_mul(n, p, p);
        mpz_mul(n, n, q);

        //calc1 is p-1
        mpz_sub_ui(calc1, p, 1);
        //Remainder of q div calc1
        mpz_mod(calc1, q, calc1);

        //calc2 is q-1
        mpz_sub_ui(calc2, q, 1);
        //Remainder of p div calc2
        mpz_mod(calc2, p, calc2);

    } while (mpz_cmp_ui(calc1, 0) == 0 && mpz_cmp_ui(calc2, 0) == 0);

    mpz_mul(n, p, p);
    mpz_mul(n, n, q);

    mpz_clears(calc1, calc2, NULL);
}

void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {

    //p * q is stored into pq
    //make x = p - 1
    //make y = q - 1
    //gcd (x, y) equal to d
    //then do z = x * y
    //then divide z by d

    mpz_mul(pq, p, q);

    mpz_t subp, subq, mul, n;
    mpz_inits(subp, subq, mul, n, NULL);

    mpz_mul(n, p, p);
    mpz_mul(n, n, q);

    mpz_sub_ui(subp, p, 1);
    mpz_sub_ui(subq, q, 1);

    gcd(d, subp, subq);

    mpz_mul(mul, subp, subq);

    mpz_fdiv_q(d, mul, d);

    mod_inverse(d, n, d);

    mpz_clears(subp, subq, mul, n, NULL);
}

void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {

    //We have to write the public ss key to the pbfile
    //the form of the key should be m, then the uesr name, each with a trailing new line
    //the value n should be written as a hex string

    gmp_fprintf(pbfile, "%Zx\n", n);

    fprintf(pbfile, "%s", username);
}

void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", pq);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    //First take in a line for the number
    //Then take in a line for the user name

    mpz_inp_str(n, pbfile, 16);

    fscanf(pbfile, "%s", username);
}

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    mpz_inp_str(pq, pvfile, 16);
    mpz_inp_str(d, pvfile, 16);
}

void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {

    pow_mod(c, m, n, n);
}

void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {

    //We calculate block size
    size_t x = mpz_sizeinbase(n, 2);
    x /= 2;
    x--;
    x /= 8;

    //We allocate block with size k
    uint8_t *block = (uint8_t *) calloc(x, sizeof(uint8_t));

    //We read up to x-1 bytes
    //Store bytes read in j

    size_t j = -1;

    for (j = fread(block, sizeof(uint8_t), x - 1, infile); j != 0;
         j = fread(block, sizeof(uint8_t), x - 1, infile)) {

        //Move all elements by 1 forward
        for (int i = j - 1; i >= 0; i--) {
            block[i + 1] = block[i];
        }

        //Make the first element equal to 0xFF
        block[0] = 0xFF;

        mpz_t imports, c;
        mpz_inits(imports, c, NULL);

        mpz_import(imports, j + 1, 1, sizeof(uint8_t), 1, 0, block);

        ss_encrypt(c, imports, n);

        gmp_fprintf(outfile, "%Zx\n", c);

        mpz_clears(imports, c);
    }

    free(block);
    block = NULL;
}

void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {

    pow_mod(m, c, d, pq);
}

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {

    //Find k size for block
    //Dynamically allocate array that can hold k
    size_t x = mpz_sizeinbase(pq, 2);
    x -= 1;
    x /= 8;

    uint8_t *block = (uint8_t *) calloc(x, sizeof(uint8_t));

    //Scan using fscan into c
    //Decrypt

    size_t j;

    mpz_t c, exports, m;
    mpz_inits(c, exports, m, NULL);

    while (gmp_fscanf(infile, "%ZX", c) != EOF) {

        ss_decrypt(m, c, d, pq);

        mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m);

        fwrite(&block[1], sizeof(uint8_t), j - 1, outfile);
    }
    mpz_clears(c, exports, m, NULL);
    free(block);
    block = NULL;
}
