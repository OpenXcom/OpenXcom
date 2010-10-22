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
	for (std::vector<Sound*>::iterator i = _sounds.begin(); i != _sounds.end(); i++)
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
 * @param wav Are the sounds in WAV format?
 * @sa http://www.ufopaedia.org/index.php?title=SOUND
 */
void SoundSet::loadCat(std::string filename, bool wav)
{
	// Load CAT file
	std::ifstream sndFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!sndFile)
	{
		throw "Failed to load CAT";
	}

	// Get amount of files
	int first;
	sndFile.read((char*)&first, sizeof(first));

	int amount = first / sizeof(first) / 2;

	// Get sound offsets
	sndFile.seekg(0, std::ios::beg);

	int *offset = new int[amount];
	int *size = new int[amount];

	for (int i = 0; i < amount; i++)
	{
		sndFile.read((char*)&offset[i], sizeof(*offset));
		sndFile.read((char*)&size[i], sizeof(*size));
	}
	
	// Load each sound file
	for (int i = 0; i < amount; i++)
	{
		sndFile.seekg(offset[i], std::ios::beg);

		char namesize, *name;

		// Read filename
		sndFile.read(&namesize, 1);
		name = new char[namesize];
		sndFile.read(name, namesize);

		delete[] name;

		// Read WAV chunk
		char *sound = new char[size[i]];
		sndFile.read(sound, size[i]);

		// If there's no WAV header (44 bytes), add it
		// Assuming sounds are 8-bit 8000Hz (DOS version)
		char *newsound = 0;
		if (!wav)
		{
			char header[] = {'R', 'I', 'F', 'F', 0x00, 0x00, 0x00, 0x00, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
							 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x40, 0x1f, 0x00, 0x00, 0x40, 0x1f, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00,
							 'd', 'a', 't', 'a', 0x00, 0x00, 0x00, 0x00};
			int headersize = size[i] + 36;
			int soundsize = size[i];
			memcpy(header + 4, &headersize, sizeof(headersize));
			memcpy(header + 40, &soundsize, sizeof(soundsize));

			newsound = new char[44 + size[i]];
			memcpy(newsound, header, 44);
			memcpy(newsound + 44, sound, size[i]);
		}

		Sound *s = new Sound();
		try
		{
			if (wav)
				s->load(sound, size[i]);
			else
				s->load(newsound, 44 + size[i]);
		}
		catch (char* c)
		{
			// Ignore junk in the file
			c = c;
		}
		_sounds.push_back(s);

		delete[] sound;
		if (!wav)
			delete[] newsound;
	}

	delete[] offset;
	delete[] size;

	sndFile.close();
}

/**
 * Returns a particular wave from the sound set.
 * @param i Sound number in the set.
 * @return Pointer to the respective sound.
 */
Sound *SoundSet::getSound(unsigned int i)
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
