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
#include "ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Engine/Font.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Music.h"
#include "../Geoscape/Polygon.h"
#include "../Geoscape/Polyline.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes a blank resource set pointing to a folder.
 */
ResourcePack::ResourcePack() : _palettes(), _fonts(), _surfaces(), _sets(), _sounds(), _polygons(), _polylines(), _musics()
{
	_muteMusic = new Music();
	_muteSound = new Sound();
}

/**
 * Deletes all the loaded resources.
 */
ResourcePack::~ResourcePack()
{
	delete _muteMusic;
	delete _muteSound;
	for (std::map<std::string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); ++i)
	{
		delete i->second;
	}
	for (std::list<Polygon*>::iterator i = _polygons.begin(); i != _polygons.end(); ++i)
	{
		delete *i;
	}
	for (std::list<Polyline*>::iterator i = _polylines.begin(); i != _polylines.end(); ++i)
	{
		delete *i;
	}
	for (std::map<std::string, Palette*>::iterator i = _palettes.begin(); i != _palettes.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, Music*>::iterator i = _musics.begin(); i != _musics.end(); ++i)
	{
		delete i->second;
	}
	for (std::map<std::string, SoundSet*>::iterator i = _sounds.begin(); i != _sounds.end(); ++i)
	{
		delete i->second;
	}
}

/**
 * Returns a specific font from the resource set.
 * @param name Name of the font.
 * @return Pointer to the font.
 */
Font *ResourcePack::getFont(const std::string &name) const
{
	std::map<std::string, Font*>::const_iterator i = _fonts.find(name);
	if (_fonts.end() != i) return i->second; else return 0;
}

/**
 * Returns a specific surface from the resource set.
 * @param name Name of the surface.
 * @return Pointer to the surface.
 */
Surface *ResourcePack::getSurface(const std::string &name) const
{
	std::map<std::string, Surface*>::const_iterator i = _surfaces.find(name);
	if (_surfaces.end() != i) return i->second; else return 0;
}

/**
 * Returns a specific surface set from the resource set.
 * @param name Name of the surface set.
 * @return Pointer to the surface set.
 */
SurfaceSet *ResourcePack::getSurfaceSet(const std::string &name) const
{
	std::map<std::string, SurfaceSet*>::const_iterator i = _sets.find(name);
	if (_sets.end() != i) return i->second; else return 0;
}

/**
 * Returns the list of polygons in the resource set.
 * @return Pointer to the list of polygons.
 */
std::list<Polygon*> *ResourcePack::getPolygons()
{
	return &_polygons;
}

/**
 * Returns the list of polylines in the resource set.
 * @return Pointer to the list of polylines.
 */
std::list<Polyline*> *ResourcePack::getPolylines()
{
	return &_polylines;
}

/**
 * Returns a specific music from the resource set.
 * @param name Name of the music.
 * @return Pointer to the music.
 */
Music *ResourcePack::getMusic(const std::string &name) const
{
	if (Options::getBool("mute"))
	{
		return _muteMusic;
	}
	else
	{
		std::map<std::string, Music*>::const_iterator i = _musics.find(name);
		if (_musics.end() != i) return i->second; else return 0;
	}
}

/**
 * Returns a random music from the resource set.
 * @param name Name of the music to pick from.
 * @return Pointer to the music.
 */
Music *ResourcePack::getRandomMusic(const std::string &name) const
{
	if (Options::getBool("mute"))
	{
		return _muteMusic;
	}
	else
	{
		std::vector<Music*> music;
		for (std::map<std::string, Music*>::const_iterator i = _musics.begin(); i != _musics.end(); ++i)
		{
			if (i->first.find(name) != std::string::npos)
			{
				music.push_back(i->second);
			}
		}
		if (_musics.empty())
			return _muteMusic;
		else
			return music[RNG::generate(0, music.size()-1)];
	}
}

/**
 * Returns a specific sound from the resource set.
 * @param set Name of the sound set.
 * @param sound ID of the sound.
 * @return Pointer to the sound.
 */
Sound *ResourcePack::getSound(const std::string &set, unsigned int sound) const
{
	if (Options::getBool("mute"))
	{
		return _muteSound;
	}
	else
	{
		std::map<std::string, SoundSet*>::const_iterator i = _sounds.find(set);
		if (_sounds.end() != i) return i->second->getSound(sound); else return 0;
	}
}

/**
 * Returns a specific palette from the resource set.
 * @param name Name of the palette.
 * @return Pointer to the palette.
 */
Palette *ResourcePack::getPalette(const std::string &name) const
{
	std::map<std::string, Palette*>::const_iterator i = _palettes.find(name);
	if (_palettes.end() != i) return i->second; else return 0;
}

/**
 * Changes the palette of all the graphics in the resource set.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void ResourcePack::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	for (std::map<std::string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); ++i)
	{
		i->second->getSurface()->setPalette(colors, firstcolor, ncolors);
	}
	for (std::map<std::string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		if(i->first.substr(i->first.length()-3, i->first.length()) != "LBM")
			i->second->setPalette(colors, firstcolor, ncolors);
	}
	for (std::map<std::string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); ++i)
	{
		i->second->setPalette(colors, firstcolor, ncolors);
	}
}

/**
 * Returns the list of voxeldata in the resource set.
 * @return Pointer to the list of voxeldata.
 */
std::vector<Uint16> *ResourcePack::getVoxelData()
{
	return &_voxelData;
}

}
