//Main Script by PistonHead2k

/* Debug Info in Console */
//#define DEBUG

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

#include "Render/Path3.h"
//#include "Render/Path1.h"

#include "Load/OBJ.h"

//Joystick
#include "Pad.h"

#include "Def/cmath.h"

int main(int argc, char *argv[])
{
	Path3::Load::OBJ("CUBE.OBJ");
	printf("Finished Loading\n");

	// The buffers to be used.
	Path3::framebuffer_t fBuffer[2];
	Path3::zbuffer_t zBuffer;
	Path3::texbuffer_t texbuf;

	// Init GIF dma channel.
	dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
	dma_channel_fast_waits(DMA_CHANNEL_GIF);

	// Init the GS, framebuffer, and zbuffer.
	Path3::init_gs(fBuffer, &zBuffer, &texbuf);

	// Init the drawing environment and framebuffer.
	Path3::init_drawing_environment(fBuffer, &zBuffer);

	// Load the texture into vram.
	Path3::load_texture(&texbuf);

	// Setup texture buffer
	Path3::setup_texture(&texbuf);

	// Render Call
	Path3::render(fBuffer, &zBuffer);

	// Sleep
	SleepThread();

	// End program.
	return 0;

}
