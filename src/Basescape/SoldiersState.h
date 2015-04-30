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
#ifndef OPENXCOM_SOLDIERSSTATE_H
#define OPENXCOM_SOLDIERSSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;

/**
 * Soldiers screen that lets the player
 * manage all the soldiers in a base.
 */
class SoldiersState : public State
{
private:
	TextButton *_btnOk, *_btnPsiTraining, *_btnMemorial;
	Window *_window;
	Text *_txtTitle, *_txtName, *_txtRank, *_txtCraft;
	TextList *_lstSoldiers;
	Base *_base;
public:
	/// Creates the Soldiers state.
	SoldiersState(Base *base);
	/// Cleans up the Soldiers state.
	~SoldiersState();
	/// Updates the soldier names.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Psi Training button.
	void btnPsiTrainingClick(Action *action);
	/// Handler for clicking the Memorial button.
	void btnMemorialClick(Action *action);
	/// Handler for clicking the Soldiers list.
	void lstSoldiersClick(Action *action);
};

}

#endif
