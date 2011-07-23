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
#ifndef OPENXCOM_CROSSPLATFORM_H
#define OPENXCOM_CROSSPLATFORM_H

#include <string>

namespace OpenXcom
{

/**
 * Generic purpose functions that need different
 * implementations for different platforms.
 */
namespace CrossPlatform
{
	/// Gets the correctly-cased path for a data file.
	std::string getDataFile(const std::string &filename);
	/// Displays an error message.
	void showError(const std::string &error);
	/// Displays an error message.
	void showError(const std::wstring &error);

	std::wstring findDataFolder();

	std::wstring findUserFolder();
}

}

#endif
