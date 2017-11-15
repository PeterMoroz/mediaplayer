#include <cassert>

#include "stream_demuxer.h"


StreamDemuxer::StreamDemuxer()
	: _consumers()
{
	
}

StreamDemuxer::~StreamDemuxer()
{
	
}

void StreamDemuxer::AddStreamTarget(int stream_idx, PacketConsumer* packet_consumer) throw (std::logic_error)
{
	if (_consumers.count(stream_idx) != 0)
		throw std::logic_error("StreamTarget already exist.");
	assert(packet_consumer != NULL);
	_consumers.insert(std::make_pair(stream_idx, packet_consumer));
}

void StreamDemuxer::RemoveStreamTarget(int stream_idx)
{
	_consumers.erase(stream_idx);
}

bool StreamDemuxer::AcceptPacket(const AVPacket& packet) noexcept
{
	std::map<int, PacketConsumer*>::iterator it = _consumers.find(packet.stream_index);
	if (it == _consumers.end())
		return false;
	return (*it->second).AcceptPacket(packet);
}
