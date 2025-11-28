//Debug Script by Mateo Proruk

#if defined(DEBUG)

#ifndef Debug_h
#define Debug_h

#include <stdio.h>
#include <math3d.h>
#include <stdint.h>

namespace Debug
{
    /* Masks */
    #define X 0b1
    #define Y 0b10
    #define Z 0b100
    #define W 0b1000
    /* */
    
    /* Vector Output */
    void pVector(const char * Nametag, VECTOR Vector, uint8_t Mask = (X|Y|Z|W))
    {
	    if (Mask & X) printf("%s X: %f\n", Nametag, Vector[0]);
	    if (Mask & Y) printf("%s Y: %f\n", Nametag, Vector[1]);
	    if (Mask & Z) printf("%s Z: %f\n", Nametag, Vector[2]);
	    if (Mask & W) printf("%s W: %f\n", Nametag, Vector[3]);
    }
};

#endif
#endif
