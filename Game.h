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

class State;

#include <list>
#include "SDL.h"
#include "Screen.h"
#include "Cursor.h"
#include "ResourcePack.h"
#include "SavedGame.h"
#include "Ruleset.h"

class Game
{
private:
	SDL_Event _event;
	Screen *_screen;
	Cursor *_cursor;

	list<State*> _states, _deleted;
	ResourcePack *_res;
	SavedGame *_save;
	Ruleset *_rules;
	bool _quit;
public:
	Game(char* title, int width, int height, int bpp);
	~Game();
	void run();
	void quit();
	Screen *getScreen();
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	void setState(State *state);
	void pushState(State *state);
	void popState();
	ResourcePack *getResourcePack();
	void setResourcePack(ResourcePack *res);
	SavedGame *getSavedGame();
	void setSavedGame(SavedGame *save);
	Ruleset *getRuleset();
	void setRuleset(Ruleset *rules);
};

#endif