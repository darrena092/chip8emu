#include <SDL2/SDL.h>
#include <stdio.h>
#include "video.h"
#include "cpu.h"
#include "debug.h"

int main(int argc, char* args[])
{
    //freopen("out3.log", "w", stdout);
    char *romfname = args[1];
    dbgPrint(DBG_LV0, DBG_APP, "Starting video manager and CPU...");
    dbgPrint(DBG_LV0, DBG_APP, "Command parameters: %s.", romfname);
    c_Video *VideoManager;
    VideoManager = new c_Video();
    VideoManager->Init();
    VideoManager->Reset();

    c_CPU *CPU;
    CPU = new c_CPU();
    CPU->Init(VideoManager);
    CPU->Reset();
    //CPU->LoadROM("sc-games/ANT");
    //CPU->LoadROM("chip8roms/GUESS");
    CPU->LoadROM(romfname);

    while(CPU->Run())
    {
    }

    dbgPrint(DBG_LV0, DBG_APP, "Exiting.");
	return 0;
}
