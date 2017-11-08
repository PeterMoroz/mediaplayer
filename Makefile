# CPPFLAGS := $(CPPFLAGS) -I$(HOME)/ffmpeg_build/include
# CXXFLAGS += -std=c++11 -g
# LDFLAGS := $(LDFLAGS) -L$(HOME)/ffmpeg_build/lib
# LDFLAGS += -Wl,--start-group -lavformat -lavcodec -lswscale -lz -Wl,--end-group

# CC := g++

# all: mediaplayer

# SRC := mediaplayer.cpp
# OBJS := $(SRC:.cpp=.o)
# mediaplayer: $(OBJS)

PKG_CONFIG_PATH := $(HOME)/ffmpeg_build/lib/pkgconfig

all: mediaplayer

SRC := codec_context.cpp format_context.cpp frame.cpp frames_reader.cpp packet.cpp ppm_writer.cpp stream_demuxer.cpp video_decoder.cpp mediaplayer.cpp
INC := codec_context.h dump_ffmpeg_error.h format_context.h frame.h frame_consumer.h frames_reader.h packet.h packet_consumer.h ppm_writer.h stream_demuxer.h video_decoder.h

mediaplayer: $(SRC) $(INC)
	# g++ -std=c++11 -g -I$(HOME)/ffmpeg_build/include format_context.cpp mediaplayer.cpp -o mediaplayer -L$(HOME)/ffmpeg_build/lib -lavformat -lavcodec -lswscale -lz
	# c++ -std=c++11  -o mediaplayer format_context.cpp mediaplayer.cpp  -I$(HOME)/projects/ffmpeg/library/include -L$(HOME)/projects/ffmpeg/library/lib -Wl,--start-group -lavformat -lavcodec -lswscale -lz -Wl,--end-group
	# c++ -std=c++11  -o mediaplayer format_context.cpp mediaplayer.cpp  `pkg-config libavformat libavcodec --cflags --libs`
	c++ -std=c++11 -Wall -g -O0 -o mediaplayer $(SRC)  `pkg-config libavformat libavcodec libswscale --cflags --libs` -lstdc++fs
	
clean:
	# rm *.o mediaplayer
	rm mediaplayer