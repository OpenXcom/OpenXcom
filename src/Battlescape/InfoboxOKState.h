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
#include <string>

namespace OpenXcom
{

class TextButton;
class Frame;
class Text;

/**
 * Notifies the player about things like soldiers going unconscious or dying from wounds.
 */
class InfoboxOKState : public State
{
private:
	TextButton *_btnOk;
	Frame *_frame;
	Text *_txtTitle;
public:
	/// Creates the InfoboxOKState.
	InfoboxOKState(const std::string &msg);
	/// Cleans up the InfoboxOKState.
	~InfoboxOKState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
};

}
