#include "SDLRender.h"

SDLRender::SDLRender()
{

}

int SDLRender::InitVideo(void *hwnd)
{
    SDL_Init(SDL_INIT_VIDEO);
    m_SdlWindow = SDL_CreateWindowFrom(hwnd);
    m_SdlRender = SDL_CreateRenderer(m_SdlWindow, -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED);
    return S_OK;
}

int SDLRender::CreateVideoSurface(int width, int height)
{
    m_width = width;
    m_height = height;

    m_SdlTexture = SDL_CreateTexture(m_SdlRender, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, width, height);
    return S_OK;
}

int SDLRender::Display(BYTE** data, int* linesize)
{
    void* piexels = nullptr;
    int pitch;

    //int ret = SDL_LockTexture(m_SdlTexture, NULL, &piexels, &pitch);
    //if(ret < 0)
    //    return ret;

    //uint8_t* yuv[3] = {
    //    (uint8_t*)piexels,
    //    (uint8_t*)piexels + pitch * m_height,
    //    (uint8_t*)piexels + pitch * m_height + ((pitch >> 1) * (m_height >> 1)) 
    //};

    //for(int i = 0; i < m_height; i++)
    //{
    //    memcpy(yuv[0] + i * pitch, data[0] + i * linesize[0], linesize[0]);
    //    if(i % 2 == 0)
    //    {
    //        memcpy(yuv[1] + (i >> 1) * (pitch >> 1), data[2] + (i >> 1) * linesize[2], linesize[2]);
    //        memcpy(yuv[2] + (i >> 1) * (pitch >> 1), data[1] + (i >> 1) * linesize[1], linesize[1]);
    //    }
    //}

    //SDL_UnlockTexture(m_SdlTexture);



	if (data[0] > 0 && linesize[1] > 0 && linesize[2] > 0) {
		SDL_UpdateYUVTexture(m_SdlTexture, NULL, data[0], linesize[0],
			data[1], linesize[1],
			data[2], linesize[2]);
	}
	else if (linesize[0] < 0 && linesize[1] < 0 && linesize[2] < 0) {
		SDL_UpdateYUVTexture(m_SdlTexture, NULL, data[0] + linesize[0] * (m_height - 1), -linesize[0],
			data[1] + linesize[1] * (AV_CEIL_RSHIFT(m_height, 1) - 1), -linesize[1],
			data[2] + linesize[2] * (AV_CEIL_RSHIFT(m_height, 1) - 1), -linesize[2]);
	}
	else {
	}



    SDL_RenderClear(m_SdlRender);
    SDL_RenderCopy(m_SdlRender, m_SdlTexture, NULL, NULL);
    SDL_RenderPresent(m_SdlRender);
    return S_OK;
}
static unsigned sws_flags = SWS_BICUBIC;

int SDLRender::Display(AVFrame *frame)
{
	BYTE** data = frame->data;
	int* linesize = frame->linesize;

	SDL_UpdateYUVTexture(m_SdlTexture, NULL, data[0], linesize[0],
		data[1], linesize[1],
		data[2], linesize[2]);





	//struct SwsContext *img_convert_ctx ;
	////img_convert_ctx = sws_getContext(
	////	frame->width, frame->height, static_cast<AVPixelFormat>(frame->format), frame->width, frame->height,
	////	AV_PIX_FMT_BGRA, sws_flags, NULL, NULL, NULL);
	//if (img_convert_ctx != NULL) {
	//	AVFrame	*pFrameYUV;
	//	pFrameYUV = av_frame_alloc();

	//	//uint8_t *pixels[4] = {0};
	//	//int pitch[4];
	//	/*if (!SDL_LockTexture(m_SdlTexture, NULL, (void **)pixels, pitch))*/ {
	//		//sws_scale(img_convert_ctx, (const uint8_t* const*)frame->data, frame->linesize,
	//		//	0, frame->height, pFrameYUV->data, pFrameYUV->linesize);
	//		//sws_scale(img_convert_ctx, (const uint8_t* const*)frame->data, frame->linesize, 0, m_height, pFrameYUV->data, pFrameYUV->linesize);


	//		//SDL_UpdateTexture(m_SdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);

	//		SDL_UpdateYUVTexture(m_SdlTexture, NULL, frame->data[0] + frame->linesize[0] * (m_height - 1), -frame->linesize[0],
	//			frame->data[1] + frame->linesize[1] * (AV_CEIL_RSHIFT(m_height, 1) - 1), -frame->linesize[1],
	//			frame->data[2] + frame->linesize[2] * (AV_CEIL_RSHIFT(m_height, 1) - 1), -frame->linesize[2]);


	//		//SDL_UnlockTexture(m_SdlTexture);
	//	}
	//}
	SDL_RenderClear(m_SdlRender);
	SDL_RenderCopy(m_SdlRender, m_SdlTexture, NULL, NULL);
	SDL_RenderPresent(m_SdlRender);
	return S_OK;
}