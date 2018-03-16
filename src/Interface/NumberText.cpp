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
#include "NumberText.h"
#include <sstream>
#include <string>

namespace OpenXcom
{

/**
 * Sets up a blank number text.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
NumberText::NumberText(int width, int height, int x, int y) : Surface(width, height, x, y), _value(0), _bordered(false), _color(0)
{
	_chars[0] = new Surface(3, 5);
	_chars[0]->lock();
	_chars[0]->setPixel(0, 0, 1);
	_chars[0]->setPixel(1, 0, 1);
	_chars[0]->setPixel(2, 0, 1);
	_chars[0]->setPixel(0, 1, 1);
	_chars[0]->setPixel(0, 2, 1);
	_chars[0]->setPixel(0, 3, 1);
	_chars[0]->setPixel(2, 1, 1);
	_chars[0]->setPixel(2, 2, 1);
	_chars[0]->setPixel(2, 3, 1);
	_chars[0]->setPixel(0, 4, 1);
	_chars[0]->setPixel(1, 4, 1);
	_chars[0]->setPixel(2, 4, 1);
	_chars[0]->unlock();

	_chars[1] = new Surface(3, 5);
	_chars[1]->lock();
	_chars[1]->setPixel(1, 0, 1);
	_chars[1]->setPixel(1, 1, 1);
	_chars[1]->setPixel(1, 2, 1);
	_chars[1]->setPixel(1, 3, 1);
	_chars[1]->setPixel(0, 4, 1);
	_chars[1]->setPixel(1, 4, 1);
	_chars[1]->setPixel(2, 4, 1);
	_chars[1]->setPixel(0, 1, 1);
	_chars[1]->unlock();

	_chars[2] = new Surface(3, 5);
	_chars[2]->lock();
	_chars[2]->setPixel(0, 0, 1);
	_chars[2]->setPixel(1, 0, 1);
	_chars[2]->setPixel(2, 0, 1);
	_chars[2]->setPixel(2, 1, 1);
	_chars[2]->setPixel(0, 2, 1);
	_chars[2]->setPixel(1, 2, 1);
	_chars[2]->setPixel(2, 2, 1);
	_chars[2]->setPixel(0, 3, 1);
	_chars[2]->setPixel(0, 4, 1);
	_chars[2]->setPixel(1, 4, 1);
	_chars[2]->setPixel(2, 4, 1);
	_chars[2]->unlock();

	_chars[3] = new Surface(3, 5);
	_chars[3]->lock();
	_chars[3]->setPixel(0, 0, 1);
	_chars[3]->setPixel(1, 0, 1);
	_chars[3]->setPixel(2, 0, 1);
	_chars[3]->setPixel(2, 1, 1);
	_chars[3]->setPixel(2, 2, 1);
	_chars[3]->setPixel(2, 3, 1);
	_chars[3]->setPixel(0, 2, 1);
	_chars[3]->setPixel(1, 2, 1);
	_chars[3]->setPixel(0, 4, 1);
	_chars[3]->setPixel(1, 4, 1);
	_chars[3]->setPixel(2, 4, 1);
	_chars[3]->unlock();

	_chars[4] = new Surface(3, 5);
	_chars[4]->lock();
	_chars[4]->setPixel(0, 0, 1);
	_chars[4]->setPixel(0, 1, 1);
	_chars[4]->setPixel(0, 2, 1);
	_chars[4]->setPixel(1, 2, 1);
	_chars[4]->setPixel(2, 0, 1);
	_chars[4]->setPixel(2, 1, 1);
	_chars[4]->setPixel(2, 2, 1);
	_chars[4]->setPixel(2, 3, 1);
	_chars[4]->setPixel(2, 4, 1);
	_chars[4]->unlock();

	_chars[5] = new Surface(3, 5);
	_chars[5]->lock();
	_chars[5]->setPixel(0, 0, 1);
	_chars[5]->setPixel(1, 0, 1);
	_chars[5]->setPixel(2, 0, 1);
	_chars[5]->setPixel(0, 1, 1);
	_chars[5]->setPixel(0, 2, 1);
	_chars[5]->setPixel(1, 2, 1);
	_chars[5]->setPixel(2, 2, 1);
	_chars[5]->setPixel(2, 3, 1);
	_chars[5]->setPixel(0, 4, 1);
	_chars[5]->setPixel(1, 4, 1);
	_chars[5]->setPixel(2, 4, 1);
	_chars[5]->unlock();

	_chars[6] = new Surface(3, 5);
	_chars[6]->lock();
	_chars[6]->setPixel(0, 0, 1);
	_chars[6]->setPixel(1, 0, 1);
	_chars[6]->setPixel(2, 0, 1);
	_chars[6]->setPixel(0, 1, 1);
	_chars[6]->setPixel(0, 2, 1);
	_chars[6]->setPixel(1, 2, 1);
	_chars[6]->setPixel(2, 2, 1);
	_chars[6]->setPixel(0, 3, 1);
	_chars[6]->setPixel(2, 3, 1);
	_chars[6]->setPixel(0, 4, 1);
	_chars[6]->setPixel(1, 4, 1);
	_chars[6]->setPixel(2, 4, 1);
	_chars[6]->unlock();

	_chars[7] = new Surface(3, 5);
	_chars[7]->lock();
	_chars[7]->setPixel(0, 0, 1);
	_chars[7]->setPixel(1, 0, 1);
	_chars[7]->setPixel(2, 0, 1);
	_chars[7]->setPixel(2, 1, 1);
	_chars[7]->setPixel(2, 2, 1);
	_chars[7]->setPixel(2, 3, 1);
	_chars[7]->setPixel(2, 4, 1);
	_chars[7]->unlock();

	_chars[8] = new Surface(3, 5);
	_chars[8]->lock();
	_chars[8]->setPixel(0, 0, 1);
	_chars[8]->setPixel(1, 0, 1);
	_chars[8]->setPixel(2, 0, 1);
	_chars[8]->setPixel(0, 1, 1);
	_chars[8]->setPixel(0, 2, 1);
	_chars[8]->setPixel(0, 3, 1);
	_chars[8]->setPixel(2, 1, 1);
	_chars[8]->setPixel(2, 2, 1);
	_chars[8]->setPixel(2, 3, 1);
	_chars[8]->setPixel(1, 2, 1);
	_chars[8]->setPixel(0, 4, 1);
	_chars[8]->setPixel(1, 4, 1);
	_chars[8]->setPixel(2, 4, 1);
	_chars[8]->unlock();

	_chars[9] = new Surface(3, 5);
	_chars[9]->lock();
	_chars[9]->setPixel(0, 0, 1);
	_chars[9]->setPixel(1, 0, 1);
	_chars[9]->setPixel(2, 0, 1);
	_chars[9]->setPixel(0, 1, 1);
	_chars[9]->setPixel(0, 2, 1);
	_chars[9]->setPixel(2, 1, 1);
	_chars[9]->setPixel(2, 2, 1);
	_chars[9]->setPixel(2, 3, 1);
	_chars[9]->setPixel(1, 2, 1);
	_chars[9]->setPixel(0, 4, 1);
	_chars[9]->setPixel(1, 4, 1);
	_chars[9]->setPixel(2, 4, 1);
	_chars[9]->unlock();

	for (int i = 0; i < 10; ++i)
	{
		_borderedChars[i] = new Surface(5, 7);
		// give it a border
		// this is the "darker" shade that goes in the corners.
		for (int j = 0; j <= 2; j += 2)
		{
			for (int k = 0; k <= 2; k += 2)
			{
				_chars[i]->blitNShade(_borderedChars[i], j, k, 11);
			}
		}
		// this is the "slightly darker" version that goes in four cardinals.
		for (int z = 0; z <= 2; z += 2)
		{
			_chars[i]->blitNShade(_borderedChars[i], z, 1, 8);
			_chars[i]->blitNShade(_borderedChars[i], 1, z, 8);
		}
		// and finally the number itself
		_chars[i]->blitNShade(_borderedChars[i], 1, 1, 0);
	}
}

/**
 *
 */
NumberText::~NumberText()
{
	for (int i = 0; i < 10; ++i)
	{
		delete _chars[i];
		delete _borderedChars[i];
	}
}

/**
 * Changes the value used to render the number.
 * @param value Number value.
 */
void NumberText::setValue(unsigned int value)
{
	_value = value;
	_redraw = true;
}

/**
 * Returns the value used to render the number.
 * @return Number value.
 */
unsigned int NumberText::getValue() const
{
	return _value;
}

/**
 * Changes the color used to render the number.
 * @param color Color value.
 */
void NumberText::setColor(Uint8 color)
{
	_color = color;
	_redraw = true;
}

/**
 * Returns the color used to render the number.
 * @return Color value.
 */
Uint8 NumberText::getColor() const
{
	return _color;
}

/**
 * Replaces a certain amount of colors in the number text palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void NumberText::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	for (int i = 0; i < 10; ++i)
	{
		_chars[i]->setPalette(colors, firstcolor, ncolors);
		_borderedChars[i]->setPalette(colors, firstcolor, ncolors);
	}
}

/**
 * Draws all the digits in the number.
 */
void NumberText::draw()
{
	Surface::draw();
	std::ostringstream ss;
	ss << _value;
	std::string s = ss.str();
	int x = 0;
	if (!_bordered)
	{
		for (std::string::iterator i = s.begin(); i != s.end(); ++i)
		{
			_chars[*i - '0']->setX(x);
			_chars[*i - '0']->setY(0);
			_chars[*i - '0']->blit(this);
			x += _chars[*i - '0']->getWidth() + 1;
		}
	}
	else
	{
		for (std::string::iterator i = s.begin(); i != s.end(); ++i)
		{
			_borderedChars[*i - '0']->setX(x);
			_borderedChars[*i - '0']->setY(0);
			_borderedChars[*i - '0']->blit(this);
			x += _chars[*i - '0']->getWidth() + 1; // no this isn't a typo, i want to use the same spacing regardless.
		}
	}

	this->offset(_color);
}

void NumberText::setBordered(bool bordered)
{
	_bordered = bordered;
}

}
