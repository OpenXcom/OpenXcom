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
#include "LocalizedText.h"
#include "Language.h"

namespace OpenXcom
{

/**
 * Replace the next argument placeholder with @a val.
 * @param val The value to place in the next placeholder's position.
 * @return A translated string with all occurrences of the marker replaced by @a val.
 */
LocalizedText LocalizedText::arg(const std::string &val) const
{
	std::ostringstream os;
	os << '{' << _nextArg << '}';
	std::string marker(os.str());
	size_t pos = _text.find(marker);
	if (std::string::npos == pos)
		return *this;
	std::string ntext(_text);
	for (/*empty*/ ; std::string::npos != pos; pos = ntext.find(marker, pos + val.length()))
	{
		ntext.replace(pos, marker.length(), val);
	}
	return LocalizedText(ntext, _nextArg);
}

/**
 * Replace the next argument placeholder with @a val.
 * @param val The value to place in the next placeholder's position.
 * @return The translated string with all occurrences of the marker replaced by @a val.
 */
LocalizedText &LocalizedText::arg(const std::string &val)
{
	std::ostringstream os;
	os << '{' << _nextArg << '}';
	std::string marker(os.str());
	size_t pos = _text.find(marker);
	if (std::string::npos != pos)
	{
		for (/*empty*/ ; std::string::npos != pos; pos = _text.find(marker, pos + val.length()))
		{
			_text.replace(pos, marker.length(), val);
		}
		++_nextArg;
	}
	return *this;
}

}
