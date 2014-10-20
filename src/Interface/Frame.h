/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#ifndef OPENXCOM_FRAME_H
#define OPENXCOM_FRAME_H

#include "../Engine/Surface.h"

namespace OpenXcom
{

/**
 * Fancy frame border thing used for windows and other elements.
 */
class Frame : public Surface
{
private:
	Uint8 _color, _bg;
	int _thickness;
	bool _contrast;
public:
	/// Creates a new frame with the specified size and position.
	Frame(int width, int height, int x = 0, int y = 0);
	/// Cleans up the frame.
	~Frame();
	/// Sets the border color.
	void setColor(Uint8 color);
	/// Gets the border color.
	Uint8 getColor() const;
	void setBorderColor(Uint8 color);
	/// Sets the background color.
	void setSecondaryColor(Uint8 bg);
	/// Gets the background color.
	Uint8 getSecondaryColor() const;
	/// Sets the high contrast color setting.
	void setHighContrast(bool contrast);
	/// Sets the border thickness.
	void setThickness(int thickness);
	/// Draws the frame.
	void draw();
};

}

#endif
