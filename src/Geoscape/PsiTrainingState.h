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
 * Screen shown monthly when the player has psi labs available.
 */
class PsiTrainingState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle;
	std::vector<TextButton*> _btnBases;
	std::vector<Base*> _bases;
public:
	/// Creates the Psi Training state.
	PsiTrainingState();
	/// Cleans up the Psi Training state.
	~PsiTrainingState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking a Base button.
	void btnBaseXClick(Action *action);
};

}

#endif
