#pragma once

#include <functional>
#include <memory>
#include <queue>
#include <stdexcept>

#include <SDL.h>

#include "packet_consumer.h"


class SDLPacketQueue final : public PacketConsumer
{
	SDLPacketQueue(const SDLPacketQueue&) = delete;
	const SDLPacketQueue& operator=(const SDLPacketQueue&) = delete;
	SDLPacketQueue(SDLPacketQueue&&) = delete;
	const SDLPacketQueue& operator=(SDLPacketQueue&&) = delete;
	
public:
	SDLPacketQueue() throw(std::runtime_error);
	~SDLPacketQueue();
	
	// implementation of PacketConsumer interface
	bool AcceptPacket(const AVPacket& packet) noexcept override;
	
	bool GetPacket(AVPacket& packet) noexcept;
	
private:
	std::queue<AVPacket> _queue;
	std::unique_ptr<SDL_mutex, std::function<void(SDL_mutex*)>> _mutex;
	std::unique_ptr<SDL_cond, std::function<void(SDL_cond*)>> _cond;
};
