/*
 * Copyright 2010 Daniel Albano
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
#include <sys/stat.h>

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
 * @return True if the sound was loaded successfully, False otherwise.
 */
bool Sound::load(string filename)
{
	struct stat info;
	if (stat(filename.c_str(), &info) != 0) 
	{
		return false;
	}
	_sound = Mix_LoadWAV(filename.c_str());
	if (_sound == 0) 
	{
		//throw Mix_GetError();
		return false;
	}
	return true;
}

/**
 * Plays the contained sound effect.
 */
void Sound::play()
{
	if (Mix_PlayChannel(-1, _sound, 0) == -1) 
	{
		//throw Mix_GetError();
	}
}
