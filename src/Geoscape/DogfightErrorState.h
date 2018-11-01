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
#include <string>
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class Craft;

/**
 * Window used to notify the player when
 * an error occurs with a dogfight procedure.
 */
class DogfightErrorState : public State
{
private:
	Craft *_craft;
	TextButton *_btnIntercept, *_btnBase;
	Window *_window;
	Text *_txtCraft, *_txtMessage;
public:
	/// Creates the Craft Error state.
	DogfightErrorState(Craft *craft, const std::string &msg);
	/// Cleans up the Craft Error state.
	~DogfightErrorState();
	/// Handler for clicking the Continue Interception button.
	void btnInterceptClick(Action *action);
	/// Handler for clicking the Return To Base button.
	void btnBaseClick(Action *action);
};

}
