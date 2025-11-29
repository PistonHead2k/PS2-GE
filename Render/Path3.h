//Render Library for PistonHead2k

#ifndef Path3_h
#define Path3_h

/* Using Gif Channel (Slow) */
namespace Path3
{
/* REVISE st because it is partially loaded */
#include <graph.h>

#include <draw.h>
#include <draw3d.h>

#include <unistd.h> //for uint_t


#include <math3d.h> //for 3d math like calculate_vertices();
#include <kernel.h>
#include <stdlib.h>
#include <malloc.h>
#include <tamtypes.h>
#include <math3d.h>

#include <packet.h>

#include <dma_tags.h>
#include <gif_tags.h>
#include <gs_psm.h>

#include <dma.h>

#include <stdio.h>

//Joystick
#include "Pad.h"

#include "Def/cmath.h"

#include "Load/OBJ.h"

#include "mesh_data.c"

#include "String/Debug.h"

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

void init_gs(framebuffer_t *frame, zbuffer_t *z, texbuffer_t *texbuf)
{

	// Define a 32-bit 640x512 framebuffer.
	frame->width = 640; //640
	frame->height = 512; //512
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

	// Allocate some vram for the texture buffer
	texbuf->width = 256;
	texbuf->psm = GS_PSM_24;
	texbuf->address = graph_vram_allocate(256,256,GS_PSM_24,GRAPH_ALIGN_BLOCK);

	// Initialize the screen and tie the first framebuffer to the read circuits.
	graph_initialize(frame->address, frame->width, frame->height, frame->psm, 0, 0);

}

#include "texture.c"
extern unsigned char texture[];
void load_texture(texbuffer_t *texbuf)
{

	packet_t *packet = packet_init(50,PACKET_NORMAL);

	qword_t *q;

	q = packet->data;

	q = draw_texture_transfer(q, texture,256,256,GS_PSM_24,texbuf->address,texbuf->width);
	q = draw_texture_flush(q);

	dma_channel_send_chain(DMA_CHANNEL_GIF,packet->data, q - packet->data, 0,0);
	dma_wait_fast();

	packet_free(packet);

}

void setup_texture(texbuffer_t *texbuf)
{

	packet_t *packet = packet_init(10,PACKET_NORMAL);

	qword_t *q = packet->data;

	// Using a texture involves setting up a lot of information.
	clutbuffer_t clut;

	lod_t lod;

	lod.calculation = LOD_USE_K;
	lod.max_level = 0;
	lod.mag_filter = LOD_MAG_NEAREST;
	lod.min_filter = LOD_MIN_NEAREST;
	lod.l = 0;
	lod.k = 0;

	texbuf->info.width = draw_log2(256);
	texbuf->info.height = draw_log2(256);
	texbuf->info.components = TEXTURE_COMPONENTS_RGB;
	texbuf->info.function = TEXTURE_FUNCTION_DECAL;

	clut.storage_mode = CLUT_STORAGE_MODE1;
	clut.start = 0;
	clut.psm = 0;
	clut.load_method = CLUT_NO_LOAD;
	clut.address = 0;

	q = draw_texture_sampling(q,0,&lod);
	q = draw_texturebuffer(q,0,texbuf,&clut);

	// Now send the packet, no need to wait since it's the first.
	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data,q - packet->data, 0, 0);
	dma_wait_fast();

	packet_free(packet);

}

VECTOR camera_position = { 0.00f, 1.00f,  5.00f, 1.00f };
VECTOR camera_rotation = { 0.00f, 0.00f,  0.00f, 1.00f };

//Camera Forward Vector
VECTOR camera_normal;

VECTOR *temp_normals;
VECTOR *temp_lights;
VECTOR *temp_colours;
VECTOR *temp_vertices;

xyz_t *xyz;
color_t *rgbaq;
texel_t *st;

int light_count = 2;

VECTOR light_direction[2] = 
{
	{  1.00f,  0.00f,  0.00f, 1.00f },
	{  1.00f,  0.00f,  0.00f, 1.00f }
};

