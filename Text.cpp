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
#include "Text.h"

Text::Text(Font *big, Font *small, int width, int height, int x, int y) : Surface(width, height, x, y), _big(big), _small(small), _font(small), _text(""), _wrap(true), _align(ALIGN_LEFT), _invert(false)
{
	
}

Text::~Text()
{
	Surface::~Surface();
}

void Text::setBig()
{
	_font = _big;
}

void Text::setSmall()
{
	_font = _small;
}

void Text::setText(string text)
{
	if (_text == text)
		return;

	_text = text;
}

string Text::getText()
{
	return _text;
}

void Text::setWordWrap(bool wrap)
{
	_wrap = wrap;
}

void Text::setInvert(bool invert)
{
	_invert = invert;
}

void Text::setAlign(TextAlign align)
{
	_align = align;
}

void Text::setColor(Uint8 color)
{
	_color = color;
}

Uint8 Text::getColor()
{
	return _color;
}

void Text::blit(Surface *surface)
{
	if (_text.empty())
		return;

	Surface::clear();

	// For drawing each letter
	int x = 0;
	int y = 0;

	// For reading each line
	int w = 0;
	string buffer;
	bool newline = false, remword = false, end = false;
	Font* font = _font;
	
	for (string::iterator c = _text.begin(); !end; c++)
	{
		if (c != _text.end())
		{
			// If word doesn't fit in current line, go down
			if (_wrap && x + w > _width)
			{
				newline = true;
				remword = true;
			}
			if (*c == '\n' || *c == 2)
			{
				newline = true;
			}
		}
		// Reached end of string, draw the last line
		else
		{
			newline = true;
			end = true;
			c--;
		}
		// Add character to line buffer
		if (!newline)
		{
			buffer += *c;
			if (*c == ' ')
			{
				w += _font->getWidth() / 2;
			}
			else
			{
				w += _font->getChar(*c)->getCrop()->w + _font->getSpacing();
			}
		}
		else
		{
			// Remove last word (doesn't fit)
			if (remword)
			{
				buffer.erase(buffer.find_last_of(" "), buffer.length());
				while (*c != ' ')
				{
					Surface* chr = _font->getChar(*c);
					w -= chr->getCrop()->w + _font->getSpacing();
					c--;
				}
				remword = false;
			}

			// Draw the last line read
			switch (_align)
			{
			case ALIGN_LEFT:
				x = 0;
				break;
			case ALIGN_CENTER:
				x = (_width - w - 1) / 2;
				break;
			case ALIGN_RIGHT:
				x = _width - w - 1;
			}

			for (string::iterator l = buffer.begin(); l != buffer.end(); l++)
			{
				if (*l == ' ')
				{
					x += _font->getWidth() / 2;
				}
				else
				{
					Surface* chr = _font->getChar(*l);
					chr->setX(x);
					chr->setY(y);
					chr->blit(this);
					x += chr->getCrop()->w + _font->getSpacing();
				}
			}

			// Go down a line
			y += _font->getHeight();
			x = 0;

			// Reset buffer
			buffer.clear();
			w = 0;
			newline = false;
		}
		if (*c == 2)
		{
			_font = _small;
		}
	}
	_font = font;

	this->offset(_color);
	if (_invert)
		this->invert(_color+3);

	Surface::blit(surface);
}