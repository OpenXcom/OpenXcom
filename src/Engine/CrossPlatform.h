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
#ifndef OPENXCOM_CROSSPLATFORM_H
#define OPENXCOM_CROSSPLATFORM_H

#include <SDL.h>
#include <string>
#include <vector>
#include <utility>

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
    /// Gets the path for a data folder
	std::string getDataFolder(const std::string &foldername);
	/// Creates a folder.
	bool createFolder(const std::string &path);
	/// Terminates a path.
	std::string endPath(const std::string &path);
	/// Returns the list of files in a folder.
	std::vector<std::string> getFolderContents(const std::string &path, const std::string &ext = "");
	/// Returns the list of files in a data folder.
	std::vector<std::string> getDataContents(const std::string &path, const std::string &ext = "");
	/// Checks if the path is an existing folder.
	bool folderExists(const std::string &path);
	/// Checks if the path is an existing file.
	bool fileExists(const std::string &path);
	/// Deletes the specified file.
	bool deleteFile(const std::string &path);
	/// Gets the basename of a file.
	std::string baseFilename(const std::string &path, int(*transform)(int) = 0);
	/// Sanitizes the characters in a filename.
	std::string sanitizeFilename(const std::string &filename);
	/// Removes the extension from a file.
	std::string noExt(const std::string &file);
	/// Gets the system locale.
	std::string getLocale();
	/// Checks if an event is a quit shortcut.
	bool isQuitShortcut(const SDL_Event &ev);
	/// Gets the modified date of a file.
	time_t getDateModified(const std::string &path);
	/// Converts a timestamp to a string.
	std::pair<std::wstring, std::wstring> timeToString(time_t time);
	/// Compares two strings by natural order.
	bool naturalCompare(const std::wstring &a, const std::wstring &b);
	/// Move/rename a file between paths.
	bool moveFile(const std::string &src, const std::string &dest);
	/// Flashes the game window.
	void flashWindow();
	/// Gets the DOS-style executable path.
	std::string getDosPath();
	/// Sets the window icon.
	void setWindowIcon(int winResource, const std::string &unixPath);
}

}

#endif
