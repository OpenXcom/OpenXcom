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
#ifndef __TESTSTATE_H__
#define __TESTSTATE_H__

#include "State.h"
#include "Palette.h"
#include "SurfaceSet.h"
#include "Surface.h"
#include "Button.h"
#include "Window.h"
#include "Text.h"
#include "TextList.h"

class TestState : public State
{
private:
	SurfaceSet *_set;
	Surface *_surf;
	Button *_button;
	Window *_window;
	Text *_text;
	TextList *_list;
	int _i;

	SDL_Surface *testSurface();
public:
	TestState(Game *game);
	~TestState();
	void handle(SDL_Event *ev, int scale);
	void think();
	void blit();
};

#endif
