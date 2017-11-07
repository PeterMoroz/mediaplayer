#pragma once

extern "C"
{
	#include <libavcodec/avcodec.h>
}

class Packet final
{
	Packet(const Packet&) = delete;
	const Packet& operator=(const Packet&) = delete;
	Packet(Packet&&) = delete;
	const Packet& operator=(Packet&&) = delete;
	
public:
	Packet();
	~Packet();
	
	int stream_index() const noexcept { return _packet.stream_index; }

	//operator AVPacket*() noexcept { return &_packet; }
	AVPacket* operator&() noexcept { return &_packet; }
	const AVPacket* operator&() const noexcept { return &_packet; }
	
private:
	AVPacket _packet;
};