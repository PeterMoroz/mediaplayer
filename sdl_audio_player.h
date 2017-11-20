#pragma once

#include <cstdint>
#include <array>
#include <stdexcept>

#include <SDL.h>

extern "C"
{
	#include <libavutil/frame.h>
	#include <libavcodec/avcodec.h>
	#include <libswresample/swresample.h>
}



#include "sdl_packet_queue.h"
#include "codec_context.h"

class SDLAudioPlayer final
{
	SDLAudioPlayer(const SDLAudioPlayer&) = delete;
	const SDLAudioPlayer& operator=(const SDLAudioPlayer&) = delete;
	SDLAudioPlayer(SDLAudioPlayer&&) = delete;
	const SDLAudioPlayer& operator=(SDLAudioPlayer&&) = delete;

	static constexpr std::size_t MAX_AUDIO_FRAME_SIZE = 192000;
	static constexpr std::size_t BUFFER_SIZE = (MAX_AUDIO_FRAME_SIZE * 3) / 2;

public:
	SDLAudioPlayer(CodecContext& codec_ctx, SDLPacketQueue& packet_queue);
	~SDLAudioPlayer();

	void InitResample() throw (std::runtime_error);
	void OpenDevice() throw (std::runtime_error);
	void RunPlayback() throw (std::logic_error);

	/*
	void Stop() noexcept;
	 */

private:
	static void AudioCallback(void* userdata, Uint8* stream, int len) noexcept;

	void AudioDeviceCallback(Uint8* stream, int len) noexcept;

	bool DecodeNextFrame() noexcept;

private:
	CodecContext& _codec_ctx;
	SDLPacketQueue& _packet_queue;
	std::array<std::uint8_t, BUFFER_SIZE> _buffer;
	std::size_t _data_size;
	std::size_t _rd_offset;

	struct SwrContext* _resample_context;

	AVPacket _packet;
	AVFrame _frame;
	std::uint8_t* _packet_data;
	int _packet_size;

	SDL_AudioDeviceID _device_id;
};
