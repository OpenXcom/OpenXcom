/*
 * Copyright 2010-2014 OpenXcom Developers.
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
	int _nonInlineThaiVowelCount;

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
	static std::wstring formatNumber(int value, std::wstring currency = L"");
	/// Formats an integer value as currency.
	static std::wstring formatFunding(int funds);
	/// Formats an integer value as percentage.
	static std::wstring formatPercentage(int value);
	/// Check if a character is upper Thai vowels.
	static inline bool isUpperThaiVowel(wchar_t c) { return (c == L'\u0E31' || c == L'\u0E34' || c == L'\u0E35' || c == L'\u0E36' || c == L'\u0E37' || c == L'\u0E47' || c == L'\u0E48' || c == L'\u0E49' || c == L'\u0E4A' || c == L'\u0E4B' || c == L'\u0E4C' || c == L'\u0E4D'); }
	/// Check if a character is lower Thai vowels.
	static inline bool isLowerThaiVowel(wchar_t c) { return (c == L'\u0E38' || c == L'\u0E39' || c == L'\u0E3A'); }	
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
	/// Gets non-inline Thai Vowel count
	int getNonInlineThaiVowelCount();	
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
	/// Sets the text's color.
	void setColor(Uint8 color);
	/// Gets the text's color.
	Uint8 getColor() const;
	/// Sets the text's secondary color.
	void setSecondaryColor(Uint8 color);
	/// Gets the text's secondary color.
	Uint8 getSecondaryColor() const;
	/// Gets the rendered text's width.
	int getTextWidth(int line = -1) const;
	/// Gets the rendered text's height.
	int getTextHeight(int line = -1) const;
	/// Draws the text.
	void draw();
};

}

#endif
