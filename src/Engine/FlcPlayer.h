#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
/*
 * Based on http://www.libsdl.org/projects/flxplay/
 */
#include <SDL.h>

namespace OpenXcom
{

class Screen;
class Game;

class FlcPlayer
{
private:

	Uint8 *_fileBuf;
	Uint32 _fileSize;
	Uint8 *_videoFrameData;
	Uint8 *_chunkData;
	Uint8 *_audioFrameData;
	Uint16 _frameCount;    /* Frame Counter */
	Uint32 _headerSize;    /* Fli file size */
	Uint16 _headerType;    /* Fli header check */
	Uint16 _headerFrames;  /* Number of frames in flic */
	Uint16 _headerWidth;   /* Fli width */
	Uint16 _headerHeight;  /* Fli height */
	Uint16 _headerDepth;   /* Color depth */
	Uint16 _headerSpeed;   /* Number of video ticks between frame */
	Uint32 _videoFrameSize;     /* Frame size in bytes */
	Uint16 _videoFrameType;
	Uint16 _frameChunks;   /* Number of chunks in frame */
	Uint32 _chunkSize;     /* Size of chunk */
	Uint16 _chunkType;     /* Type of chunk */
	Uint16 _delayOverride; /* FRAME_TYPE extension */
	Uint32 _audioFrameSize;
	Uint16 _audioFrameType;

	void (*_frameCallBack)();

	SDL_Surface *_mainScreen;
	Screen *_realScreen;
	SDL_Color _colors[256];
	int _screenWidth;
	int _screenHeight;
	int _screenDepth;
	int _dx, _dy;
	int _offset;
	int _playingState;
	bool _hasAudio, _useInternalAudio;
	int _videoDelay;
	double _volume;

	typedef struct AudioBuffer
	{
		Sint16 *samples;
		int sampleCount;
		int sampleBufSize;
		int currSamplePos;
	}AudioBuffer;


	typedef struct AudioData
	{
		int sampleRate;
		AudioBuffer *loadingBuffer;
		AudioBuffer *playingBuffer;
		SDL_sem *sharedLock;

	}AudioData;

	AudioData _audioData;

	Game *_game;

	void readU16(Uint16 &dst, const Uint8 *const src);
	void readU32(Uint32 &dst, const Uint8 *const src);
	void readS16(Sint16 &dst, const Sint8 *const src);
	void readS32(Sint32 &dst, const Sint8 *const src);
	void readFileHeader();

	bool isValidFrame(Uint8 *frameHeader, Uint32 &frameSize, Uint16 &frameType);
	void decodeVideo(bool skipLastFrame);
	void decodeAudio(int frames);
	void waitForNextFrame(Uint32 delay);
	void SDLPolling();
	bool shouldQuit();

	void playVideoFrame();
	void color256();
	void fliBRun();
	void fliCopy();
	void fliSS2();
	void fliLC();
	void color64();
	void black();

	void playAudioFrame(Uint16 sampleRate);
	void initAudio(Uint16 format, Uint8 channels);
	void deInitAudio();

	bool isEndOfFile(Uint8 *pos);

	static void audioCallback(void *userData, Uint8 *stream, int len);

public:

	FlcPlayer();
	~FlcPlayer();

	/// Open FLC or FLI file, read header, prepare to play it
	bool init(const char *filename, void(*frameCallBack)(), Game *game, bool useAudio, int dx, int dy);
	/// Play the loaded file; set flc.mainScreen first!
	void play(bool skipLastFrame);
	/// Free memory, free love, etc.
	void deInit();
	// Stop FLC Player
	void stop();
	/// Delay player at the end
	void delay(Uint32 milliseconds);
	void setHeaderSpeed(int speed);
	int getFrameCount();
	bool wasSkipped();
};

}
