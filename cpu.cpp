#include "cpu.h"
#include "debug.h"
#include "fontset.h"
#include <SDL2/SDL.h>

void c_CPU::Init(c_Video *vManager) {
    dbgPrint(DBG_LV0, DBG_CPU, "Initialising.");
    VideoManager = vManager;
}

void c_CPU::WarmReset(void) {
    dbgPrint(DBG_LV0, DBG_CPU, "Warm Reset.");
    //memset(&m_Memory, 0xFFF, sizeof(unsigned char));
    memset(&m_Regs, 0xF, sizeof(unsigned char));
    m_I = 0x000;
    m_PC = ROM_START;
    m_SP = 0;
    memcpy(&m_Memory, &fontset, 80);
    dbgPrint(DBG_LV0, DBG_CPU, "Memory and registers cleared. Fontset loaded. Continuing.");
    delaytimer = 0x00;
    firstcycle = true;
    resetsignal = false;
}

void c_CPU::Reset(void) {
    dbgPrint(DBG_LV0, DBG_CPU, "Reset.");
    memset(m_Memory, 0, sizeof(m_Memory));
    memset(m_Regs, 0, sizeof(m_Regs));
    m_I = 0x000;
    m_PC = ROM_START;
    m_SP = 0;
    memcpy(m_Memory, fontset, 80);
    //memcpy(&m_Memory[80], fontsetS, 160);
    int cnt;
    for(cnt = 0; cnt < 160; cnt++)
    {
        m_Memory[cnt+80] = fontsetS[cnt];
    }
    dbgPrint(DBG_LV0, DBG_CPU, "Memory and registers cleared. Fontset loaded. Continuing.");
    firstcycle = true;
    resetsignal = false;
    delaytimer = 0x00;
    run1 = true;
}

void c_CPU::LoadROM(const char *fname) {
    FILE *fptr;
    unsigned char *buffer;
    unsigned int filesize;

    fptr = fopen(fname, "rb");
    if(fptr != NULL)
    {
    //Get size of rom file.
    fseek(fptr, 0, SEEK_END);
    filesize = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    dbgPrint(DBG_LV0, DBG_CPU, "Loading ROM. Size: %i bytes.", filesize);
    buffer = (unsigned char*)malloc(filesize);
    fread(buffer, sizeof(unsigned char), filesize, fptr);
    dbgPrint(DBG_LV0, DBG_CPU, "Copying to ROM_START (0x%x). Base of CHIP8 mem: 0x%x.", &m_Memory[ROM_START], &m_Memory);
    memcpy(&m_Memory[ROM_START], buffer, filesize); //Copy file buffer to emulated system memory.
    dbgPrint(DBG_LV0, DBG_CPU, "ROM Copied to emulated memory.", filesize);
    fclose(fptr);
    free(buffer);
    }
    else {
    dbgPrint(DBG_LV0, DBG_CPU, "Error loading ROM [%s].", fname);
    perror("Can't open file");
    }
}

