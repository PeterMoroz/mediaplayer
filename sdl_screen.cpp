#include <cassert>

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>


extern "C"
{
	#include <libavcodec/avcodec.h>
}

#include "sdl_screen.h"


SDLScreen::SDLScreen()
	: _width(0)
	, _height(0)
	, _scale_context(NULL)
	, _window(NULL, SDL_DestroyWindow)
	, _renderer(NULL, SDL_DestroyRenderer)
	, _texture(NULL, SDL_DestroyTexture)
	, _in_height(0)
	, _yplane(NULL, free)
	, _uplane(NULL, free)
	, _vplane(NULL, free)
	, _uv_pitch(0)
	{
	}

SDLScreen::~SDLScreen()
{
	sws_freeContext(_scale_context);
}

void SDLScreen::Initialize(const char* caption, int width, int height) throw (std::runtime_error)
{
	using namespace std;

	unique_ptr<SDL_Window, function<void (SDL_Window*)>> window(
			SDL_CreateWindow(caption, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0),
			SDL_DestroyWindow);
	if (!window)
		throw runtime_error("SDL_CreateWindow() failed.");
	unique_ptr<SDL_Renderer, function<void (SDL_Renderer*)>> renderer(SDL_CreateRenderer(window.get(), -1, 0), SDL_DestroyRenderer);
	if (!renderer)
		throw runtime_error("SDL_CreateRenderer() failed.");
	unique_ptr<SDL_Texture, function<void (SDL_Texture*)>> texture(
			SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, width, height),
			SDL_DestroyTexture);
	if (!texture)
		throw runtime_error("SDL_CreateTexture() failed.");

	size_t yplane_sz = width * height;
	size_t uvplane_sz = width * height / 4;

	unique_ptr<uint8_t, function<void(void*)>> yplane(static_cast<uint8_t*>(malloc(yplane_sz)), free);
	unique_ptr<uint8_t, function<void(void*)>> uplane(static_cast<uint8_t*>(malloc(uvplane_sz)), free);
	unique_ptr<uint8_t, function<void(void*)>> vplane(static_cast<uint8_t*>(malloc(uvplane_sz)), free);

	if (!yplane || !uplane || !vplane)
		throw runtime_error("malloc() failed.");

	_width = width;
	_height = height;
	_uv_pitch = width / 2;

	_texture.swap(texture);
	_renderer.swap(renderer);
	_window.swap(window);

	_yplane.swap(yplane);
	_uplane.swap(uplane);
	_vplane.swap(vplane);
}

void SDLScreen::AcceptFrame(const Frame& frame)
{
	assert(_scale_context);
	assert(_yplane && _uplane && _vplane);

	AVPicture picture;
	picture.data[0] = _yplane.get();
	picture.data[1] = _uplane.get();
	picture.data[2] = _vplane.get();
	picture.linesize[0] = _width;
	picture.linesize[1] = _uv_pitch;
	picture.linesize[2] = _uv_pitch;

	int dst_height = sws_scale(_scale_context, frame->data, frame->linesize, 0, _in_height, picture.data, picture.linesize);
	// TO DO: check that dst_height == _dst_height


	SDL_UpdateYUVTexture(_texture.get(), NULL, _yplane.get(), _width, _uplane.get(), _uv_pitch, _vplane.get(), _uv_pitch);

	SDL_RenderClear(_renderer.get());
	SDL_RenderCopy(_renderer.get(), _texture.get(), NULL, NULL);
	SDL_RenderPresent(_renderer.get());
}

void SDLScreen::SetInputConversion(AVPixelFormat in_pix_fmt, int in_width, int in_height) throw (std::logic_error, std::runtime_error)
{
	SwsContext* scale_context = sws_getContext(in_width, in_height, in_pix_fmt,
												_width, _height, AV_PIX_FMT_YUV420P,
												SWS_BILINEAR, NULL, NULL, NULL);
	if (!scale_context)
		throw std::runtime_error("sws_getContext() failed.");
	std::swap(_scale_context, scale_context);
	sws_freeContext(scale_context);

	_in_height = in_height;
}