VECTOR light_colour[2] = 
{
	{ 0.50f, 0.50f, 0.50f, 1.00f },
	{ 1.00f, 1.00f, 1.00f, 1.00f }
};

int light_type[2] = 
{
	LIGHT_AMBIENT,
	LIGHT_DIRECTIONAL
};
	

qword_t *Model(qword_t *q, MATRIX view_screen, VECTOR object_position, VECTOR object_rotation, prim_t *prim, color_t *color, framebuffer_t *frame, zbuffer_t *z)
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

	/* Using World View Matrix To Get LookAt */
	VECTOR Direction;
	Direction[0] = -world_view[8];
	Direction[1] = -world_view[9]; //negative because the pitch is inverted relative to movement (inverted vertical diagonal)
	Direction[2] = world_view[10];
	//Direction[3] = world_view[11];
	vector_normalize(camera_normal, Direction);

	#if defined(DEBUG)
		Debug::pVector("camera normal", Direction);
	#endif

	using namespace Load;
	uint16_t vertex_count = Scene.Mesh[Scene.MeshCount].VertexCount;
	VECTOR *vertices = Scene.Mesh[Scene.MeshCount].Vertex;

	VECTOR *normals = Scene.Mesh[Scene.MeshCount].Normal;

	//Debug
	#if defined(DEBUG)
	static bool a;
	if (a == false)
	{
		for (int x = 0; x < vertex_count - 1; x++)for (int y = 0; y < 4; y++) printf(ToString(vertices[x][y]));
		a = true;
	}
	#endif

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

	// Generate the ST register values.
	draw_convert_st(st, vertex_count, (vertex_f_t*)temp_vertices, (texel_f_t*)coordinates);

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

VECTOR CameraPosition = { 0.00f, 0.00f,  5.00f, 1.00f };
//Camera Front Vector
VECTOR CameraFront = {0.0f, 0.0f, -1.0f, 1.0f};

bool a = 1;

