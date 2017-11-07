#include <cassert>

#include "frame_converter.h"

FrameConverter::FrameConverter(int src_width, int src_height, AVPixelFormat src_format) throw(std::runtime_error)
	: _src_width(src_width)
	, _src_height(src_height)
	, _src_format(src_format)
	, _dst_width(0)
	, _dst_height(0)
	, _dst_format(AV_PIX_FMT_NONE)
	, _scale_context()
	, _frame()
	{
		assert(src_width > 0 && src_height > 0);
		assert(src_format != AV_PIX_FMT_NONE);
	}


FrameConverter::~FrameConverter()
{
	
}

void FrameConverter::CreateConversionContext(int dst_width, int dst_height, AVPixelFormat dst_format) throw (std::logic_error, std::runtime_error)
{
	for (const FrameConsumer* consumer : consumers())
	{
		// we have to options here: reject conversion or remove coneumer from the list
		// currently we choose first strategy
		if (!consumer->AgreeWith(dst_format, dst_width, dst_height))
			throw std::logic_error("One of consumers doesn't accept output format if conversion.");
	}
		
	_scale_context.Create(_src_width, _src_height, _src_format, dst_width, dst_height, dst_format, SWS_BILINEAR);
	_frame.CreateBuffer(dst_format, dst_width, dst_height);
	
	_dst_width = dst_width;
	_dst_height = dst_height;
	_dst_format = dst_format;
}

void FrameConverter::AcceptFrame(const Frame& frame)
{
	if (!_scale_context)
		throw std::logic_error("No scale context created.");
	assert(_frame);
	
	int dst_height = sws_scale(_scale_context, frame->data, frame->linesize, 0, _src_height, _frame->data, _frame->linesize);
	// TO DO: check that dst_height == _dst_height
	
	for (FrameConsumer* consumer : consumers())
		consumer->AcceptFrame(_frame);
}
