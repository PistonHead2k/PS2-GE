//USART Library by PistonHead2k


#ifndef USART
#define USART

namespace USART
{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Casts Signed 32-bit whole number to string. */
const char* ToString(int Value)
{
    static char Buffer[32];
    memset(Buffer, 0, sizeof(Buffer[0]) * 32); // Clear String
    sprintf(Buffer, "Value: %i\n", Value);
    return Buffer;
}

/* Casts Unsigned 32-bit whole number to string. */
const char* ToString(unsigned int Value)
{
    static char Buffer[32];
    memset(Buffer, 0, sizeof(Buffer[0]) * 32); // Clear String
    sprintf(Buffer, "Value: %u\n", Value);
    return Buffer;
}

/* Casts Signed 32-bit float to string */
const char* ToString(float Value)
{
    static char Buffer[32];
    memset(Buffer, 0, sizeof(Buffer[0]) * 32); // Clear String
    
    const char* tmpSign = (Value < 0) ? "-" : ""; //Sign (+ or -) 
    float tmpVal = (Value < 0) ? -Value : Value;

    uint32_t tmpInt1 = tmpVal;                  // Get the integer (678).
    float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
    uint32_t tmpInt2 = (uint32_t)(tmpFrac * 1e4);  // Turn into integer (123).

    sprintf(Buffer, "Value: %s%lu.%04lu\n", tmpSign, tmpInt1, tmpInt2);

    return Buffer;
}

/* Casts Signed 64-bit float to string */
const char* ToString(double Value)
{
    static char Buffer[32];
    memset(Buffer, 0, sizeof(Buffer[0]) * 32); // Clear String
    
    const char* tmpSign = (Value < 0) ? "-" : ""; //Sign (+ or -) 
    double tmpVal = (Value < 0) ? -Value : Value;

    uint32_t tmpInt1 = tmpVal;                  // Get the integer (678).
    double tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
    uint32_t tmpInt2 = (uint32_t)(tmpFrac * 1e8);  // Turn into integer (123).

    sprintf(Buffer, "Value: %s%lu.%08lu\n", tmpSign, tmpInt1, tmpInt2);

    return Buffer;
}
}
#endif