#include "numtheory.h"
#include <stdio.h>
#include <stdbool.h>
#include "randstate.h"

void gcd(mpz_t g, const mpz_t a, const mpz_t b) {

    mpz_t temp_a, temp_b, temp;
    mpz_inits(temp_a, temp_b, temp, NULL);
    mpz_set(temp_b, b);
    mpz_set(temp_a, a);

    while (mpz_cmp_ui(temp_b, 0) != 0) {
        mpz_set(temp, temp_b);
        mpz_mod(temp_b, temp_a, temp_b);
        mpz_set(temp_a, temp);
    }
    mpz_set(g, temp_a);

    mpz_clears(temp_a, temp_b, temp, NULL);
}

void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {

    mpz_t r1, r2, t1, t2, q, newTemp, newTemp2;
    mpz_inits(r1, r2, t1, t2, q, newTemp, newTemp2, NULL);

    mpz_set(r1, n);
    mpz_set(r2, a);
    mpz_set_ui(t1, 0);
    mpz_set_ui(t2, 1);

    while (mpz_cmp_ui(r2, 0) != 0) {
        mpz_fdiv_q(q, r1, r2);
        mpz_set(newTemp, r1);
        mpz_set(r1, r2);
        mpz_mul(newTemp2, q, r2);
        mpz_sub(r2, newTemp, newTemp2);

        mpz_set(newTemp, t1);
        mpz_set(t1, t2);
        mpz_mul(newTemp2, q, t2);
        mpz_sub(t2, newTemp, newTemp2);
    }
    if (mpz_cmp_ui(r1, 1) > 0) {
        mpz_set_ui(o, 0);
        return;
    }
    if (mpz_cmp_ui(t1, 0) < 0) {
        mpz_add(t1, t1, n);
    }
    mpz_set(o, t1);

    mpz_clears(r1, r2, t1, t2, q, newTemp, newTemp2, NULL);
}

void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {

    mpz_set_ui(o, 1);

    mpz_t p, new_d;
    mpz_inits(p, new_d, NULL);

    mpz_set(p, a);

    mpz_set(new_d, d);

    while (mpz_cmp_ui(new_d, 0) > 0) {
        if (mpz_odd_p(new_d) == 1) {
            mpz_mul(o, o, p);
            mpz_mod(o, o, n);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, n);
        mpz_fdiv_q_ui(new_d, new_d, 2);
    }
    mpz_clears(p, new_d, NULL);
}

bool is_prime(const mpz_t n, uint64_t iters) {

    if (mpz_cmp_ui(n, 2) == 0) {
        return true;
    }
    if (mpz_even_p(n) || mpz_cmp_ui(n, 1) <= 0) {
        return false;
    }

    mpz_t loop_r, exponent, r, s;
    mpz_inits(loop_r, exponent, r, s, NULL);

    mpz_set(loop_r, n);
    mpz_sub_ui(loop_r, loop_r, 1);

    mpz_set_ui(exponent, 1);

    mpz_set_ui(s, 0);

    do {
        mpz_add_ui(s, s, 1);
        mpz_mul_ui(exponent, exponent, 2);
        mpz_fdiv_q(r, loop_r, exponent);
    } while (mpz_even_p(r));

    mpz_t random_num, up_limit, y, ycopy, j, temp, temp2, num2;
    mpz_inits(random_num, up_limit, y, ycopy, j, temp, temp2, num2, NULL);
    mpz_sub_ui(up_limit, n, 4);

    mpz_set_ui(num2, 2);

    for (uint64_t i = 0; i < iters; i++) {
        mpz_urandomm(random_num, state, up_limit);
        mpz_add_ui(random_num, random_num, 2);
        pow_mod(y, random_num, r, n);

        mpz_sub_ui(temp, n, 1);

        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, temp) != 0) {
            mpz_set_ui(j, 1);
            mpz_sub_ui(temp2, s, 1);
            while ((mpz_cmp(j, temp2) <= 0) && (mpz_cmp(y, temp) != 0)) {
                mpz_set(ycopy, y);
                pow_mod(y, ycopy, num2, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(loop_r, exponent, r, s, random_num, up_limit, y, ycopy, j, temp,
                        temp2, num2, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, temp) != 0) {
                mpz_clears(loop_r, exponent, r, s, random_num, up_limit, y, ycopy, j, temp, temp2,
                    num2, NULL);
                return false;
            }
        }
    }

    mpz_clears(loop_r, exponent, r, s, random_num, up_limit, y, ycopy, j, temp, temp2, num2, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    bits++;

    mpz_t mpz_bits, temp, mul, size, lower, upper, num2, num1, mm;
    mpz_inits(mpz_bits, temp, mul, size, lower, upper, num2, num1, mm, NULL);

    mpz_set_ui(mpz_bits, bits);

    mpz_sub_ui(temp, mpz_bits, 1);

    mpz_set_ui(num2, 2);
    mpz_set_ui(num1, 1);

    while (true) {

        mpz_mul_2exp(upper, num1, bits);

        mpz_mul_2exp(lower, num1, bits - 1);

        mpz_sub_ui(upper, upper, 1);

        mpz_sub(upper, upper, lower);

        mpz_urandomm(p, state, upper);

        mpz_add(p, p, lower);

        if (is_prime(p, iters)) {
            break;
        }
    }

    mpz_clears(mpz_bits, temp, mul, size, lower, upper, num2, num1, mm, NULL);
}
