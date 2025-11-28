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

/* Vector Math */
#include <math3d.h>
namespace Math3d
{

//Subtracts minend - subend and stores it in sub
void vector_sub(VECTOR sub, VECTOR minend, VECTOR subend)
{
  VECTOR work;
  work[0] = minend[0]-subend[0];
  work[1] = minend[1]-subend[1];
  work[2] = minend[2]-subend[2];
  work[3] = minend[3]-subend[3];
  vector_copy(sub, work);
}

//Returns 1 scalar value of the dot product of 2 vectors
void vector_dot(float out, VECTOR input0, VECTOR input1)
{
    out = input0[0] * input1[0] + input0[1] * input1[1] + input0[2] * input1[2];
}

};
#endif