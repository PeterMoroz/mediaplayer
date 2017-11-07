#pragma once

#include <stdexcept>

extern "C"
{
	#include <libavutil/pixfmt.h>
	#include <libswscale/swscale.h>
}

class ScaleContext
{
	ScaleContext(const ScaleContext&) = delete;
	const ScaleContext& operator=(const ScaleContext&) = delete;
	ScaleContext(ScaleContext&&) = delete;
	const ScaleContext& operator=(ScaleContext&&) = delete;
	
public:
	ScaleContext();
	~ScaleContext();
	
	void Create(int src_width, int src_height, AVPixelFormat src_format,
				int dst_width, int dst_height, AVPixelFormat dst_format, int flags) throw (std::runtime_error);

	operator const struct SwsContext*() const noexcept
	{ return _scale_context; }
	operator struct SwsContext*() noexcept
	{ return _scale_context; }
	
	operator bool() const noexcept { return _scale_context != NULL; }

private:
	struct SwsContext* _scale_context;
};
