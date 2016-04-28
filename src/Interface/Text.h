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
#include "../Engine/Surface.h"
#include <vector>
#include <string>
#include <stdint.h>

namespace OpenXcom
{

class Font;
class Language;

enum TextHAlign { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT };
enum TextVAlign { ALIGN_TOP, ALIGN_MIDDLE, ALIGN_BOTTOM };

/**
 * Text string displayed on screen.
 * Takes the characters from a Font and puts them together on screen
 * to display a string of text, taking care of any required aligning
 * or wrapping.
 */
class Text : public Surface
{
private:
	Font *_big, *_small, *_font;
	Language *_lang;
	std::wstring _text, _wrappedText;
	std::vector<int> _lineWidth, _lineHeight;
	bool _wrap, _invert, _contrast, _indent;
	TextHAlign _align;
	TextVAlign _valign;
	Uint8 _color, _color2;

	/// Processes the contained text.
	void processText();
	/// Gets the X position of a text line.
	int getLineX(int line) const;
public:
	/// Creates a new text with the specified size and position.
	Text(int width, int height, int x = 0, int y = 0);
	/// Cleans up the text.
	~Text();
	/// Formats an integer value as number with separators.
	static std::wstring formatNumber(int64_t value, const std::wstring &currency = L"");
	/// Formats an integer value as currency.
	static std::wstring formatFunding(int64_t funds);
	/// Formats an integer value as percentage.
	static std::wstring formatPercentage(int value);
	/// Sets the text size to big.
	void setBig();
	/// Sets the text size to small.
	void setSmall();
	/// Gets the text's current font.
	Font *getFont() const;
	/// Initializes the resources for the text.
	void initText(Font *big, Font *small, Language *lang);
	/// Sets the text's string.
	void setText(const std::wstring &text);
	/// Gets the text's string.
	std::wstring getText() const;
	/// Sets the text's wordwrap setting.
	void setWordWrap(bool wrap, bool indent = false);
	/// Sets the text's color invert setting.
	void setInvert(bool invert);
	/// Sets the text's high contrast color setting.
	void setHighContrast(bool contrast);
	/// Sets the text's horizontal alignment.
	void setAlign(TextHAlign align);
	/// Gets the text's horizontal alignment.
	TextHAlign getAlign() const;
	/// Sets the text's vertical alignment.
	void setVerticalAlign(TextVAlign valign);
	/// Gets the text's vertical alignment.
	TextVAlign getVerticalAlign() const;
	/// Sets the text's color.
	void setColor(Uint8 color);
	/// Gets the text's color.
	Uint8 getColor() const;
	/// Sets the text's secondary color.
	void setSecondaryColor(Uint8 color);
	/// Gets the text's secondary color.
	Uint8 getSecondaryColor() const;
	/// Gets the number of lines in the (wrapped, if wrapping is enabled) text
	int getNumLines() const;
	/// Gets the rendered text's width.
	int getTextWidth(int line = -1) const;
	/// Gets the rendered text's height.
	int getTextHeight(int line = -1) const;
	/// Draws the text.
	void draw();
};

}
