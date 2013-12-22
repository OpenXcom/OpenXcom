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
#ifndef OPENXCOM_GAME_H
#define OPENXCOM_GAME_H

#include <list>
#include <string>
#include <SDL.h>

namespace OpenXcom
{

class State;
class Screen;
class Cursor;
class Language;
class ResourcePack;
class SavedGame;
class Ruleset;
class FpsCounter;

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
	Language *_lang;
	std::list<State*> _states, _deleted;
	ResourcePack *_res;
	SavedGame *_save;
	Ruleset *_rules;
	bool _quit, _init;
	FpsCounter *_fpsCounter;
	bool _mouseActive;

#ifdef __MORPHOS__		
	Uint32 waittime;// = 1000.0f/FPS;
	Uint32 framestarttime;// = 0;
	Sint32 delaytime;
#endif
public:
	/// Creates a new game and initializes SDL.
	Game(const std::string &title);
	/// Cleans up all the game's resources and shuts down SDL.
	~Game();
	/// Starts the game's state machine.
	void run();
	/// Quits the game.
	void quit();
	/// Sets the game's audio volume.
	void setVolume(int sound, int music);
	/// Gets the game's display screen.
	Screen *getScreen() const;
	/// Gets the game's cursor.
	Cursor *getCursor() const;
	/// Gets the FpsCounter.
	FpsCounter *getFpsCounter() const;
	/// Sets the game's 8bpp palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Resets the state stack to a new state.
	void setState(State *state);
	/// Pushes a new state into the state stack.
	void pushState(State *state);
	/// Pops the last state from the state stack.
	void popState();
	/// Gets the currently loaded language.
	Language *getLanguage() const;
	/// Loads a new language for the game.
	void loadLanguage(const std::string &filename);
	/// Gets the currently loaded resource pack.
	ResourcePack *getResourcePack() const;
	/// Sets a new resource pack for the game.
	void setResourcePack(ResourcePack *res);
	/// Gets the currently loaded saved game.
	SavedGame *getSavedGame() const;
	/// Sets a new saved game for the game.
	void setSavedGame(SavedGame *save);
	/// Gets the currently loaded ruleset.
	Ruleset *getRuleset() const;
	/// Loads a new ruleset for the game.
	void loadRuleset();
	/// Sets whether the mouse cursor is activated.
	void setMouseActive(bool active);
	/// Returns whether current state is the param state
	bool isState(State *state) const;
	/// Returns whether the game is shutting down.
	bool isQuitting() const;
};

}

#endif
