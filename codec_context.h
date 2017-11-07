#pragma once

#include <stdexcept>

extern "C"
{
#include <libavcodec/avcodec.h>
}

class CodecContext final
{
	CodecContext(const CodecContext&) = delete;
	const CodecContext& operator=(const CodecContext&) = delete;
	CodecContext(CodecContext&&) = delete;
	const CodecContext& operator=(CodecContext&&) = delete;
	
public:
	CodecContext();
	~CodecContext();
	
	/*
	void CopyContext(AVCodecContext* codec_context) throw (std::logic_error, std::runtime_error);
	void OpenCodec() throw (std::logic_error, std::runtime_error);
	 */
	 
	void OpenCodec(AVCodecContext* codec_context_orig) throw (std::logic_error, std::runtime_error);
	
	AVCodecContext* operator->() const noexcept
	{ return _codec_context; }
	operator AVCodecContext*() const noexcept
	{ return _codec_context; }
	
	operator bool() const noexcept { return _codec_context != NULL; }
	
private:
	AVCodecContext* _codec_context;
	AVCodec* _codec;
};
