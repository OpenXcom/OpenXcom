/*
 * Copyright 2010-2012 OpenXcom Developers.
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

/**
 * Screen which allow to change Production settings(assigned engineer, units to build).
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
	/// Handler for the Stop button
	void btnStopClick (Action * action);
	/// Handler for the OK button
	void btnOkClick (Action * action);
	/// Handler for pressing the more engineer button
	void moreEngineerPress(Action * action);
	/// Handler for releasing the more engineer button
	void moreEngineerRelease(Action * action);
	/// Handler for pressing the more unit button
	void moreUnitPress(Action * action);
	/// Handler for releasing the more unit button
	void moreUnitRelease(Action * action);
	/// Handler for pressing the less engineer button
	void lessEngineerPress(Action * action);
	/// Handler for releasing the less engineer button
	void lessEngineerRelease(Action * action);
	/// Handler for pressing the less unit button
	void lessUnitPress(Action * action);
	/// Handler for releasing the less unit button
	void lessUnitRelease(Action * action);
	/// Add one engineer to production (if possible)
	void onMoreEngineer();
	/// Remove one engineer to production (if possible)
	void onLessEngineer();
	/// Increase count of number of unit to do
	void onMoreUnit();
	/// Decrease count of number of unit to do(if possible)
	void onLessUnit();
	/// Update display of assigned/available engineer, workshop space
	void setAssignedEngineer();
	/// Runs state functionality every cycle.
	void think();
	/// Build the User Interface
	void buildUi();
	/// Helper to exit the State
	void exitState();
public:
	/// Create the State(new production)
	ManufactureInfoState (Game * game, Base * base, RuleManufacture * _item);
	/// Create the State(modify production)
	ManufactureInfoState (Game * game, Base * base, Production * production);
};
}
#endif
