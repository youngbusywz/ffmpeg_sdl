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
    ///����������
    AVFormatContext*    m_inputContext = nullptr;

    ///���������
    AVFormatContext*    m_outputContext = nullptr;

    ///��Ƶ����
    int                 m_videoIndex;
    ///��Ƶ����
    int                 m_audioIndex;

    CGPlayState         m_playStatus = PClose;

    std::shared_ptr<SDLRender>  m_spSDLRender;

	///����
    std::shared_ptr<std::thread>    m_readVideoThread;
	//�����ʾ
	std::shared_ptr<std::thread>	m_decodeVideoThread;

	std::mutex		m_mtPacket;
	std::queue<std::shared_ptr<AVPacket>>		m_packets;

};

#endif // VIDEODECODE_H
