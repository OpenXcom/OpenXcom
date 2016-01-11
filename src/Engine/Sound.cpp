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
#include "Sound.h"
#include "Exception.h"
#include "Options.h"
#include "Logger.h"
#include "Language.h"

namespace OpenXcom
{

/**
 * Initializes a new sound effect.
 */
Sound::Sound() : _sound(0)
{
}

/**
 * Deletes the loaded sound content.
 */
Sound::~Sound()
{
	Mix_FreeChunk(_sound);
}

/**
 * Loads a sound file from a specified filename.
 * @param filename Filename of the sound file.
 */
void Sound::load(const std::string &filename)
{
	// SDL only takes UTF-8 filenames
	// so here's an ugly hack to match this ugly reasoning
	std::string utf8 = Language::wstrToUtf8(Language::fsToWstr(filename));

	_sound = Mix_LoadWAV(utf8.c_str());
	if (_sound == 0)
	{
		std::string err = filename + ":" + Mix_GetError();
		throw Exception(err);
	}
}

/**
 * Loads a sound file from a specified memory chunk.
 * @param data Pointer to the sound file in memory
 * @param size Size of the sound file in bytes.
 */
void Sound::load(const void *data, unsigned int size)
{
	SDL_RWops *rw = SDL_RWFromConstMem(data, size);
	_sound = Mix_LoadWAV_RW(rw, 1);
	if (_sound == 0)
	{
		throw Exception(Mix_GetError());
	}
}

/**
 * Plays the contained sound effect.
 * @param channel Use specified channel, -1 to use any channel
 */
void Sound::play(int channel, int angle, int distance) const
 {
	if (!Options::mute && _sound != 0)
 	{
		int chan = Mix_PlayChannel(channel, _sound, 0);
		if (chan == -1)
		{
			Log(LOG_WARNING) << Mix_GetError();
		}
		else if (Options::StereoSound)
		{
			if (!Mix_SetPosition(chan, angle, distance))
			{
				Log(LOG_WARNING) << Mix_GetError();
			}
		}
	}
}

/**
 * Stops all sounds playing.
 */
void Sound::stop()
{
	if (!Options::mute)
	{
		Mix_HaltChannel(-1);
	}
}

/**
 * Plays the contained sound effect repeatedly on the reserved ambience channel.
 */
void Sound::loop()
{
	if (!Options::mute && _sound != 0 && Mix_Playing(3) == 0)
	{
		int chan = Mix_PlayChannel(3, _sound, -1);
		if (chan == -1)
		{
			Log(LOG_WARNING) << Mix_GetError();
		}
	}
}

/**
 * Stops the contained sound from looping.
 */
void Sound::stopLoop()
{
	if (!Options::mute)
	{
		Mix_HaltChannel(3);
	}
}

}
