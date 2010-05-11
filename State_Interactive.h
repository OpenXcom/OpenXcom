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

class State
{
protected:
	Game *_game;
	vector<Surface*> _surfaces;
	bool _screen;

public:
	State(Game* game);
	virtual ~State();
	void add(Surface *surface);
	bool isScreen();
	virtual void init();
	virtual void handle(SDL_Event* ev, int scale);
	virtual void think();
	virtual void blit();
};

typedef State &(State::*EventHandler)(SDL_Event *, int);

class InteractiveSurface : public Surface
{
protected:
	EventHandler _click, _press, _release, _in, _out, _keyPress, _keyRelease;
	bool _isPressed, _isHovered, _isFocused;

public:
	InteractiveSurface(int width, int height, int x, int y);
	virtual ~InteractiveSurface();
	virtual void handle(SDL_Event *ev, int scale, State *state);
	void focus();
	void onMouseClick(EventHandler handler);
	void onMousePress(EventHandler handler);
	void onMouseRelease(EventHandler handler);
	void onMouseIn(EventHandler handler);
	void onMouseOut(EventHandler handler);
	void onKeyboardPress(EventHandler handler);
	void onKeyboardRelease(EventHandler handler);
	
	virtual void mousePress(SDL_Event *ev, int scale, State *state);
	virtual void mouseRelease(SDL_Event *ev, int scale, State *state);
	virtual void mouseClick(SDL_Event *ev, int scale, State *state);
	virtual void mouseIn(SDL_Event *ev, int scale, State *state);
	virtual void mouseOut(SDL_Event *ev, int scale, State *state);
	virtual void keyboardPress(SDL_Event *ev, int scale, State *state);
	virtual void keyboardRelease(SDL_Event *ev, int scale, State *state);

};

#endif