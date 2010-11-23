/*
 * Copyright 2010 OpenXcom Developers.
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
	std::string _text, _wrappedText;
	std::vector<int> _lineWidth, _lineHeight;
	bool _wrap, _invert;
	TextHAlign _align;
	TextVAlign _valign;
	Uint8 _color;

	/// Processes the contained text.
	void processText();
public:
	/// Creates a new text with the specified size and position.
	Text(int width, int height, int x = 0, int y = 0);
	/// Cleans up the text.
	~Text();
	/// Formats an integer value as currency.
	static std::string formatFunding(int funds);
	/// Sets the text size to big.
	void setBig();
	/// Sets the text size to small.
	void setSmall();
	/// Gets the text's current font.
	Font *const getFont() const;
	/// Sets the text's various fonts.
	void setFonts(Font *big, Font *small);
	/// Sets the text's string.
	void setText(const std::string &text);
	/// Gets the text's string.
	std::string getText() const;
	/// Sets the text's wordwrap setting.
	void setWordWrap(bool wrap);
	/// Sets the text's color invert setting.
	void setInvert(bool invert);
	/// Sets the text's horizontal alignment.
	void setAlign(TextHAlign align);
	/// Sets the text's vertical alignment.
	void setVerticalAlign(TextVAlign valign);
	/// Sets the text's color.
	void setColor(Uint8 color);
	/// Gets the text's color.
	Uint8 getColor() const;
	/// Draws the text.
	void draw();
};

#endif
