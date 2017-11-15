#include <cassert>

#include "logging.h"
#include "sdl_packet_queue.h"


class SDLMutexLock final
{
	SDLMutexLock(const SDLMutexLock&) = delete;
	const SDLMutexLock& operator=(const SDLMutexLock&) = delete;
	SDLMutexLock(SDLMutexLock&&) = delete;
	const SDLMutexLock& operator=(SDLMutexLock&&) = delete;
	
public:
	explicit SDLMutexLock(SDL_mutex* sdl_mutex)  throw (std::runtime_error);
	~SDLMutexLock();
	
	
private:
	SDL_mutex* _sdl_mutex;
};


SDLMutexLock::SDLMutexLock(SDL_mutex* sdl_mutex) throw (std::runtime_error)
	: _sdl_mutex(sdl_mutex)
	{
		assert(_sdl_mutex != NULL);
		
		if (SDL_LockMutex(_sdl_mutex) != 0)
		{
			_sdl_mutex = NULL;
			Log::Err() << "SDL_LockMutex() failed: " << SDL_GetError() << '\n';
			throw std::runtime_error("SDL_LockMutex() failed.");
		}
	}
	
SDLMutexLock::~SDLMutexLock()
{
	assert(_sdl_mutex != NULL);
	if (SDL_UnlockMutex(_sdl_mutex) != 0)
		Log::Err() << "SDL_UnlockMutex() failed: " << SDL_GetError() << '\n';
}




SDLPacketQueue::SDLPacketQueue() throw (std::runtime_error)
	: _queue()
	, _mutex(NULL, SDL_DestroyMutex)
	, _cond(NULL, SDL_DestroyCond)
	{
		std::unique_ptr<SDL_mutex, std::function<void(SDL_mutex*)>> mutex(SDL_CreateMutex(), SDL_DestroyMutex);
		if (!mutex)
		{
			Log::Err() << "SDL_CreateMutex() failed: " << SDL_GetError() << '\n';
			throw std::runtime_error("SDL_CreateMutex() failed.");
		}
		
		std::unique_ptr<SDL_cond, std::function<void(SDL_cond*)>> cond(SDL_CreateCond(), SDL_DestroyCond);
		if (!cond)
		{
			Log::Err() << "SDL_CreateCond() failed: " << SDL_GetError() << '\n';
			throw std::runtime_error("SDL_CreateCond() failed.");
		}
		
		std::swap(_mutex, mutex);
		std::swap(_cond, cond);
	}
	
SDLPacketQueue::~SDLPacketQueue()
{
	
}

bool SDLPacketQueue::AcceptPacket(const AVPacket& packet) noexcept
{
	assert(_mutex);
	assert(_cond);
	
	try {
		SDLMutexLock(_mutex.get());
			
		_queue.push(packet);
		
		if (SDL_CondSignal(_cond.get()) != 0)
		{
			Log::Err() << "SDL_CondSignal() failed: " << SDL_GetError() << '\n';
			throw std::runtime_error("SDL_CondSignal() failed.");
		}
		
	} catch (const std::exception& ex) {
		Log::Err() << __FILE__ << ':' << __LINE__ << " Exception: " << ex.what() << '\n';
		return false;
	}

	return true;
}

bool SDLPacketQueue::GetPacket(AVPacket& packet) noexcept
{
	assert(_mutex);
	assert(_cond);
	
	try {
	
		SDLMutexLock(_mutex.get());
		if (_queue.empty())
		{
			if (SDL_CondWait(_cond.get(), _mutex.get()) != 0)
			{
				Log::Err() << "SDL_CondWait() failed: " << SDL_GetError() << '\n';
				throw std::runtime_error("SDL_CondWait() failed.");
			}
		}
		
		packet = _queue.front();
		_queue.pop();
		
		return true;
		
	} catch (const std::exception& ex) {
		Log::Err() << __FILE__ << ':' << __LINE__ << " Exception: " << ex.what() << '\n';
		return false;		
	}

	assert(false);	// have tor be unreachable
	return false;
}
