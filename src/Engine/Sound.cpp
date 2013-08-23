/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include <SDL.h>
#include "Exception.h"
#include "Options.h"
#include "Logger.h"

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
	_sound = Mix_LoadWAV(filename.c_str());
	if (_sound == 0)
	{
		std::string err;
		err = Mix_GetError() + std::string(":") + filename;
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
 */
void Sound::play(int channel) const
{
	if (!Options::getBool("mute") && _sound != 0 && Mix_PlayChannel(channel, _sound, 0) == -1)
	{
		Log(LOG_WARNING) << Mix_GetError();
	}
}

}
