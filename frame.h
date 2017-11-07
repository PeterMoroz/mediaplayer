#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>

extern "C"
{
	#include <libavutil/frame.h>
}

class Frame
{
	Frame(const Frame&) = delete;
	const Frame& operator=(const Frame&) = delete;
	Frame(Frame&&) = delete;
	const Frame& operator=(Frame&&) = delete;
	
public:
	Frame() throw(std::runtime_error);
	~Frame();
	
	AVFrame* operator&() noexcept { return _frame; }
	const AVFrame* operator&() const noexcept { return _frame; }
	
	AVFrame* operator->() noexcept
	{ return _frame; }
	const AVFrame* operator->() const noexcept
	{ return _frame; }
	
	operator bool() const noexcept { return _frame != NULL; }
	
	void CreateBuffer(AVPixelFormat pix_fmt, int width, int height) throw(std::logic_error, std::runtime_error);
	
private:
	AVFrame* _frame;
	std::unique_ptr<std::uint8_t, std::function<void(void*)>> _buffer;
};
