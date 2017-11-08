#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include <experimental/filesystem>


#include "ppm_writer.h"


PPMWriter::PPMWriter(int out_width, int out_height)
	: _out_width(out_width)
	, _out_height(out_height)
	, _frames_count(0)
	, _output_dir(".")
	, _scale_context(NULL)
	, _in_height(0)
	, _frame()
	{
	}
	
PPMWriter::~PPMWriter()
{
	sws_freeContext(_scale_context);
}
	
void PPMWriter::AcceptFrame(const Frame& frame)
{
	if (_scale_context)
	{
		int dst_height = sws_scale(_scale_context, frame->data, frame->linesize, 0, _in_height, _frame->data, _frame->linesize);
		// TO DO: check that dst_height == _dst_height
		WriteFrame(_frame);
		return;
	}
	
	WriteFrame(frame);
}


void PPMWriter::CreateFrameBuffer() throw (std::logic_error, std::runtime_error)
{
	_frame.CreateBuffer(AV_PIX_FMT_RGB24, _out_width, _out_height);
}

void PPMWriter::SetOutputDirectory(const std::string& outdir)
{
	namespace fs = std::experimental::filesystem;
	
	_output_dir = outdir;
	if (!fs::exists(_output_dir.c_str()))
		fs::create_directory(_output_dir.c_str());
}

void PPMWriter::SetInputConversion(AVPixelFormat in_pix_fmt, int in_width, int in_height) throw (std::logic_error, std::runtime_error)
{
	SwsContext* scale_context = sws_getContext(in_width, in_height, in_pix_fmt, 
												_out_width, _out_height, AV_PIX_FMT_RGB24, 
												SWS_BILINEAR, NULL, NULL, NULL);
	if (!scale_context)
		throw std::runtime_error("sws_getContext() failed.");
	std::swap(_scale_context, scale_context);
	sws_freeContext(scale_context);
	
	_in_height = in_height;
}

void PPMWriter::WriteFrame(const Frame& frame) noexcept
{
	try {
		std::ostringstream oss;
		oss << _output_dir << "/frame-" << _frames_count << ".ppm";
		
		std::ofstream ofs(oss.str().c_str(), std::ofstream::binary);
		if (!ofs)
		{
			std::cerr << __FILE__ << ':' << __LINE__
				<< " Couldn't create file " << oss.str() << std::endl;
			return;
		}
		
		ofs << "P6\n" << _out_width << ' ' << _out_height << "\n255\n";
		for (int row = 0; row < _out_height; row++)
		{
			const char* data = reinterpret_cast<char*>(const_cast<unsigned char*>(frame->data[0]));
			if (!ofs.write(data + row * frame->linesize[0], _out_width * 3))
				throw std::runtime_error("write failed.");
		}

		_frames_count++;
	} catch (const std::exception& ex) {
		std::cerr << __FILE__ << ':' << __LINE__
				<< " Exception: " << ex.what() << std::endl;
	}
}
