/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "SurfaceSet.h"
#include <SDL_endian.h>
#include <fstream>
#include "Surface.h"
#include "Exception.h"

namespace OpenXcom
{

/**
 * Sets up a new empty surface set for frames of the specified size.
 * @param width Frame width in pixels.
 * @param height Frame height in pixels.
 */
SurfaceSet::SurfaceSet(int width, int height) : _width(width), _height(height), _frames()
{

}

/**
 * Performs a deep copy of an existing surface set.
 * @param other Surface set to copy from.
 */
SurfaceSet::SurfaceSet(const SurfaceSet& other)
{
	_width = other._width;
	_height = other._height;
	
	for (std::map<int, Surface*>::const_iterator f = other._frames.begin(); f != other._frames.end(); ++f)
	{
		_frames[f->first] = new Surface(*f->second);
	}
}

/**
 * Deletes the images from memory.
 */
SurfaceSet::~SurfaceSet()
{
	for (std::map<int, Surface*>::iterator i = _frames.begin(); i != _frames.end(); ++i)
	{
		delete i->second;
	}
}

/**
 * Loads the contents of an X-Com set of PCK/TAB image files
 * into the surface. The PCK file contains an RLE compressed
 * image, while the TAB file contains the offsets to each
 * frame in the image.
 * @param pck Filename of the PCK image.
 * @param tab Filename of the TAB offsets.
 * @sa http://www.ufopaedia.org/index.php?title=Image_Formats#PCK
 */
void SurfaceSet::loadPck(const std::string &pck, const std::string &tab)
{
	int nframes = 0;

	// Load TAB and get image offsets
	if (!tab.empty())
	{
		std::ifstream offsetFile(tab.c_str(), std::ios::in | std::ios::binary);
		if (!offsetFile)
		{
			throw Exception(tab + " not found");
		}
		std::streampos begin, end;
		begin = offsetFile.tellg();
		int off;
		offsetFile.read((char*)&off, sizeof(off));
		offsetFile.seekg(0, std::ios::end);
		end = offsetFile.tellg();
		int size = end - begin;
		// 16-bit offsets
		if (off != 0)
		{
			nframes = size / 2;
		}
		// 32-bit offsets
		else
		{
			nframes = size / 4;
		}
		offsetFile.close();
		for (int frame = 0; frame < nframes; ++frame)
		{
			_frames[frame] = new Surface(_width, _height);
		}
	}
	else
	{
		nframes = 1;
		_frames[0] = new Surface(_width, _height);
	}

	// Load PCK and put pixels in surfaces
	std::ifstream imgFile (pck.c_str(), std::ios::in | std::ios::binary);
	if (!imgFile)
	{
		throw Exception(pck + " not found");
	}

	Uint8 value;

	for (int frame = 0; frame < nframes; ++frame)
	{
		int x = 0, y = 0;

		// Lock the surface
		_frames[frame]->lock();

		imgFile.read((char*)&value, 1);
		for (int i = 0; i < value; ++i)
		{
			for (int j = 0; j < _width; ++j)
			{
				_frames[frame]->setPixelIterative(&x, &y, 0);
			}
		}

		while (imgFile.read((char*)&value, 1) && value != 255)
		{
			if (value == 254)
			{
				imgFile.read((char*)&value, 1);
				for (int i = 0; i < value; ++i)
				{
					_frames[frame]->setPixelIterative(&x, &y, 0);
				}
			}
			else
			{
				_frames[frame]->setPixelIterative(&x, &y, value);
			}
		}

		// Unlock the surface
		_frames[frame]->unlock();
	}

	imgFile.close();
}

/**
 * Loads the contents of an X-Com DAT image file into the
 * surface. Unlike the PCK, a DAT file is an uncompressed
 * image with no offsets so these have to be figured out
 * manually, usually by splitting the image into equal portions.
 * @param filename Filename of the DAT image.
 * @sa http://www.ufopaedia.org/index.php?title=Image_Formats#SCR_.26_DAT
 */
void SurfaceSet::loadDat(const std::string &filename)
{
	int nframes = 0;

	// Load file and put pixels in surface
	std::ifstream imgFile (filename.c_str(), std::ios::in | std::ios::binary);
	if (!imgFile)
	{
		throw Exception(filename + " not found");
	}

	imgFile.seekg(0, std::ios::end);
	std::streamoff size = imgFile.tellg();
	imgFile.seekg(0, std::ios::beg);

	nframes = (int)size / (_width * _height);

	for (int i = 0; i < nframes; ++i)
	{
		Surface *surface = new Surface(_width, _height);
		_frames[i] = surface;
	}

	Uint8 value;
	int x = 0, y = 0, frame = 0;

	// Lock the surface
	_frames[frame]->lock();

	while (imgFile.read((char*)&value, 1))
	{
		_frames[frame]->setPixelIterative(&x, &y, value);

		if (y >= _height)
		{
			// Unlock the surface
			_frames[frame]->unlock();

			frame++;
			x = 0;
			y = 0;

			if (frame >= nframes)
				break;
			else
				_frames[frame]->lock();
		}
	}

	imgFile.close();
}

/**
 * Returns a particular frame from the surface set.
 * @param i Frame number in the set.
 * @return Pointer to the respective surface.
 */
Surface *SurfaceSet::getFrame(int i)
{
	if (_frames.find(i) != _frames.end())
	{
		return _frames[i];
	}
	return 0;
}

/**
 * Creates and returns a particular frame in the surface set.
 * @param i Frame number in the set.
 * @return Pointer to the respective surface.
 */
Surface *SurfaceSet::addFrame(int i)
{
	_frames[i] = new Surface(_width, _height);
	return _frames[i];
}

/**
 * Returns the full width of a frame in the set.
 * @return Width in pixels.
 */
int SurfaceSet::getWidth() const
{
	return _width;
}

/**
 * Returns the full height of a frame in the set.
 * @return Height in pixels.
 */
int SurfaceSet::getHeight() const
{
	return _height;
}

/**
 * Returns the total amount of frames currently
 * stored in the set.
 * @return Number of frames.
 */
size_t SurfaceSet::getTotalFrames() const
{
	return _frames.size();
}

/**
 * Replaces a certain amount of colors in all of the frames.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void SurfaceSet::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	for (std::map<int, Surface*>::iterator i = _frames.begin(); i != _frames.end(); ++i)
	{
		(*i).second->setPalette(colors, firstcolor, ncolors);
	}
}

std::map<int, Surface*> *SurfaceSet::getFrames()
{
	return &_frames;
}
}
