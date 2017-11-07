#include <cassert>

#include "codec_context.h"
#include "dump_ffmpeg_error.h"


CodecContext::CodecContext()
	: _codec_context(NULL)
	, _codec(NULL)
{
	
}

CodecContext::~CodecContext()
{
	avcodec_close(_codec_context);
	_codec_context = NULL;
	_codec = NULL;
}

/*	
void CodecContext::CopyContext(AVCodecContext* codec_context_orig) throw (std::logic_error, std::runtime_error)
{
	assert(codec_context_orig != NULL);
	
	AVCodec* codec = avcodec_find_decoder(codec_context_orig->codec_id);
	if (!codec)
		throw std::runtime_error("Can't copy codec context - unsupported codec.");
		
	AVCodecContext* codec_context = avcodec_alloc_context3(codec);
	if (!codec_context)
		throw std::runtime_error("Can't copy codec context - allocation failed.");
	
	int rc = avcodec_copy_context(codec_context, codec_context_orig);
	if (rc != 0)
	{
		mediaplayer::DumpFFmpegError(rc, std::cerr);
		throw std::runtime_error("Can't copy codec context - copying of context failed.");
	}
	
	if (_codec_context != NULL)
		avcodec_close(_codec_context);
		
	_codec_context = codec_context;
	_codec = codec;
}

void CodecContext::OpenCodec() throw (std::logic_error, std::runtime_error)
{
	
}
*/

void CodecContext::OpenCodec(AVCodecContext* codec_context_orig) throw (std::logic_error, std::runtime_error)
{
	assert(codec_context_orig != NULL);
	
	AVCodec* codec = avcodec_find_decoder(codec_context_orig->codec_id);
	if (!codec)
		throw std::runtime_error("Can't open codec context - unsupported codec.");
		
	AVCodecContext* codec_context = avcodec_alloc_context3(codec);
	if (!codec_context)
		throw std::runtime_error("Can't open codec context - allocation failed.");
	
	int rc = avcodec_copy_context(codec_context, codec_context_orig);
	if (rc != 0)
	{
		mediaplayer::DumpFFmpegError(rc, std::cerr);
		throw std::runtime_error("Can't open codec context - copying of context failed.");
	}
	
	if (_codec_context != NULL)
		avcodec_close(_codec_context);
		
	_codec_context = codec_context;
	_codec = codec;
	
	rc = avcodec_open2(_codec_context, _codec, NULL);
	if (rc < 0)
	{
		mediaplayer::DumpFFmpegError(rc, std::cerr);
		throw std::runtime_error("Can't open codec context - opening of codec failed.");
	}
}
