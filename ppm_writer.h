#pragma once

#include <cstdint>
#include <string>

#include <exception>
#include <stdexcept>

#include "frame_consumer.h"

class PPMWriter final : public FrameConsumer
{
	PPMWriter(const PPMWriter&) = delete;
	const PPMWriter& operator=(const PPMWriter&) = delete;	
	PPMWriter(PPMWriter&&) = delete;
	const PPMWriter& operator=(PPMWriter&&) = delete;

public:
	PPMWriter(AVPixelFormat pix_fmt, int width, int height);
	~PPMWriter();
	
	// implementation of FrameConsumer interface
	bool AgreeWith(AVPixelFormat format, int width, int height) const noexcept override
	{
		return (format == _frame_pix_format && width == _frame_width && height == _frame_height);
	}
	void AcceptFrame(const Frame& frame) override;

public:	
	void SetOutputDirectory(const std::string& outdir);
	
private:
	void WriteFrame(const Frame& frame) noexcept;
	
private:
	AVPixelFormat _frame_pix_format;
	int _frame_width;
	int _frame_height;
	std::uint32_t _frames_count;
	std::string _output_dir;
};