/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#ifndef OPENXCOM_TEXT_H
#define OPENXCOM_TEXT_H

#include "../Engine/Surface.h"
#include <vector>
#include <string>

namespace OpenXcom
{

class Font;

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
	std::wstring _text, _wrappedText;
	std::vector<int> _lineWidth, _lineHeight;
	bool _wrap, _invert, _contrast;
	TextHAlign _align;
	TextVAlign _valign;
	Uint8 _color, _color2;

	/// Processes the contained text.
	void processText();
public:
	/// Creates a new text with the specified size and position.
	Text(int width, int height, int x = 0, int y = 0);
	/// Cleans up the text.
	~Text();
	/// Formats an integer value as number with separators.
	static std::wstring formatNumber(int value, std::wstring currency = L"");
	/// Formats an integer value as currency.
	static std::wstring formatFunding(int funds);
	/// Formats an integer value as percentage.
	static std::wstring formatPercentage(int value);
	/// Sets the text size to big.
	void setBig();
	/// Sets the text size to small.
	void setSmall();
	/// Gets the text's current font.
	Font *getFont() const;
	/// Sets the text's various fonts.
	void setFonts(Font *big, Font *small);
	/// Sets the text's string.
	void setText(const std::wstring &text);
	/// Gets the text's string.
	std::wstring getText() const;
	/// Sets the text's wordwrap setting.
	void setWordWrap(bool wrap);
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
	/// Sets the text's color.
	void setColor(Uint8 color);
	/// Gets the text's color.
	Uint8 getColor() const;
	/// Sets the text's secondary color.
	void setSecondaryColor(Uint8 color);
	/// Gets the text's secondary color.
	Uint8 getSecondaryColor() const;
	/// Gets the rendered text's width.
	int getTextWidth() const;
	/// Gets the rendered text's height.
	int getTextHeight() const;
	/// Draws the text.
	void draw();
};

}

#endif
