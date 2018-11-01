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

/**
 * Generic window used to display error messages.
 */
class ErrorMessageState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtMessage;

	void create(const std::string &str, SDL_Color *palette, Uint8 color, const std::string &bg, int bgColor);
public:
	/// Creates the Error state.
	ErrorMessageState(const std::string &msg, SDL_Color *palette, Uint8 color, const std::string &bg, int bgColor);
	/// Cleans up the Error state.
	~ErrorMessageState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
};

}
