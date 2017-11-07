#pragma once

#include "frame.h"

class FrameConsumer
{
public:
	virtual ~FrameConsumer() = default;

	virtual bool AgreeWith(AVPixelFormat format, int width, int height) const noexcept = 0;	
	virtual void AcceptFrame(const Frame& frame) = 0;
};
