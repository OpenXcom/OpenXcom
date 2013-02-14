/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "../Engine/Font.h"
#include "../Engine/Options.h"
#include "../Engine/Language.h"

namespace OpenXcom
{

/**
 * Sets up a blank text with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Text::Text(int width, int height, int x, int y) : Surface(width, height, x, y), _big(0), _small(0), _font(0), _text(L""), _wrap(false), _invert(false), _contrast(false), _align(ALIGN_LEFT), _valign(ALIGN_TOP), _color(0), _color2(0)
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
std::wstring Text::formatFunding(int funds)
{
	setlocale (LC_MONETARY,""); // see http://www.cplusplus.com/reference/clocale/localeconv/
	struct lconv * lc;
	lc=localeconv();
	std::wstring thousands_sep = Language::cpToWstr(lc->mon_thousands_sep);

	bool negative = false;
	if (funds < 0)
	{
		negative = true;
		funds = -funds;
	}
	std::wstringstream ss;
	ss << funds;
	std::wstring s = ss.str();
	size_t spacer = s.size() - 3;
	while (spacer > 0 && spacer < s.size())
	{
		s.insert(spacer, thousands_sep);
		spacer -= 3;
	}
	s.insert(0, L"$");
	if (negative)
		s.insert(0, L"-");
	return s;
}

/**
 * Takes an integer value and formats it as percentage,
 * adding a % sign.
 * @param value The percentage value.
 * @return The formatted string.
 */
std::wstring Text::formatPercentage(int value)
{
	std::wstringstream ss;
	ss << value;
	ss << "%";
	return ss.str();
}

/**
 * Changes the text to use the big-size font.
 */
void Text::setBig()
{
	_font = _big;
	processText();
}

/**
 * Changes the text to use the small-size font.
 */
void Text::setSmall()
{
	_font = _small;
	processText();
}

/**
 * Returns the font currently used by the text.
 * @return Pointer to font.
 */
Font *Text::getFont() const
{
	return _font;
}

/**
 * Changes the various fonts for the text to use.
 * The different fonts need to be passed in advance since the
 * text size can change mid-text.
 * @param big Pointer to large-size font.
 * @param small Pointer to small-size font.
 */
void Text::setFonts(Font *big, Font *small)
{
	_big = big;
	_small = small;
	_font = _small;
	processText();
}

/**
 * Changes the string displayed on screen.
 * @param text Text string.
 */
void Text::setText(const std::wstring &text)
{
	_text = text;
	processText();
	// If big text won't fit the space, try small text
	if (_font == _big && getTextWidth() > getWidth() && _text[_text.size()-1] != L'.')
	{
		setSmall();
	}
}

/**
 * Returns the string displayed on screen.
 * @return Text string.
 */
std::wstring Text::getText() const
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
	if (wrap != _wrap)
	{
		_wrap = wrap;
		processText();
	}
}

/**
 * Enables/disables color inverting. Mostly used to make
 * button text look pressed along with the button.
 * @param invert Invert setting.
 */
void Text::setInvert(bool invert)
{
	_invert = invert;
	_redraw = true;
}

/**
 * Enables/disables high contrast color. Mostly used for
 * Battlescape UI.
 * @param contrast High contrast setting.
 */
void Text::setHighContrast(bool contrast)
{
	_contrast = contrast;
	_redraw = true;
}

/**
 * Changes the way the text is aligned horizontally
 * relative to the drawing area.
 * @param align Horizontal alignment.
 */
void Text::setAlign(TextHAlign align)
{
	_align = align;
	_redraw = true;
}

/**
 * Returns the way the text is aligned horizontally
 * relative to the drawing area.
 * @return Horizontal alignment.
 */
TextHAlign Text::getAlign() const
{
	return _align;
}

/**
 * Changes the way the text is aligned vertically
 * relative to the drawing area.
 * @param valign Vertical alignment.
 */
void Text::setVerticalAlign(TextVAlign valign)
{
	_valign = valign;
	_redraw = true;
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
	_redraw = true;
}

/**
 * Returns the color used to render the text.
 * @return Color value.
 */
Uint8 Text::getColor() const
{
	return _color;
}

/**
 * Changes the secondary color used to render the text. The text
 * switches between the primary and secondary color whenever there's
 * a 0x01 in the string.
 * @param color Color value.
 */
void Text::setSecondaryColor(Uint8 color)
{
	_color2 = color;
	_redraw = true;
}

/**
 * Returns the secondary color used to render the text.
 * @return Color value.
 */
Uint8 Text::getSecondaryColor() const
{
	return _color2;
}

/**
 * Returns the rendered text's height. Useful to check if wordwrap applies.
 * @return Height in pixels.
 */
int Text::getTextHeight() const
{
	int height = 0;
	for (std::vector<int>::const_iterator i = _lineHeight.begin(); i != _lineHeight.end(); ++i)
	{
		height += *i;
	}
	return height;
}

/**
  * Returns the rendered text's width.
  * @return Width in pixels.
  */
