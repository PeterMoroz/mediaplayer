#include "scale_context.h"

ScaleContext::ScaleContext()
	: _scale_context(NULL)
	{
		
	}
	
ScaleContext::~ScaleContext()
{
	sws_freeContext(_scale_context);
}

void ScaleContext::Create(int src_width, int src_height, AVPixelFormat src_format,
						int dst_width, int dst_height, AVPixelFormat dst_format, int flags) throw (std::runtime_error)
{
	SwsContext* scale_context = sws_getContext(src_width, src_height, src_format, 
												dst_width, dst_height, dst_format, 
												flags, NULL, NULL, NULL);
	if (!scale_context)
		throw std::runtime_error("sws_getContext() failed.");
	std::swap(_scale_context, scale_context);
	sws_freeContext(scale_context);
}
