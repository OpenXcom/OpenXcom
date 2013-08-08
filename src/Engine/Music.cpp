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
#include "Music.h"
#include "Exception.h"
#include "Options.h"
#include "Logger.h"
#include "Language.h"

namespace OpenXcom
{

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
#ifndef __NO_MUSIC
	Mix_FreeMusic(_music);
#endif
}

/**
 * Loads a music file from a specified filename.
 * @param filename Filename of the music file.
 */
void Music::load(const std::string &filename)
{
#ifndef __NO_MUSIC
	// SDL only takes UTF-8 filenames
	// so here's an ugly hack to match this ugly reasoning
	std::wstring wstr = Language::cpToWstr(filename);
	std::string utf8 = Language::wstrToUtf8(wstr);

	_music = Mix_LoadMUS(utf8.c_str());
	if (_music == 0)
	{
		throw Exception(Mix_GetError());
	}
#endif
}

/**
 * Loads a music file from a specified memory chunk.
 * @param data Pointer to the music file in memory
 * @param size Size of the music file in bytes.
 */
void Music::load(const void *data, size_t size)
{
#ifndef __NO_MUSIC
	SDL_RWops *rwops = SDL_RWFromConstMem(data, size);
	_music = Mix_LoadMUS_RW(rwops);
	SDL_FreeRW(rwops);
	if (_music == 0)
	{
		throw Exception(Mix_GetError());
	}
#endif
}

/**
 * Plays the contained music track.
 */
void Music::play(int loop) const
{
#ifndef __NO_MUSIC
	if (!Options::getBool("mute"))
	{
		Mix_HaltMusic();
		if (_music != 0 && Mix_PlayMusic(_music, loop) == -1)
		{
			Log(LOG_WARNING) << Mix_GetError();
		}
	}
#endif
}

}
