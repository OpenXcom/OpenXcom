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
#include "Unicode.h"
#include <sstream>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <locale.h>
#include <stdlib.h>
#include <ctype.h>
#endif

namespace OpenXcom
{
namespace Unicode
{

/**
 * Takes a Unicode 32-bit string and converts it
 * to a 8-bit string encoded in UTF-8.
 * Used for rendering text.
 * @note Adapted from https://stackoverflow.com/a/148766/2683561
 * @param src UTF-8 string.
 * @return Unicode string.
 */
UString convUtf8ToUtf32(const std::string &src)
{
	if (src.empty())
		return UString();
	UString out;
	out.reserve(src.size());
	UCode codepoint = 0;
	for (std::string::const_iterator i = src.begin(); i != src.end();)
	{
		unsigned char ch = static_cast<unsigned char>(*i);
		if (ch <= 0x7f)
			codepoint = ch;
		else if (ch <= 0xbf)
			codepoint = (codepoint << 6) | (ch & 0x3f);
		else if (ch <= 0xdf)
			codepoint = ch & 0x1f;
		else if (ch <= 0xef)
			codepoint = ch & 0x0f;
		else
			codepoint = ch & 0x07;
		++i;
		if (i == src.end() || ((*i & 0xc0) != 0x80 && codepoint <= 0x10ffff))
		{
			out.append(1, codepoint);
		}
	}
	return out;
}

/**
 * Takes a Unicode 32-bit string and converts it
 * to a 8-bit string encoded in UTF-8.
 * Used for rendering text.
 * @note Adapted from https://stackoverflow.com/a/148766/2683561
 * @param src Unicode string.
 * @return UTF-8 string.
 */
std::string convUtf32ToUtf8(const UString &src)
{
	if (src.empty())
		return std::string();
	std::string out;
	out.reserve(src.size());
	for (UString::const_iterator i = src.begin(); i != src.end(); ++i)
	{
		UCode codepoint = *i;
		if (codepoint <= 0x7f)
		{
			out.append(1, static_cast<char>(codepoint));
		}
		else if (codepoint <= 0x7ff)
		{
			out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
			out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
		}
		else if (codepoint <= 0xffff)
		{
			out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
			out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
			out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
		}
		else
		{
			out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
			out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
			out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
			out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
		}
	}
	return out;
}

/**
 * Takes a wide-character string and converts it to a
 * multibyte 8-bit string in a given encoding.
 * Used for Win32 APIs.
 * @param src Wide-character string.
 * @param cp Codepage of the destination string.
 * @return Multibyte string.
 */
std::string convWcToMb(const std::wstring &src, unsigned int cp)
{
	if (src.empty())
		return std::string();
#ifdef _WIN32
	int size = WideCharToMultiByte(cp, 0, &src[0], (int)src.size(), NULL, 0, NULL, NULL);
	std::string str(size, 0);
	WideCharToMultiByte(cp, 0, &src[0], (int)src.size(), &str[0], size, NULL, NULL);
	return str;
#else
	const int MAX = 500;
	char buffer[MAX + 1];
	cp = LC_ALL;
	setlocale(cp, "");
	size_t len = wcstombs(buffer, src.c_str(), MAX);
	setlocale(cp, "C");
	if (len == (size_t)-1)
		return "?";
	return std::string(buffer);
#endif
}

/**
 * Takes a multibyte 8-bit string in a given encoding
 * and converts it to a wide-character string.
 * Used for Win32 APIs.
 * @param src Multibyte string.
 * @param cp Codepage of the source string.
 * @return Wide-character string.
 */
std::wstring convMbToWc(const std::string &src, unsigned int cp)
{
	if (src.empty())
		return std::wstring();
#ifdef _WIN32
	int size = MultiByteToWideChar(cp, 0, &src[0], (int)src.size(), NULL, 0);
	std::wstring wstr(size, 0);
	MultiByteToWideChar(cp, 0, &src[0], (int)src.size(), &wstr[0], size);
	return wstr;
#else
	const int MAX = 500;
	wchar_t buffer[MAX + 1];
	cp = LC_ALL;
	setlocale(cp, "");
	size_t len = mbstowcs(buffer, src.c_str(), MAX);
	setlocale(cp, "C");
	if (len == (size_t)-1)
		return L"?";
	return std::wstring(buffer, len);
#endif
}

/**
 * Takes a filesystem path and converts it to a UTF-8 string.
 * On Windows the C paths use the local ANSI codepage.
 * Used for SDL APIs.
 * @param src Filesystem path.
 * @return UTF-8 string.
 */
std::string convPathToUtf8(const std::string &src)
{
#ifdef _WIN32
	return convWcToMb(convMbToWc(src, CP_ACP), CP_UTF8);
#else
	return src;
#endif
}

/**
 * Takes a UTF-8 string and converts it to a filesystem path.
 * On Windows the C paths use the local ANSI codepage.
 * Used for SDL APIs.
 * @param src UTF-8 string.
 * @return Filesystem path.
 */
std::string convUtf8ToPath(const std::string &src)
{
#ifdef _WIN32
	return convWcToMb(convMbToWc(src, CP_UTF8), CP_ACP);
#else
	return src;
#endif
}

/**
 * Compares two UTF-8 strings using natural human ordering.
 * @param a String A.
 * @param b String B.
 * @return String A comes before String B.
 */
bool naturalCompare(const std::string &a, const std::string &b)
{
#ifdef _WIN32
	typedef int (WINAPI *WinStrCmp)(PCWSTR, PCWSTR);
	WinStrCmp pWinStrCmp = (WinStrCmp)GetProcAddress(GetModuleHandleA("shlwapi.dll"), "StrCmpLogicalW");
	if (pWinStrCmp)
	{
		std::wstring wa = convMbToWc(a, CP_UTF8);
		std::wstring wb = convMbToWc(b, CP_UTF8);
		return (pWinStrCmp(wa.c_str(), wb.c_str()) < 0);
	}
	else
#endif
	{
		// sorry unix users you get ASCII sort
		setlocale(LC_ALL, "");
		std::string::const_iterator i, j;
		for (i = a.begin(), j = b.begin(); i != a.end() && j != b.end() && tolower(*i) == tolower(*j); ++i, ++j);
		bool result = (i != a.end() && j != b.end() && tolower(*i) < tolower(*j));
		setlocale(LC_ALL, "C");
		return result;
	}
}

/**
 * Uppercases a UTF-8 string, modified in place.
 * Used for case-insensitive comparisons.
 * @param s Source string.
 */
void upperCase(std::string &s)
{
#ifdef _WIN32
	std::wstring ws = convMbToWc(s, CP_UTF8);
	CharUpperW(&ws[0]);
	s = convWcToMb(ws, CP_UTF8);
#else
	setlocale(LC_ALL, "");
	for (std::string::iterator i = s.begin(); i != s.end(); ++i) *i = toupper(*i);
	setlocale(LC_ALL, "C");
#endif
}

/**
 * Replaces every instance of a substring.
 * @param str The string to modify.
 * @param find The substring to find.
 * @param replace The substring to replace it with.
 */
void replace(std::string &str, const std::string &find, const std::string &replace)
{
	for (size_t i = str.find(find); i != std::string::npos; i = str.find(find, i + replace.length()))
	{
		str.replace(i, find.length(), replace);
	}
}

/**
 * Takes an integer value and formats it as number with separators (spacing the thousands).
 * @param value The value.
 * @param currency Currency symbol.
 * @return The formatted string.
 */
std::string formatNumber(int64_t value, const std::string &currency)
{
	const std::string thousands_sep = "\xC2\xA0"; // TOK_NBSP

	bool negative = (value < 0);
	std::ostringstream ss;
	ss << (negative ? -value : value);
	std::string s = ss.str();
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
		s.insert(0, "-");
	}
	return s;
}

/**
 * Takes an integer value and formats it as currency,
 * spacing the thousands and adding a $ sign to the front.
 * @param funds The funding value.
 * @return The formatted string.
 */
std::string formatFunding(int64_t funds)
{
	return formatNumber(funds, "$");
}

/**
 * Takes an integer value and formats it as percentage,
 * adding a % sign.
 * @param value The percentage value.
 * @return The formatted string.
 */
std::string formatPercentage(int value)
{
	std::ostringstream ss;
	ss << value << "%";
	return ss.str();
}

}
}
