/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_BASEDEFENSESTATE_H
#define OPENXCOM_BASEDEFENSESTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class Base;
class Ufo;
class TextList;
class GeoscapeState;
class Timer;

enum BaseDefenseActionType { BDA_NONE, BDA_FIRE, BDA_RESOLVE, BDA_DESTROY, BDA_EXPLODING1, BDA_EXPLODING2, BDA_END };

/**
 * Base Defense Screen for when ufos try to attack.
 */
class BaseDefenseState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtInit;
	TextList *_lstDefenses;
	Base *_base;
	Ufo *_ufo;
	int _thinkcycles, _row, _passes, _gravShields, _defenses, _attacks;
	BaseDefenseActionType _action;
	Timer *_timer;
	GeoscapeState *_state;
public:
	/// Creates the Base Defense state.
	BaseDefenseState(Base *base, Ufo *ufo, GeoscapeState *state);
	/// Cleans up the Base Defense state.
	~BaseDefenseState();
	/// Handle the Timer.
	void think();
	/// do the next step.
	void nextStep();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
};

}

#endif
