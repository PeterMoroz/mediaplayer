#pragma once

#include <map>
#include <stdexcept>

#include "packet_consumer.h"


class StreamDemuxer final : public PacketConsumer
{
	StreamDemuxer(const StreamDemuxer&) = delete;
	const StreamDemuxer& operator=(const StreamDemuxer&) = delete;
	StreamDemuxer(StreamDemuxer&&) = delete;
	const StreamDemuxer& operator=(StreamDemuxer&&) = delete;

public:
	StreamDemuxer();
	~StreamDemuxer();
	
	void AddStreamTarget(int stream_idx, PacketConsumer* packet_consumer) throw (std::logic_error);
	void RemoveStreamTarget(int stream_idx);
	
	bool AcceptPacket(const AVPacket& packet) noexcept override;
	
private:
	std::map<int, PacketConsumer*> _consumers;
};