void render(framebuffer_t *frame, zbuffer_t *z)
{
	//Init Pad
	IO::PadSetup();

	int context = 0;

	// Packets for doublebuffering dma sends
	packet_t *packets[2];
	packet_t *current;

	// This packet is special for framebuffer switching
	packet_t *flip_pkt;

	qword_t *q;

	prim_t prim;
	color_t color;

	MATRIX view_screen;

	packets[0] = packet_init(5000, PACKET_NORMAL);
	packets[1] = packet_init(5000, PACKET_NORMAL);

	// Uncached accelerated
	flip_pkt = packet_init(3,PACKET_UCAB);



	// Define the triangle primitive we want to use.
	prim.type = PRIM_TRIANGLE;
	prim.shading = PRIM_SHADE_GOURAUD;
	prim.mapping = DRAW_ENABLE;
	prim.mapping_type = PRIM_MAP_ST;
	prim.fogging = DRAW_DISABLE;
	prim.blending = DRAW_ENABLE;
	prim.antialiasing = DRAW_ENABLE;
	prim.colorfix = PRIM_UNFIXED;


	color.r = 0x80;
	color.g = 0x80;
	color.b = 0x80;
	color.a = 0x80;
	color.q = 1.0f;

	using namespace Load;
	uint16_t vertex_count = Load::Scene.Mesh[Scene.MeshCount].VertexCount + 1;

	// Allocate calculation space.
	temp_normals  = (VECTOR*)memalign(128, sizeof(VECTOR) * vertex_count);
	temp_lights   = (VECTOR*)memalign(128, sizeof(VECTOR) * vertex_count);
	temp_colours  = (VECTOR*)memalign(128, sizeof(VECTOR) * vertex_count);
	temp_vertices = (VECTOR*)memalign(128, sizeof(VECTOR) * vertex_count);

	// Allocate register space.
	xyz   =   (xyz_t*)memalign(128, sizeof(u64) * vertex_count);
	rgbaq = (color_t*)memalign(128, sizeof(u64) * vertex_count);
	st    = (texel_t*)memalign(128, sizeof(u64) * vertex_count);

	// Create the view_screen matrix.
	create_view_screen(view_screen, graph_aspect_ratio(), -3.00f, 3.00f, -3.00f, 3.00f, 1.00f, 2000.00f);

	while (true)
	{
		//Pad Poll
		IO::PadPoll();

		//if (IO::new_pad & PAD_TRIANGLE) printf("l1");

		qword_t *dmatag;

		current = packets[context];

		q = current->data;

		dmatag = q;
		q++;

		// Clear framebuffer without any pixel testing.
		q = draw_disable_tests(q,0,z);
		q = draw_clear(q,0,2048.0f-320.0f,2048.0f-256.0f,frame->width,frame->height, 20, 20, 20);
		q = draw_enable_tests(q,0,z);

		DMATAG_CNT(dmatag,q-dmatag - 1,0,0,0);

		//Render Stuff
		static float Yaw;
		static float Pitch;
		Yaw -= float(IO::buttons.rjoy_h - 127) / 127 * 0.01f; //Rotate Stuff
		Pitch -= float(IO::buttons.rjoy_v - 127) / 127 * 0.01f; //Rotate Stuff

		//Apply Rotation
		camera_rotation[0] = Pitch;
		camera_rotation[1] = Yaw;

		static float MoveX;
		static float MoveY;
		MoveX = -float(IO::buttons.ljoy_h - 127) / 127.0f * 0.1f; //Rotate Stuff
		MoveY = float(IO::buttons.ljoy_v - 127) / 127.0f * 0.1f; //Rotate Stuff

		VECTOR CameraFront;
		CameraFront[3] = 0.0f;
		CameraFront[2] = fastcos(Yaw) * fastcos(Pitch);
        CameraFront[1] = -fastsin(Pitch); //negative because the pitch is inverted relative to movement (inverted vertical diagonal)
        CameraFront[0] = fastsin(Yaw) * fastcos(Pitch);
		vector_normalize(CameraFront, CameraFront);
	
		#if defined(DEBUG)
			Debug::pVector("camera front", CameraFront, X|Y|Z);
		#endif

		vector_copy(CameraFront, camera_normal);

		VECTOR WorldUp = { 0.0f, 1.0f, 0.0f, 0.0f};
		VECTOR CameraRight;
		vector_cross_product(CameraRight, CameraFront, WorldUp);
		vector_normalize(CameraRight, CameraRight);


		camera_position[0] += CameraFront[0] * MoveY + CameraRight[0] * MoveX;
		camera_position[1] += CameraFront[1] * MoveY + CameraRight[1] * MoveX;
		camera_position[2] += CameraFront[2] * MoveY + CameraRight[2] * MoveX;

		vector_copy(light_direction[1], CameraFront);

		if(IO::new_pad & PAD_LEFT) 
		{
        	printf("LEFT\n");
        }

		if(IO::new_pad & PAD_UP) 
		{
        	printf("UP\n");
        }
		

		VECTOR object_position = { 0.00f, 0.00f, 0.00f, 1.00f };
		VECTOR object_rotation = { 0.0f, 0.0f, 0.0f, 1.00f };

		if (IO::new_pad & PAD_LEFT) a ^= 1;
		if (a) q = Model(q, view_screen, object_position, object_rotation, &prim, &color, frame, z);


		dmatag = q;
		q++;

		q = draw_finish(q);

		DMATAG_END(dmatag,q-dmatag-1,0,0,0);

		// Now send our current dma chain.
		dma_wait_fast();
		dma_channel_send_chain(DMA_CHANNEL_GIF,current->data, q - current->data, 0, 0);

		// Either block until a vsync, or keep rendering until there's one available.
		graph_wait_vsync();

		draw_wait_finish();
		graph_set_framebuffer_filtered(frame[context].address,frame[context].width,frame[context].psm,0,0);

		// Switch context.
		context ^= 1;

		// We need to flip buffers outside of the chain, for some reason,
		// so we use a separate small packet.
		flip_buffers(flip_pkt,&frame[context]);

	}

	free(packets[0]);
	free(packets[1]);

}
};

#endif