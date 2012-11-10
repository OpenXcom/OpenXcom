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
#ifndef OPENXCOM_ALLOCATEPSITRAININGSTATE_H
#define OPENXCOM_ALLOCATEPSITRAININGSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class Base;
class TextList;
class Soldier;

/**
 * Report screen shown monthly to display
 * changes in the player's performance and funding.
 */
class AllocatePsiTrainingState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtTraining, *_txtName, *_txtCraft;
	TextList *_lstSoldiers;
	std::vector<Soldier*> _soldiers;
	unsigned int _sel;
	Base *_base;
public:
	/// Creates the Psi Training state.
	AllocatePsiTrainingState(Game *game, Base *base);
	/// Cleans up the Psi Training state.
	~AllocatePsiTrainingState();
	/// Updates the palette.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	void btnBase1Click();
	void lstSoldiersPress(Action *action);
	void lstSoldiersRelease(Action *action);
	void lstSoldiersClick(Action *action);
};

}

#endif
