//Main Script by PistonHead2k

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

#include "Render.h"
#include "Load/OBJ.h"

//Joystick
#include "Pad.h"



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

	packets[0] = packet_init(40000, PACKET_NORMAL);
	packets[1] = packet_init(40000, PACKET_NORMAL);

	// Uncached accelerated
	flip_pkt = packet_init(3,PACKET_UCAB);

	VECTOR object_position = { 0.00f, 0.00f, 0.00f, 1.00f };
	VECTOR object_rotation = { 0.0f, 0.0f, 0.0f, 1.00f };

	// Define the triangle primitive we want to use.
	prim.type = PRIM_TRIANGLE;
	prim.shading = PRIM_SHADE_GOURAUD;
	prim.mapping = DRAW_DISABLE;
	prim.mapping_type = DRAW_DISABLE;
	prim.fogging = DRAW_DISABLE;
	prim.blending = DRAW_ENABLE;
	prim.antialiasing = DRAW_DISABLE;
	prim.colorfix = PRIM_FIXED;

	color.r = 0x80;
	color.g = 0x80;
	color.b = 0x80;
	color.a = 0x80;
	color.q = 1.0f;

	using namespace Load;
	uint16_t vertex_count = Load::Scene.Mesh[Scene.MeshCount].VertexCount + 1;

	// Allocate calculation space.
	Draw::temp_normals  = (VECTOR*)memalign(128, sizeof(VECTOR) * vertex_count);
	Draw::temp_lights   = (VECTOR*)memalign(128, sizeof(VECTOR) * vertex_count);
	Draw::temp_colours  = (VECTOR*)memalign(128, sizeof(VECTOR) * vertex_count);
	Draw::temp_vertices = (VECTOR*)memalign(128, sizeof(VECTOR) * vertex_count);

	// Allocate register space.
	Draw::xyz   =   (xyz_t*)memalign(128, sizeof(u64) * vertex_count);
	Draw::rgbaq = (color_t*)memalign(128, sizeof(u64) * vertex_count);

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


		//Draw::camera_position[0] += float(IO::buttons.ljoy_h - 127) / 127 * 0.05f;
		//Draw::camera_position[2] += float(IO::buttons.ljoy_v - 127) / 127 * 0.05f; 

		Draw::camera_rotation[1] -= float(IO::buttons.rjoy_h - 127) / 127 * 0.005f; //Rotate Stuff
		Draw::camera_rotation[0] -= float(IO::buttons.rjoy_v - 127) / 127 * 0.005f; //Rotate Stuff


		VECTOR CameraForwardMovement;
		float x = float(IO::buttons.ljoy_v - 127) / 127 * 0.005f; //Rotate Stuff
		CameraForwardMovement[0] = x;
		CameraForwardMovement[1] = x;
		CameraForwardMovement[2] = x;

		VECTOR CameraPositionOut;
		int i = 1;
		i++;
		vector_multiply(CameraPositionOut, CameraForwardMovement, Draw::camera_normal);
		
		static VECTOR CameraPosition;
		vector_add(CameraPosition, CameraPosition, CameraPositionOut);
		vector_copy(Draw::camera_position, CameraPosition);
		

		//VECTOR Input;
		//Input[0] = float(IO::buttons.ljoy_h - 127) / 127 * 0.05f;
		//Input[1] = float(IO::buttons.ljoy_h - 127) / 127 * 0.05f;
		//Input[3] = 1.0f;

		

		//printf("h%d\n", IO::buttons.ljoy_v - 127);
		//printf("v%d\n", IO::buttons.ljoy_h - 127);
		//Draw::camera_rotation[2] += 0.05f; //Rotate Stuff

		if(IO::new_pad & PAD_LEFT) 
		{
        	printf("LEFT\n");
        }

		if(IO::new_pad & PAD_UP) 
		{
        	printf("UP\n");
        }
		

		q = Draw::teapot(q, view_screen, object_position, object_rotation, &prim, &color, frame, z);
		


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
		Render::flip_buffers(flip_pkt,&frame[context]);

	}

	free(packets[0]);
	free(packets[1]);

}

int main(int argc, char *argv[])
{
	Load::OBJ("CUBE.OBJ");
	printf("Finished Loading\n");

	// The buffers to be used.
	framebuffer_t fBuffer[2];
	zbuffer_t zBuffer;

	// Init GIF dma channel.
	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	// Init the GS, framebuffer, and zbuffer.
	Render::init_gs(fBuffer, &zBuffer);

	// Init the drawing environment and framebuffer.
	Render::init_drawing_environment(fBuffer, &zBuffer);

	// Render Call
	render(fBuffer, &zBuffer);

	// Sleep
	SleepThread();

	// End program.
	return 0;

}
