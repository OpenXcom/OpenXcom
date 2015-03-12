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
#ifndef OPENXCOM_SACKSOLDIERSTATE_H
#define OPENXCOM_SACKSOLDIERSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class Soldier;
class TextButton;
class Window;
class Text;

/**
 * Window shown when the player tries to
 * sack a soldier.
 */
class SackSoldierState : public State
{
private:
	Base *_base;
	size_t _soldierId;

	TextButton *_btnOk, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtSoldier;
public:
	/// Creates the Sack Soldier state.
	SackSoldierState(Base *base, size_t soldierId);
	/// Cleans up the Sack Soldier state.
	~SackSoldierState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}

#endif
