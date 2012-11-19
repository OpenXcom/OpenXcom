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
#ifndef OPENXCOM_PSITRAININGSTATE_H
#define OPENXCOM_PSITRAININGSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class Base;

/**
 * Report screen shown monthly to display
 * changes in the player's performance and funding.
 */
class PsiTrainingState : public State
{
private:
	TextButton *_btnOk, *_btnBase1, *_btnBase2, *_btnBase3, *_btnBase4;
	TextButton *_btnBase5, *_btnBase6, *_btnBase7, *_btnBase8;
	Window *_window;
	Text *_txtTitle;
	Base *_base1, *_base2, *_base3, *_base4, *_base5, *_base6, *_base7, *_base8;
public:
	/// Creates the Psi Training state.
	PsiTrainingState(Game *game);
	/// Cleans up the Psi Training state.
	~PsiTrainingState();
	/// Updates the palette.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	void btnBase1Click(Action *action);
	void btnBase2Click(Action *action);
	void btnBase3Click(Action *action);
	void btnBase4Click(Action *action);
	void btnBase5Click(Action *action);
	void btnBase6Click(Action *action);
	void btnBase7Click(Action *action);
	void btnBase8Click(Action *action);
};

}

#endif
