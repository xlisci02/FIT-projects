// projekt: RSA
// autor: Matus Liscinsky
// login: xlisci02
// mail: xlisci02@stud.fit.vutbr.cz
// datum: 2.5.2020


#include<iostream>
#include <gmp.h>
#include <gmpxx.h>
#include <math.h>

#define USAGE_MSG "USAGE:\n\
Key Generation\n\
Input: ./kry -g B \n\
Output: P Q N E D \n\
\n\
Encryption\n\
Input: ./kry -e E N M\n\
Output: C\n\
\n\
Decryption (0.5b)\n\
Input: ./kry -d D N C\n\
Output: M\n\
\n\
Breaking RSA\n\
Input: ./kry -b E N C\n\
Output: P Q M"

#define K 64
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

enum actions{NO_ACTION,GENERATE,ENCRYPT,DECRYPT,BREAK_RSA};

using namespace std;

bool miller_rabin_test(mpz_t n, gmp_randstate_t state){
    if(mpz_even_p(n) || mpz_cmp_ui(n,3) <= 0)
        return (mpz_cmp_ui(n,3) == 0 || mpz_cmp_ui(n,2) == 0);
    bool can_be_prime;
    mpz_t d,r;
    mpz_inits(d,r, NULL);
    mpz_sub_ui(d, n, 1);
    do{
        mpz_div_ui(d, d, 2);
        mpz_add_ui(r,r,1);
    }
    while(mpz_even_p(d));

    mpz_t a, x, nminus3, nminus1, copy_r;
    mpz_inits(a,x, nminus1, nminus3, copy_r, NULL);
    mpz_sub_ui(nminus3, n, 3);
    mpz_sub_ui(nminus1, n, 1);
    for(int i = 0; i < K; i++){
        can_be_prime = false;
        // random number from interval <2;n-2>
        mpz_urandomm(a, state, nminus3);
        mpz_add_ui(a, a, 2);
        mpz_powm(x, a, d, n);
        if (mpz_cmp_ui(x, 1) == 0 || mpz_cmp(x, nminus1) == 0 )
            continue;
        mpz_set(copy_r, r);
        while(mpz_cmp_ui(copy_r, 1) != 0){
            mpz_powm_ui(x,x,2,n);
            if(mpz_cmp(x, nminus1) == 0)
                can_be_prime = true;
            mpz_sub_ui(copy_r, copy_r, 1);
        }
        if(!can_be_prime){
            mpz_clears(d,r,a,x,nminus1,nminus3,copy_r,NULL);
            return false;

        }
    }
    mpz_clears(d,r,a,x,nminus1,nminus3,copy_r,NULL);
    return true;
}


void gcd_custom(mpz_t gcd_of_ab, mpz_t a, mpz_t b){

    mpz_t t;
    mpz_init(t);
    mpz_t aa, bb;
    mpz_init_set(aa, a);
    mpz_init_set(bb, b);
    while (mpz_cmp_ui(bb, 0) != 0){
        // t = b;
        mpz_set(t,bb);
        // b = a mod b;
        mpz_mod(bb,aa,bb);
        // a = t;
        mpz_set(aa,t);
    }
    mpz_set(gcd_of_ab, aa);
    mpz_clears(t,aa,bb,NULL);
}


bool gcd_is1(mpz_t a, mpz_t b){
    mpz_t gcd_of_ab;
    mpz_init(gcd_of_ab);
    gcd_custom(gcd_of_ab, a, b);
    bool res = (mpz_cmp_ui(gcd_of_ab,1) == 0);
    mpz_clear(gcd_of_ab);
    return res;
}

void get_prime_num(mpz_t number, mp_bitcnt_t bit_range, gmp_randstate_t state){
    do{
        mpz_urandomb(number, state, bit_range);
        // test size in base get how many bites takes the number, we want n-bit number
    }while(mpz_sizeinbase(number,2) != bit_range || !miller_rabin_test(number, state));
}

void get_e(mpz_t e, mpz_t phi_n, gmp_randstate_t state){
    do{
        mpz_urandomm(e, state, phi_n);
    }while((mpz_cmp_ui(e, 1) <= 0) || !gcd_is1(e, phi_n));
}

