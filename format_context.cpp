#include <cassert>

#include "format_context.h"
#include "dump_ffmpeg_error.h"

FormatContext::FormatContext()
	: _format_context(NULL)
{
	
}

FormatContext::~FormatContext()
{
	if (_format_context)
	{
		avformat_close_input(&_format_context);
		_format_context = NULL;
	}
}

void FormatContext::OpenInput(const char* filename) throw (std::logic_error, std::runtime_error)
{
	assert(filename != NULL);
	if (_format_context != NULL)
		throw std::logic_error("FormatContext is already opened.");
		
	if (avformat_open_input(&_format_context, filename, NULL, NULL) != 0)
		throw std::runtime_error("FormatContext couldn't open file.");
}

void FormatContext::FindStreamInfo() throw (std::logic_error, std::runtime_error)
{
	if (_format_context == NULL)
		throw std::logic_error("FormatContext isn't opened yet.");
		
	int rc = avformat_find_stream_info(_format_context, NULL);
	if (rc < 0)
	{
		mediaplayer::DumpFFmpegError(rc, std::cerr);
		throw std::runtime_error("FindStreamInfo failed.");
	}
}

int FormatContext::FindFirstStreamByType(int type) throw (std::logic_error)
{
	if (_format_context == NULL)
		throw std::logic_error("FormatContext isn't opened yet.");
		
	for (int idx = 0; idx < _format_context->nb_streams; idx++)
		if (_format_context->streams[idx]->codec->codec_type == type)
			return idx;
	return -1;
}

AVCodecContext* FormatContext::GetCodecContext(int stream_idx) const throw (std::logic_error)
{
	if (!_format_context)
		throw std::logic_error("FormatContext isn't opened yet.");
	if (stream_idx < 0 || stream_idx >= _format_context->nb_streams)
		return NULL;
	return _format_context->streams[stream_idx]->codec;
}