#ifndef VIDEO_H
#define VIDEO_H

#include <SDL2/SDL.h>

class c_Video {
    private:
        SDL_Window *window;
        SDL_Renderer *rendersurface;
        SDL_Texture *drawBuffer;
        uint32_t scaledbuffer[1280*640];
        int SCREEN_WIDTH = 640;
        int SCREEN_HEIGHT = 320;

    public:
        //Member Variables
        unsigned char m_FrameBuffer[128*64];
        unsigned char m_TestFrameBuffer[128][64];
        bool extended;
        int nonscaledw = 64;

        //Public Functions
        void Init(void);
        void Reset(void);
        void Redraw(void);
        void Cleanup(void);
        void Clear(void);
        void EnableExtended(void);
        void ScrollDown(unsigned char lines);
        void ScrollRight(void);
        void ScrollLeft(void);
};

#endif
