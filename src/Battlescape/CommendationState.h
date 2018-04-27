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
class Soldier;

/**
 * Medals screen that displays new soldier medals.
 */
class CommendationState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle;
	TextList *_lstSoldiers;
public:
	/// Creates the Medals state.
	CommendationState(std::vector<Soldier*> soldiers);
	/// Cleans up the Medals state.
	~CommendationState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
};

}
