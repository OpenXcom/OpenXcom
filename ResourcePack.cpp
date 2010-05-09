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
#include "ResourcePack.h"

ResourcePack::ResourcePack(string folder) : _folder(folder), _currentLang(0), _langName(""), _palettes(), _fonts(), _languages(), _surfaces(), _sets(), _polygons(), _musics()
{
}

ResourcePack::~ResourcePack()
{
	for (map<string, Language*>::iterator i = _languages.begin(); i != _languages.end(); i++)
	{
		delete i->second;
	}
	for (map<string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); i++)
	{
		delete i->second;
	}
	for (map<string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); i++)
	{
		delete i->second;
	}
	for (map<string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); i++)
	{
		delete i->second;
	}
	for (map<string, Palette*>::iterator i = _palettes.begin(); i != _palettes.end(); i++)
	{
		delete i->second;
	}
	for (map<string, Music*>::iterator i = _musics.begin(); i != _musics.end(); i++)
	{
		delete i->second;
	}
}

string ResourcePack::getFolder()
{
	return _folder;
}

Language *ResourcePack::getLanguage()
{
	return _currentLang;
}

string ResourcePack::getLanguageName()
{
	return _langName;
}

void ResourcePack::setLanguage(string lang)
{
	_currentLang = _languages[lang];
	_langName = lang;
}

Font *ResourcePack::getFont(string name)
{
	return _fonts[name];
}

Surface *ResourcePack::getSurface(string name)
{
	return _surfaces[name];
}

SurfaceSet *ResourcePack::getSurfaceSet(string name)
{
	return _sets[name];
}

vector<Polygon*> *ResourcePack::getPolygons()
{
	return &_polygons;
}

Music *ResourcePack::getMusic(string name)
{
	return _musics[name];
}

Palette *ResourcePack::getPalette(string name)
{
	return _palettes[name];
}

void ResourcePack::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	for (map<string, Font*>::iterator i = _fonts.begin(); i != _fonts.end(); i++)
	{
		i->second->getSurface()->setPalette(colors, firstcolor, ncolors);
	}
	for (map<string, Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); i++)
	{
		i->second->setPalette(colors, firstcolor, ncolors);
	}
	for (map<string, SurfaceSet*>::iterator i = _sets.begin(); i != _sets.end(); i++)
	{
		i->second->getSurface()->setPalette(colors, firstcolor, ncolors);
	}
}