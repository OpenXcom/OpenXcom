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
#ifndef OPENXCOM_BASENAMESTATE_H
#define OPENXCOM_BASENAMESTATE_H

#include "../Engine/State.h"
#include "Globe.h"

namespace OpenXcom
{

class Base;
class Window;
class Text;
class TextEdit;
class TextButton;
class Globe;

/**
 * Window used to input a name for a new base.
 * Player's first Base uses this screen
 * additional bases use ConfirmNewBaseState
 */
class BaseNameState : public State
{
private:
	Base *_base;
	Globe *_globe;
	Window *_window;
	Text *_txtTitle;
	TextEdit *_edtName;
	TextButton *_btnOk;
	bool _first;
public:
	/// Creates the Base Name state.
	BaseNameState(Base *base, Globe *globe, bool first);
	/// Cleans up the Base Name state.
	~BaseNameState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for changing text on the Name edit.
	void edtNameChange(Action *action);
};

}

#endif
