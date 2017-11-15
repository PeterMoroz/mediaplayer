PKG_CONFIG_PATH := $(HOME)/ffmpeg_build/lib/pkgconfig

all: mediaplayer

SRC := codec_context.cpp format_context.cpp frame.cpp frames_reader.cpp sdl_screen.cpp sdl_audio_player.cpp sdl_packet_queue.cpp stream_demuxer.cpp video_decoder.cpp mediaplayer.cpp
INC := codec_context.h dump_ffmpeg_error.h format_context.h frame.h frame_consumer.h frames_reader.h packet_consumer.h sdl_audio_player.h sdl_packet_queue.h sdl_screen.h stream_demuxer.h video_decoder.h

mediaplayer: $(SRC) $(INC)
	c++ -std=c++11 -Wall -g -O0 -o mediaplayer $(SRC)  `pkg-config libavformat libavcodec libswscale --cflags --libs` -lstdc++fs `sdl2-config --cflags --libs`
	
clean:
	rm mediaplayer
