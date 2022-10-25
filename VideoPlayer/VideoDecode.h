#ifndef VIDEODECODE_H
#define VIDEODECODE_H

#include "videoplayerheader.h"
#include "SDLRender.h"
#include <Queue>
#include <mutex>

enum  CGPlayState
{
    PClose = 0,
    POpen,
    PStart
};

class VideoDecode
{
public:
    VideoDecode();

	~VideoDecode();

    void Init();

    int OpenInput(std::string inputUrl);

    int Play(void* hwnd);

private:
    int InitVideoDecodeContext();

    std::shared_ptr<AVPacket> readPacketFromSource();

    bool VideoFrameDecode(AVPacket* packet, AVFrame* frame);

protected:
    void _ReadVideoProc();

	void _DecodeVideoProc();

private:
    ///输入上下文
    AVFormatContext*    m_inputContext = nullptr;

    ///输出上下文
    AVFormatContext*    m_outputContext = nullptr;

    ///视频索引
    int                 m_videoIndex;
    ///音频索引
    int                 m_audioIndex;

    CGPlayState         m_playStatus = PClose;

    std::shared_ptr<SDLRender>  m_spSDLRender;

	///读包
    std::shared_ptr<std::thread>    m_readVideoThread;
	//解包显示
	std::shared_ptr<std::thread>	m_decodeVideoThread;

	std::mutex		m_mtPacket;
	std::queue<std::shared_ptr<AVPacket>>		m_packets;

};

#endif // VIDEODECODE_H
