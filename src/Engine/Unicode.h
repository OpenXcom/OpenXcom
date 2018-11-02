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
	const char TOK_BREAK_SMALLLINE = 2;		/// line break and change to small font
	const char TOK_FLIP_COLORS = 1;			/// flip color scheme
	const char TOK_NBSP = 0xA0;				/// non-breaking space

	/// Checks if a character is a linebreak.
	inline bool isLinebreak(UCode c) { return (c == '\n' || c == TOK_BREAK_SMALLLINE); }
	/// Checks if a character is a blank space (includes non-breaking spaces).
	inline bool isSpace(UCode c) { return (c == ' ' || c == TOK_NBSP); }
	/// Checks if a character is a word separator.
	inline bool isSeparator(UCode c) { return (c == '-' || c == '/'); }
	/// Checks if a character is visible to the user.
	inline bool isPrintable(UCode c) { return (c > 32 && c != TOK_NBSP); }
	
	/// Converts a wide-string to UTF-8.
	std::string wstrToUtf8(const std::wstring &src);
	/// Converts a wide-string to local-codepage string.
	std::string wstrToCp(const std::wstring &src);
	/// Converts a wide-string to filesystem string.
	std::string wstrToFs(const std::wstring &src);
	/// Converts a UTF-8 string to wide-string.
	std::wstring utf8ToWstr(const std::string &src);
	/// Converts a local-codepage string to wide-string.
	std::wstring cpToWstr(const std::string &src);
	/// Converts a filesystem string to wide-string.
	std::wstring fsToWstr(const std::string &src);
	/// Converts a filesystem string to UTF-8.
	std::string fsToUtf8(const std::string &src);
	/// Converts a UTF-8 string to filesystem.
	std::string utf8ToFs(const std::string &src);

	UString unpackUtf8(const std::string &src);

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
