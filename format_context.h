#pragma once

extern "C"
{
#include <libavformat/avformat.h>
}

struct AVCodecContext;

#include <stdexcept>


class FormatContext final
{
	FormatContext(const FormatContext&) = delete;
	const FormatContext& operator=(const FormatContext&) = delete;
	FormatContext(FormatContext&&) = delete;
	const FormatContext& operator=(FormatContext&&) = delete;
	
public:
	FormatContext();
	~FormatContext();
	
	void OpenInput(const char* filename) throw (std::logic_error, std::runtime_error);
	void FindStreamInfo() throw (std::logic_error, std::runtime_error);
	
	int FindFirstStreamByType(int type) throw (std::logic_error);
	AVCodecContext* GetCodecContext(int stream_idx) const throw (std::logic_error);
	

	/* TO DO: unify these methods under single class template
	 * ex.
	 * template <typename T>
	 * class Resource
	 * {
	 * public:
	 * 	T* operator->() const noexcept { return _resource; }
	 * 	operator T*() const noexcept { return _resource; }
	 * operator bool() const noexcept { return _resource != NULL; }
	 * protected:
	 * 	T* _resource;
	 * };
	*/ 
	AVFormatContext* operator->() const noexcept
	{ return _format_context; }
	operator AVFormatContext*() const noexcept
	{ return _format_context; }
	
	operator bool() const noexcept { return _format_context != NULL; }

	
private:
	AVFormatContext* _format_context;
};
