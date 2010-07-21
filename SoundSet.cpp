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
#include "SoundSet.h"
#include <fstream>
#include "Sound.h"

/**
 * Sets up a new empty sound set.
 */
SoundSet::SoundSet() : _sounds()
{

}

/**
 * Deletes the sounds from memory.
 */
SoundSet::~SoundSet()
{
	for (vector<Sound*>::iterator i = _sounds.begin(); i != _sounds.end(); i++)
	{
		delete *i;
	}
}

/**
 * Loads the contents of an X-Com CAT file which usually contains
 * a set of sound files. The CAT starts with an index of the offset
 * and size of every file contained within. Each file consists of a
 * filename followed by its contents.
 * @param filename Filename of the CAT set.
 * @sa http://www.ufopaedia.org/index.php?title=SOUND
 */
void SoundSet::loadCat(string filename)
{
	// Load CAT file
	ifstream sndFile (filename, ios::in | ios::binary);
	if (!sndFile)
	{
		throw "Failed to load CAT";
	}

	// Get amount of files
	int first;
	sndFile.read((char*)&first, sizeof(first));

	int amount = first / sizeof(first) / 2;

	// Get sound offsets
	sndFile.seekg(0, ios::beg);

	int *offset = new int[amount];
	int *size = new int[amount];

	for (int i = 0; i < amount; i++)
	{
		sndFile.read((char*)&offset[i], sizeof(offset));
		sndFile.read((char*)&size[i], sizeof(size));
	}
	
	// Load each sound file
	for (int i = 0; i < amount; i++)
	{
		sndFile.seekg(offset[i], ios::beg);

		char namesize, *name;

		// Read filename
		sndFile.read(&namesize, 1);
		name = new char[namesize];
		sndFile.read(name, namesize);

		delete name;

		// Read WAV chunk
		char *sound = new char[size[i]];
		sndFile.read(sound, size[i]);

		Sound *s = new Sound();
		try
		{
			s->load(sound, size[i]);
		}
		catch (char* c)
		{
			// Ignore junk in the file
		}
		_sounds.push_back(s);

		delete sound;
	}

	delete offset;
	delete size;

	sndFile.close();
}

/**
 * Returns a particular wave from the sound set.
 * @param i Sound number in the set.
 * @return Pointer to the respective sound.
 */
Sound *SoundSet::getSound(int i)
{
	if (i >= _sounds.size())
		return 0;
	return _sounds[i];
}

/**
 * Returns the total amount of sounds currently
 * stored in the set.
 * @return Number of sounds.
 */
int SoundSet::getTotalSounds()
{
  return _sounds.size();
}
