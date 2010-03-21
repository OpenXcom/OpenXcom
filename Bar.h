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

class Bar : public Surface
{
private:
	Uint8 _color;
	double _scale, _max, _value;
public:
	Bar(int width, int height, int x = 0, int y = 0);
	~Bar();
	void setColor(Uint8 color);
	Uint8 getColor();
	void setScale(double scale);
	double getScale();
	void setMax(double max);
	double getMax();
	void setValue(double value);
	double getValue();
	void blit(Surface *surface);
};

#endif