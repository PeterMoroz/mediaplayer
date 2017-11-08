#pragma once

#include <cstdint>
#include <string>

#include <exception>
#include <stdexcept>

extern "C"
{
	#include <libavutil/pixfmt.h>
	#include <libswscale/swscale.h>
}

#include "frame_consumer.h"

class PPMWriter final : public FrameConsumer
{
	PPMWriter(const PPMWriter&) = delete;
	const PPMWriter& operator=(const PPMWriter&) = delete;	
	PPMWriter(PPMWriter&&) = delete;
	const PPMWriter& operator=(PPMWriter&&) = delete;

public:
	PPMWriter(int out_width, int out_height);
	~PPMWriter();
	
	// implementation of FrameConsumer interface
	void AcceptFrame(const Frame& frame) override;

public:
	void CreateFrameBuffer() throw (std::logic_error, std::runtime_error);
	void SetOutputDirectory(const std::string& outdir);
	void SetInputConversion(AVPixelFormat in_pix_fmt, int in_width, int in_height) throw (std::logic_error, std::runtime_error);
	
private:
	void WriteFrame(const Frame& frame) noexcept;
	
private:
	int _out_width;
	int _out_height;
	std::uint32_t _frames_count;
	std::string _output_dir;
	struct SwsContext* _scale_context;	// TO DO: replace by unique_ptr with deleter function
	int _in_height;	// needed by sws_scale
	Frame _frame;
};
