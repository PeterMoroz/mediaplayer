#pragma once

#include <list>

#include "packet_consumer.h"
#include "codec_context.h"

class FrameConsumer;

class VideoDecoder final : public PacketConsumer
{
	VideoDecoder(const VideoDecoder&) = delete;
	const VideoDecoder& operator=(const VideoDecoder&) = delete;
	VideoDecoder(VideoDecoder&&) = delete;
	const VideoDecoder& operator=(VideoDecoder&&) = delete;
	
public:
	explicit VideoDecoder(CodecContext& codec_ctx);
	~VideoDecoder();
	
	// implementation of PacketConsumer interface
	bool AcceptPacket(const AVPacket& packet) noexcept override;
	
	void AddFrameConsumer(FrameConsumer* frame_consumer);
		
private:
	CodecContext& _codec_ctx;
	std::list<FrameConsumer*> _consumers;
};
