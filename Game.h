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

#include <list>
#include <string>
#include "SDL.h"

using namespace std;

class State;
class Screen;
class Cursor;
class ResourcePack;
class SavedGame;
class Ruleset;

/**
 * The core of the game engine, manages the game's entire contents and structure.
 * Takes care of encapsulating all the core SDL commands, provides access to all
 * the game's resources and contains a stack state machine to handle all the
 * initializations, events and blits of each state, as well as transitions.
 */
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
	/// Creates a new game and initializes SDL.
	Game(string title, int width, int height, int bpp);
	/// Cleans up all the game's resources and shuts down SDL.
	~Game();
	/// Starts the game's state machine.
	void run();
	/// Quits the game.
	void quit();
	/// Gets the game's display screen.
	Screen *getScreen();
	/// Gets the game's states.
	list<State*> *getStates();
	/// Sets the game's 8bpp palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Resets the state stack to a new state.
	void setState(State *state);
	/// Pushes a new state into the state stack.
	void pushState(State *state);
	/// Pops the last state from the state stack.
	void popState();
	/// Gets the currently loaded resource pack.
	ResourcePack *getResourcePack();
	/// Sets a new resource pack for the game.
	void setResourcePack(ResourcePack *res);
	/// Gets the currently loaded saved game.
	SavedGame *getSavedGame();
	/// Sets a new saved game for the game
	void setSavedGame(SavedGame *save);
	/// Gets the currently loaded ruleset
	Ruleset *getRuleset();
	/// Sets a new ruleset for the game
	void setRuleset(Ruleset *rules);
};

#endif
