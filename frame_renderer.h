#pragma once

#include "frame.h"

class FrameRenderer
{
public:
	virtual ~FrameRenderer() = default;
	
	virtual void DrawFrame(const Frame& frame, int width, int height) = 0;
};