void extended_euclidean_algorithm(mpz_t a, mpz_t b, mpz_t x){
    mpz_t aa, bb,y,u,v,q,tmp,m,n,r;
    // aa and bb are copies of a and b
    mpz_inits(v,q,tmp,m,n,r, NULL);
    mpz_init_set(aa, a);
    mpz_init_set(bb, b);
    mpz_init_set_ui(u,1);
    mpz_init_set_ui(y,1);
    while(mpz_cmp_ui(aa, 0) != 0){
        mpz_div(q,bb,aa); // q = b/a
        mpz_mod(r,bb,aa); // r = b%a

        mpz_mul(tmp,u,q); // tmp = u*q
        mpz_sub(m, x, tmp); // m = x - u*q
        mpz_mul(tmp,v,q); // tmp = v*q
        mpz_sub(n, y, tmp); // n = y - v*q

        mpz_set(bb,aa);
        mpz_set(aa,r);
        mpz_set(x,u);
        mpz_set(y,v);
        mpz_set(u,m);
        mpz_set(v,n);
    }
    mpz_clears(aa, bb,y,u,v,q,tmp,m,n,r,NULL);
}

void eulers_totient(mpz_t phi_n, mpz_t p, mpz_t q){
    mpz_t pminus1,qminus1;
    mpz_inits(pminus1,qminus1, NULL);
    mpz_sub_ui(pminus1, p, 1);
    mpz_sub_ui(qminus1, q, 1);
    mpz_mul(phi_n, pminus1, qminus1);
    mpz_clears(pminus1,qminus1,NULL);
}


int encrypt(mpz_t message, mpz_t e, mpz_t n){
    if(mpz_sizeinbase(message,2) >= mpz_sizeinbase(n,2)){
        cerr << "Message has to be shorter than a public modulus!" << endl;
        return EXIT_FAILURE;
    }
    mpz_t cipher;
    mpz_init(cipher);
    mpz_powm(cipher,message,e,n);
    cout << hex << showbase << cipher << endl;
    mpz_clear(cipher);
    return EXIT_SUCCESS;
}

void decrypt(mpz_t cipher, mpz_t d, mpz_t n){
    mpz_t message;
    mpz_init(message);
    mpz_powm(message,cipher,d,n);
    cout << hex << showbase << message << endl;
    mpz_clear(message);
}

int generate_keys(mp_bitcnt_t bit_range_for_n){
    mpz_t p,q,n,phi_n,e,d;
    mpz_inits(n,p,q,phi_n,e, d, NULL);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    get_prime_num(p, bit_range_for_n/2, state); // p
    do{
        get_prime_num(q, bit_range_for_n-bit_range_for_n/2, state); // q
    } while (mpz_cmp(p,q) == 0);

    mpz_mul(n,p,q); // n = p * q
    eulers_totient(phi_n,p,q); // phi_n = (p-1) * (q-1)
    get_e(e, phi_n, state); // e

    // private key
    extended_euclidean_algorithm(e, phi_n, d); // inverse public key
    mpz_mod(d, d, phi_n); // inverse public key mod phi_n = private key (d)

    if(mpz_cmp(e,d) == 0){
        cerr << "Public exponent is equal to private exponent, e=d=" << e << endl;
        mpz_clears(p,q,n,phi_n,e,d, NULL);
        gmp_randclear(state);
        return EXIT_FAILURE;
    }
    cout << hex << showbase << p << " " << q << " " << n  << " " << e  << " " << d << endl;
    mpz_clears(p,q,n,phi_n,e,d, NULL);
    gmp_randclear(state);
    return EXIT_SUCCESS;
}


// y = (x^2 + 1) mod n
void g(mpz_t y, mpz_t x, mpz_t n){
    mpz_t xpow2;
    mpz_init(xpow2);
    mpz_pow_ui(xpow2,x,2); // x^2
    mpz_add_ui(xpow2,xpow2, 1); // x^2 + 1
    mpz_mod(y, xpow2, n); // (x^2 + 1) mod n
    mpz_clear(xpow2);
}

int pollard_rho(mpz_t result, mpz_t n, gmp_randstate_t state){
    mpz_t x,y,d,nminus2,abs_diff;
    mpz_inits(x,abs_diff,nminus2,NULL);
    mpz_init_set_ui(d, 1);

    // generate number from range <2,n) and store it to x and y
    mpz_sub_ui(nminus2, n, 2);
    mpz_urandomm(x, state, nminus2);
    mpz_add_ui(x,x,2);
    mpz_init_set(y,x);

    while(mpz_cmp_ui(d,1) == 0){
        // x = g(x)
        g(x, x, n);

        // y = g(g(y))
        g(y, y, n);
        g(y, y, n);

        // d = gcd(|x - y|, n)
        mpz_sub(abs_diff, x,y);
        mpz_abs(abs_diff, abs_diff);
        mpz_gcd(d, abs_diff, n);
    }
    if(mpz_cmp(d,n) == 0){
        mpz_set(result, d);
        mpz_clears(x,y,d,abs_diff,nminus2,NULL);
        return EXIT_FAILURE; // error
    }
    mpz_set(result, d);
    mpz_clears(x,y,d,abs_diff,nminus2,NULL);
    return EXIT_SUCCESS;
}



