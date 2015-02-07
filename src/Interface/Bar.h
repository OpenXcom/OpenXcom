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
#ifndef OPENXCOM_BAR_H
#define OPENXCOM_BAR_H

#include "../Engine/Surface.h"

namespace OpenXcom
{

/**
 * Bar graphic that represents a certain value.
 * Drawn with a coloured border and partially
 * filled content to contrast two values, typically
 * used for showing base and soldier stats.
 */
class Bar : public Surface
{
private:
	Uint8 _color, _color2, _borderColor;
	double _scale, _max, _value, _value2;
	bool _invert, _secondOnTop;
public:
	/// Creates a new bar with the specified size and position.
	Bar(int width, int height, int x = 0, int y = 0);
	/// Cleans up the bar.
	~Bar();
	/// Sets the bar's color.
	void setColor(Uint8 color);
	/// Gets the bar's color.
	Uint8 getColor() const;
	/// Sets the bar's second color.
	void setSecondaryColor(Uint8 color);
	/// Gets the bar's second color.
	Uint8 getSecondaryColor() const;
	/// Sets the bar's scale.
	void setScale(double scale);
	/// Gets the bar's scale.
	double getScale() const;
	/// Sets the bar's maximum value.
	void setMax(double max);
	/// Gets the bar's maximum value.
	double getMax() const;
	/// Sets the bar's current value.
	void setValue(double value);
	/// Gets the bar's current value.
	double getValue() const;
	/// Sets the bar's second current value.
	void setValue2(double value);
	/// Gets the bar's second current value.
	double getValue2() const;
	/// Defines whether the second value should be drawn on top.
	void setSecondValueOnTop(bool onTop);
	/// Sets the bar's color invert setting.
	void setInvert(bool invert);
	/// Draws the bar.
	void draw();
	/// set the outline color for the bar.
	void setBorderColor(Uint8 bc);
};

}

#endif
