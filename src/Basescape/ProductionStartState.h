/*
 * Copyright 2011 OpenXcom Developers.
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
#ifndef OPENXCOM_PRODUCTIONSTARTSTATE_H
#define OPENXCOM_PRODUCTIONSTARTSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{
class Base;
class RuleManufactureInfo;
class Window;
class TextButton;
class Text;
class TextList;

/**
 * Screen which display needed elements to start productions(items/required workshop state/cost to build a unit, ...).
*/
class ProductionStartState : public State
{
private:
	Base * _base;
	RuleManufactureInfo * _item;
	Window * _window;
	TextButton * _btnCancel, * _btnStart;
	Text * _txtTitle, * _txtManHour, * _txtCost, * _txtWorkSpace, * _txtNeededItemsTitle, * _txtItemNameColumn, * _txtUnitRequiredColumn, * _txtUnitAvailableColumn;
	TextList * _lstNeededItems;
public:
	/// Create the State
	ProductionStartState(Game * game, Base * base, RuleManufactureInfo * item);
	/// Handler for the Cancel button
	void btnCancelClick(Action * action);
	/// Handler for the start button
	void btnStartClick(Action * action);
};
}
#endif