bool c_CPU::Run(void) {
    if(run1) {
    unsigned short opcode;
    bool incpc = true;
    unsigned char reg1, reg2;
    unsigned short x, y, yline, xpix;
    unsigned char value;
    unsigned char height, width;
    unsigned short gdata;
    unsigned char cnt;
    bool bigsprite;
    int ycnt;
    bigsprite = false;
    ycnt = 0;
    unsigned char gdata2[2];
    dbgPrint(DBG_LV2, DBG_CPU, "Cycle.");

    opcode = m_Memory[m_PC] << 8 | m_Memory[m_PC+1]; //Store 2 byte opcode from memory into short.
    dbgPrint(DBG_LV3, DBG_CPU, "Opcode PC(0x%x): 0x%x.", m_PC, opcode);

    if(firstcycle) {
        dbgPrint(DBG_LV0, DBG_CPU, "CPU Running. Clock speed: %i Hz.", CLOCK_SPEED);
        firstcycle = false;
    }

    if(resetsignal) {
        WarmReset();
        VideoManager->Reset();
        return true;
    }

    switch(opcode & 0xF000)
    {
        case 0x0000:
            switch(opcode & 0x00FF)
            {
                case 0x00E0: //00E0: Clear the screen.
                    VideoManager->Clear();
                    dbgPrint(DBG_LV0, DBG_CPU, "Clearing the screen.", m_PC);
                break;

                case 0x00EE: //00EE: Return from a subroutine.
                    m_SP--;
                    m_PC = m_Stack[m_SP];
                    incpc=false;
                    dbgPrint(DBG_LV3, DBG_CPU, "Returning to 0x%x from subroutine.", m_PC);
                break;

                case 0x00FB: //SCHIP - 00FB. Scroll display 4 pixels right.
                    dbgPrint(DBG_LV0, DBG_CPU, "SCHIP: Scrolling display 4 pixels right.");
                    VideoManager->ScrollRight();
                break;

                case 0x00FC: //SCHIP - 00FC. Scroll display 4 pixels left.
                    dbgPrint(DBG_LV0, DBG_CPU, "SCHIP: Scrolling display 4 pixels left.");
                    VideoManager->ScrollLeft();
                break;

                case 0x00FD: //SCHIP - 00FD. Exit chip interpreter.
                    dbgPrint(DBG_LV0, DBG_CPU, "SCHIP: Exiting chip interpreter.");
                break;

                case 0x00FE: //SCHIP - 00FE. Disable extended screen mode.
                    dbgPrint(DBG_LV0, DBG_CPU, "SCHIP: Disabling extended screen mode.");
                break;

                case 0x00FF: //SCHIP - 00FF. Enable extended screen mode.
                    CLOCK_SPEED = SCHIP_CLOCK;
                    dbgPrint(DBG_LV0, DBG_CPU, "Enabling SCHIP mode. Clock speed now %i Hz.", CLOCK_SPEED);
                    VideoManager->EnableExtended();
                break;

                default:
                    switch(opcode & 0x00F0)
                    {
                        case 0x00C0: //SCHIP - 00CN. Scroll display N lines down.
                            value = (opcode & 0x000F);
                            dbgPrint(DBG_LV0, DBG_CPU, "SCHIP: Scrolling display 0x%x lines down.", value);
                            VideoManager->ScrollDown(value);
                        break;

                        default:
                            dbgPrint(DBG_LV0, DBG_CPU, "HALTING: Unknown opcode: 0x%x.", opcode);
                            run1 = false;
                        break;
                    }
                break;

            }
        break;

        case 0x1000: //1NNN: Jump to address at NNN.
            m_PC = opcode & 0xFFF;
            dbgPrint(DBG_LV3, DBG_CPU, "Jumping to 0x%x.", m_PC);
            incpc = false;
        break;

        case 0x2000: //2NNN: Call subroutine at NNN.
            m_Stack[m_SP++] = m_PC+2; //Store address of next instruction incase we return later.
            m_PC = opcode & 0xFFF; //Jump to subroutine.
            incpc = false;
            dbgPrint(DBG_LV3, DBG_CPU, "Calling subroutine at 0x%x.", m_PC);
        break;

        case 0x3000: //3XNN: Skip next instruction if VX = NN.
            reg1 = (opcode & 0x0F00) >> 8;
            value = (opcode & 0x00FF);
            if(m_Regs[reg1] == value)
            {
                m_PC+=2;
            }
            dbgPrint(DBG_LV3, DBG_CPU, "Comparing V%x (0x%x) with 0x%x.", reg1, m_Regs[reg1], value);
        break;

        case 0x4000: //4XNN: Skip next instruction if VX != NN.
            reg1 = (opcode & 0x0F00) >> 8;
            value = (opcode & 0x00FF);
            if(m_Regs[reg1] != value)
            {
                m_PC+=2;
            }
            dbgPrint(DBG_LV3, DBG_CPU, "Comparing V%x (0x%x) with 0x%x.", reg1, m_Regs[reg1], value);
        break;

        case 0x5000: //5XY0: Skip next instruction if VX = VY.
            reg1 = (opcode & 0x0F00) >> 8;
            reg2 = (opcode & 0x00F0) >> 4;
            if(m_Regs[reg1] == m_Regs[reg2])
            {
                m_PC+=2;
            }
            dbgPrint(DBG_LV3, DBG_CPU, "Comparing V%x (0x%x) with V%x (0x%x).", reg1, m_Regs[reg1], reg2, m_Regs[reg2]);
        break;

        case 0x6000: //6XNN: Set VX to NN
            reg1 = (opcode & 0x0F00) >> 8;
            value = (opcode & 0x00FF);
            m_Regs[reg1] = value;
            dbgPrint(DBG_LV3, DBG_CPU, "V%x = 0x%x", reg1, value);
        break;

        case 0x7000: //7XNN: Adds NN to VX.
            reg1 = (opcode & 0x0F00) >> 8;
            value = (opcode & 0x00FF);
            m_Regs[reg1] += value;
            dbgPrint(DBG_LV3, DBG_CPU, "V%x += 0x%x", reg1, value);
        break;

        case 0x8000: //Register operations.
            switch(opcode & 0x000F)
            {
                case 0x0000: //8XY0: VX = VY.
                    reg1 = (opcode & 0x0F00) >> 8;
                    reg2 = (opcode & 0x00F0) >> 4;

                    m_Regs[reg1] = m_Regs[reg2];
                    dbgPrint(DBG_LV3, DBG_CPU, "V%x = V%x", reg1, reg2);
                break;

                case 0x0001: //8XY1: VX = VX | VY.
                    reg1 = (opcode & 0x0F00) >> 8;
                    reg2 = (opcode & 0x00F0) >> 4;

                    m_Regs[reg1] = m_Regs[reg1] | m_Regs[reg2];
                    dbgPrint(DBG_LV3, DBG_CPU, "V%x = V%x | V%x", reg1, reg1, reg2);
                break;

                case 0x0002: //8XY2: VX = VX & VY.
                    reg1 = (opcode & 0x0F00) >> 8;
                    reg2 = (opcode & 0x00F0) >> 4;

                    m_Regs[reg1] = m_Regs[reg1] & m_Regs[reg2];
                    dbgPrint(DBG_LV3, DBG_CPU, "V%x = V%x & V%x", reg1, reg1, reg2);
                break;

                case 0x0003: //8XY3: VX = VX ^ VY.
                    reg1 = (opcode & 0x0F00) >> 8;
                    reg2 = (opcode & 0x00F0) >> 4;

                    m_Regs[reg1] = m_Regs[reg1] ^ m_Regs[reg2];
                    dbgPrint(DBG_LV3, DBG_CPU, "V%x = V%x ^ V%x", reg1, reg1, reg2);
                break;

                case 0x0004: //8XY4: Add VX to VY. Set VF to 1 if there's a carry. VX = VX + VY.
                    reg1 = (opcode & 0x0F00) >> 8;
                    reg2 = (opcode & 0x00F0) >> 4;

                    if(((int)m_Regs[reg1] + (int)m_Regs[reg2]) < 256)
                        m_Regs[0xF] = 0; //No carry.
                    else
                        m_Regs[0xF] = 1; //Carry.

                    m_Regs[reg1] += m_Regs[reg2];
                    dbgPrint(DBG_LV3, DBG_CPU, "V%x = V%x + V%x", reg1, reg1, reg2);
                break;

                case 0x0005: //8XY5: VX = VX - VY. VF = 0 when borrow. 1 when none.
                    reg1 = (opcode & 0x0F00) >> 8;
                    reg2 = (opcode & 0x00F0) >> 4;

                    if(((int)m_Regs[reg1] - (int)m_Regs[reg2]) >= 0)
                        m_Regs[0xF] = 1; //No borrow.
                    else
                        m_Regs[0xF] = 0; //Borrow.

                    m_Regs[reg1] -= m_Regs[reg2];
                    dbgPrint(DBG_LV3, DBG_CPU, "V%x = V%x - V%x", reg1, reg1, reg2);
                break;

                case 0x0006: //8XY6: Shift VX right by one bit. VF is set to LSB (odd or even).
                    reg1 = (opcode & 0x0F00) >> 8;
                    m_Regs[0xF] = m_Regs[reg1] & 7;
                    m_Regs[reg1] = m_Regs[reg1] >> 1;
                    dbgPrint(DBG_LV3, DBG_CPU, "V%x = V%x >> 1. LSB: %x", reg1, m_Regs[0xF]);
                break;

                case 0x0007: //8XY7: VX = VY - VX. VF = borrow.
                    reg1 = (opcode & 0x0F00) >> 8;
                    reg2 = (opcode & 0x00F0) >> 4;

                    if(((int)m_Regs[reg2] - (int)m_Regs[reg1]) > 0)
                        m_Regs[0xF] = 1; //No borrow.
                    else
                        m_Regs[0xF] = 0; //Borrow.

                    m_Regs[reg1] = m_Regs[reg2] - m_Regs[reg1];
                    dbgPrint(DBG_LV3, DBG_CPU, "V%x = V%x - V%x", reg1, reg2, reg1);
                break;

                case 0x000E: //8XYE: Shift VX left by one. Set VF to MSB before shift.
                    reg1 = (opcode & 0x0F00) >> 8;
                    m_Regs[0xF] = m_Regs[reg1] >> 7;
                    m_Regs[reg1] = m_Regs[reg1] << 1;
                    dbgPrint(DBG_LV3, DBG_CPU, "V%x = V%x << 1. MSB: %x", reg1, m_Regs[0xF]);
                break;

            }
        break;

        case 0x9000: //9XY0: Skips next instruction if VX != VY.
            reg1 = (opcode & 0x0F00) >> 8;
            reg2 = (opcode & 0x00F0) >> 4;
            if(m_Regs[reg1] != m_Regs[reg2])
            {
                m_PC+=2;
            }
            dbgPrint(DBG_LV3, DBG_CPU, "Comparing V%x (0x%x) with V%x (0x%x).", reg1, m_Regs[reg1], reg2, m_Regs[reg2]);
        break;

        case 0xA000: //ANNN: Sets I to NNN.
            m_I = opcode & 0x0FFF;
            dbgPrint(DBG_LV3, DBG_CPU, "I = 0x%x", m_I);
        break;

        case 0xB000: //BNNN: Jumps to NNN + V0.
            m_PC = (opcode & 0x0FFF) + m_Regs[0];
            dbgPrint(DBG_LV3, DBG_CPU, "Jumping to 0x%x + V0(0x%x). PC = 0x%x.", (opcode & 0x0FFF), m_Regs[0], m_PC);
            incpc = false;
        break;

        case 0xC000: //CXNN: Sets VX to a random number & NN.
            reg1 = (opcode & 0x0F00) >> 8;
            value = opcode & 0x00FF;
            m_Regs[reg1] = rand() & value;
            dbgPrint(DBG_LV3, DBG_CPU, "V0x%x = rand & 0x%x. Result: 0x%x.", reg1, value, m_Regs[reg1]);
        break;

        case 0xD000: //DXYN: Draw sprite.
        reg1 = (opcode & 0x0F00) >> 8;
        reg2 = (opcode & 0x00F0) >> 4;
        x = m_Regs[reg1];
        y = m_Regs[reg2];
        height = (opcode & 0x000F);
        m_Regs[0xF] &= 0;
        if(height==0) {
            height = 16;
            width = 16;
            bigsprite=true;
        }
        else width=8;

        for(yline = 0; yline < height; yline++) {
            if(bigsprite) {
                //2 bytes per line
                gdata = (m_Memory[m_I + (yline*2)] << 8) | m_Memory[m_I + (yline*2) + 1];

            }
            else{
                //1 byte per line
                gdata = m_Memory[m_I + yline];
            }

            for(xpix = 0; xpix < width; xpix++) //Each sprite is 8 pixels wide.
            {
                switch(bigsprite)
                {
                    case true:
                        if(gdata & (0x800>>xpix)) {
                            if(VideoManager->m_TestFrameBuffer[x+xpix][y+yline] == 1){
                                m_Regs[0xF] = 1; //There has been a collision.
                            }
                            VideoManager->m_TestFrameBuffer[x+xpix][y+yline] ^= 1;
                        }
                    break;

                    case false:
                        if(gdata & (0x80>>xpix)) {
                            if(VideoManager->m_TestFrameBuffer[x+xpix][y+yline] == 1){
                                m_Regs[0xF] = 1; //There has been a collision.
                            }
                            VideoManager->m_TestFrameBuffer[x+xpix][y+yline] ^= 1;
                        }
                    break;
                }

            }
        }

        VideoManager->Redraw();
        break;


        case 0xE000: //Multiple. To be implemented.
            switch(opcode & 0x00FF)
            {
                case 0x00A1: //EXA1 - Skip next instruction if key in VX not pressed.
                m_PC += 2;
                break;
            }
        break;

        case 0xF000: //Multiple.
            switch(opcode & 0x00FF)
            {
                case 0x0007: //FX07. Sets VX to value of display timer.
                    reg1 = (opcode & 0x0F00) >> 8;
                    m_Regs[reg1] = delaytimer;
                    dbgPrint(DBG_LV0, DBG_CPU, "Setting V%x to value of delay timer.", reg1);
                break;

                case 0x000A:
                break;

                case 0x0015: //FX15. Set delay timer to VX.
                    reg1 = (opcode & 0x0F00) >> 8;
                    delaytimer = m_Regs[reg1];
                    dbgPrint(DBG_LV0, DBG_CPU, "Setting delay timer to value of V%x.", reg1);
                break;

                case 0x0018:
                break;

                case 0x001E: //FX1E: Add VX to I.
                    reg1 = (opcode & 0x0F00) >> 8;
                    m_I += m_Regs[reg1];
                    dbgPrint(DBG_LV3, DBG_CPU, "Adding V%x (0x%x) to I. Result: 0x%x.", reg1, m_Regs[reg1], m_I);
                break;

                case 0x0029: //FX29: Set I to memory location for font character stored in VX.
                    reg1 = (opcode & 0x0F00) >> 8;
                    m_I = m_Regs[reg1] * 5;
                    dbgPrint(DBG_LV3, DBG_CPU, "Setting I to mem location for char (0x%x) in V0x%x. I = 0x%x.", m_Regs[reg1], reg1, m_I);
                break;

                case 0x0033: //FX33: Store binary coded decimal representation of VX at I.
                    reg1 = (opcode & 0x0F00) >> 8;
                    m_Memory[m_I] = m_Regs[reg1] / 100;
                    m_Memory[m_I+1] = (m_Regs[reg1] / 10) % 10;
                    m_Memory[m_I+2] = m_Regs[reg1] % 10;
                break;

                case 0x0055: //FX55: Stores V0 to VX in memory starting at I.
                    reg1 = (opcode & 0x0F00) >> 8;
                    for(cnt = 0; cnt < reg1; cnt++)
                    {
                        m_Memory[m_I+cnt] = m_Regs[cnt];
                    }
                    dbgPrint(DBG_LV3, DBG_CPU, "Storing V0 to V%x in memory starting at I(0x%x).", reg1, m_I);
                break;

                case 0x0065: //FX65: Fills V0 to VX with values from memory starting at I.
                    reg1 = (opcode & 0x0F00) >> 8;
                    for(cnt = 0; cnt < reg1; cnt++)
                    {
                        m_Regs[cnt] = m_Memory[m_I+cnt];
                    }
                    dbgPrint(DBG_LV3, DBG_CPU, "Storing memory values in V0 to V%x starting at I(0x%x).", reg1, m_I);
                break;

                case 0x0030: //SCHIP - FX30. Point I to 10-byte font sprite for digit in VX.
                    reg1 = (opcode & 0x0F00) >> 8;
                    m_I = (m_Regs[reg1] * 10) + 80;
                    dbgPrint(DBG_LV0, DBG_CPU, "SCHIP: Pointing I to 10-byte font sprite. I = 0x%x", m_I);
                break;

                case 0x0075: //SCHIP - FX75. Store V0 to VX in RPL user flags.
                    dbgPrint(DBG_LV0, DBG_CPU, "SCHIP: Storing regs in RPL flags.");
                    reg1 = (opcode & 0x0F00) >> 8;
                    for(cnt = 0; cnt < reg1; cnt++)
                    {
                        m_RPL[cnt] = m_Regs[cnt];
                    }
                break;

                case 0x0085: //SCHIP - FX85. Read V0 to VX from RPL user flags.
                    dbgPrint(DBG_LV0, DBG_CPU, "SCHIP: Reading regs from RPL flags.");
                    reg1 = (opcode & 0x0F00) >> 8;
                    for(cnt = 0; cnt < reg1; cnt++)
                    {
                        m_Regs[cnt] = m_RPL[cnt];
                    }
                break;
            }
        break;

                default:
                    dbgPrint(DBG_LV0, DBG_CPU, "HALTING: Unknown opcode: 0x%x.", opcode);
                    run1 = false;
                break;
    }

    if(incpc)
    {
        m_PC+=2;
    }
    if(delaytimer > 0)
    {
        delaytimer--;
    }
    }

    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            Cleanup();
            return false;
        }
        if(event.type == SDL_KEYDOWN)
        {
            VideoManager->ScrollDown(0x1);
        }
    }

    SDL_Delay(1000/CLOCK_SPEED);
    return true;

}

void c_CPU::Cleanup(void) {
    VideoManager->Cleanup();
    SDL_Quit();
}

