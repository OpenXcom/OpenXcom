/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifndef OPENXCOM_MANUFACTUREINFOSTATE_H
#define OPENXCOM_MANUFACTUREINFOSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{
class Base;
class Window;
class Text;
class ArrowButton;
class TextButton;
class RuleManufacture;
class Production;
class Timer;
class InteractiveSurface;

/**
 * Screen that allows changing of Production settings (assigned engineer, units to build).
*/
class ManufactureInfoState : public State
{
private:
	Base * _base;
	RuleManufacture * _item;
	Production * _production;
	Window * _window;
	ArrowButton * _btnUnitUp, * _btnUnitDown, * _btnEngineerUp, * _btnEngineerDown;
	TextButton * _btnStop, * _btnOk;
	Text * _txtTitle, * _txtAvailableEngineer, * _txtAvailableSpace, * _txtAllocatedEngineer, * _txtUnitToProduce, * _txtUnitUp, * _txtUnitDown, * _txtEngineerUp, * _txtEngineerDown, * _txtAllocated, * _txtTodo;
	Timer * _timerMoreEngineer, * _timerMoreUnit, * _timerLessEngineer, * _timerLessUnit;
	InteractiveSurface *_surface1, *_surface2;
	int _changeValueByMouseWheel;
	/// Handler for the Stop button.
	void btnStopClick (Action * action);
	/// Handler for the OK button.
	void btnOkClick (Action * action);
	/// Adds given number of engineers to the project if possible.
	void moreEngineer(int change);
	/// Handler for pressing the more engineer button.
	void moreEngineerPress(Action * action);
	/// Handler for releasing the more engineer button.
	void moreEngineerRelease(Action * action);
	/// Handler for clicking the more engineer button.
	void moreEngineerClick(Action * action);
	/// Adds given number of units to produce to the project if possible.
	void moreUnit(int change);
	/// Handler for pressing the more unit button.
	void moreUnitPress(Action * action);
	/// Handler for releasing the more unit button.
	void moreUnitRelease(Action * action);
	/// Handler for clicking the more unit button.
	void moreUnitClick(Action * action);
	/// Removes the given number of engineers from the project if possible.
	void lessEngineer(int change);
	/// Handler for pressing the less engineer button.
	void lessEngineerPress(Action * action);
	/// Handler for releasing the less engineer button.
	void lessEngineerRelease(Action * action);
	/// Handler for clicking the less engineer button.
	void lessEngineerClick(Action * action);
	/// Removes the given number of units to produce from the project if possible.
	void lessUnit(int change);
	/// Handler for pressing the less unit button.
	void lessUnitPress(Action * action);
	/// Handler for releasing the less unit button.
	void lessUnitRelease(Action * action);
	/// Handler for clicking the less unit button.
	void lessUnitClick(Action * action);
	/// Adds one engineer to the production (if possible).
	void onMoreEngineer();
	/// Removes one engineer from the production (if possible).
	void onLessEngineer();
	/// Handler for using the mouse wheel on the Engineer-part of the screen.
	void handleWheelEngineer(Action *action);
	/// Increases count of number of units to make.
	void onMoreUnit();
	/// Decreases count of number of units to make (if possible).
	void onLessUnit();
	/// Handler for using the mouse wheel on the Unit-part of the screen.
	void handleWheelUnit(Action *action);
	/// Updates display of assigned/available engineers and workshop space.
	void setAssignedEngineer();
	/// Runs state functionality every cycle.
	void think();
	/// Builds the User Interface.
	void buildUi();
	/// Helper to exit the State.
	void exitState();
public:
	/// Creates the State (new production).
	ManufactureInfoState (Game * game, Base * base, RuleManufacture * _item);
	/// Creates the State (modify production).
	ManufactureInfoState (Game * game, Base * base, Production * production);
};
}
#endif
