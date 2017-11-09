#pragma once

#include <cstdint>
#include <string>

#include <functional>
#include <memory>

#include <exception>
#include <stdexcept>

extern "C"
{
	#include <libavutil/pixfmt.h>
	#include <libswscale/swscale.h>
}

#include <SDL.h>

#include "frame_consumer.h"

class SDLScreen final : public FrameConsumer
{
	SDLScreen(const SDLScreen&) = delete;
	const SDLScreen& operator=(const SDLScreen&) = delete;	
	SDLScreen(SDLScreen&&) = delete;
	const SDLScreen& operator=(SDLScreen&&) = delete;

public:
	SDLScreen();
	~SDLScreen();
	
	// implementation of FrameConsumer interface
	void AcceptFrame(const Frame& frame) override;

public:
	void Initialize(const char* caption, int width, int height) throw(std::runtime_error);
	void SetInputConversion(AVPixelFormat in_pix_fmt, int in_width, int in_height) throw (std::logic_error, std::runtime_error);
		
private:
	int _width;
	int _height;
	struct SwsContext* _scale_context;	// TO DO: replace by unique_ptr with deleter function
	std::unique_ptr<SDL_Window, std::function<void (SDL_Window*)>> _window;
	std::unique_ptr<SDL_Renderer, std::function<void (SDL_Renderer*)>> _renderer;
	std::unique_ptr<SDL_Texture, std::function<void (SDL_Texture*)>> _texture;
	int _in_height;	// needed by sws_scale
	//Frame _frame;
	std::unique_ptr<std::uint8_t, std::function<void(void*)>> _yplane;
	std::unique_ptr<std::uint8_t, std::function<void(void*)>> _uplane;
	std::unique_ptr<std::uint8_t, std::function<void(void*)>> _vplane;
	int _uv_pitch;
};