int Text::getTextWidth() const
{
	int width = 0;
	for (std::vector<int>::const_iterator i = _lineWidth.begin(); i != _lineWidth.end(); ++i)
	{
		if (*i > width)
		{
			width = *i;
		}
	}
	return width;
}

/**
 * Takes care of any text post-processing like calculating
 * line metrics for alignment and wordwrapping if necessary.
 */
void Text::processText()
{
	if (_font == 0)
	{
		return;
	}

	std::wstring *s = &_text;

	// Use a separate string for wordwrapping text
	if (_wrap)
	{
		_wrappedText = _text;
		s = &_wrappedText;
	}

	_lineWidth.clear();
	_lineHeight.clear();

	int width = 0, word = 0;
	std::wstring::iterator space = s->begin();
	bool start = true;
	Font *font = _font;

	// Go through the text character by character
	for (std::wstring::iterator c = s->begin(); c <= s->end(); ++c)
	{
		// End of the line
		if (c == s->end() || *c == L'\n' || *c == 2)
		{
			// Add line measurements for alignment later
			_lineWidth.push_back(width);
			_lineHeight.push_back(font->getHeight() + font->getSpacing());
			width = 0;
			word = 0;
			start = true;

			if (c == s->end())
				break;
			// \x02 marks start of small text
			else if (*c == 2)
				font = _small;
		}
		// Keep track of spaces for wordwrapping
		else if (*c == L' ')
		{
			space = c;
			width += font->getWidth() / 2;
			word = 0;
			start = false;
		}
		// Keep track of the width of the last line and word
		else if (*c != 1)
		{
			width += font->getChar(*c)->getCrop()->w + font->getSpacing();
			word += font->getChar(*c)->getCrop()->w + font->getSpacing();

			// Wordwrap if the last word doesn't fit the line
			if (_wrap && width > getWidth() && !start)
			{
				// Go back to the last space and put a linebreak there
				*space = L'\n';
				width -= word + font->getWidth() / 2;
				_lineWidth.push_back(width);
				_lineHeight.push_back(font->getHeight() + font->getSpacing());
				width = word;
				start = true;
			}
		}
	}

	_redraw = true;
}

/**
 * Draws all the characters in the text with a really
 * nasty complex gritty text rendering algorithm logic stuff.
 */
void Text::draw()
{
	Surface::draw();
	if (_text.empty() || _font == 0)
	{
		return;
	}

	// Show text borders for debugging
	if (Options::getBool("debugUi"))
	{
		SDL_Rect r;
		r.w = getWidth();
		r.h = getHeight();
		r.x = 0;
		r.y = 0;
		this->drawRect(&r, 5);
		r.w-=2;
		r.h-=2;
		r.x++;
		r.y++;
		this->drawRect(&r, 0);
	}

	int x = 0, y = 0, line = 0, height = 0;
	Font *font = _font;
	int color = _color;
	std::wstring *s = &_text;

	for (std::vector<int>::iterator i = _lineHeight.begin(); i != _lineHeight.end(); ++i)
	{
		height += *i;
	}

	switch (_valign)
	{
	case ALIGN_TOP:
		y = 0;
		break;
	case ALIGN_MIDDLE:
		y = (getHeight() - height) / 2;
		break;
	case ALIGN_BOTTOM:
		y = getHeight() - height;
		break;
	}

	switch (_align)
	{
	case ALIGN_LEFT:
		x = 0;
		break;
	case ALIGN_CENTER:
		x = (getWidth() - _lineWidth[line]) / 2;
		break;
	case ALIGN_RIGHT:
		x = (getWidth() - _lineWidth[line]) - 1;
		break;
	}

	if (_wrap)
	{
		s = &_wrappedText;
	}

	// Set up text color
	int mul = 1;
	if (_contrast)
	{
		mul = 3;
	}

	// Invert text by inverting the font palette on index 3 (font palettes use indices 1-5)
	int mid = _invert ? 3 : 0;

	font->getSurface()->paletteShift(color, mul, mid);

	// Draw each letter one by one
	for (std::wstring::iterator c = s->begin(); c != s->end(); ++c)
	{
		if (*c == ' ')
		{
			x += font->getWidth() / 2;
		}
		else if (*c == '\n' || *c == 2)
		{
			line++;
			y += font->getHeight() + font->getSpacing();
			switch (_align)
			{
			case ALIGN_LEFT:
				x = 0;
				break;
			case ALIGN_CENTER:
				x = (getWidth() - _lineWidth[line]) / 2;
				break;
			case ALIGN_RIGHT:
				x = getWidth() - _lineWidth[line];
				break;
			}
			if (*c == 2)
			{
				font->getSurface()->paletteRestore();
				font = _small;
				font->getSurface()->paletteShift(color, mul, mid);
			}
		}
		else if (*c == 1)
		{
			font->getSurface()->paletteRestore();
			color = (color == _color ? _color2 : _color);
			font->getSurface()->paletteShift(color, mul, mid);
		}
		else
		{
			Surface* chr = font->getChar(*c);
			chr->setX(x);
			chr->setY(y);
			chr->blit(this);
			x += chr->getCrop()->w + font->getSpacing();
		}
	}

	// Revert text color
	font->getSurface()->paletteRestore();
}

}
