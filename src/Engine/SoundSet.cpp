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
#include "SoundSet.h"
#include "CatFile.h"
#include "Sound.h"
#include "Exception.h"
#include <sstream>
namespace OpenXcom
{

/**
 * Sets up a new empty sound set.
 */
SoundSet::SoundSet()
{

}

/**
 * Deletes the sounds from memory.
 */
SoundSet::~SoundSet()
{
	for (std::map<int, Sound*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
	{
		delete i->second;
	}
}

/**
 * Converts a 8Khz sample to 11Khz.
 * @param oldsound Pointer to original sample buffer.
 * @param oldsize Original buffer size.
 * @param newsound Pointer to converted sample buffer.
 * @return Converted buffer size.
 */
int SoundSet::convertSampleRate(Uint8 *oldsound, unsigned int oldsize, Uint8 *newsound) const
{
	const Uint32 step16 = (8000 << 16) / 11025;
	int newsize = 0;
	for (Uint32 offset16 = 0; (offset16 >> 16) < oldsize; offset16 += step16, ++newsound, ++newsize)
	{
		*newsound = oldsound[offset16 >> 16];
	}
	return newsize;
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
		// Assuming sounds are 6-bit 8000Hz (DOS version)
		unsigned char *newsound = 0;
		const int headerSize = 44;
		if (!wav)
		{
			if (size > 5) size -= 5; // skip 5 garbage name bytes at beginning
			if (size) size--; // omit trailing null byte
			if (size != 0)
			{
				char header[] = {'R', 'I', 'F', 'F', 0x00, 0x00, 0x00, 0x00, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
								 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x2b, 0x00, 0x00, 0x11, 0x2b, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00,
								 'd', 'a', 't', 'a', 0x00, 0x00, 0x00, 0x00};

				// scale to 8 bits
				for (unsigned int n = 0; n < size; ++n) sound[5 + n] *= 4;

				// copy and do the conversion...
				newsound = new unsigned char[headerSize + size*2];
				memcpy(newsound, header, headerSize);
				memcpy(newsound + headerSize, sound + 5, size);
				int newsize = convertSampleRate(sound + 5, size, newsound + headerSize);
				size = newsize + headerSize;

				// Rewrite the number of samples in the WAV file
				int headersize = newsize + 36;
				int soundsize = newsize;
				memcpy(newsound + 4, &headersize, sizeof(headersize));
				memcpy(newsound + 40, &soundsize, sizeof(soundsize));
			}
		}
		// so it's WAV, but in 8 khz, we have to convert it to 11 khz sound
		else if (0x40 == sound[0x18] && 0x1F == sound[0x19] && 0x00 == sound[0x1A] && 0x00 == sound[0x1B])
		{
			unsigned char *sound2 = new unsigned char[size*2];

			// rewrite the samplerate in the header to 11 khz
			sound[0x18]=0x11; sound[0x19]=0x2B; sound[0x1C]=0x11; sound[0x1D]=0x2B;

			// copy and do the conversion...
			memcpy(sound2, sound, size);
			int newsize = convertSampleRate(sound + headerSize, size - headerSize, sound2 + headerSize);
			size = newsize + headerSize;

			// Rewrite the number of samples in the WAV file
			memcpy(sound2 + 0x28, &newsize, sizeof(newsize));

			// Ok, now replace the original with the converted:
			delete[] sound;
			sound = sound2;
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
		catch (const Exception &)
		{
			// Ignore junk in the file
		}
		_sounds[i] = s;

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
Sound *SoundSet::getSound(unsigned int i)
{
	if (_sounds.find(i) != _sounds.end())
	{
		return _sounds[i];
	}
	return 0;
}


/**
 * Creates and returns a particular wave in the sound set.
 * @param i Sound number in the set.
 * @return Pointer to the respective sound.
 */
Sound *SoundSet::addSound(unsigned int i)
{
	_sounds[i] = new Sound();
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

/**
 * Loads individual contents of a TFTD CAT file by index.
 * a set of sound files. The CAT starts with an index of the offset
 * and size of every file contained within. Each file consists of a
 * filename followed by its contents.
 * @param filename Filename of the CAT set.
 * @param index which index in the cat file do we load?
 * @sa http://www.ufopaedia.org/index.php?title=SOUND
 */
void SoundSet::loadCatbyIndex(const std::string &filename, int index)
{
	// Load CAT file
	CatFile sndFile (filename.c_str());
	if (!sndFile)
	{
		throw Exception(filename + " not found");
	}
	if (index >= sndFile.getAmount())
	{
		std::ostringstream err;
		err << filename << " does not contain " << index << " sound files.";
		throw Exception(err.str());
	}

	// Read WAV chunk
	unsigned char *sound = (unsigned char*) sndFile.load(index);
	unsigned int size = sndFile.getObjectSize(index);

	// there's no WAV header (44 bytes), add it
	// sounds are 8-bit 11025Hz, signed
	unsigned char *newsound = 0;

	if (size != 0)
	{
		char header[] = {'R', 'I', 'F', 'F', 0x00, 0x00, 0x00, 0x00, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
							0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x11, 0x2b, 0x00, 0x00, 0x11, 0x2b, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00,
							'd', 'a', 't', 'a', 0x00, 0x00, 0x00, 0x00};


		if (size > 5) size -= 5; // skip 5 garbage name bytes at beginning
		if (size) size--; // omit trailing null byte

		int headersize = size + 36;
		int soundsize = size;
		memcpy(header + 4, &headersize, sizeof(headersize));
		memcpy(header + 40, &soundsize, sizeof(soundsize));

		newsound = new unsigned char[44 + size];
		memcpy(newsound, header, 44);

		// TFTD sounds are signed, so we need to convert them.
		for (unsigned int n = 5; n < size + 5; ++n)
		{
			int value = (int)sound[n] + 128;
			sound[n] = (uint8_t)value;
		}

		if (size) memcpy(newsound + 44, sound+5, size);
		size = size + 44;
	}

	Sound *s = new Sound();
	try
	{
		if (size == 0)
		{
			throw Exception("Invalid sound file");
		}
		s->load(newsound, size);
	}
	catch (const Exception &)
	{
		// Ignore junk in the file
	}
	_sounds[getTotalSounds()] = s;

	delete[] sound;
	delete[] newsound;
}

}
