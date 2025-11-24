#ifndef LookAt
#define LookAt


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

void lookAtMatrix(MATRIX out, VECTOR positionOfCamera)
{
    VECTOR forwardVector = {0.0f, 0.0f, -1.0f, 1.0f};
    
    VECTOR positionOfTarget;
    vector_sub(forwardVector, positionOfCamera, positionOfTarget);
    vector_normalize(forwardVector,  forwardVector);
    

    VECTOR rightVector = {1.0f, 0.0f, 0.0f, 1.0f};
    VECTOR tempUpVector = {0.0f, 1.0f, 0.0f, 1.0f};
    vector_cross_product(rightVector, tempUpVector, forwardVector);
    vector_normalize(rightVector, rightVector);

    VECTOR upVector;
    vector_cross_product(upVector, forwardVector, rightVector);
    vector_normalize(upVector, upVector);

    float translationX;
    vector_dot(translationX, positionOfCamera, rightVector);
    float translationY;
    vector_dot(translationY, positionOfCamera, upVector);
    float translationZ;
    vector_dot(translationZ, positionOfCamera, forwardVector);

    out[0] = rightVector[0];
    out[1] = upVector[0];
    out[2] =  forwardVector[0];
    out[3] = 0.0f;

    out[4] = rightVector[1];
    out[5] = upVector[1];
    out[6] = forwardVector[1];
    out[7] =  0.0f;

    out[8] = rightVector[2],
    out[9] = upVector[2],
    out[10] = forwardVector[2],
    out[11] = 0.0f,

    out[12] = -translationX;
    out[13] = -translationY;
    out[14] = -translationZ;
    out[15] = 1.0f;
}
};

#endif