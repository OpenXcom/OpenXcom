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
 *e
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <stdint.h>
#include <SDL.h>

namespace OpenXcom
{

/**
 * Represents a 32-bit Unicode codepoint.
 * Not technically a character, but close enough for our purposes.
 */
typedef Uint32 UCode;
/**
 * Represents a string where each character
 * is a Unicode codepoint. Used for rendering text.
 */
typedef std::basic_string<UCode> UString;

/**
 * Functions for dealing with encoding, strings, text
 * and all related operations.
 */
namespace Unicode
{
	/* Special text tokens */
	const char TOK_NL_SMALL = 2;			/// line break and change to small font
	const char TOK_COLOR_FLIP = 1;			/// alternate between primary and secondary color
	const unsigned char TOK_NBSP = 0xA0;	/// non-breaking space

	/// Checks if a character is a linebreak.
	inline bool isLinebreak(UCode c) { return (c == '\n' || c == TOK_NL_SMALL); }
	/// Checks if a character is a blank space (includes non-breaking spaces).
	inline bool isSpace(UCode c) { return (c == ' ' || c == TOK_NBSP); }
	/// Checks if a character is a word separator.
	inline bool isSeparator(UCode c) { return (c == '-' || c == '/'); }
	/// Checks if a character is visible to the user.
	inline bool isPrintable(UCode c) { return (c > 32 && c != TOK_NBSP); }

	/// Find a UTF-8 locale.
	void getUtf8Locale();
	/// Converts a UTF-8 string to UTF-32.
	UString convUtf8ToUtf32(const std::string &src);
	/// Converts a UTF-32 string to UTF-8.
	std::string convUtf32ToUtf8(const UString &src);
	/// Converts a path to a UTF-8 string.
	std::string convPathToUtf8(const std::string &src);
	/// Converts a UTF-8 string to a path.
	std::string convUtf8ToPath(const std::string &src);

	/// Compares two strings by natural order.
	bool naturalCompare(const std::string &a, const std::string &b);
	/// Compares two strings case-insensitive.
	bool caseCompare(const std::string &a, const std::string &b);
	/// Find a string in another case-insensitive.
	bool caseFind(const std::string &haystack, const std::string &needle);
	/// Uppercases a string in place.
	void upperCase(std::string &s);
	/// Lowercases a string in place.
	void lowerCase(std::string &s);
	/// Replaces a substring.
	void replace(std::string &str, const std::string &find, const std::string &replace);
	/// Formats an integer value as number with separators.
	std::string formatNumber(int64_t value, const std::string &currency = "");
	/// Formats an integer value as currency.
	std::string formatFunding(int64_t funds);
	/// Formats an integer value as percentage.
	std::string formatPercentage(int value);
}

}
