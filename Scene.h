//Scene Script by PistonHead2k

#ifndef __SCENE_H__
#define __SCENE_H__

/* Mesh */
struct mesh_t
{
    /* Mesh Name */
    char *Name;

    /* Number of Vertices */
    uint16_t VertexCount;
    /* Vertex Array */
    VECTOR *Vertex;

    /* Number of Faces */
    uint16_t FaceCount;
    /* Face Array */
    uint16_t *Face;

    /* Normal Array */
    VECTOR *Normal;
};

/* Scene */
struct scene_t
{
    /* Number of Meshes in Mesh[]*/
    uint8_t MeshCount; 
    /* Mesh Array */
    mesh_t *Mesh;
};

scene_t Scene;

#endif