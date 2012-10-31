/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include <vector>

namespace OpenXcom
{

/**
 * Generic purpose functions that need different
 * implementations for different platforms.
 */
namespace CrossPlatform
{
	/// Displays an error message.
	void showError(const std::string &error);
	/// Finds the game's data folders in the system.
	std::vector<std::string> findDataFolders();
	/// Finds the game's user folders in the system.
	std::vector<std::string> findUserFolders();
	/// Finds the game's config folder in the system.
	std::string findConfigFolder();
	/// Gets the path for a data file.
	std::string getDataFile(const std::string &filename);
	/// Creates a folder.
	bool createFolder(const std::string &path);
	/// Terminates a path.
	std::string endPath(const std::string &path);
	/// Returns the list of files in a folder.
	std::vector<std::string> getFolderContents(const std::string &path, const std::string &ext = "");
	/// Checks if the path is an existing folder.
	bool folderExists(const std::string &path);
	/// Checks if the path is an existing file.
	bool fileExists(const std::string &path);
}

}

#endif
