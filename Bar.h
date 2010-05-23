/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__BAR_H
#define OPENXCOM__BAR_H

#include <cmath>
#include "SDL.h"
#include "Surface.h"

/**
 * Bar graphic that represents a certain value.
 * Drawn with a coloured border and partially
 * filled content to contrast two values, typically
 * used for showing base and soldier stats.
 */
class Bar : public Surface
{
private:
	Uint8 _color;
	double _scale, _max, _value;
	bool _invert;
public:
	/// Creates a new bar with the specified size and position.
	Bar(int width, int height, int x = 0, int y = 0);
	/// Cleans up the bar.
	~Bar();
	/// Sets the bar's color.
	void setColor(Uint8 color);
	/// Gets the bar's color.
	Uint8 getColor();
	/// Sets the bar's scale.
	void setScale(double scale);
	/// Gets the bar's scale.
	double getScale();
	/// Sets the bar's maximum value.
	void setMax(double max);
	/// Gets the bar's maximum value.
	double getMax();
	/// Sets the bar's current value.
	void setValue(double value);
	/// Gets the bar's current value.
	double getValue();
	/// Sets the text's color invert setting.
	void setInvert(bool invert);
	/// Blits the bar onto a surface.
	void blit(Surface *surface);
};

#endif