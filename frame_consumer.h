#pragma once

#include "frame.h"

class FrameConsumer
{
public:
	virtual ~FrameConsumer() = default;

	virtual void AcceptFrame(const Frame& frame) = 0;
};
