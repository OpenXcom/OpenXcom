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
#include "../Engine/InteractiveSurface.h"

namespace OpenXcom
{

/**
 * Regular image that works like a button.
 * Unlike the TextButton, this button doesn't draw
 * anything on its own. It takes an existing graphic and
 * treats it as a button, inverting colors when necessary.
 * This is necessary for special buttons like in the Geoscape.
 */
class ImageButton : public InteractiveSurface
{
protected:
	Uint8 _color;
	ImageButton **_group;
	bool _inverted;
public:
	/// Creates a new image button with the specified size and position.
	ImageButton(int width, int height, int x = 0, int y = 0);
	/// Cleans up the image button.
	virtual ~ImageButton();
	/// Sets the image button's color.
	void setColor(Uint8 color);
	/// Gets the image button's color.
	Uint8 getColor() const;
	/// Sets the image button's group.
	void setGroup(ImageButton **group);
	/// Special handling for mouse presses.
	void mousePress(Action *action, State *state);
	/// Special handling for mouse releases.
	void mouseRelease(Action *action, State *state);
	/// Invert a button explicitly either ON or OFF.
	void toggle(bool invert);
};

}
