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
#ifndef OPENXCOM__GAME_H
#define OPENXCOM__GAME_H

#define DATA_FOLDER "./DATA/"

class State;

#include <map>
#include <sstream>
#include "SDL.h"
#include "Screen.h"
#include "Language.h"
#include "Surface.h"
#include "SurfaceSet.h"
#include "Font.h"
#include "Palette.h"
#include "Cursor.h"

class Game
{
private:
	SDL_Event _event;
	Screen *_screen;
	Cursor *_cursor;
	State *_state, *_newState;
	Language *_currentLang;
	string _langName;
	map<string, Palette*> _palettes;
	map<string, Font*> _fonts;
	map<string, Language*> _languages;
	map<string, Surface*> _surfaces;
	map<string, SurfaceSet*> _sets;
	bool _quit;
public:
	Game(char* title, int width, int height, int bpp);
	~Game();
	void load();
	void run();
	void quit();
	Screen *getScreen();
	Language *getLanguage();
	string getLanguageName();
	void setLanguage(string lang);
	Font *getFont(string name);
	Surface *getSurface(string name);
	SurfaceSet *getSurfaceSet(string name);
	Palette *getPalette(string name);
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	void setState(State *state);
};

#endif