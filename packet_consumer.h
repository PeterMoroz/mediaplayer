#pragma once

#include "packet.h"

class PacketConsumer
{
public:
	virtual ~PacketConsumer() = default;
	
	virtual bool AcceptPacket(const Packet& packet) noexcept = 0;
};
