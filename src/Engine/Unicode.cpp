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
#endif

namespace OpenXcom
{

namespace Unicode
{

/**
 * Takes a wide-character string and converts it
 * to a 8-bit string encoded in UTF-8.
 * @note Adapted from http://stackoverflow.com/questions/148403/utf8-to-from-wide-char-conversion-in-stl
 * @param src Wide-character string.
 * @return UTF-8 string.
 */
std::string Unicode::wstrToUtf8(const std::wstring &src)
{
	if (src.empty())
		return "";
#ifdef _WIN32
	int size = WideCharToMultiByte(CP_UTF8, 0, &src[0], (int)src.size(), NULL, 0, NULL, NULL);
	std::string str(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, &src[0], (int)src.size(), &str[0], size, NULL, NULL);
	return str;
#else
	std::string out;
	unsigned int codepoint = 0;
	for (std::wstring::const_iterator i = src.begin(); i != src.end(); ++i)
	{
		wchar_t ch = *i;
		if (ch >= 0xd800 && ch <= 0xdbff)
			codepoint = ((ch - 0xd800) << 10) + 0x10000;
		else
		{
			if (ch >= 0xdc00 && ch <= 0xdfff)
				codepoint |= ch - 0xdc00;
			else
				codepoint = ch;

			if (codepoint <= 0x7f)
				out.append(1, static_cast<char>(codepoint));
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
			codepoint = 0;
		}
	}
	return out;
#endif
}

/**
 * Takes a wide-character string and converts it to an
 * 8-bit string encoded in the current system codepage.
 * @param src Wide-character string.
 * @return Codepage string.
 */
std::string Unicode::wstrToCp(const std::wstring &src)
{
	if (src.empty())
		return "";
#ifdef _WIN32
	int size = WideCharToMultiByte(CP_ACP, 0, &src[0], (int)src.size(), NULL, 0, NULL, NULL);
	std::string str(size, 0);
	WideCharToMultiByte(CP_ACP, 0, &src[0], (int)src.size(), &str[0], size, NULL, NULL);
	return str;
#else
	const int MAX = 500;
	char buffer[MAX];
	setlocale(LC_ALL, "");
	wcstombs(buffer, src.c_str(), MAX);
	setlocale(LC_ALL, "C");
	std::string str(buffer);
	return str;
#endif
}

/**
 * Takes a wide-character string and converts it to an
 * 8-bit string with the filesystem encoding.
 * @param src Wide-character string.
 * @return Filesystem string.
 */
std::string Unicode::wstrToFs(const std::wstring &src)
{
#ifdef _WIN32
	return Unicode::wstrToCp(src);
#else
	return Unicode::wstrToUtf8(src);
#endif
}

/**
 * Takes an 8-bit string encoded in UTF-8 and converts it
 * to a wide-character string.
 * @note Adapted from http://stackoverflow.com/questions/148403/utf8-to-from-wide-char-conversion-in-stl
 * @param src UTF-8 string.
 * @return Wide-character string.
 */
std::wstring Unicode::utf8ToWstr(const std::string &src)
{
	if (src.empty())
		return L"";
#ifdef _WIN32
	int size = MultiByteToWideChar(CP_UTF8, 0, &src[0], (int)src.size(), NULL, 0);
	std::wstring wstr(size, 0);
	MultiByteToWideChar(CP_UTF8, 0, &src[0], (int)src.size(), &wstr[0], size);
	return wstr;
#else
	std::wstring out;
	unsigned int codepoint = 0;
	int following = 0;
	for (std::string::const_iterator i = src.begin(); i != src.end(); ++i)
	{
		unsigned char ch = *i;
		if (ch <= 0x7f)
		{
			codepoint = ch;
			following = 0;
		}
		else if (ch <= 0xbf)
		{
			if (following > 0)
			{
				codepoint = (codepoint << 6) | (ch & 0x3f);
				--following;
			}
		}
		else if (ch <= 0xdf)
		{
			codepoint = ch & 0x1f;
			following = 1;
		}
		else if (ch <= 0xef)
		{
			codepoint = ch & 0x0f;
			following = 2;
		}
		else
		{
			codepoint = ch & 0x07;
			following = 3;
		}
		if (following == 0)
		{
			if (codepoint > 0xffff)
			{
				out.append(1, static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
				out.append(1, static_cast<wchar_t>(0xdc00 + (codepoint & 0x03ff)));
			}
			else
				out.append(1, static_cast<wchar_t>(codepoint));
			codepoint = 0;
		}
	}
	return out;
#endif
}

/**
 * Takes an 8-bit string encoded in the current system codepage
 * and converts it to a wide-character string.
 * @param src Codepage string.
 * @return Wide-character string.
 */
std::wstring Unicode::cpToWstr(const std::string &src)
{
	if (src.empty())
		return L"";
#ifdef _WIN32
	int size = MultiByteToWideChar(CP_ACP, 0, &src[0], (int)src.size(), NULL, 0);
	std::wstring wstr(size, 0);
	MultiByteToWideChar(CP_ACP, 0, &src[0], (int)src.size(), &wstr[0], size);
	return wstr;
#else
	const int MAX = 500;
	wchar_t buffer[MAX + 1];
	setlocale(LC_ALL, "");
	size_t len = mbstowcs(buffer, src.c_str(), MAX);
	setlocale(LC_ALL, "C");
	if (len == (size_t)-1)
		return L"?";
	return std::wstring(buffer, len);
#endif
}

/**
 * Takes an 8-bit string with the filesystem encoding
 * and converts it to a wide-character string.
 * @param src Filesystem string.
 * @return Wide-character string.
 */
std::wstring Unicode::fsToWstr(const std::string &src)
{
#ifdef _WIN32
	return Unicode::cpToWstr(src);
#else
	return Unicode::utf8ToWstr(src);
#endif
}

/**
 * Takes an 8-bit string with the filesystem encoding
 * and converts it to a UTF-8 string. Required for SDL.
 * @param src Filesystem string.
 * @return Wide-character string.
 */
std::string Unicode::fsToUtf8(const std::string &src)
{
	return Unicode::wstrToUtf8(Unicode::fsToWstr(src));
}

/**
 * Takes a UTF-8 string and converts it to an
 * 8-bit string with the filesystem encoding. Required for SDL.
 * @param src Filesystem string.
 * @return Wide-character string.
 */
std::string Unicode::utf8ToFs(const std::string &src)
{
	return Unicode::wstrToFs(Unicode::utf8ToWstr(src));
}

UString Unicode::unpackUtf8(const std::string &src)
{
	UString out;
	out.reserve(src.size());
	UCode codepoint = 0;
	int following = 0;
	for (std::string::const_iterator i = src.begin(); i != src.end(); ++i)
	{
		unsigned char ch = *i;
		if (ch <= 0x7f)
		{
			codepoint = ch;
			following = 0;
		}
		else if (ch <= 0xbf)
		{
			if (following > 0)
			{
				codepoint = (codepoint << 6) | (ch & 0x3f);
				--following;
			}
		}
		else if (ch <= 0xdf)
		{
			codepoint = ch & 0x1f;
			following = 1;
		}
		else if (ch <= 0xef)
		{
			codepoint = ch & 0x0f;
			following = 2;
		}
		else
		{
			codepoint = ch & 0x07;
			following = 3;
		}
		if (following == 0)
		{
			if (codepoint > 0xffff)
			{
				out.append(1, 0xd800 + (codepoint >> 10));
				out.append(1, 0xdc00 + (codepoint & 0x03ff));
			}
			else
			{
				out.append(1, codepoint);
			}
			codepoint = 0;
		}
	}
	return out;
}

/**
 * Replaces every instance of a substring.
 * @param str The string to modify.
 * @param find The substring to find.
 * @param replace The substring to replace it with.
 */
void Unicode::replace(std::string &str, const std::string &find, const std::string &replace)
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
std::string Unicode::formatNumber(int64_t value, const std::string &currency)
{
	const char thousands_sep = TOK_NBSP;

	bool negative = (value < 0);
	std::ostringstream ss;
	ss << (negative ? -value : value);
	std::string s = ss.str();
	size_t spacer = s.size() - 3;
	while (spacer > 0 && spacer < s.size())
	{
		s.insert(spacer, 1, thousands_sep);
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
std::string Unicode::formatFunding(int64_t funds)
{
	return formatNumber(funds, "$");
}

/**
 * Takes an integer value and formats it as percentage,
 * adding a % sign.
 * @param value The percentage value.
 * @return The formatted string.
 */
std::string Unicode::formatPercentage(int value)
{
	std::ostringstream ss;
	ss << value << "%";
	return ss.str();
}

}

}
