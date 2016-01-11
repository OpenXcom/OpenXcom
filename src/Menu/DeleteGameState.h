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
#include "OptionsBaseState.h"
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;

/**
 * window used to confirm save game deletion.
 */
class DeleteGameState : public State
{
private:
	TextButton *_btnNo, *_btnYes;
	Window *_window;
	Text *_txtMessage;
	std::string _filename;
	OptionsOrigin _origin;
public:
	/// Creates the Confirm state.
	DeleteGameState(OptionsOrigin origin, const std::string &save);
	/// Cleans up the Confirm state.
	~DeleteGameState();
	/// Handler for clicking the confirm button.
	void btnYesClick(Action *action);
	/// Handler for clicking the cancel button.
	void btnNoClick(Action *action);
};

}
