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
#ifndef OPENXCOM_NEWPOSSIBLEMANUFACTURESTATE
#define OPENXCOM_NEWPOSSIBLEMANUFACTURESTATE

#include "../Engine/State.h"
namespace OpenXcom
{
class Game;
class Window;
class TextButton;
class Text;
class Base;
class TextList;
class RuleManufacture;

/**
 * Window which inform the player of new possible manufacture projects.
 * Also allow to go to the ManufactureState to dispatch available engineers.
 */
class NewPossibleManufactureState : public State
{
private:
	Window *_window;
	Text *_txtTitle;
	TextList * _lstPossibilities;
	TextButton *_btnManufacture, *_btnOk;
	Base * _base;
public:
	/// Creates the NewPossibleManufacture state.
	NewPossibleManufactureState(Base * base, const std::vector<RuleManufacture *> & possibilities);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Allocate Manufacture button.
	void btnManufactureClick(Action *action);
};

}
#endif
