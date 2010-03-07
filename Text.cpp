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
	_text = text;
	refresh();
}

string Text::getText()
{
	return _text;
}

void Text::setWordWrap(bool wrap)
{
	_wrap = wrap;
	refresh();
}

void Text::setInvert(bool invert)
{
	_invert = invert;
	refresh();
}

void Text::setAlign(TextAlign align)
{
	_align = align;
	refresh();
}

void Text::setColor(Uint8 color)
{
	_color = color;
	refresh();
}

Uint8 Text::getColor()
{
	return _color;
}

void Text::blit(Surface *surface)
{
	Surface::blit(surface);
}

// Unlike other elements, text isn't redrawn every frame.
// This is because text rendering is very slow, so it's
// manually redrawn only when an attribute is changed.
void Text::refresh()
{
	if (_text.empty())
		return;

	clear();

	// For drawing each letter
	int x = 0;
	int y = 0;

	// For reading each line
	int w = 0, wf = 0;
	string::iterator start = _text.begin(), end = _text.end();
	bool blit = false, small = false;
	Font* font = _font;

	for (string::iterator c = _text.begin(); c <= _text.end(); c++)
	{
		// Check how many characters fit in a line
		if (!blit)
		{
			// Forced linebreak
			if (c == _text.end() || *c == '\n' || *c == 2)
			{
				blit = true;
				wf = w;
				if (c != _text.end() && *c == 2)
					small = true;
			}
			else 
			{
				// Lines end at blank spots
				if (*c == ' ')
				{
					end = c;
					wf = w;
					w += _font->getWidth() / 2;
				}
				// Keep on counting the width
				else
				{
					w += _font->getChar(*c)->getCrop()->w + _font->getSpacing();
				}
				// Reached the width limit
				if (_wrap && w > _width)
				{
					c = end;
					blit = true;
				}
			}
		}
		// Draw the line
		if (blit)
		{
			switch (_align)
			{
			case ALIGN_LEFT:
				x = 0;
				break;
			case ALIGN_CENTER:
				x = (_width - wf) / 2;
				break;
			case ALIGN_RIGHT:
				x = _width - wf;
			}
			
			// Character by character
			for (; start < c; start++)
			{
				if (*start == ' ')
				{
					x += _font->getWidth() / 2;
				}
				else
				{
					Surface* chr = _font->getChar(*start);
					chr->setX(x);
					chr->setY(y);
					chr->blit(this);
					x += chr->getCrop()->w + _font->getSpacing();
				}
			}
			if (start != _text.end())
				start++;

			// Go down a line
			y += _font->getHeight() + _font->getSpacing();
			x = 0;

			// Reset counters
			w = 0;
			blit = false;

			// Switch to small text
			if (small)
			{
				_font = _small;
				small = false;
			}
		}
		// Manual override because string iterators don't like going past the end
		if (c == _text.end())
			break;

	}
	_font = font;

	this->offset(_color);
	if (_invert)
		this->invert(_color+3);
}