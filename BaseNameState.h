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
#ifndef OPENXCOM__BASENAMESTATE_H
#define OPENXCOM__BASENAMESTATE_H

#include "State.h"

class Base;
class Window;
class Text;
class TextEdit;

/**
 * Window used to input a name for a new base.
 */
class BaseNameState : public State
{
private:
	Base *_base;
	Window *_window;
	Text *_txtTitle;
	TextEdit *_edtName;
	bool _first;
public:
	/// Creates the Base Name state.
	BaseNameState(Game *game, Base *base, bool first);
	/// Cleans up the Base Name state.
	~BaseNameState();
	/// Handler for releasing a key on the Name edit.
	void edtNameKeyPress(SDL_Event *ev, int scale);
};

#endif
