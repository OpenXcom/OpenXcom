/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_WARNINGMESSAGE_H
#define OPENXCOM_WARNINGMESSAGE_H

#include "../Engine/Surface.h"

namespace OpenXcom
{

class Text;
class Timer;
class Font;

/**
 * Coloured box with text inside that fades out after it is displayed.
 * Used to display warning/error messages on the Battlescape.
 */
class WarningMessage : public Surface
{
private:
	Text *_text;
	Timer *_timer;
	Uint8 _color, _fade;
public:
	/// Creates a new warning message with the specified size and position.
	WarningMessage(int width, int height, int x = 0, int y = 0);
	/// Cleans up the warning message.
	~WarningMessage();
	/// Sets the color for the warning message.
	void setColor(Uint8 color);
	/// Sets the text color for the warning message.
	void setTextColor(Uint8 color);
	/// Initializes the warning message's resources.
	void initText(Font *big, Font *small, Language *lang);
	/// Sets the warning message's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Shows the warning message.
	void showMessage(const std::wstring &msg);
	/// Handles the timers.
	void think();
	/// Fades the message.
	void fade();
	/// Draws the message.
	void draw();
};

}

#endif
