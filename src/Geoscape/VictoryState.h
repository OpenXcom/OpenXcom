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
#ifndef OPENXCOM_VICTORYSTATE_H
#define OPENXCOM_VICTORYSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class InteractiveSurface;
class Text;

/**
 * Game Over Screens.
 */
class VictoryState : public State
{
private:
	InteractiveSurface *_screen;
	std::vector<Text*> _txtText;
	int _screenNumber;
public:
	/// Creates the Victory state.
	VictoryState(Game *game);
	/// Cleans up the Victory state.
	~VictoryState();
	void init();
	/// Handler for clicking the screen.
	void windowClick(Action *action);
	void nextScreen();
};

}

#endif
