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
#ifndef OPENXCOM_LOCALIZEDTEXT_H
#define OPENXCOM_LOCALIZEDTEXT_H

#include <string>
#include <sstream>

/// @file

/** @def OX_REQUIRED_RESULT
 * This is used to enable warning of unused results, to warn the user of costly function calls.
 */

#ifndef OX_REQUIRED_RESULT
#  if defined(__GNUC_) && !defined(__INTEL_COMPILER) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#    define OX_REQUIRED_RESULT __attribute__ ((warn_unused_result))
#  else
#    define OX_REQUIRED_RESULT
#  endif
#endif
namespace OpenXcom
{

/**
 * A string that is already translated.
 * Using this class allows argument substitution in the translated strings.
 */
class LocalizedText
{
public:
	/// Create from existing unicode string.
	LocalizedText(const std::wstring &);
	/// Create the empty string.
	LocalizedText() : _nextArg(1) { /* Empty by design. */ }
	/// Return constant wide string.
	operator std::wstring const&() const OX_REQUIRED_RESULT;
	/// Return the UTF-8 representation of this string.
	std::string asUTF8() const OX_REQUIRED_RESULT;
	/// Get a pointer to underlying wchat_t data.
	const wchar_t *c_str() const OX_REQUIRED_RESULT { return _text.c_str(); }

	// Argument substitution.
	/// Replace next argument.
	LocalizedText arg(const std::wstring &) const OX_REQUIRED_RESULT;
	/// Replace next argument.
	LocalizedText &arg(const std::wstring &) OX_REQUIRED_RESULT;
	/// Replace next argument.
	template <typename T> LocalizedText arg(T) const OX_REQUIRED_RESULT;
	/// Replace next argument.
	template <typename T> LocalizedText &arg(T) OX_REQUIRED_RESULT;
private:
	std::wstring _text; ///< The actual localized text.
	unsigned _nextArg; ///< The next argument ID.
	LocalizedText(const std::wstring &, unsigned);
};

/**
 * Create a LocalizedText from a localized std::wstring.
 */
inline LocalizedText::LocalizedText(const std::wstring &text)
  : _text(text), _nextArg(0)
{
	// Empty by design.
}

/**
 * Create a LocalizedText with some arguments already replaced.
 */
inline LocalizedText::LocalizedText(const std::wstring &text, unsigned replaced)
  : _text(text), _nextArg(replaced + 1)
{
	// Empty by design.
}

/**
 * Typecast to constant std::wstring reference.
 * This is used to avoid copying when the string will not change.
 */
inline LocalizedText::operator std::wstring const&() const
{
	return _text;
}

/**
 * Replace the next argument placeholder with @a val.
 * @tparam T The type of the replacement value. It should be streamable to std::owstringstream.
 * @param val The value to place in the next placeholder's position.
 * @return A translated string with all occurences of the marker replaced by @a val.
 */
template <typename T>
LocalizedText LocalizedText::arg(T val) const
{
	std::wostringstream os;
	os << '{' << _nextArg << '}';
	std::wstring marker(os.str());
	size_t pos = _text.find(marker);
	if (std::string::npos == pos)
		return *this;
	std::wstring ntext(_text);
	os.str(L"");
	os << val;
	std::wstring tval(os.str());
	for (/*empty*/ ; std::wstring::npos != pos; pos = ntext.find(marker, pos + tval.length()))
	{
		ntext.replace(pos, marker.length(), tval);
	}
	return LocalizedText(ntext, _nextArg);
}

/**
 * Replace the next argument placeholder with @a val.
 * @tparam T The type of the replacement value. It should be streamable to std::owstringstream.
 * @param val The value to place in the next placeholder's position.
 * @return The translated string with all occurences of the marker replaced by @a val.
 */
template <typename T>
LocalizedText &LocalizedText::arg(T val)
{
	std::wostringstream os;
	os << '{' << _nextArg << '}';
	std::wstring marker(os.str());
	size_t pos = _text.find(marker);
	if (std::string::npos != pos)
	{
		os.str(L"");
		os << val;
		std::wstring tval(os.str());
		for (/*empty*/ ; std::wstring::npos != pos; pos = _text.find(marker, pos + tval.length()))
		{
			_text.replace(pos, marker.length(), tval);
		}
		++_nextArg;
	}
	return *this;
}

/// Allow streaming of LocalizedText objects.
inline std::wostream &operator<<(std::wostream &os, const LocalizedText &txt)
{
	os << static_cast<std::wstring const &>(txt);
	return os;
}
}

#endif
