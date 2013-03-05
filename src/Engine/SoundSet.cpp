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
#include "SoundSet.h"
#include "CatFile.h"
#include "Sound.h"
#include "Exception.h"

namespace OpenXcom
{

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
	for (std::vector<Sound*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
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
void SoundSet::loadCat(const std::string &filename, bool wav)
{
	// Load CAT file
	CatFile sndFile (filename.c_str());
	if (!sndFile)
	{
		throw Exception(filename + " not found");
	}

	// Load each sound file
	for (int i = 0; i < sndFile.getAmount(); ++i)
	{
		// Read WAV chunk
		unsigned char *sound = (unsigned char*) sndFile.load(i);
		unsigned int size = sndFile.getObjectSize(i);

		// If there's no WAV header (44 bytes), add it
		// Assuming sounds are 8-bit 8000Hz (DOS version)
		unsigned char *newsound = 0;
		if (!wav)
		{
			char header[] = {'R', 'I', 'F', 'F', 0x00, 0x00, 0x00, 0x00, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
							 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x2b, 0x00, 0x00, 0x11, 0x2b, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00,
							 'd', 'a', 't', 'a', 0x00, 0x00, 0x00, 0x00};

			for (unsigned int n = 0; n < size; ++n) sound[n] *= 4; // scale to 8 bits
			if (size > 5) size -= 5; // skip 5 garbage name bytes at beginning
			if (size) size--; // omit trailing null byte

			int headersize = size + 36;
			int soundsize = size;
			memcpy(header + 4, &headersize, sizeof(headersize));
			memcpy(header + 40, &soundsize, sizeof(soundsize));

			newsound = new unsigned char[44 + size*2];
			memcpy(newsound, header, 44);
			if (size) memcpy(newsound + 44, sound+5, size);
			Uint32 step16 = (8000<<16)/11025;
			Uint8 *w = newsound+44;
			int newsize = 0;
			for (Uint32 offset16 = 0; (offset16>>16) < size; offset16 += step16, ++w, ++newsize)
			{
				*w = sound[5 + (offset16>>16)];
			}
			size = newsize + 44;
		}

		Sound *s = new Sound();
		try
		{
			if (size == 0)
			{
				throw Exception("Invalid sound file");
			}
			if (wav)
				s->load(sound, size);
			else
				s->load(newsound, size);
		}
		catch (Exception &e)
		{
			// Ignore junk in the file
			e = e;
		}
		_sounds.push_back(s);

		delete[] sound;
		if (!wav)
		{
			delete[] newsound;
		}
	}
}

/**
 * Returns a particular wave from the sound set.
 * @param i Sound number in the set.
 * @return Pointer to the respective sound.
 */
Sound *SoundSet::getSound(unsigned int i) const
{
	if (i >= _sounds.size())
	{
		return 0;
	}
	return _sounds[i];
}

/**
 * Returns the total amount of sounds currently
 * stored in the set.
 * @return Number of sounds.
 */
size_t SoundSet::getTotalSounds() const
{
	return _sounds.size();
}

}
