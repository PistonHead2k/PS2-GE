#ifndef mathc
#define mathc

//The Good Ol Pi
#define PI 3.14159
//Taylor Series Terms
#define TERMS 7

float pow(float base, int exp) {
    if(exp < 0) {
        if(base == 0)
            return -0; // Error!!
        return 1 / (base * pow(base, (-exp) - 1));
    }
    if(exp == 0)
        return 1;
    if(exp == 1)
        return base;
    return base * pow(base, exp - 1);
}

int fact(int n) 
{
    return n <= 0 ? 1 : n * fact(n-1);
}

float sin(float deg)
{
    //deg %= 360; // make it less than 360
    float rad = deg * PI / 180;

    rad = deg;

    float sine = 0;

    int i;
    for(i = 0; i < TERMS; i++) { // That's Taylor series!!
        sine += pow(-1, i) * pow(rad, 2 * i + 1) / fact(2 * i + 1);
    }
    return sine;
}

float cos(float deg) {
    //deg %= 360; // make it less than 360
    float rad = deg * PI / 180;

    rad = deg;

    float cosine = 0;

    int i;
    for(i = 0; i < TERMS; i++) { // That's also Taylor series!!
        cosine += pow(-1, i) * pow(rad, 2 * i) / fact(2 * i);
    }
    return cosine;
}
#endif