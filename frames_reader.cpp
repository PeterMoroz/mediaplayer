#include <cassert>
#include <iostream>

extern "C"
{
	#include <libavformat/avformat.h>
}

#include <SDL.h>

#include "frames_reader.h"
#include "packet_consumer.h"


FramesReader::FramesReader()
	: _consumers()
	{
		
	}
	
FramesReader::~FramesReader()
{
	
}

void FramesReader::AddPacketConsumer(PacketConsumer* packet_consumer)
{
	assert(packet_consumer != NULL);
	_consumers.push_back(packet_consumer);
}

void FramesReader::ReadFrames(FormatContext& format_context) throw (std::runtime_error)
{
	Packet packet;
	//AVPacket packet;
	int rc = 0;
	while ((rc = av_read_frame(format_context, &packet)) >= 0)
	{
		for (PacketConsumer* consumer : _consumers)
			consumer->AcceptPacket(packet);	// ignore returned value
		av_free_packet(&packet);
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;
	}
	rc = rc; // dummy code. TO DO: check here, if EOF or error
}
