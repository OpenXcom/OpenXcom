/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#ifndef OPENXCOM_EXCEPTION_H
#define OPENXCOM_EXCEPTION_H

#include <exception>
#include <string>

namespace OpenXcom
{

/**
 * Basic exception class to distinguish
 * OpenXcom exceptions from the rest.
 */
class Exception : public std::exception
{
private:
	std::string _msg;
public:
	/// Creates an exception.
	Exception(const std::string &msg) throw();
	/// Cleans up the exception.
	~Exception() throw();
	/// Returns the exception message.
	const char *what() const throw();
};

}

#endif
