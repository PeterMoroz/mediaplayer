extern "C"
{
#include <libavutil/error.h>
#include <libavutil/pixfmt.h>
}

#include <SDL.h>
#include <SDL_thread.h>

#include <cassert>

#include <iostream>
#include <exception>

#include "format_context.h"
#include "codec_context.h"
#include "frames_reader.h"
#include "stream_demuxer.h"
#include "video_decoder.h"
#include "sdl_screen.h"
#include "sdl_packet_queue.h"
#include "sdl_audio_player.h"

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

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
		std::exit(-1);
	}

	try {
		FormatContext format_ctx;
		format_ctx.OpenInput(argv[1]);
		format_ctx.FindStreamInfo();

		assert(format_ctx);

		av_dump_format(format_ctx, 0, argv[1], 0);

		int video_stream_idx = format_ctx.FindFirstStreamByType(AVMEDIA_TYPE_VIDEO);
		if (video_stream_idx == -1)
			throw std::logic_error("Couldn't find videostream in container.");

		int audio_stream_idx = format_ctx.FindFirstStreamByType(AVMEDIA_TYPE_AUDIO);


		AVCodecContext* video_codec_ctx = format_ctx.GetCodecContext(video_stream_idx);
		if (video_codec_ctx == NULL)
			throw std::logic_error("Couldn't find video codec context.");

		AVCodecContext* audio_codec_ctx = audio_stream_idx >= 0 ? format_ctx.GetCodecContext(audio_stream_idx) : NULL;
		if (audio_stream_idx >= 0 && audio_codec_ctx == NULL)
			throw std::logic_error("Couldn't find audio codec context.");


		CodecContext video_codec_context;
		video_codec_context.OpenCodec(video_codec_ctx);

		CodecContext audio_codec_context;
		if (audio_stream_idx >= 0)
			audio_codec_context.OpenCodec(audio_codec_ctx);


		FramesReader frames_reader;

		StreamDemuxer stream_demuxer;
		frames_reader.AddPacketConsumer(&stream_demuxer);

		SDLPacketQueue audio_queue;

		VideoDecoder video_decoder(video_codec_context);
		stream_demuxer.AddStreamTarget(video_stream_idx, &video_decoder);
		if (audio_stream_idx >= 0)
			stream_demuxer.AddStreamTarget(audio_stream_idx, &audio_queue);

		SDLAudioPlayer audio_player(audio_codec_context, audio_queue);
		if (audio_stream_idx >= 0)
		{
			audio_player.OpenDevice();
			audio_player.InitResample();
			audio_player.RunPlayback();
		}

		SDLScreen sdl_screen;
		video_decoder.AddFrameConsumer(&sdl_screen);

		sdl_screen.Initialize("FFmpeg videoplayer", video_codec_context->width, video_codec_context->height);
		sdl_screen.SetInputConversion(video_codec_context->pix_fmt, video_codec_context->width, video_codec_context->height);

		frames_reader.ReadFrames(format_ctx);

	} catch (const std::exception& ex) {
		std::cerr << "Exception: " << ex.what() << std::endl;
		SDL_Quit();
		std::exit(-1);
	}

	SDL_Quit();
	std::exit(0);

	return 0;
}
