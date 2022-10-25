#ifndef SDLRENDER_H
#define SDLRENDER_H

#include "videoplayerheader.h"

class SDLRender
{
public:
    SDLRender();

    int InitVideo(void* hwnd);

    int CreateVideoSurface(int width, int height);

    int Display(BYTE** data, int* linesize);

	int Display(AVFrame *frame);

private:
    SDL_Window*     m_SdlWindow;

    SDL_Renderer*   m_SdlRender;

    SDL_Texture*    m_SdlTexture;

    int     m_width;

    int     m_height;

};

#endif // SDLRENDER_H
