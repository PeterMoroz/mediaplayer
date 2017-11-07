#pragma once

#include <cassert>
#include <stdexcept>

#include "frame_consumer.h"
#include "frame_provider.h"
#include "scale_context.h"

class FrameConverter final : public FrameConsumer, public FrameProvider
{
	FrameConverter(const FrameConverter&) = delete;
	const FrameConverter& operator=(const FrameConverter&) = delete;
	FrameConverter(FrameConverter&&) = delete;
	const FrameConverter& operator=(FrameConverter&&) = delete;
	
public:
	FrameConverter(int src_width, int src_height, AVPixelFormat src_format) throw(std::runtime_error);
	~FrameConverter();
	
	void CreateConversionContext(int dst_width, int dst_height, AVPixelFormat dst_format) throw (std::logic_error, std::runtime_error);

	// implementation of FrameConsumer interface
	bool AgreeWith(AVPixelFormat format, int width, int height) const noexcept override
	{
		return (format == _src_format && width == _src_width && height == _src_height);
	}
	void AcceptFrame(const Frame& frame) override;
	
private:
	// implementation if FrameProvider interface
	bool TestConsumer(const FrameConsumer* frame_consumer) const noexcept override
	{
		assert(frame_consumer != NULL);
		return frame_consumer->AgreeWith(_dst_format, _dst_width, _dst_height);
	}

private:
	int _src_width;
	int _src_height;
	AVPixelFormat _src_format;
	int _dst_width;
	int _dst_height;
	AVPixelFormat _dst_format;
	ScaleContext _scale_context;
	Frame _frame;
};
