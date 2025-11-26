//Filesystem Loader by PistonHead2k

#ifndef File_h
#define File_h

namespace Load
{

//Error Definitions
/* */
#define FILE_ERROR -1
//-----------------

const char *ErrorHandler(int Function) //Remember to add erorr codes
{
    switch(Function)
    {
        case FILE_ERROR:
        return "FILE ERROR";
        break;
        default: "FILE LOADED SUCCESFULLY";
    }
}

//-----------------


#include <stdio.h>

/* Opens a File in the Heap */
/* Make Sure to free() after use */
char* File(const char *fpath) //MAKE ERROR CODES
{
    //fopen in read mode.
    FILE *fstream = fopen(fpath, "r");

    //Seek how many bytes are in the file.
    fseek(fstream, 0L, SEEK_END);
    int fBytes = ftell(fstream);
    fseek(fstream, 0L, SEEK_SET);

    //char fstring[fBytes];
    char *fstring = (char*)malloc(fBytes);

    fread(fstring, 1, fBytes, fstream);
    fclose(fstream);
    
    //Prints The File Contents.
    //printf(fstring);

    return fstring;
}
};

#endif