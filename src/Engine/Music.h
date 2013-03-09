/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_MUSIC_H
#define OPENXCOM_MUSIC_H

#include <string>
#include <SDL_mixer.h>

namespace OpenXcom
{

/**
 * Container for music tracks.
 * Handles loading and playing various formats through SDL_mixer.
 */
class Music
{
private:
	Mix_Music *_music;
public:
	/// Creates a blank music track.
	Music();
	/// Cleans up the music track.
	~Music();
	/// Loads music from the specified file.
	void load(const std::string &filename);
	/// Loads music from a chunk of memory.
	void load(const void *data, size_t size);
	/// Plays the music.
	void play(int loop = -1) const;
};

}

#endif
