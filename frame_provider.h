#pragma once

#include <list>

// #include "frame_consumer.h"
class FrameConsumer;

class FrameProvider
{
public:
	virtual ~FrameProvider() = default;
	
	bool AddFrameConsumer(FrameConsumer* frame_consumer);
	
protected:
	virtual bool TestConsumer(const FrameConsumer* frame_consumer) const noexcept = 0;

private:
	std::list<FrameConsumer*> _consumers;
	
protected:
	const std::list<FrameConsumer*>& consumers() const noexcept
	{ return _consumers; }
};
