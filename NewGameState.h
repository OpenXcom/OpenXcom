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
#ifndef OPENXCOM__NEWGAMESTATE_H
#define OPENXCOM__NEWGAMESTATE_H

#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "Button.h"
#include "Window.h"
#include "Text.h"
#include "GeoscapeState.h"

class NewGameState : public State
{
private:
	Button *_btnBeginner, *_btnExperienced, *_btnVeteran, *_btnGenius, *_btnSuperhuman;
	Window *_window;
	Text *_txtTitle;
public:
	NewGameState(Game *game);
	~NewGameState();
	void think();
	void btnBeginnerClick(SDL_Event *ev, int scale);
	void btnExperiencedClick(SDL_Event *ev, int scale);
	void btnVeteranClick(SDL_Event *ev, int scale);
	void btnGeniusClick(SDL_Event *ev, int scale);
	void btnSuperhumanClick(SDL_Event *ev, int scale);
};

#endif
