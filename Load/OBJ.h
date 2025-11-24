//Mesh Loader by PistonHead2k

#ifndef OBJ_h
#define OBJ_h

#include "Load/File.h"
namespace Load
{

#include <stdio.h>
#include <unistd.h>

#include "USART.h"

//For OBJ Interpreter
#include <stdlib.h>
#include "String/String.h"
using namespace String;
#include "Scene.h"

//Declare Temporary Normal Array
VECTOR *temp_Normal;
//Declare NormalCount
uint16_t NormalCount = 0;
/* OBJ Format Interpreter */
void OBJInterpreter(char *lstring) //Line String
{   
    //Buffer String
    char* bstring;

    //Detect Comment In The Line
    bstring = strchr(lstring, '#');
    //Removes The Comment From The Line
    strremove(lstring, bstring);
    
    /* Mesh Detection */
    //Detect The Character 'o' in a line wich means object name
    if ((bstring = strchr(lstring, 'o')) != NULL)
    {
        //Removes 'o ' from the line
        strremove(lstring, "o ");
        
        //Heap Arrays Start at arr[1] rather than arr[0]
        Scene.MeshCount++;

        /* Reallocate Mesh[] */
        Scene.Mesh = (mesh_t*)realloc(Scene.Mesh, (Scene.MeshCount + 1) * sizeof(mesh_t)); //Dont Forget to free()
        
        //When Scene.Mesh is Reallocated, Triangle Will Contain Garbage, Wich will make realloc() crash on it... Fixed
        Scene.Mesh[Scene.MeshCount].Name = nullptr;
        Scene.Mesh[Scene.MeshCount].VertexCount = NULL;
        Scene.Mesh[Scene.MeshCount].Vertex = nullptr;
        Scene.Mesh[Scene.MeshCount].FaceCount = NULL;
        Scene.Mesh[Scene.MeshCount].Face = nullptr;
        Scene.Mesh[Scene.MeshCount].Normal = nullptr;

        //We Update the name of the mesh
        Scene.Mesh[Scene.MeshCount].Name = lstring;
        return;
    }

    /* Vertex Loader */
    //Detect The Character 'v' in a line wich means vertex
    if (lstring[0] == 'v' && lstring[1] == ' ')
    {
        bstring = lstring;

        //Removes 'v ' from the line
        strremove(bstring, "v ");

        /* Reallocate Vertex[] Array */
        Scene.Mesh[Scene.MeshCount].Vertex
        = (VECTOR*)realloc(Scene.Mesh[Scene.MeshCount].Vertex,
                          (Scene.Mesh[Scene.MeshCount].VertexCount + 1) * uint16_t(sizeof(VECTOR)));
        
        /* Separates each value using the space between them as reference I.E.: "-1.000000 " for X, and so on*/
        //We Iterate 3 Times for X, Y, and Z
        for (int i = 0; i < 3; i++)
        {
            switch (i) 
            {
                case 0: bstring = strtok(bstring, " "); break;
                default: bstring = strtok(nullptr, " "); break;
            }

            float XYZ = atof(bstring);
        
            /* We Update The Vertex[] Array */
            Scene.Mesh[Scene.MeshCount].Vertex[Scene.Mesh[Scene.MeshCount].VertexCount][i] = XYZ;
        }
        
        //Debug
        printf("Vertex Count ");
        printf(ToString((int)Scene.Mesh[Scene.MeshCount].VertexCount));
        //*****

        /*Scene.Mesh[Scene.MeshCount].Vertex[Scene.Mesh[Scene.MeshCount].VertexCount][0] = X;
        Scene.Mesh[Scene.MeshCount].Vertex[Scene.Mesh[Scene.MeshCount].VertexCount][1] = Y;
        Scene.Mesh[Scene.MeshCount].Vertex[Scene.Mesh[Scene.MeshCount].VertexCount][2] = Z;
        Scene.Mesh[Scene.MeshCount].Vertex[Scene.Mesh[Scene.MeshCount].VertexCount][3] = 1.0f;*/


        //Debug
        printf("Vertex ");
        for (int i = 0; i < 4; i++) printf(ToString(Scene.Mesh[Scene.MeshCount].Vertex[Scene.Mesh->VertexCount][i]));

        Scene.Mesh[Scene.MeshCount].VertexCount++;
        return;
    }

    /* Normal Interpreter */
    if (lstring[0] == 'v' && lstring[1] == 'n')
    {
        bstring = lstring;

        //Removes 'vn ' from the line
        strremove(bstring, "vn ");
        
        /* Reallocate temp_Normal[] Array */
        temp_Normal = (VECTOR*)realloc(temp_Normal, (NormalCount + 1) * uint16_t(sizeof(VECTOR)));
        
        /* Separates each value using the space between them as reference I.E.: "-1.000000 " for X, and so on*/
        //We Iterate 3 Times for X, Y, and Z
        for (int i = 0; i < 3; i++)
        {
            switch (i) 
            {
                case 0: bstring = strtok(bstring, " "); break;
                default: bstring = strtok(nullptr, " "); break;
            }

            float nXYZ = atof(bstring);
        
            /* We Update The Normal[] Array */
            temp_Normal[NormalCount][i] = nXYZ;
        }

        //Debug
        printf("Temporary Normals: \n");
        for (int i = 0; i < 3; i++) printf(ToString(temp_Normal[NormalCount][i]));
       
        
        

        NormalCount++;
        return;
    }

    //Detect Smooth Shading Flag In The Line
    bstring = strchr(lstring, 's');
    //Removes The Flag Altogheter (Does The PS2 Support Smooth Shading?)
    strremove(lstring, bstring);

    /* Face Interpreter */
    if (lstring[0] == 'f')
    {
        //Removes 'f ' from the line
        strremove(lstring, "f ");

        //Makes a Copy of lstring to work cleanly with strtok
        char* pb = bstring = (char*)malloc(strlen(lstring) + 1);
        strcpy(bstring, lstring);
    
        /* Reallocate Face[] Array */
        Scene.Mesh[Scene.MeshCount].Face
        = (uint16_t*)realloc(Scene.Mesh[Scene.MeshCount].Face,   //* 3 because we need 3 ints to store a face in the array
                            (Scene.Mesh[Scene.MeshCount].FaceCount + 1) * 3 * sizeof(uint16_t));

        /* Separates each value using the space between them as reference */
        //We Iterate 3 Times for V0, V1, and V2
        for (int i = 0; i < 3; i++)
        {
            switch (i) 
            {
                case 0: bstring = strtok(bstring, " "); break;
                default: bstring = strtok(nullptr, " "); break;
            }
            //We Subtract 1 Because The OBJ Face Index Start at 1, so we make it start at 0.
            uint16_t XYZ = atoi(bstring) - 1;

            /* We Update The Face[] Array */
            Scene.Mesh[Scene.MeshCount].Face[Scene.Mesh[Scene.MeshCount].FaceCount + i] = XYZ;
        }

        free(pb);
        pb = nullptr;

        //Debug
        printf("Face Count ");
        printf(ToString(Scene.Mesh[Scene.MeshCount].FaceCount / 3));

        //Debug
        for (int i = 0; i < 3; i++) printf(ToString(Scene.Mesh[Scene.MeshCount].Face[Scene.Mesh[Scene.MeshCount].FaceCount + i]));

        Scene.Mesh[Scene.MeshCount].FaceCount += 3;
    }

    /* Normal Index Interpreter */
    if (strchr(lstring, '/') != NULL)
    {
        //Makes a Copy of lstring to work cleanly with strtok
        char* pb = bstring = (char*)malloc(strlen(lstring) + 1);
        strcpy(bstring, lstring);
        
        /* Separates each value using the "//" between them as reference */
        //We Iterate 3 Times for V0, V1, and V2
        //We Subtract 1 Because The OBJ Face Index Start at 1, so we make it start at 0.
        bstring = strtok(bstring, "//");
        for (int i = 0; i < 3; i++)
        {
            bstring = strtok(nullptr, "//");
            uint16_t niXYZ = atoi(bstring) - 1;

            /* Reallocate Normal[] Array */
            Scene.Mesh[Scene.MeshCount].Normal
            = (VECTOR*)realloc(Scene.Mesh[Scene.MeshCount].Normal,
                          (NormalCount + 1) * uint16_t(sizeof(VECTOR)));

            Scene.Mesh[Scene.MeshCount].Normal[NormalCount][i] = temp_Normal[niXYZ][i];
        }

        //Debug
        printf("Normals: \n");
        for (int i = 0; i < 3; i++) printf(ToString(Scene.Mesh[Scene.MeshCount].Normal[NormalCount][i]));

        //We Grow The Normal[] Array
        NormalCount++;
    

        free(pb);
        pb = nullptr;
    }
    
    printf("\n");
}

/* OBJ Loader */
void OBJ(const char *fpath)
{
    char *fstring = File(fpath);
    
    /* Allocate VBO */
    //Vertex = (vertex_t*)malloc(sizeof(vertex_t));

    //We Iterate line by line
    char *line;
    while(fstring)
    {
        char * nextLine = strchr(fstring, '\n');
        if (nextLine) *nextLine = '\0';  // temporarily terminate the current line

        //printf("curLine=[%s]\n", fstring);
        OBJInterpreter(fstring);

        if (nextLine) *nextLine = '\n';  // then restore newline-char, just to be tidy    
        fstring = nextLine ? (nextLine+1) : NULL;
    }

    //Don't forget eh!
    free(fstring); //File Buffer
    fstring = nullptr;
    //Free Temp Normals
    free(temp_Normal);
    temp_Normal = nullptr;
    //Reset NormalCount
    NormalCount = 0;
}

}
#endif