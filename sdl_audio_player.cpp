#include <cassert>
#include <algorithm>

#include "logging.h"
#include "sdl_audio_player.h"


SDLAudioPlayer::SDLAudioPlayer(CodecContext& codec_ctx, SDLPacketQueue& packet_queue)
	: _codec_ctx(codec_ctx)
	, _packet_queue(packet_queue)
	, _buffer()
	, _data_size(0)
	, _rd_offset(0)
	, _packet()
	, _frame()
	, _packet_data(NULL)
	, _packet_size(0)
	, _device_id(0)
	{
		
	}
	
SDLAudioPlayer::~SDLAudioPlayer()
{
	/* Stop(); */
}

void SDLAudioPlayer::OpenDevice() throw (std::runtime_error)
{
	SDL_AudioSpec desired, obtained;
	SDL_zero(desired);
	
	static constexpr Uint16 SDL_AUDIO_BUFFER_SIZE = 1024;
	
	desired.freq = _codec_ctx->sample_rate;
	desired.format = AUDIO_S16SYS;
	desired.channels = _codec_ctx->channels;
	desired.silence = 0;
	desired.samples = SDL_AUDIO_BUFFER_SIZE;
	desired.callback = &SDLAudioPlayer::AudioCallback;
	desired.userdata = this;
	
	SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (device_id == 0)
	{
		Log::Err() << "SDL_OpenAudioDevice() failed. " << '\n';
		throw std::runtime_error("SDL_OpenAudioDevice() failed.");
	}
	
	std::swap(_device_id, device_id);
	SDL_CloseAudioDevice(device_id);
}

void SDLAudioPlayer::RunPlayback() throw (std::logic_error)
{
	if (_device_id == 0)
		throw std::logic_error("AudioDevice isn't opened yet.");
	SDL_PauseAudioDevice(_device_id, 0);
}

/*
void SDLAudioPlayer::Stop() noexcept
{
	if (_device_id != 0)
	{
		SDL_CLoseAudioDevice(_device_id);
		_device_id = 0;
	}
}
 */

void SDLAudioPlayer::AudioCallback(void* userdata, Uint8* stream, int len) noexcept
{
	assert(userdata != NULL);
	SDLAudioPlayer* host = static_cast<SDLAudioPlayer*>(userdata);
	
	try {
		host->AudioDeviceCallback(stream, len);
	} catch (const std::exception& ex) {
		Log::Err() << __FILE__ << ':' << __LINE__ << " exception: " << ex.what() << '\n';
		std::fill_n(stream, len, 0);
	}
}

void SDLAudioPlayer::AudioDeviceCallback(Uint8* stream, int len) noexcept
{
	while (len > 0)
	{
		if (_rd_offset >= _data_size)
		{
			/*
			try {
				DecodeNextFrame();
			} catch (const std::exception& ex) {
				_data_size = len;
				_buffer.fill(0);
			}
			 */
			if (!DecodeNextFrame())
			{
				_data_size = len;
				_buffer.fill(0);				
			}
			_rd_offset = 0;
		}
		
		int n = static_cast<int>(_data_size - _rd_offset);
		if (n > len)
			n = len;
			
		std::copy_n(_buffer.data() + _rd_offset, n, stream);
		len -= n;
		stream += n;
		_rd_offset += n;
	} // while (len > 0)

	
}

// 1. Fetch next frame from queue (or wait untill any will be enqueued)
// 2. fill _buffer with data, and set _data_size to appropriate value
// return true in case of success, otherwise - false
bool SDLAudioPlayer::DecodeNextFrame() noexcept
{
	
	AVPacket _packet;
	AVFrame _frame;
	std::uint8_t* _packet_data;
	int _packet_size;
		
	for (;;)
	{
		while (_packet_size > 0)
		{
			int got_frame = 0;
			int len = avcodec_decode_audio4(_codec_ctx, &_frame, &got_frame, &_packet);
			if (len < 0)
			{
				Log::Err() << "An error when decoding audio frame. " << '\n';
				_packet_size = 0;
				break;
			}
			_packet_data += len;
			_packet_size -= len;
			
			if (got_frame)
			{
				int data_size = av_samples_get_buffer_size(NULL, _codec_ctx->channels, _frame.nb_samples, _codec_ctx->sample_fmt, 1);
				if (data_size < 0)
				{
					Log::Err() << "An error when get calculate data size required to audio data. " << '\n';
					return false;
				}
				if (data_size == 0)
					continue;
			
				_data_size = data_size;
				std::copy_n(_frame.data[0], data_size, _buffer.data());
				return true;
			}
		} // while (_packet_size > 0)
		
		if (_packet.data)
			av_packet_unref(&_packet);
		
		/*
		if (_quit) {
			return false;
		}
		 */
		 
		// blocking, retrurns false in case of error
		if (_packet_queue.GetPacket(_packet))
		{
			Log::Err() << "Couldn't get packet from audio packets queue." << '\n';
			return false;
		}
		
		_packet_data = _packet.data;
		_packet_size = _packet.size;
	} // for (;;)
	
	return false;
}
