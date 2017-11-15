#pragma once

extern "C"
{
	#include <libavcodec/avcodec.h>
}


class PacketConsumer
{
public:
	virtual ~PacketConsumer() = default;
	
	virtual bool AcceptPacket(const AVPacket& packet) noexcept = 0;
};
