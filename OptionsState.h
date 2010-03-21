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
#ifndef OPENXCOM__OPTIONSSTATE_H
#define OPENXCOM__OPTIONSSTATE_H

#include "State_Interactive.h"
#include "LangString.h"
#include "Palette.h"
#include "Button.h"
#include "Window.h"
#include "Text.h"
#include "AbandonGameState.h"
#include "GeoscapeState.h"

class OptionsState : public State
{
private:
	Button *_btnLoad, *_btnSave, *_btnAbandon, *_btnCancel;
	Window *_window;
	Text *_txtTitle;
public:
	OptionsState(Game *game);
	~OptionsState();
	void think();
	void btnLoadClick(SDL_Event *ev, int scale);
	void btnSaveClick(SDL_Event *ev, int scale);
	void btnAbandonClick(SDL_Event *ev, int scale);
	void btnCancelClick(SDL_Event *ev, int scale);
};

#endif
