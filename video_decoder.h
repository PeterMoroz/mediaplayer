#pragma once

#include <list>

#include "packet_consumer.h"
#include "frame_provider.h"
#include "codec_context.h"


class VideoDecoder final : public PacketConsumer, public FrameProvider
{
	VideoDecoder(const VideoDecoder&) = delete;
	const VideoDecoder& operator=(const VideoDecoder&) = delete;
	VideoDecoder(VideoDecoder&&) = delete;
	const VideoDecoder& operator=(VideoDecoder&&) = delete;
	
public:
	explicit VideoDecoder(CodecContext& codec_ctx);
	~VideoDecoder();
	
	// implementation of PacketConsumer interface
	bool AcceptPacket(const Packet& packet) noexcept override;
	
private:
	// implementation of FrameProvider interface
	bool TestConsumer(const FrameConsumer* frame_consumer) const noexcept override;
		
private:
	CodecContext& _codec_ctx;
};
