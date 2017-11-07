extern "C"
{
	#include <libavcodec/avcodec.h>
}

#include "frame.h"


Frame::Frame() throw(std::runtime_error)
	: _frame(NULL)
	, _buffer(NULL, av_free)
	{
		_frame = av_frame_alloc();
		if (!_frame)
			throw std::runtime_error("av_frame_alloc() failed.");
	}
	
Frame::~Frame()
{
	//av_free(_frame);
	if (_frame)
	{
		av_frame_free(&_frame);
		_frame = NULL;
	}	
}

void Frame::CreateBuffer(AVPixelFormat pix_fmt, int width, int height) throw(std::logic_error, std::runtime_error)
{
	int num_bytes = avpicture_get_size(pix_fmt, width, height);
	if (num_bytes == -1)
		throw std::logic_error("avpicture_get_size() failed, probably too large dimensions.");
	
	std::unique_ptr<std::uint8_t, std::function<void(void*)>> buffer(static_cast<std::uint8_t*>(av_malloc(num_bytes * sizeof(std::uint8_t))), av_free);
	if (!buffer)
		throw std::runtime_error("av_malloc() failed.");
		
	int num_bytes2 = avpicture_fill(reinterpret_cast<AVPicture*>(_frame), buffer.get(), pix_fmt, width, height);
	if (num_bytes2 == -1)
		throw std::runtime_error("avpicture_fill() failed.");
	// TO DO: check, that num_bytes == num_bytes2	
	std::swap(_buffer, buffer);
}
