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
#include "Music.h"

/**
 * Initializes a new music track.
 */
Music::Music() : _music(0)
{
}

/**
 * Deletes the loaded music content.
 */
Music::~Music()
{
	Mix_FreeMusic(_music);
}

/**
 * Loads a music file from a specified filename.
 * @param filename Filename of the music file.
 */
void Music::load(const std::string &filename)
{
	_music = Mix_LoadMUS(filename.c_str());
	if (_music == 0) 
	{
		throw Mix_GetError();
	}
}

/**
 * Plays the contained music track.
 */
void Music::play()
{
	if (_music != 0 && Mix_PlayMusic(_music, -1) == -1)
	{
		throw Mix_GetError();
	}
}
