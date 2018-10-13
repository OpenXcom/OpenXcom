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
#include <SDL_mixer.h>
#include <map>
#include <string>

namespace OpenXcom
{

class Sound;

/**
 * Container of a set of sounds.
 * Used to manage file sets that contain a pack
 * of sounds inside.
 */
class SoundSet
{
private:
	std::map<int, Sound*> _sounds;

	int convertSampleRate(Uint8 *oldsound, unsigned int oldsize, Uint8 *newsound) const;
public:
	/// Crates a sound set.
	SoundSet();
	/// Cleans up the sound set.
	~SoundSet();
	/// Loads an X-Com CAT set of sound files.
	void loadCat(const std::string &filename, bool wav = true);
	/// Gets a particular sound from the set.
	Sound *getSound(unsigned int i);
	/// Creates a new sound and returns a pointer to it.
	Sound *addSound(unsigned int i);
	/// Gets the total sounds in the set.
	size_t getTotalSounds() const;
	/// Loads a specific entry from a CAT file into the soundset.
	void loadCatbyIndex(const std::string &filename, int index);
};

}
