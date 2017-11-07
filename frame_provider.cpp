#include <cassert>

#include "frame_consumer.h"
#include "frame_provider.h"

bool FrameProvider::AddFrameConsumer(FrameConsumer* frame_consumer)
{
	assert(frame_consumer != NULL);
	if (!TestConsumer(frame_consumer))
		return false;
	_consumers.push_back(frame_consumer);
	return true;
}
