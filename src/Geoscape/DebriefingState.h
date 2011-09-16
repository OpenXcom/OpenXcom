/*
 * Copyright 2010 OpenXcom Developers.
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
#ifndef OPENXCOM_DEBRIEFINGSTATE_H
#define OPENXCOM_DEBRIEFINGSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;

/**
 * Debriefing screen shown after a Battlescape
 * mission that displays the results.
 */
class DebriefingState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtQuantity, *_txtScore, *_txtUfoRecovery, *_txtRating;
	TextList *_lstStats, *_lstUfoRecovery, *_lstTotal;
public:
	/// Creates the Debriefing state.
	DebriefingState(Game *game);
	/// Cleans up the Debriefing state.
	~DebriefingState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
};

}

#endif
