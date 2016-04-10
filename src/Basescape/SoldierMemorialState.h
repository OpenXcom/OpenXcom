#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;

/**
 * Screen that shows all the soldiers
 * that have died throughout the game.
 */
class SoldierMemorialState : public State
{
private:
	TextButton *_btnOk, *_btnStatistics;
	Window *_window;
	Text *_txtTitle, *_txtName, *_txtRank, *_txtDate, *_txtRecruited, *_txtLost;
	TextList *_lstSoldiers;
public:
	/// Creates the Soldiers state.
	SoldierMemorialState();
	/// Cleans up the Soldiers state.
	~SoldierMemorialState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Statistics button.
	void btnStatisticsClick(Action *action);
	/// Handler for clicking the Soldiers list.
	void lstSoldiersClick(Action *action);
};

}
