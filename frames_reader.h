#pragma once

#include <list>
#include <stdexcept>

#include "format_context.h"

class PacketConsumer;

class FramesReader final
{
	FramesReader(const FramesReader&) = delete;
	const FramesReader& operator=(const FramesReader&) = delete;
	FramesReader(FramesReader&&) = delete;
	const FramesReader& operator=(FramesReader&&) = delete;
	
public:
	FramesReader();
	~FramesReader();
	
	void AddPacketConsumer(PacketConsumer* packet_consumer);
	
	void ReadFrames(FormatContext& format_context) throw (std::runtime_error);
	
private:
	std::list<PacketConsumer*> _consumers;
};
