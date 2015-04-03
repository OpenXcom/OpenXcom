/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_ADLIBMUSIC_H
#define OPENXCOM_ADLIBMUSIC_H

#include "Music.h"
#include <map>
#include <string>
#include <SDL_mixer.h>

namespace OpenXcom
{

/**
 * Container for Adlib music tracks.
 * Uses a custom YM3812 music player passed to SDL_mixer.
 */
class AdlibMusic : public Music
{
private:
	char *_data;
	size_t _size;
	float _volume;
	static int delay, rate;
	static std::map<int, int> delayRates;
public:
	/// Creates a blank music track.
	AdlibMusic(float volume = 1.0f);
	/// Cleans up the music track.
	~AdlibMusic();
	/// Loads music from the specified file.
	void load(const std::string &filename);
	/// Loads music from a chunk of memory.
	void load(const void *data, int size);
	/// Plays the music.
	void play(int loop = -1) const;
	/// Adlib music player.
	static void player(void *udata, Uint8 *stream, int len);
	bool isPlaying();
};

}

#endif
