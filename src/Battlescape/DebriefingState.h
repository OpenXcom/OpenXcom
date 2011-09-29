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
#include <string>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;

struct DebriefingStat { DebriefingStat(std::string _item, bool recovery) : item(_item), qty(0), score(0), recovery(recovery) {}; std::string item; int qty; int score; bool recovery; };


/**
 * Debriefing screen shown after a Battlescape
 * mission that displays the results.
 */
class DebriefingState : public State
{
private:
	std::vector<DebriefingStat*> _stats;
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtQuantity, *_txtScore, *_txtUfoRecovery, *_txtRating;
	TextList *_lstStats, *_lstUfoRecovery, *_lstTotal;
	void addStat(const std::string &name, int quantity, int score);
	void prepareDebriefing();
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
