/*
 * Copyright 2010-2015 OpenXcom Developers.
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

#ifndef OPENXCOM_STATE_H
#define OPENXCOM_STATE_H

#include <vector>
#include <string>
#include <SDL.h>
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleInterface.h"

namespace OpenXcom
{

class Game;
class Surface;
class InteractiveSurface;
class Action;
class LocalizedText;

/**
 * A game state that receives user input and reacts accordingly.
 * Game states typically represent a whole window or screen that
 * the user interacts with, making the game... well, interactive.
 * They automatically handle child elements used to transmit
 * information from/to the user, and are linked to the core game
 * engine which manages them.
 */
class State
{
	friend class Timer;

protected:
	static Game *_game;
	std::vector<Surface*> _surfaces;
	bool _screen;
	InteractiveSurface *_modal;
	SDL_Color _palette[256];
	Uint8 _cursorColor;
public:
	/// Creates a new state linked to a game.
	State();
	/// Cleans up the state.
	virtual ~State();
	/// Adds a child element to the state.
	void add(Surface *surface);
	/// Adds a child element to the state.
	void add(Surface *surface, const std::string id, const std::string category, Surface *parent = 0);
	/// Gets whether the state is a full-screen.
	bool isScreen() const;
	/// Toggles whether the state is a full-screen.
	void toggleScreen();
	/// Initializes the state.
	virtual void init();
	/// Handles any events.
	virtual void handle(Action *action);
	/// Runs state functionality every cycle.
	virtual void think();
	/// Blits the state to the screen.
	virtual void blit();
	/// Hides all the state surfaces.
	void hideAll();
	/// Shws all the state surfaces.
	void showAll();
	/// Resets all the state surfaces.
	void resetAll();
	/// Get the localized text.
	const LocalizedText &tr(const std::string &id) const;
	/// Get the localized text.
	LocalizedText tr(const std::string &id, unsigned n) const;
	/// redraw all the text-type surfaces.
	void redrawText();
	/// center all surfaces relative to the screen.
	void centerAllSurfaces();
	/// lower all surfaces by half the screen height.
	void lowerAllSurfaces();
	/// switch the colours to use the battlescape palette.
	void applyBattlescapeTheme();
	/// Sets game object pointer
	static void setGamePtr(Game* game);
	/// Sets a modal surface.
	void setModal(InteractiveSurface *surface);
	/// Changes a set of colors on the state's 8bpp palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256, bool immediately = true);
	/// Changes the state's 8bpp palette with certain resources.
	void setPalette(const std::string &palette, int backpals = -1);
	/// Gets the state's 8bpp palette.
	SDL_Color *const getPalette();
	/// Let the state know the window has been resized.
	virtual void resize(int &dX, int &dY);
	/// Re-orients all the surfaces in the state.
	virtual void recenter(int dX, int dY);
};

}

#endif
