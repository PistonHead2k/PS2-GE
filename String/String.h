//String Functions by PistonHead2k

#ifndef String_h
#define String_h

namespace String
{

#include <string.h>
/* Removes Substring from String */
void strremove(char *str, const char *substr) 
{
    char *p, *q, *r;
    if (*substr && (q = r = strstr(str, substr)) != NULL) {
       
        //Caps String Size as 254 + '\n' characters.
        uint8_t len = strlen(substr);

        while ((r = strstr(p = r + len, substr)) != NULL)
        {
            while (p < r) *q++ = *p++; 
        }

        while ((*q++ = *p++) != '\0') continue;
    }
    //return 0;
}

}

#endif