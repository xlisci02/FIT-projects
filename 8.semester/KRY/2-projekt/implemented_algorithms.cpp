#include <iostream>
#include <gmp.h>


// po NOVOM podlal geeks for geeks !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void pollard_rho_geeks(mpz_t result, mpz_t n, gmp_randstate_t state){
    // cout << "n: " << n << endl;
    mpz_t tmp;
    mpz_init(tmp);
    if(mpz_cmp_ui(n,1) == 0) // n=1
        mpz_set(result, n);
    if(mpz_even_p(n)) // n is even
        mpz_set_ui(result, 2);
    // generate from range <2,n) and store it to x and y
    mpz_t nminus1, nminus2, x, y;
    mpz_inits(nminus1, nminus2, x, NULL);
    mpz_sub_ui(nminus2, n, 2);
    mpz_urandomm(x, state, nminus2);
    mpz_add_ui(x,x,2);
    mpz_init_set(y,x);

    // generate from range <1,n) and store it to c
    mpz_t c;
    mpz_init(c);
    mpz_sub_ui(nminus1, n, 1);
    mpz_urandomm(c, state, nminus1);
    mpz_add_ui(c,c,1);
    // return;

    mpz_t d;
    mpz_init_set_ui(d, 1);

    mpz_t abs_diff;
    mpz_init(abs_diff);
    while(mpz_cmp_ui(d,1) == 0){
        // Tortoise move
        mpz_powm_ui(x, x,2,n);  //  x^2
        mpz_add(x, x, c); // x^2+c
        mpz_add(x, x, n); // x^2+c+n
        mpz_mod(x, x, n);  // (x^2+c+n) % n
        // cout << "x: " << x << endl;
        // Hare move
        mpz_powm_ui(y,y,2,n);  //  y^2
        mpz_add(y, y, c); // y^2+c
        mpz_add(y, y, n); // y^2+c+n
        mpz_mod(y, y, n); // (y^2+c+n) % n
        mpz_powm_ui(y, y,2,n);  //  y^2
        mpz_add(y, y, c); // y^2+c
        mpz_add(y, y, n); // y^2+c+n
        mpz_mod(y, y, n);  // (y^2+c+n) % n
        // cout << "y: " << y << endl;


        mpz_sub(abs_diff, x, y); // x-y
        mpz_abs(abs_diff, abs_diff); // |x-y|
        mpz_gcd(d,abs_diff,n);
        if(mpz_cmp(d,n) == 0)
        // {
        //     // generate new x and c
        //     mpz_urandomm(x, state, nminus2);
        //     mpz_add_ui(x,x,2);
        //     mpz_set(y,x);
        //     mpz_urandomm(c, state, nminus1);
        //     mpz_add_ui(c,c,1);
        //     mpz_set_ui(d, 1);
        // }
            pollard_rho_geeks(result, n, state);
        // cout << "i am here" << endl;
    }
    // cout << "halabal" << endl;
    mpz_set(result, d);
}


// podla WIKIPEDIE bez kniznice
long long int gcd11(long long int a, long long int b){
    while (a != b)
    {
        if (a > b)
            a = a - b;
        else
            b = b - a;
    }
    return a;
}

long long int g1(long long int x, long long int n){
    return (x*x+1)%n;
}

long long int Polar(long long n){
    long long int x = 2;
    long long int y = 2;
    long long int d = 1;

    while (d==1)
    {
        x = g1(x,n);
        y = g1(g1(y,n),n);
        d = gcd11(llabs(x-y),n);
    }

    if(d == n)
        return 0;
    else
        return d;

    // x ← 2
    // y ← 2
    // d ← 1

    // while d = 1:
    //     x ← g(x)
    //     y ← g(g(y))
    //     d ← gcd(|x - y|, n)

    // if d = n:
    //     return failure
    // else:
    //     return d
}


// https://www.geeksforgeeks.org/program-find-hcf-iteratively/
void gcd1(mpz_t gcd_of_ab, mpz_t a, mpz_t b){
    if(mpz_cmp(a,b) == 0 || (mpz_cmp_ui(b,0) == 0)){
        mpz_set(gcd_of_ab, a);
    }
    else if(mpz_cmp_ui(a,0) == 0){
        mpz_set(gcd_of_ab, b);
    }else{
        // aa and bb are copies of a and b
        mpz_t aa, bb;
        mpz_init_set(aa, a);
        mpz_init_set(bb, b);
        int i = 0;
        while (mpz_cmp(aa,bb) != 0)
        {
            // cout << "helo" << i++ << endl;
            if (mpz_cmp(aa,bb) > 0)
                mpz_sub(aa, aa, bb);
                // a = a - b;
            else
                mpz_sub(bb, bb, aa);
                // b = b - a;
            cout << "a: " << aa << " b: " << bb << endl;
        }
        cout << "idem prec" << (mpz_cmp_ui(a,1) == 0) << endl;
        mpz_set(gcd_of_ab, aa);
    }
}
