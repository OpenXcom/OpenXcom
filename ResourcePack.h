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
#ifndef OPENXCOM__RESOURCEPACK_H
#define OPENXCOM__RESOURCEPACK_H

#include <map>
#include <string>
#include <vector>
#include "SDL.h"

class Language;
class Surface;
class SurfaceSet;
class Font;
class Palette;
class Polygon;
class Polyline;
class Music;
class SoundSet;

using namespace std;

/**
 * Packs of external game media.
 * Resource packs contain all the game media that's
 * loaded externally, like graphics, fonts, languages,
 * audio and world map.
 * @note The game is still hardcoded to X-Com resources,
 * so for now this just serves to keep all the file loading
 * in one place.
 */
class ResourcePack
{
protected:
	string _folder;
	Language *_currentLang;
	string _langName;
	map<string, Palette*> _palettes;
	map<string, Font*> _fonts;
	map<string, Language*> _languages;
	map<string, Surface*> _surfaces;
	map<string, SurfaceSet*> _sets;
	map<string, SoundSet*> _sounds;
	vector<Polygon*> _polygons;
	vector<Polyline*> _polylines;
	map<string, Music*> _musics;
public:
	/// Create a new resource pack with a folder's contents.
	ResourcePack(string folder);
	/// Cleans up the resource pack.
	virtual ~ResourcePack();
	/// Converts a filename to its existing case-insensitive name.
	string insensitive(string filename);
	/// Gets the current language.
	Language *getLanguage();
	/// Gets the name of the current language.
	string getLanguageName();
	/// Sets a new language as current.
	void setLanguage(string lang);
	/// Gets a particular font.
	Font *getFont(string name);
	/// Gets a particular surface.
	Surface *getSurface(string name);
	/// Gets a particular surface set.
	SurfaceSet *getSurfaceSet(string name);
	/// Gets the list of world polygons.
	vector<Polygon*> *getPolygons();
	/// Gets the list of world polylines.
	vector<Polyline*> *getPolylines();
	/// Gets a particular music.
	Music *getMusic(string name);
	/// Gets a particular sound set.
	SoundSet *getSoundSet(string name);
	/// Gets a particular palette.
	Palette *getPalette(string name);
	/// Sets a new palette.
	void setPalette(SDL_Color *colors, int firstcolor, int ncolors);
};

#endif
