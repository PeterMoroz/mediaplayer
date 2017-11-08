#include <cassert>

#include <exception>
#include <iostream>
#include <stdexcept>

#include "dump_ffmpeg_error.h"
#include "frame_consumer.h"
#include "video_decoder.h"

VideoDecoder::VideoDecoder(CodecContext& codec_ctx)
	: _codec_ctx(codec_ctx)
	, _consumers()
	{
		
	}

VideoDecoder::~VideoDecoder()
{
	
}

bool VideoDecoder::AcceptPacket(const Packet& packet) noexcept
{
	try {
		Frame frame;
		int frame_finished = 0;
		int rc = avcodec_decode_video2(_codec_ctx, &frame, &frame_finished, &packet);
		if (rc < 0)
		{
			mediaplayer::DumpFFmpegError(rc, std::cerr);
			throw std::runtime_error("avcodec_decode_video2() failed.");
		}
		
		if (frame_finished)
		{
			for (FrameConsumer* consumer : _consumers)
				consumer->AcceptFrame(frame);
		}
		
	} catch (const std::exception& ex) {
		std::cerr << "Exception when videodecoder was accepting packet: " << ex.what() << std::endl;
		return false;
	}
	
	return true;
}

void VideoDecoder::AddFrameConsumer(FrameConsumer* frame_consumer)
{
	assert(frame_consumer != NULL);
	_consumers.push_back(frame_consumer);
}
