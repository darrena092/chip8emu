#include <stdio.h>
#include <stdint.h>
#include "video.h"
#include "debug.h"

void c_Video::Init(void) {
    dbgPrint(DBG_LV0, DBG_VID, "Initialising.");
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
        dbgPrint(DBG_LV0, DBG_VID, "SDL could not initialise. SDL_Error: %s", SDL_GetError());
	}
    else {
        window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if( window == NULL )
		{
			dbgPrint(DBG_LV0, DBG_VID, "Window could not be created! SDL_Error: %s", SDL_GetError());
		}
		else {
            rendersurface = SDL_CreateRenderer(window, -1, 0);
            SDL_SetRenderDrawColor(rendersurface, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(rendersurface);
            SDL_RenderPresent(rendersurface);

            drawBuffer = SDL_CreateTexture(rendersurface, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
		}
    }
    extended = false;
}

void c_Video::Reset(void) {
    dbgPrint(DBG_LV0, DBG_VID, "Reset.");
    Clear();
}

void c_Video::Redraw(void) {
    dbgPrint(DBG_LV3, DBG_VID, "Refreshing screen.");
    int xc, yc, scalec;
    for(yc = 0; yc < SCREEN_HEIGHT; yc++)
    {
        for(xc = 0; xc < SCREEN_WIDTH; xc++)
        {
                scaledbuffer[xc+yc*SCREEN_WIDTH] = m_TestFrameBuffer[xc/10][yc/10] ? 0xFFFFFFFF : 0;
        }
    }
    SDL_UpdateTexture(drawBuffer, NULL, &scaledbuffer, 4*SCREEN_WIDTH);
    SDL_RenderCopy(rendersurface, drawBuffer, NULL, NULL);
    SDL_RenderPresent(rendersurface);

}

void c_Video::Cleanup(void) {

}


void c_Video::Clear(void) {
    dbgPrint(DBG_LV3, DBG_VID, "Clearing screen.");
    int xc, yc;

    for(yc = 0; yc < 64; yc++)
    {
        for(xc = 0; xc < 128; xc++)
        {
            m_TestFrameBuffer[xc][yc] = 0x00;
        }
    }
}

void c_Video::EnableExtended(void) {
    Redraw();
    SCREEN_WIDTH = 1280;
    SCREEN_HEIGHT = 640;
    nonscaledw = 128;
    SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_DestroyTexture(drawBuffer);
    drawBuffer = SDL_CreateTexture(rendersurface, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    extended = true;
}

void c_Video::ScrollDown(unsigned char lines) {
    int ycount, xcount;
    unsigned char tempbuffer[128][64];

    for(ycount = 0; ycount < 64; ycount++)
    {
        for(xcount = 0; xcount < 128; xcount++)
        {
            if((ycount+lines) < 64)
                tempbuffer[xcount][ycount+lines] = m_TestFrameBuffer[xcount][ycount];
        }
    }

    for(ycount = 0; ycount < 64; ycount++)
    {
        for(xcount = 0; xcount < 128; xcount++)
        {
            m_TestFrameBuffer[xcount][ycount] = tempbuffer[xcount][ycount];
        }
    }

    Redraw();
}

void c_Video::ScrollRight(void) {
    int ycount, xcount;
    unsigned char tempbuffer[128][64];
    unsigned char lines = 4;

    for(ycount = 0; ycount < 64; ycount++)
    {
        for(xcount = 0; xcount < 128; xcount++)
        {
            if((xcount+lines) < 128)
                tempbuffer[xcount+lines][ycount] = m_TestFrameBuffer[xcount][ycount];
        }
    }

    for(ycount = 0; ycount < 64; ycount++)
    {
        for(xcount = 0; xcount < 128; xcount++)
        {
            m_TestFrameBuffer[xcount][ycount] = tempbuffer[xcount][ycount];
        }
    }
}

void c_Video::ScrollLeft(void) {
    int ycount, xcount;
    unsigned char tempbuffer[128][64];
    unsigned char lines = 4;

    for(ycount = 0; ycount < 64; ycount++)
    {
        for(xcount = 0; xcount < 128; xcount++)
        {
            if((xcount-lines) > 0)
                tempbuffer[xcount-lines][ycount] = m_TestFrameBuffer[xcount][ycount];
        }
    }

    for(ycount = 0; ycount < 64; ycount++)
    {
        for(xcount = 0; xcount < 128; xcount++)
        {
            m_TestFrameBuffer[xcount][ycount] = tempbuffer[xcount][ycount];
        }
    }

}
