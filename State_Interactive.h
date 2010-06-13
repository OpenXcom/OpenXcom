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

// Prevent cyclic dependency
#ifndef OPENXCOM__STATE_INTERACTIVE_H
#define OPENXCOM__STATE_INTERACTIVE_H

#include <vector>
#include "SDL.h"
#include "Game.h"
#include "Surface.h"

using namespace std;

class InteractiveSurface;

/**
 * A game state that receives user input and reacts accordingly.
 ª Game states typically represent a whole window or screen that
 * the user interacts with, making the game... well, interactive.
 * They automatically handle child elements used to transmit
 * information from/to the user, and are linked to the core game
 * engine which manages them.
 */
class State
{
protected:
	Game *_game;
	vector<Surface*> _surfaces;
	bool _screen;

public:
	/// Creates a new state linked to a game.
	State(Game* game);
	/// Cleans up the state.
	virtual ~State();
	/// Adds a child element to the state.
	void add(Surface *surface);
	/// Gets whether the state is a full-screen.
	bool isScreen();
	/// Initializes the state.
	virtual void init();
	/// Handles any events
	virtual void handle(SDL_Event* ev, int scale);
	/// Runs state functionality every cycle.
	virtual void think();
	/// Blits the state to the screen.
	virtual void blit();
};

typedef State &(State::*EventHandler)(SDL_Event *, int);

/**
 * Surface that the user can interact with.
 * Specialized version of the standard Surface that
 * processes all the various SDL events and turns
 * them into useful interactions with the Surface,
 * so specialized subclasses don't need to worry about it.
 */
class InteractiveSurface : public Surface
{
protected:
	EventHandler _click, _press, _release, _in, _over, _out, _keyPress, _keyRelease;
	bool _isPressed, _isHovered, _isFocused;

public:
	/// Creates a new interactive surface with the specified size and position.
	InteractiveSurface(int width, int height, int x = 0, int y = 0);
	/// Cleans up the interactive surface.
	virtual ~InteractiveSurface();
	/// Processes any pending events.
	virtual void handle(SDL_Event *ev, int scale, State *state);
	/// Sets focus on this surface.
	void focus();
	/// Hooks an event handler to a mouse click on the surface.
	void onMouseClick(EventHandler handler);
	/// Hooks an event handler to a mouse press over the surface.
	void onMousePress(EventHandler handler);
	/// Hooks an event handler to a mouse release over the surface.
	void onMouseRelease(EventHandler handler);
	/// Hooks an event handler to moving the mouse into the surface.
	void onMouseIn(EventHandler handler);
	/// Hooks an event handler to moving the mouse over the surface.
	void onMouseOver(EventHandler handler);
	/// Hooks an event handler to moving the mouse out of the surface.
	void onMouseOut(EventHandler handler);
	/// Hooks an event handler to pressing a key when the surface is focused.
	void onKeyboardPress(EventHandler handler);
	/// Hooks an event handler to releasing a key when the surface is focused.
	void onKeyboardRelease(EventHandler handler);
	
	/// Processes a mouse button press event.
	virtual void mousePress(SDL_Event *ev, int scale, State *state);
	/// Processes a mouse button release event.
	virtual void mouseRelease(SDL_Event *ev, int scale, State *state);
	/// Processes a mouse click event.
	virtual void mouseClick(SDL_Event *ev, int scale, State *state);
	/// Processes a mouse hover in event.
	virtual void mouseIn(SDL_Event *ev, int scale, State *state);
	/// Processes a mouse hover event.
	virtual void mouseOver(SDL_Event *ev, int scale, State *state);
	/// Processes a mouse hover out event.
	virtual void mouseOut(SDL_Event *ev, int scale, State *state);
	/// Processes a keyboard key press event.
	virtual void keyboardPress(SDL_Event *ev, int scale, State *state);
	/// Processes a keyboard key release event.
	virtual void keyboardRelease(SDL_Event *ev, int scale, State *state);

};

#endif