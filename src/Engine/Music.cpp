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
#include "Music.h"
#include "Exception.h"
#include "Options.h"
#include "Logger.h"
#include "Language.h"
#include "Adlib/adlplayer.h"
#include "AdlibMusic.h"

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
	stop();
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
	std::string utf8 = Language::wstrToUtf8(Language::fsToWstr(filename));

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
void Music::load(const void *data, int size)
{
#ifndef __NO_MUSIC
	SDL_RWops *rwops = SDL_RWFromConstMem(data, size);
	_music = Mix_LoadMUS_RW(rwops, 0);
	SDL_FreeRW(rwops);
	if (_music == 0)
	{
		throw Exception(Mix_GetError());
	}
#endif
}

/**
 * Plays the contained music track.
 * @param loop Amount of times to loop the track. -1 = infinite
 */
void Music::play(int loop) const
{
#ifndef __NO_MUSIC
	if (!Options::mute)
	{
		if (_music != 0)
		{
			stop();
			if (Mix_PlayMusic(_music, loop) == -1)
			{
				Log(LOG_WARNING) << Mix_GetError();
			}
		}
	}
#endif
}

/**
 * Stops all music playing.
 */
void Music::stop()
{
#ifndef __NO_MUSIC
	if (!Options::mute)
	{
		func_mute();
		Mix_HookMusic(NULL, NULL);
		Mix_HaltMusic();
	}
#endif
}

/**
 * Pauses music playback when game loses focus.
 */
void Music::pause()
{
#ifndef __NO_MUSIC
	if (!Options::mute)
	{
		Mix_PauseMusic();
		if (Mix_GetMusicType(0) == MUS_NONE)
			Mix_HookMusic(NULL, NULL);
	}
#endif
}

/**
 * Resumes music playback when game gains focus.
 */
void Music::resume()
{
#ifndef __NO_MUSIC
	if (!Options::mute)
	{
		Mix_ResumeMusic();
		if (Mix_GetMusicType(0) == MUS_NONE)
			Mix_HookMusic(AdlibMusic::player, NULL);
	}
#endif
}

/**
 * Checks if any music is playing.
 */
bool Music::isPlaying()
{
#ifndef __NO_MUSIC
	if (!Options::mute)
	{
		return Mix_Playing(-1) != 0;
	}
#endif
	return false;
}

}
