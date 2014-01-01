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
#ifndef OPENXCOM_PSITRAININGSTATE_H
#define OPENXCOM_PSITRAININGSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class ArrowButton;
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
	ArrowButton *_up, *_down;
	Timer *_timerUp, *_timerDown;
	std::vector<TextButton*> _btnBases;
	std::vector<Base*> _bases;
	size_t _startIndex;
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking a Base button.
	void btnBaseXClick(Action *action);
	/// Handler for pressing the up arrowbutton.
	void upPress(Action * action);
	/// Handler for releasing the up arrowbutton.
	void upRelease(Action * action);
	/// Handler for clicking the up arrowbutton.
	void upClick(Action * action);
	/// Handler for pressing the down arrowbutton.
	void downPress(Action * action);
	/// Handler for releasing the down arrowbutton.
	void downRelease(Action * action);
	/// Handler for clicking the down arrowbutton.
	void downClick(Action * action);
	/// Event handler for _timerUp.
	void onTimerUp();
	/// Event handler for _timerDown.
	void onTimerDown();
	/// Scrolls up by the number of change.
	void scrollUp(int change);
	/// Scrolls down by the number of change.
	void scrollDown(int change);
	/// Updates the names of the _baseButtons, and arrowButtons.
	void updateButtons();
	/// Runs state functionality every cycle.
	void think();
public:
	/// Creates the Psi Training state.
	PsiTrainingState(Game *game);
	/// Cleans up the Psi Training state.
	~PsiTrainingState();
};

}

#endif
