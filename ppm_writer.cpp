#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include <experimental/filesystem>


#include "ppm_writer.h"


PPMWriter::PPMWriter(AVPixelFormat pix_fmt, int width, int height)
	: _frame_pix_format(pix_fmt)
	, _frame_width(width)
	, _frame_height(height)
	, _frames_count(0)
	, _output_dir(".")
	{
		
	}
	
PPMWriter::~PPMWriter()
{
	
}
	
void PPMWriter::AcceptFrame(const Frame& frame)
{
	WriteFrame(frame);
}


void PPMWriter::SetOutputDirectory(const std::string& outdir)
{
	namespace fs = std::experimental::filesystem;
	
	_output_dir = outdir;
	if (!fs::exists(_output_dir.c_str()))
		fs::create_directory(_output_dir.c_str());
	
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
		
		ofs << "P6\n" << _frame_width << ' ' << _frame_height << "\n255\n";
		for (int row = 0; row < _frame_height; row++)
		{
			const char* data = reinterpret_cast<char*>(const_cast<unsigned char*>(frame->data[0]));
			if (!ofs.write(data + row * frame->linesize[0], _frame_width * 3))
				throw std::runtime_error("write failed.");
		}

		_frames_count++;
	} catch (const std::exception& ex) {
		std::cerr << __FILE__ << ':' << __LINE__
				<< " Exception: " << ex.what() << std::endl;
	}
}
