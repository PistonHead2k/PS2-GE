#ifndef mathc
#define mathc

//The Good Ol Pi
#define PI 3.14159265358979323846f
//Taylor Series Terms
#define TERMS 7

/* Fast Abs */
inline float fabsf(float x) {
    const uint32_t mask = 0x7FFFFFFF;
    uint32_t bits = (*(uint32_t*)&x) & mask;
    return *(float*)&bits;
}

/* Fast Mod */
inline float fmodf(float x, float m) 
{
    return x - (int)(x / m) * m;
}

inline float fastsin(float x) {
    // wrap to [-PI, PI]
    x = fmodf(x + PI, 2*PI) - PI;

    const float B = 4.0f / PI;
    const float C = -4.0f / (PI * PI);

    float y = B * x + C * x * fabsf(x);

    // optional correction
    const float P = 0.225f;
    y = P * (y * fabsf(y) - y) + y;

    return y;
}

inline float fastcos(float x)
{
    /* cos is sin shifted 90 deg */
    return fastsin(x + PI * 0.5f);
}

inline float fastdeg2rad(float degrees)
{
    return degrees * (PI / 180.0f);
}

inline float fastrad2deg(float radians)
{
    return radians * (180.0f / PI);
}
#endif