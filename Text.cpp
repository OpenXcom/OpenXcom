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
#include <sstream>
#include "Font.h"

using namespace std;

/**
 * Sets up a blank text with the specified size and position.
 * The different fonts need to be passed in advance since the
 * text size can change mid-text.
 * @param big Pointer to the big-size font.
 * @param small Pointer to the small-size font.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Text::Text(Font *big, Font *small, int width, int height, int x, int y) : Surface(width, height, x, y), _big(big), _small(small), _font(small), _text(""), _wrap(false), _invert(false), _align(ALIGN_LEFT), _color(0)
{

}

/**
 *
 */
Text::~Text()
{
	
}

/**
 * Takes an integer value and formats it as currency,
 * spacing the thousands and adding a $ sign to the front.
 * @param funds The funding value.
 * @return The formatted string.
 */
string Text::formatFunding(int funds)
{
	stringstream ss;
	ss << funds;
	string s = ss.str();
	size_t spacer = s.size() - 3;
	while (spacer > 0 && spacer < s.size())
	{
		s.insert(spacer, " ");
		spacer -= 3;
	}
	s.insert(0, "$");
	return s;
}

/**
 * Changes the text to use the big-size font.
 */
void Text::setBig()
{
	_font = _big;
}

/**
 * Changes the text to use the small-size font.
 */
void Text::setSmall()
{
	_font = _small;
}

/**
 * Returns the font currently used by the text.
 * @return Pointer to font.
 */
Font *Text::getFont()
{
	return _font;
}

/**
 * Changes the string displayed on screen.
 * @param text Text string.
 */
void Text::setText(string text)
{
	if (text != _text)
	{
		_text = text;
		draw();
	}
}

/**
 * Returns the string displayed on screen.
 * @return Text string.
 */
string Text::getText()
{
	return _text;
}

/**
 * Enables/disables text wordwrapping. When enabled, lines of
 * text are automatically split to ensure they stay within the
 * drawing area, otherwise they simply go off the edge.
 * @param wrap Wordwrapping setting.
 */
void Text::setWordWrap(bool wrap)
{
	_wrap = wrap;
	draw();
}

/**
 * Enables/disables color inverting. Mostly used to make
 * button text look pressed along with the button.
 * @param invert Invert setting.
 */
void Text::setInvert(bool invert)
{
	_invert = invert;
	draw();
}

/**
 * Changes the way the text is aligned relative to the
 * drawing area.
 * @param align Text alignment.
 */
void Text::setAlign(TextAlign align)
{
	_align = align;
	draw();
}

/**
 * Changes the color used to render the text. Unlike regular graphics,
 * fonts are greyscale so they need to be assigned a specific position
 * in the palette to be displayed.
 * @param color Color value.
 */
void Text::setColor(Uint8 color)
{
	_color = color;
	draw();
}

/**
 * Returns the color used to render the text.
 * @return Color value.
 */
Uint8 Text::getColor()
{
	return _color;
}

/**
 * Draws all the characters in the text with a really
 * nasty complex gritty text rendering algorithm logic stuff.
 */
void Text::draw()
{
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
