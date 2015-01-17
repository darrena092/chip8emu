#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "video.h"


#define ROM_START 0x200
#define SCHIP_CLOCK 250

class c_CPU {

    private:
        unsigned char m_Memory[0xFFF];
        unsigned char m_Regs[0x10];
        unsigned char m_RPL[0x10];
        unsigned short m_I; //2 byte address register.
        unsigned short m_PC; //Program counter.
        unsigned short m_Stack[12]; //Stack.
        unsigned char m_SP; //Stack pointer.
        c_Video *VideoManager;
        bool firstcycle;
        bool resetsignal;
        unsigned char delaytimer;
        SDL_Event event;
        int CLOCK_SPEED = 60;
        bool run1;

    public:
        void Init(c_Video *vManager);
        void Reset(void);
        void LoadROM(const char *fname);
        bool Run(void);
        void WarmReset(void);
        void Cleanup(void);
};

#endif
