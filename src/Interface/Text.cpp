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
#include "Text.h"
#include <cmath>
#include <sstream>
#include <numeric>
#include "../Engine/Font.h"
#include "../Engine/Options.h"
#include "../Engine/Language.h"
#include "../Engine/ShaderDraw.h"
#include "../Engine/ShaderMove.h"

// -KM Debug -
 #include <iostream>

namespace OpenXcom
{

/**
 * Sets up a blank text with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Text::Text(int width, int height, int x, int y) : Surface(width, height, x, y), _big(0), _small(0), _font(0), _lang(0), _wrap(false), _invert(false), _contrast(false), _indent(false), _align(ALIGN_LEFT), _valign(ALIGN_TOP), _color(0), _color2(0)
{
}

/**
 *
 */
Text::~Text()
{

}

/**
 * Takes an integer value and formats it as number with separators (spacing the thousands).
 * @param value The value.
 * @param currency Currency symbol.
 * @return The formatted string.
 */
std::wstring Text::formatNumber(int64_t value, const std::wstring &currency)
{
	// In the future, the whole setlocale thing should be removed from here.
	// It is inconsistent with the in-game language selection: locale-specific
	// symbols, such as thousands separators, should be determined by the game
	// language, not by system locale.
	//setlocale(LC_MONETARY, ""); // see http://www.cplusplus.com/reference/clocale/localeconv/
	//setlocale(LC_CTYPE, ""); // this is necessary for mbstowcs to work correctly
	//struct lconv * lc = localeconv();
	std::wstring thousands_sep = std::wstring(1, TOK_NBSP);

	bool negative = (value < 0);
	std::wostringstream ss;
	ss << (negative? -value : value);
	std::wstring s = ss.str();
	size_t spacer = s.size() - 3;
	while (spacer > 0 && spacer < s.size())
	{
		s.insert(spacer, thousands_sep);
		spacer -= 3;
	}
	if (!currency.empty())
	{
		s.insert(0, currency);
	}
	if (negative)
	{
		s.insert(0, L"-");
	}
	return s;
}

/**
 * Takes an integer value and formats it as currency,
 * spacing the thousands and adding a $ sign to the front.
 * @param funds The funding value.
 * @return The formatted string.
 */
std::wstring Text::formatFunding(int64_t funds)
{
	return formatNumber(funds, L"$");
}

/**
 * Takes an integer value and formats it as percentage,
 * adding a % sign.
 * @param value The percentage value.
 * @return The formatted string.
 */
std::wstring Text::formatPercentage(int value)
{
	std::wostringstream ss;
	ss << value << "%";
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
 * Changes the various resources needed for text rendering.
 * The different fonts need to be passed in advance since the
 * text size can change mid-text, and the language affects
 * how the text is rendered.
 * @param big Pointer to large-size font.
 * @param small Pointer to small-size font.
 * @param lang Pointer to current language.
 */
void Text::initText(Font *big, Font *small, Language *lang)
{
	_big = big;
	_small = small;
	_lang = lang;
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
	if (_font == _big && (getTextWidth() > getWidth() || getTextHeight() > getHeight()) && _text[_text.size()-1] != L'.')
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
 * @param indent Indent wrapped text.
 */
void Text::setWordWrap(bool wrap, bool indent)
{
	if (wrap != _wrap || indent != _indent)
	{
		_wrap = wrap;
		_indent = indent;
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
 * Returns the way the text is aligned vertically
 * relative to the drawing area.
 * @return Horizontal alignment.
 */
TextVAlign Text::getVerticalAlign() const
{
	return _valign;
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
	_color2 = color;
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

int Text::getNumLines() const
{
	return _wrap ? _lineHeight.size() : 1;
}

/**
 * Returns the rendered text's height. Useful to check if wordwrap applies.
 * @param line Line to get the height, or -1 to get whole text height.
 * @return Height in pixels.
 */
int Text::getTextHeight(int line) const
{
	if (line == -1)
	{
		int height = 0;
		for (std::vector<int>::const_iterator i = _lineHeight.begin(); i != _lineHeight.end(); ++i)
		{
			height += *i;
		}
		return height;
	}
	else
	{
		return _lineHeight[line];
	}
}

/**
 * Returns the rendered text's width.
 * @param line Line to get the width, or -1 to get whole text width.
 * @return Width in pixels.
 */
int Text::getTextWidth(int line) const
{
	if (line == -1)
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
	else
	{
		return _lineWidth[line];
	}
}

/**
 * Performs minimum raggedness word wrap using the shortest path algorithm
 * from http://xxyxyz.org/line-breaking/
 */

std::vector<size_t> Text::calcEvenWordWrap(
	const std::vector<int> & wordWidths, const std::vector<int> & spaceWidths, 
	int indentation, int width) const 
{
	size_t numWords = wordWidths.size(), i, j;

	std::vector<int> minCostSoFar(numWords+1, 1<<30);
	std::vector<int> breaks(numWords+1, 0);
	std::vector<int> cumulativeLength(1, 0), cumulativeSpaces(1, 0);

	// We want to know "length of a line starting after the kth word and 
	// ending after the nth" in constant time.

	// The length of a line starting at the beginning and ending after 
	// the kth is cumulative_length[k] + cumulative_spaces[k-1], 
	// where cumulative_length[k] is the length of a line ending after the kth 
	// word, spaces notwithstanding, and cumulative_spaces[k] is the combined 
	// length of all spaces up to and including the kth word, including the
	// spaces just after that word.

	// A line starting before kth and ending before nth with no leading or 
	// trailing spaces must thus be (length from 0th to nth) - 
	// (length from 0th to kth + space after (k-1)th word)
	// i.e.
	// cumulative_length[n] + cumulative_spaces[n-1] - cumulative_length[k] -
	//		cumulative_spaces[k]

	for (i = 0; i < numWords; ++i) 
	{
		cumulativeLength.push_back(cumulativeLength[i] + wordWidths[i]);
		cumulativeSpaces.push_back(cumulativeSpaces[i] + spaceWidths[i]);
	}

	// Now the idea is to minimize raggedness, here measured by the squared 
	// distance from the end of each line to the end of the text box. 

	minCostSoFar[0] = 0; // If we've placed no lines, the cost is zero.
	bool exceededMaxWidth = false;

	for (i = 0; i < numWords; ++i) 
	{
		exceededMaxWidth = false;
		for (j = i+1; j <= numWords && !exceededMaxWidth; ++j) 
		{
			// If we can fit all words from the ith to the jth on a line...
			int lineWidth = indentation + cumulativeLength[j] + 
				cumulativeSpaces[j-1] - (cumulativeLength[i] + 
					cumulativeSpaces[i]);

			// ... and if the cost of doing so, plus the best cost of 
			// handling word number 0..i on prior lines, is less than
			// the current record cost for handling words 0..j, update the
			// record (as we've found a better split: new line just after the
			// ith word).
			int cost = minCostSoFar[i] + pow(width-lineWidth, 2);

			// We're permitted to exceed the line width, but only once, and
			// at a very stiff penalty. The point here is to be able to handle
			// sentences with words longer than the width.
			if (lineWidth >= width)
			{
				exceededMaxWidth = true;
				cost += pow(width+1, 2);
			}

			if (cost < minCostSoFar[j]) 
			{
				minCostSoFar[j] = cost;

				// Record that the best way seen so far of wrapping the words
				// up to and including the jth involves breaking the next to 
				// last line after the ith word.
				breaks[j] = i; 
			}
		}
	}

	// Now go through the record break points. (We know that the last line 
	// ends after the final word; then use breaks[] to find out where the next
	// to last line ends and so on down to the first line.)

	std::vector<size_t> breakPoints;

	j = numWords;
	breakPoints.push_back(numWords);
	while (j > 0) 
	{
		breakPoints.push_back(breaks[j]);
		j = breaks[j];
	}

	reverse(breakPoints.begin(), breakPoints.end());
	return breakPoints;
}

/**
 * Truncate a line or string that's too long.
 * @param outputWidth is set by to the width of the returned string.
 */

std::wstring Text::getTruncatedLine(std::wstring::const_iterator 
	strBegin, std::wstring::const_iterator strEnd, Font * font, int maxWidth,
	int & outputWidth)
{
	// The truncation algorithm works like this:
	// First find a position to truncate the string plus a number of dots
	// to add to the end so that the combined width is as close to the
	// max width as possible (to avoid very ragged right points).
	// If the full string ends up longer than the max width, return the 
	// truncated string, otherwise return the full string.

	wchar_t truncateSymbol = '.'; 
	int tSymbolWidth = font->getCharSize(truncateSymbol).w;
	int minSymbols = 2, maxSymbols = 5; 

	int strWidth = 0;
	int recordDistFromEnd = maxWidth, recordSymbols;
	std::wstring::const_iterator firstAfterTrunc = strEnd, pos;

	// TODO: Fix raggedness that has now been revealed.
	for (pos = strBegin; pos != strEnd; ++pos)
	{
		if (strWidth <= maxWidth)
		{
			// Solve strWidth + x * tSymbolWidth + 1 = maxWidth, then round x 
			// down and clamp to no less than min, no more than max.
			// x = (maxWidth - strWidth - 1) / tSymbolWidth
			// The -1 is because we need an extra pixel to end the last dot,
			// so 5 dots (say) take tSymbolWidth * 5 + 1.
			int numSymbolsFit = std::min(maxSymbols, std::max(minSymbols,
				(maxWidth - strWidth-1)/tSymbolWidth));
			int totalWidth = strWidth + numSymbolsFit * tSymbolWidth + 1;
			int distanceFromEnd = maxWidth - totalWidth;

			// If we fill the line better than the current record, replace
			// it (but don't truncate just after spaces; looks weird).
			if (distanceFromEnd >= 0 && distanceFromEnd <= recordDistFromEnd
				&& (pos == strBegin || !Font::isSpace(*(pos-1))))
			{
				recordDistFromEnd = distanceFromEnd;
				firstAfterTrunc = pos;
				recordSymbols = numSymbolsFit;
			}
		}

		if (Font::isLinebreak(*pos) || *pos != TOK_FLIP_COLORS)
			strWidth += font->getCharSize(*pos).w;
	}
	// If the string is too long, truncate and return, otherwise just return
	// the whole thing.
	if (strWidth > maxWidth)
	{
		std::wstring truncation = std::wstring(recordSymbols, truncateSymbol);
		// If the string ends in a linebreak, we need to preserve it.
		if (Font::isLinebreak(*(strEnd-1)))
			return std::wstring(strBegin, firstAfterTrunc) + truncation +
				*(strEnd-1);
		return std::wstring(strBegin, firstAfterTrunc) + truncation;
	}
	
	return std::wstring(strBegin, strEnd);
}

/**
 * Handles alignment and word wrap for a single line of input text.
 */

std::wstring Text::processLine(std::wstring::const_iterator strBegin, 
	std::wstring::const_iterator strEnd, Font * font, int & indentation) 
{
	// Could really use tests with nonbreaking spaces etc...
	size_t i, j;

	int lineHeight = font->getCharSize(L'\n').h;
	int lineWidth = 0;

	std::vector<std::wstring::const_iterator> wordBeginnings,
		spaceBeginnings;
	std::vector<std::wstring> spaces;
	std::wstring::const_iterator pos;

	// Get line width.
	for (pos = strBegin; pos != strEnd; ++pos) 
	{
		if (!Font::isLinebreak(*pos) && *pos != TOK_FLIP_COLORS) 
			lineWidth += font->getCharSize(*pos).w;
	}

	// If no word wrap, just set the dimensions and return the string.
	if (!_wrap) 
	{
		std::wstring outString = getTruncatedLine(strBegin, strEnd, font, 
			getWidth(), lineWidth);
		_lineWidth.push_back(lineWidth);
		_lineHeight.push_back(lineHeight);
		return outString;
	}

	// If we're told to do indentation, count the number of leading spaces
	// and add them to the indentation int while advancing the begin 
	// iterator. (We'll add all the spaces back at the end, which is why
	// we want to advance the begin iterator, so none count twice.)
	if (_indent) 
	{
		while(strBegin != strEnd && Font::isBrkSpace(*strBegin))
		{
			++strBegin;
			++indentation;
		}
	}

	int indentationWidth = indentation * font->getCharSize(TOK_NBSP).w;

	// Split into words and get the word and space widths.
	// wordBeginnings points to the beginning of each word, while
	// spaceBeginning points to the beginning of the preceding space/s.
	// Because of corner cases like " - ", this is fairly complex.
	bool spaceRun = false;
	
	wordBeginnings.push_back(strBegin);
	spaceBeginnings.push_back(strBegin);

	for (pos = strBegin; pos != strEnd; ++pos) 
	{
		if (!Font::isBrkSpace(*pos) && spaceRun) 
		{
			spaceRun = false;
			wordBeginnings.push_back(pos);
		}
		// Set the beginning of next word to whatever came after the
		// separator -- unless there's a space next, in which case we don't
		// break at that point, because it may cause a leading space on the
		// next line.

		// If the text wrapping method is WRAP_LETTERS, then every non-space 
		// letter works like a separator.
		bool sepOrWrapLetters = Font::isSeparator(*pos) || 
			(_lang->getTextWrapping() == WRAP_LETTERS && !Font::isBrkSpace(*pos));
		
		if (!spaceRun && sepOrWrapLetters && !Font::isBrkSpace(*(pos+1)))
		{
			wordBeginnings.push_back(pos+1);
			spaceBeginnings.push_back(pos+1);
		}

		if (!spaceRun && Font::isBrkSpace(*pos)) 
		{
			spaceBeginnings.push_back(pos);
			spaceRun = true;
		}
	}	
	wordBeginnings.push_back(strEnd);
	spaceBeginnings.push_back(strEnd);

	std::vector<int> wordWidths(wordBeginnings.size()-1, 0);
	std::vector<int> spaceWidths(wordBeginnings.size()-1, 0);

	for (i = 0; i < wordBeginnings.size()-1; ++i) 
	{
		// Count the width of each word and space.
		// The word in question
		for (pos = wordBeginnings[i]; pos < spaceBeginnings[i+1]; ++pos) 
		{
			if (!Font::isLinebreak(*pos) && *pos != TOK_FLIP_COLORS) 
				wordWidths[i] += font->getCharSize(*pos).w;
		}
		// The succeeding run of spaces
		for (pos = spaceBeginnings[i+1]; pos < wordBeginnings[i+1]; ++pos) 
			spaceWidths[i] += font->getCharSize(*pos).w;
	}
	
	std::vector<size_t> breakPoints = calcEvenWordWrap(wordWidths, 
		spaceWidths, indentation, getWidth());

	std::wstring wrappedLines;

	for (i = 1; i < breakPoints.size(); ++i) 
	{
		size_t firstWord = breakPoints[i-1], lastWord = breakPoints[i];
		int lineWidth = 0;

		for (j = firstWord; j < lastWord; ++j)
			lineWidth += wordWidths[j] + spaceWidths[j];

		lineWidth -= spaceWidths[lastWord-1];
		lineWidth += indentationWidth;

		wrappedLines += std::wstring(indentation, TOK_NBSP) + std::wstring(
			wordBeginnings[firstWord], spaceBeginnings[lastWord]);

		_lineWidth.push_back(lineWidth);
		_lineHeight.push_back(lineHeight);
		if (i < breakPoints.size()-1)
		{
			wrappedLines += L"\n";	 
		}
	}

	return wrappedLines;
}

/**
 * Takes care of any text post-processing like calculating
 * line metrics for alignment and wordwrapping if necessary.
 */

  // RE specification: If a line starts with a space, then all lines after
 // that one should be indented to match that space.
 // _lineWidth and _lineHeight needs to contain the width and height of 
 // every line, but to avoid varying line heights, the height is always set
 // to the height of the newline char.

void Text::processText()
{
	if (_font == 0 || _lang == 0)
	{
		return;
	}

	std::wstring *str = &_text;

	_lineWidth.clear();
	_lineHeight.clear();
	_wrappedText.clear();

	Font *font = _font;

	size_t nextLineStart = 0, lineStart = 0;
	int indentation = 0;		// number of spaces to indent each line.

  	while (nextLineStart < str->size())
  	{
  		lineStart = nextLineStart;
  		// Find the next linebreak (end of line), or end of string.
  		nextLineStart = str->find_first_of(Font::getLinebreaks(), lineStart);

    	// Turn end of line into beginning of next by adding one.
    	// Don't add one if we're at the end of the string, since there's
    	// nothing left to get there.
  		if (nextLineStart == std::wstring::npos)
  			nextLineStart = str->size();
  		else
  			nextLineStart ++;

  		// Check if the linebreak just before the start of our line
  		// is a small line marker. If we're on the first line, we can't
  		// go past the beginning, so don't try to do so.
  		if ((*str)[lineStart == 0 ? 0 : lineStart-1] == TOK_BREAK_SMALLLINE)
  			font = _small;

    	_wrappedText += processLine(str->begin()+lineStart, 
    		str->begin()+nextLineStart, font, indentation);
	}

	_redraw = true;
	if (!_wrap)
		_text = _wrappedText;
}

/**
 * Calculates the starting X position for a line of text.
 * @param line The line number (0 = first, etc).
 * @return The X position in pixels.
 */
int Text::getLineX(int line) const
{
	int x = 0;
	switch (_lang->getTextDirection())
	{
	case DIRECTION_LTR:
		switch (_align)
		{
		case ALIGN_LEFT:
			break;
		case ALIGN_CENTER:
			x = (int)ceil((getWidth() + _font->getSpacing() - _lineWidth[line]) / 2.0);
			break;
		case ALIGN_RIGHT:
			x = getWidth() - 1 - _lineWidth[line];
			break;
		}
		break;
	case DIRECTION_RTL:
		switch (_align)
		{
		case ALIGN_LEFT:
			x = getWidth() - 1;
			break;
		case ALIGN_CENTER:
			x = getWidth() - (int)ceil((getWidth() + _font->getSpacing() - _lineWidth[line]) / 2.0);
			break;
		case ALIGN_RIGHT:
			x = _lineWidth[line];
			break;
		}
		break;
	}
	return x;
}

namespace
{

struct PaletteShift
{
	static inline void func(Uint8& dest, Uint8& src, int off, int mul, int mid)
	{
		if (src)
		{
			int inverseOffset = mid ? 2 * (mid - src) : 0;
			dest = off + src * mul + inverseOffset;
		}
	}
};

} //namespace

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
	if (Options::debugUi)
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
		y = (int)ceil((getHeight() - height) / 2.0);
		break;
	case ALIGN_BOTTOM:
		y = getHeight() - height;
		break;
	}

	x = getLineX(line);

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

	// Set up text direction
	int dir = 1;
	if (_lang->getTextDirection() == DIRECTION_RTL)
	{
		dir = -1;
	}

	// Invert text by inverting the font palette on index 3 (font palettes use indices 1-5)
	int mid = _invert ? 3 : 0;

	// Draw each letter one by one
	for (std::wstring::iterator c = s->begin(); c != s->end(); ++c)
	{
		if (Font::isSpace(*c))
		{
			x += dir * font->getCharSize(*c).w;
		}
		else if (Font::isLinebreak(*c))
		{
			line++;
			y += font->getCharSize(*c).h;
			x = getLineX(line);
			if (*c == TOK_BREAK_SMALLLINE)
			{
				font = _small;
			}
		}
		else if (*c == TOK_FLIP_COLORS)
		{
			color = (color == _color ? _color2 : _color);
		}
		else
		{
			if (dir < 0)
				x += dir * font->getCharSize(*c).w;
			Surface* chr = font->getChar(*c);
			if (chr == 0)
				chr = font->getChar(L'?');

			chr->setX(x);
			chr->setY(y);
			ShaderDraw<PaletteShift>(ShaderSurface(this, 0, 0), ShaderCrop(chr), ShaderScalar(color), ShaderScalar(mul), ShaderScalar(mid));
			if (dir > 0)
				x += dir * font->getCharSize(*c).w;
		}
	}
}

}