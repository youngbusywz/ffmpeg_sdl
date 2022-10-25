#include "VideoDecode.h"
#include <QDebug>

int64_t             lastReadPacktTime ;
static int interrupt_cb(void *ctx)
{
    int  timeout  = 3;
    if(av_gettime() - lastReadPacktTime > timeout *1000 *1000)
    {
        return -1;
    }
    return 0;
}

VideoDecode::VideoDecode()
{
    m_spSDLRender = std::make_shared<SDLRender>();
}

VideoDecode::~VideoDecode()
{
	m_playStatus = PClose;
	if (m_readVideoThread)
	{
		m_readVideoThread->join();
	}
}

void VideoDecode::Init()
{
    av_register_all();
    avfilter_register_all();
    avformat_network_init();
}

int VideoDecode::InitVideoDecodeContext()
{
    if(m_videoIndex >= 0)
    {
        auto codeId = m_inputContext->streams[m_videoIndex]->codec->codec_id;
        auto codec = avcodec_find_decoder(codeId);
        if(!codec)
            return -1;

        int ret = avcodec_open2(m_inputContext->streams[m_videoIndex]->codec, codec, NULL);
        return ret;
    }
}

std::shared_ptr<AVPacket> VideoDecode::readPacketFromSource()
{
    std::shared_ptr<AVPacket> packet(static_cast<AVPacket*>(av_malloc(sizeof(AVPacket))), [&](AVPacket* p){av_packet_free(&p); av_freep(&p);});
    av_init_packet(packet.get());

	int size = m_inputContext->streams[m_videoIndex]->codec->width *  m_inputContext->streams[m_videoIndex]->codec->height;
	av_new_packet(packet.get(), size);

	lastReadPacktTime = av_gettime();
    int ret = av_read_frame(m_inputContext, packet.get());
    if(ret >= 0)
    {
        return packet;
    }
    else
    {
        return nullptr;
    }
}

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
	char *filename)
{
	FILE *f;
	int i;

	f = fopen(filename, "w");
	fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
	for (i = 0; i < ysize; i++)
		fwrite(buf + i * wrap, 1, xsize, f);
	fclose(f);
}

bool VideoDecode::VideoFrameDecode(AVPacket* packet, AVFrame* frame)
{
#if 0 ÆúÓÃavcodec_decode_video2
    int getFrame = 0;
    auto hr = avcodec_decode_video2(m_inputContext->streams[m_videoIndex]->codec, frame, &getFrame, packet);
    if(hr >= 0 && getFrame != 0)
    {
        return true;
    }

    return false;
#endif
	char buf[1024];
	int ret;

	ret = avcodec_send_packet(m_inputContext->streams[m_videoIndex]->codec, packet);
	if (ret < 0) {
		fprintf(stderr, "Error sending a packet for decoding\n");
		return false;
	}

	ret = avcodec_receive_frame(m_inputContext->streams[m_videoIndex]->codec, frame);
	if (ret < 0)
	{
		return false;
	}

	return true;
}

int VideoDecode::OpenInput(std::string inputUrl)
{
    m_inputContext = avformat_alloc_context();
    lastReadPacktTime = av_gettime();
    m_inputContext->interrupt_callback.callback = interrupt_cb;
    int ret = avformat_open_input(&m_inputContext, inputUrl.c_str(), nullptr, nullptr);
    if(ret < 0)
    {
        qDebug()<<"Input file open input failed";
        return ret;
    }

    ret = avformat_find_stream_info(m_inputContext, nullptr);
    if(ret < 0)
    {
        qDebug()<<"Find input file stream inform failed";
    }
    else
    {
        for(int i = 0; i<m_inputContext->nb_streams; i++)
        {
            if(m_inputContext->streams[i]->codec->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO)
            {
                m_videoIndex = i;
            }
            else if(m_inputContext->streams[i]->codec->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO)
            {
                m_audioIndex = i;
            }
        }

        m_playStatus = POpen;
        QString str = QString("Open input file  %s success").arg(inputUrl.c_str());
        qDebug()<<str;
    }
    return ret;
}

void VideoDecode::_ReadVideoProc()
{
#if 0	//²¥·Å¿¨¶Ù»¨ÆÁ
    AVFrame* videoFrame = av_frame_alloc();
    while(m_playStatus == PStart)
    {
        auto packet = readPacketFromSource();
        if(packet)
        {
            if(packet->stream_index == m_videoIndex)
            {
                if(VideoFrameDecode(packet.get(), videoFrame))
                {
                    m_spSDLRender->Display(videoFrame->data, videoFrame->linesize);
                }
            }
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }

#endif
	AVFrame* videoFrame = av_frame_alloc();
	while (m_playStatus == PStart)
	{
		auto packet = readPacketFromSource();
		if (packet)
		{
			if (packet->stream_index == m_videoIndex)
			{
				std::unique_lock<std::mutex> locker(m_mtPacket);
				m_packets.push(packet);
			}
		}
	}
}

void VideoDecode::_DecodeVideoProc()
{
	while (m_playStatus == PStart)
	{
		std::unique_lock<std::mutex> locker(m_mtPacket);
		if (m_packets.empty())
			continue;

		auto packet = m_packets.front();
		AVFrame* videoFrame = av_frame_alloc();
		if (VideoFrameDecode(packet.get(), videoFrame))
		{
			//m_spSDLRender->Display(videoFrame->data, videoFrame->linesize);





			struct SwsContext *img_convert_ctx ;
			img_convert_ctx = sws_getContext(
				videoFrame->width, videoFrame->height, static_cast<AVPixelFormat>(videoFrame->format), videoFrame->width, videoFrame->height,
				AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
			if (img_convert_ctx != NULL) {
				AVFrame	*pFrameYUV;
				pFrameYUV = av_frame_alloc();

				sws_scale(img_convert_ctx, (const uint8_t* const*)videoFrame->data, videoFrame->linesize, 0, m_inputContext->streams[0]->codec->height, pFrameYUV->data, pFrameYUV->linesize);
				m_spSDLRender->Display(pFrameYUV);
				}
			















			//m_spSDLRender->Display(videoFrame);
		}
		m_packets.pop();
		av_frame_free(&videoFrame);
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
	}
}


int VideoDecode::Play(void* hwnd)
{
    int ret = -1;
    if(m_playStatus != POpen)
    {
        return ret;
    }

    if(m_videoIndex >= 0)
    {
        ret = InitVideoDecodeContext();
        if(ret < 0)
            return ret;
        m_spSDLRender->InitVideo(hwnd);

        m_spSDLRender->CreateVideoSurface(m_inputContext->streams[m_videoIndex]->codec->width, m_inputContext->streams[m_videoIndex]->codec->height);
    }

    m_playStatus = PStart;
    m_readVideoThread.reset(new std::thread(&VideoDecode::_ReadVideoProc, this));
	m_decodeVideoThread.reset(new std::thread(&VideoDecode::_DecodeVideoProc, this));
    return ret;
}



