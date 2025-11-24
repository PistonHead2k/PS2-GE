//Render Library for PistonHead2k

#ifndef Render
#define Render

namespace Render
{

#include <graph.h>

#include <draw.h>
#include <draw3d.h>

#include <unistd.h> //for uint_t


#include <math3d.h> //for 3d math like calculate_vertices();
#include "Load/OBJ.h"

#include "mesh_data.c"

#include "LookAt.h"

void flip_buffers(packet_t *flip,framebuffer_t *frame)
{

	qword_t *q = flip->data;

	q = draw_framebuffer(q,0,frame);
	q = draw_finish(q);

	dma_wait_fast();
	dma_channel_send_normal_ucab(DMA_CHANNEL_GIF,flip->data,q - flip->data, 0);

	draw_wait_finish();

}

void init_drawing_environment(framebuffer_t *frame, zbuffer_t *z)
{

	packet_t *packet = packet_init(20,PACKET_NORMAL);

	// This is our generic qword pointer.
	qword_t *q = packet->data;

	// This will setup a default drawing environment.
	q = draw_setup_environment(q,0,frame,z);

	// Now reset the primitive origin to 2048-width/2,2048-height/2.
	q = draw_primitive_xyoffset(q, 0, (2048-320), (2048-256));

	// Finish setting up the environment.
	q = draw_finish(q);

	// Now send the packet, no need to wait since it's the first.
	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data,q - packet->data, 0, 0);
	dma_wait_fast();

	free(packet);

}

void init_gs(framebuffer_t *frame, zbuffer_t *z)
{

	// Define a 32-bit 640x512 framebuffer.
	frame->width = 640;
	frame->height = 512;
	frame->mask = 0;
	frame->psm = GS_PSM_32;

	// Allocate some vram for our framebuffer.
	frame->address = graph_vram_allocate(frame->width,frame->height, frame->psm, GRAPH_ALIGN_PAGE);

	frame++;

	frame->width = 640;
	frame->height = 512;
	frame->mask = 0;
	frame->psm = GS_PSM_32;

	// Allocate some vram for our framebuffer.
	frame->address = graph_vram_allocate(frame->width,frame->height, frame->psm, GRAPH_ALIGN_PAGE);

	// Enable the zbuffer.
	z->enable = DRAW_ENABLE;
	z->mask = 0;
	z->method = ZTEST_METHOD_GREATER_EQUAL;
	z->zsm = GS_ZBUF_32;
	z->address = graph_vram_allocate(frame->width,frame->height,z->zsm, GRAPH_ALIGN_PAGE);

	// Initialize the screen and tie the first framebuffer to the read circuits.
	graph_initialize(frame->address, frame->width, frame->height, frame->psm, 0, 0);

}

};

namespace Draw
{

VECTOR camera_position = { 0.00f, 0.00f, 5.00f, 1.00f };
VECTOR camera_rotation = { 0.00f, 0.00f,   0.00f, 1.00f };

//Camera Forward Vector
VECTOR camera_normal;

VECTOR *temp_normals;
VECTOR *temp_lights;
VECTOR *temp_colours;
VECTOR *temp_vertices;

xyz_t *xyz;
color_t *rgbaq;

int light_count = 2;

VECTOR light_direction[4] = 
{
	{  1.00f,  0.00f,  0.00f, 1.00f },
	{  1.00f,  0.00f,  0.00f, 1.00f }
};

VECTOR light_colour[4] = 
{
	{ 0.40f, 0.40f, 0.40f, 1.00f },
	{ 1.00f, 1.00f, 1.00f, 1.00f }
};

int light_type[4] = 
{
	LIGHT_AMBIENT,
	LIGHT_DIRECTIONAL
};
	

qword_t *teapot(qword_t *q, MATRIX view_screen, VECTOR object_position, VECTOR object_rotation, prim_t *prim, color_t *color, framebuffer_t *frame, zbuffer_t *z)
{

	int i;

	qword_t *dmatag;

	MATRIX local_world;
	MATRIX local_light;
	MATRIX world_view;

	MATRIX local_screen;

	// Now grab our qword pointer and increment past the dmatag.
	dmatag = q;
	q++;

	// Create the local_world matrix.
	create_local_world(local_world, object_position, object_rotation);

	// Create the local_light matrix.
	create_local_light(local_light, object_rotation);

	// Create the world_view matrix.
	create_world_view(world_view, camera_position, camera_rotation);
	
	// Create the local_screen matrix.
	create_local_screen(local_screen, local_world, world_view, view_screen);

	VECTOR Direction;
	Direction[0] = world_view[8];
	Direction[1] = world_view[9];
	Direction[2] = world_view[10];
	Direction[3] = world_view[11];
	vector_normalize(camera_normal, Direction);

	printf("x%f\n", world_view[8]);
	printf("y%f\n", world_view[9]);
	printf("z%f\n", world_view[10]);
	printf("w%f\n", world_view[11]);

	
	printf("nx%f\n", camera_normal[0]);
	printf("ny%f\n", camera_normal[1]);
	printf("nz%f\n", camera_normal[2]);
	printf("nw%f\n", camera_normal[3]);

	using namespace Load;
	uint16_t vertex_count = Scene.Mesh[Scene.MeshCount].VertexCount;
	VECTOR *vertices = Scene.Mesh[Scene.MeshCount].Vertex;

	VECTOR *normals = Scene.Mesh[Scene.MeshCount].Normal;

	//Debug
	static bool a = false;
	if (a == false)
	{
		for (int x = 0; x < vertex_count - 1; x++)for (int y = 0; y < 4; y++) printf(ToString(vertices[x][y]));
		a = true;
	}

	// Calculate the normal values.
    calculate_normals(temp_normals, vertex_count, normals, local_light);

	// Calculate the lighting values.
	calculate_lights(temp_lights, vertex_count, temp_normals, light_direction, light_colour, light_type, light_count);

	// Calculate the colour values after lighting.
	//const int size = 11;
	//VECTOR colours[size];
    //for (int c = 0; c < size; c++) for (int i = 0; i < 4; i++)colours[c][i] = 1.0f;
	calculate_colours(temp_colours, vertex_count, colours, temp_lights);

	// Calculate the vertex values.
	calculate_vertices(temp_vertices, vertex_count, vertices, local_screen);

	// Convert floating point vertices to fixed point and translate to center of screen.
	draw_convert_xyz(xyz, 2048, 2048, 32, vertex_count, (vertex_f_t*)temp_vertices);

	// Convert floating point colours to fixed point.
	draw_convert_rgbq(rgbaq, vertex_count, (vertex_f_t*)temp_vertices, (color_f_t*)temp_colours, color->a);

	// Draw the triangles using triangle primitive type.
	q = draw_prim_start(q, 0, prim, color);

	uint16_t points_count = Scene.Mesh[Scene.MeshCount].FaceCount;
	uint16_t *points = Scene.Mesh[Scene.MeshCount].Face;

	for(i = 0; i < points_count; i++)
	{
		q->dw[0] = rgbaq[points[i]].rgbaq;
		q->dw[1] = xyz[points[i]].xyz;
		q++;
	}

	q = draw_prim_end(q, 2, DRAW_RGBAQ_REGLIST);

	// Define our dmatag for the dma chain.
	DMATAG_CNT(dmatag,q-dmatag-1,0,0,0);


	return q;
}
};

#endif