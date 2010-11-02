/*
 * Copyright 2010 OpenXcom Developers.
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
#include <sys/stat.h>
#include "../Engine/Palette.h"
#include "../Engine/Font.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Language.h"
#include "../Engine/Music.h"
#include "../Geoscape/Globe.h"
#include "../Geoscape/Polygon.h"
#include "../Geoscape/Polyline.h"
#include "../Engine/SoundSet.h"
#include "TerrainObjectSet.h"

/**
 * Initializes a blank resource set pointing to a folder.
 * @param folder Subfolder to load resources from.
 */
ResourcePack::ResourcePack(const std::string &folder) : _folder(folder), _currentLang(0), _langName(""), _palettes(), _fonts(), _languages(), _surfaces(), _sets(), _polygons(), _musics(), _terrainSets()
{
}

/**
 * Deletes all the loaded resources.
 */
ResourcePack::~ResourcePack()
{
	for (std::map<std::string, Language*>::iterator i = _languages.begin(); i != _languages.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); i++)
	{
		delete i->second;
	}
	for (std::list<Polygon*>::iterator i = _polygons.begin(); i != _polygons.end(); i++)
	{
		delete *i;
	}
	for (std::list<Polyline*>::iterator i = _polylines.begin(); i != _polylines.end(); i++)
	{
		delete *i;
	}
	for (std::map<std::string, Palette*>::iterator i = _palettes.begin(); i != _palettes.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, Music*>::iterator i = _musics.begin(); i != _musics.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, SoundSet*>::iterator i = _sounds.begin(); i != _sounds.end(); i++)
	{
		delete i->second;
	}
	for (std::map<std::string, TerrainObjectSet*>::iterator i = _terrainSets.begin(); i != _terrainSets.end(); i++)
	{
		delete i->second;
	}
}

/**
 * Takes a filename and tries to figure out the existing
 * case-insensitive filename for it, for file operations.
 * @param filename Original filename.
 * @return Correctly-cased filename or "" if it doesn't exist.
 * @note There's no actual method for figuring out the correct
 * filename on case-sensitive systems, this is just a workaround.
 */
std::string ResourcePack::insensitive(const std::string &filename) const
{
	std::string newName = filename;

	// Ignore DATA folder
	size_t i = newName.find("/DATA/");
	if (i != std::string::npos)
		i += 6;
	else
		i = 0;

	// Try lowercase and uppercase names
	struct stat info;
	if (stat(newName.c_str(), &info) == 0)
	{
		return newName;
	}
	else
	{
		for (std::string::iterator c = newName.begin() + i; c != newName.end(); c++)
			*c = toupper(*c);
		if (stat(newName.c_str(), &info) == 0)
		{
			return newName;
		}
		else
		{
			for (std::string::iterator c = newName.begin() + i; c != newName.end(); c++)
				*c = tolower(*c);
			if (stat(newName.c_str(), &info) == 0)
			{
				return newName;
			}
			else
			{
				return "";
			}
		}
	}
}

/**
 * Returns the currently active language.
 * Ensures localizable strings always use the
 * active language without needing to know it.
 * @return Pointer to the language.
 */
Language *const ResourcePack::getLanguage() const
{
	return _currentLang;
}

/**
 * Returns the name of the currently active language.
 * Necessary for logic specific to certain languages.
 * @return Name of the language.
 */
std::string ResourcePack::getLanguageName() const
{
	return _langName;
}

/**
 * Changes the currently active language to another
 * one in the resource pack.
 * @param lang Name of the language.
 */
void ResourcePack::setLanguage(const std::string &lang)
{
	_currentLang = _languages[lang];
	_langName = lang;
}

/**
 * Returns a specific font from the resource set.
 * @param name Name of the font.
 * @return Pointer to the font.
 */
Font *const ResourcePack::getFont(const std::string &name)
{
	return _fonts[name];
}

/**
 * Returns a specific surface from the resource set.
 * @param name Name of the surface.
 * @return Pointer to the surface.
 */
Surface *const ResourcePack::getSurface(const std::string &name)
{
	return _surfaces[name];
}

/**
 * Returns a specific surface set from the resource set.
 * @param name Name of the surface set.
 * @return Pointer to the surface set.
 */
SurfaceSet *const ResourcePack::getSurfaceSet(const std::string &name)
{
	return _sets[name];
}

/**
 * Returns the list of polygons in the resource set.
 * @return Pointer to the list of polygons.
 */
std::list<Polygon*> *const ResourcePack::getPolygons()
{
	return &_polygons;
}

/**
 * Returns the list of polylines in the resource set.
 * @return Pointer to the list of polylines.
 */
std::list<Polyline*> *const ResourcePack::getPolylines()
{
	return &_polylines;
}

/**
 * Returns a specific music from the resource set.
 * @param name Name of the music.
 * @return Pointer to the music.
 */
Music *const ResourcePack::getMusic(const std::string &name)
{
	return _musics[name];
}

/**
 * Returns a specific sound set from the resource set.
 * @param name Name of the sound set.
 * @return Pointer to the sound set.
 */
SoundSet *const ResourcePack::getSoundSet(const std::string &name)
{
	return _sounds[name];
}

/**
 * Returns a specific palette from the resource set.
 * @param name Name of the palette.
 * @return Pointer to the palette.
 */
Palette *const ResourcePack::getPalette(const std::string &name)
{
	return _palettes[name];
}

/**
 * Changes the palette of all the graphics in the resource set.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void ResourcePack::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	for (std::map<std::string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); i++)
	{
		i->second->getSurface()->setPalette(colors, firstcolor, ncolors);
	}
	for (std::map<std::string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); i++)
	{
		i->second->setPalette(colors, firstcolor, ncolors);
	}
	for (std::map<std::string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); i++)
	{
		i->second->setPalette(colors, firstcolor, ncolors);
	}
}

/**
 * Returns a specific palette from the resource set.
 * @param name Name of the palette.
 * @return Pointer to the palette.
 */
TerrainObjectSet *ResourcePack::getTerrainObjectSet(std::string name)
{
	return _terrainSets[name];
}
