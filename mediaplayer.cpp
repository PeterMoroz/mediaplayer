extern "C"
{
#include <libavutil/error.h>
#include <libavutil/pixfmt.h>
}

#include <cassert>

#include <iostream>
#include <exception>

#include "format_context.h"
#include "codec_context.h"
#include "frames_reader.h"
#include "stream_demuxer.h"
#include "video_decoder.h"
#include "ppm_writer.h"

namespace mediaplayer
{
	void DumpFFmpegError(int ec, std::ostream& os)
	{
		char buffer[1024] = {0};
		if (av_strerror(ec, buffer, sizeof(buffer)) != 0)
			os << "FFmpeg error (" << ec << "): unknonw" << std::endl;
		else
			os << "FFmpeg error (" << ec << "): " << buffer << std::endl;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " filename " << std::endl;
		std::exit(-1);
	}
	
	av_register_all();
	
	try {
		FormatContext format_ctx;
		format_ctx.OpenInput(argv[1]);
		format_ctx.FindStreamInfo();
		
		assert(format_ctx);
		
		av_dump_format(format_ctx, 0, argv[1], 0);
		
		int video_stream_idx = format_ctx.FindFirstStreamByType(AVMEDIA_TYPE_VIDEO);
		if (video_stream_idx == -1)
			throw std::logic_error("Couldn't find videostream in container.");
			
		AVCodecContext* video_codec_ctx = format_ctx.GetCodecContext(video_stream_idx);
		if (video_codec_ctx == NULL)
			throw std::logic_error("Couldn't find video codec context.");

		CodecContext codec_ctx;
		codec_ctx.OpenCodec(video_codec_ctx);
		
		FramesReader frames_reader;
		
		StreamDemuxer stream_demuxer;
		frames_reader.AddPacketConsumer(&stream_demuxer);
		
		VideoDecoder video_decoder(codec_ctx);
		stream_demuxer.AddStreamTarget(video_stream_idx, &video_decoder);
		
		PPMWriter ppm_writer(codec_ctx->width, codec_ctx->height);
		video_decoder.AddFrameConsumer(&ppm_writer);
		
		ppm_writer.CreateFrameBuffer();
		ppm_writer.SetInputConversion(codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height);
		ppm_writer.SetOutputDirectory("ppm-output");
		
		frames_reader.ReadFrames(format_ctx);

	} catch (const std::exception& ex) {
		std::cerr << "Exception: " << ex.what() << std::endl;
		std::exit(-1);
	}
	
	return 0;
}