int get_action(int argc, char ** argv){
    // -g -e -d -b
    if (argc < 3 || argc > 5)
        return NO_ACTION;
    if(argv[1][0] == '-' && strlen(argv[1]) == 2){
        switch (argv[1][1]){
        case 'g': if (argc == 3) return GENERATE; break;
        case 'e': if (argc == 5) return ENCRYPT; break;
        case 'd': if (argc == 5) return DECRYPT; break;
        case 'b': if (argc == 5) return BREAK_RSA; break;
        default: break;
        }
    }
    return NO_ACTION;
}


int break_rsa(mpz_t cipher, mpz_t e, mpz_t n){

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    mpz_t p, q, phi_n, d;
    mpz_inits(p, q, phi_n, d, NULL);

    // checking if p is a prime number
    if(miller_rabin_test(n,state)){
        cerr << "Public modulus cannot be a prime number!" << endl;
        mpz_clears(p, q, phi_n, d,NULL);
        gmp_randclear(state);
        return EXIT_FAILURE;
    }

    // checking if n = p^2
    if(mpz_perfect_square_p(n)){
        mpz_sqrt(p,n);
        mpz_set(q,p);
    }
    else
        // getting p, try until success, note: Pollard Rho may fail
        while(pollard_rho(p, n, state));

    mpz_div(q,n,p); // q = n/p

    eulers_totient(phi_n,p,q); // phi_n = (p-1) * (q-1)

    extended_euclidean_algorithm(e, phi_n, d); // inverse public key
    mpz_mod(d, d, phi_n); // inverse public key mod phi_n = private key
    cout << hex << showbase << p << " " << q << " ";
    decrypt(cipher,d, n);
    mpz_clears(p, q, phi_n, d,NULL);
    gmp_randclear(state);
    return EXIT_SUCCESS;
}

int load_hexa_vals(mpz_t a, mpz_t b, mpz_t c, char ** argv){
    // hexa numbers check
    char * endptr;
    for(int i = 2; i < 5; i++){
        endptr = NULL;
        strtoul(argv[i], &endptr, 16);
        if(*endptr != '\0' || strncmp(argv[2],"0x",2) != 0){
            cerr << "Program is expecting values in hexadecimal format starting with \"0x\"." << endl;
            return EXIT_FAILURE;
        }
    }
    mpz_init_set_str(a, (argv[2]+2), 16); // +2, skip 0x
    mpz_init_set_str(b, (argv[3]+2), 16);
    mpz_init_set_str(c, (argv[4]+2), 16);
    return EXIT_SUCCESS;
}

int main(int argc, char ** argv){
    int action = get_action(argc, argv);
    if(!action){
        cerr << USAGE_MSG << endl;
        return EXIT_FAILURE;
    }
    switch (action){
    case GENERATE:{
            int bit_range_int = stoi(argv[2],nullptr);
            if(bit_range_int <= 4){
                cerr << "Parameter B has to a be positive integer greater than 4." << endl;
                return EXIT_FAILURE;
            }
            mp_bitcnt_t bit_range_for_n = (mp_bitcnt_t)bit_range_int ;
            if(generate_keys(bit_range_for_n))
                return EXIT_FAILURE;
        }
        break;

    case ENCRYPT:{
            mpz_t message, e, n;
            if(load_hexa_vals(e, n, message, argv))
                return EXIT_FAILURE;
            if(encrypt(message,e,n)){
                mpz_clears(e, n, message,NULL);
                return EXIT_FAILURE;
            }
            mpz_clears(e, n, message,NULL);
        }
        break;

    case DECRYPT:{
            mpz_t cipher, d, n;
            if(load_hexa_vals(d, n, cipher, argv))
                return EXIT_FAILURE;
            decrypt(cipher,d,n);
            mpz_clears(cipher,d,n,NULL);
        }
        break;

    case BREAK_RSA:{
            mpz_t cipher, e, n;
            if(load_hexa_vals(e,n,cipher,argv))
                return EXIT_FAILURE;
            if(break_rsa(cipher,e,n)){
                mpz_clears(cipher,e,n,NULL);
                return EXIT_FAILURE;
            }
            mpz_clears(cipher,e,n,NULL);
        }
        break;
    default: break;
    }
    return EXIT_SUCCESS;
}
