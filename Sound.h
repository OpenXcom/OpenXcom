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
#ifndef OPENXCOM__SOUND_H
#define OPENXCOM__SOUND_H

#include "SDL_mixer.h"
#include <string>

using namespace std;

/**
 * Container for sound effects.
 * Handles loading and playing various formats through SDL_mixer.
 */
class Sound
{
private:
	Mix_Chunk *_sound;
public:
	/// Creates a blank sound effect.
	Sound();
	/// Cleans up the sound effect.
	~Sound();
	/// Loads sound from the specified file.
	void load(string filename);
	/// Loads sound from the specified memory.
	void load(void *sound, int size);
	/// Plays the sound.
	void play();
};

#endif
