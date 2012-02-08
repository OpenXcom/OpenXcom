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
#include "CrossPlatform.h"
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include "../dirent.h"
#include "Exception.h"
#include "Options.h"
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <direct.h>
#ifndef SHGFP_TYPE_CURRENT
#define SHGFP_TYPE_CURRENT 0
#endif
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#else
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace OpenXcom
{
namespace CrossPlatform
{

#ifdef _WIN32
	const char PATH_SEPARATOR = '\\';
#else
	const char PATH_SEPARATOR = '/';
#endif

/**
 * Displays a message box with an error message.
 * @param error Error message.
 */
void showError(const std::string &error)
{
#ifdef _WIN32
	MessageBoxA(NULL, error.c_str(), "OpenXcom Error", MB_ICONERROR | MB_OK);
#else
	std::cerr << "ERROR: " << error << std::endl;
#endif
}

/**
 * Displays a message box with an error message.
 * @param error Error message.
 */
void showError(const std::wstring &error)
{
#ifdef _WIN32
	MessageBoxW(NULL, error.c_str(), L"OpenXcom Error", MB_ICONERROR | MB_OK);
#else
	std::wcerr << L"ERROR: " << error << std::endl;
#endif
}

/**
 * Builds a list of predefined paths for the Data folder
 * according to the running system.
 * @return List of data paths.
 */
std::vector<std::string> findDataFolders()
{
	std::vector<std::string> list;
#ifdef _WIN32
	char path[MAX_PATH];

	// Get Documents folder
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path)))
	{
		PathAppendA(path, "OpenXcom\\Data\\");
		list.push_back(path);
	}

	// Get binary directory
	if (GetModuleFileNameA(NULL, path, MAX_PATH) != 0)
	{
		PathRemoveFileSpecA(path);
		PathAppendA(path, "Data\\");
		list.push_back(path);
	}

	// Get working directory
	if (GetCurrentDirectoryA(MAX_PATH, path) != 0)
	{
		PathAppendA(path, "Data\\");
		list.push_back(path);
	}
#else
	char *xdg_data_home = getenv("XDG_DATA_HOME"), *xdg_data_dirs = getenv("XDG_DATA_DIRS"),
		*home = getenv("HOME");
	if (!home)
	{
		struct passwd* pwd = getpwuid(getuid());
		home = pwd->pw_dir;
	}

	char path[MAXPATHLEN];
#ifdef __APPLE__
	snprintf(path, MAXPATHLEN, "%s/Library/Application Support/OpenXcom/Data/", home);
	list.push_back(path);
#else
	// Get user-specific data folders
	if (xdg_data_home == 0)
	{
		snprintf(path, MAXPATHLEN, "%s/.local/share/openxcom/data/", home);
		list.push_back(path);
	}
	else
	{
		snprintf(path, MAXPATHLEN, "%s/openxcom/data/", xdg_data_home);
		list.push_back(path);
	}

	// Get global data folders
	if (xdg_data_dirs == 0)
	{
		list.push_back("/usr/local/share/openxcom/data/");
		list.push_back("/usr/share/openxcom/data/");
	}
	else
	{
		char *dir = strtok(xdg_data_dirs, ":");
		while (dir != 0)
		{
			snprintf(path, MAXPATHLEN, "%s/openxcom/data/", dir);
			list.push_back(path);
			dir = strtok(0, ":");
		}
	}

#endif
	// Get working directory
	list.push_back("./data/");
#endif

	return list;
}

/**
 * Takes a path and tries to find it based on the
 * system's case-sensitivity.
 * @param base Base unaltered path.
 * @param path Full path to check for casing.
 * @return Correct filename or "" if it doesn't exist.
 * @note There's no actual method for figuring out the correct
 * filename on case-sensitive systems, this is just a workaround.
 */
std::string caseInsensitive(const std::string &base, const std::string &path)
{
	std::string fullPath = base + path, newPath = path;

	// Try all various case mutations
	// Normal unmangled
	struct stat info;
	if (stat(fullPath.c_str(), &info) == 0)
	{
		return fullPath;
	}

	// UPPERCASE
	std::transform(newPath.begin(), newPath.end(), newPath.begin(), toupper);
	fullPath = base + path;
	if (stat(fullPath.c_str(), &info) == 0)
	{
		return fullPath;
	}

	// lowercase
	std::transform(newPath.begin(), newPath.end(), newPath.begin(), tolower);
	fullPath = base + path;
	if (stat(fullPath.c_str(), &info) == 0)
	{
		return fullPath;
	}

	// If we got here nothing can help us
	return "";
}

/**
 * Takes a filename and tries to find it in the game's Data folders,
 * accounting for the system's case-sensitivity and path style.
 * @param filename Original filename.
 * @return Correct filename or "" if it doesn't exist.
 */
std::string getDataFile(const std::string &filename)
{
	// Correct folder separator
	std::string name = filename;
#ifdef _WIN32
	std::replace(name.begin(), name.end(), '/', PATH_SEPARATOR);
#endif

	// Check current data path
	std::string path = caseInsensitive(Options::getDataFolder(), name);
	if (path != "")
	{
		return path;
	}

	// Check every other path
	for (std::vector<std::string>::iterator i = Options::getDataList()->begin(); i != Options::getDataList()->end(); ++i)
	{
		std::string path = caseInsensitive(*i, name);
		if (path != "")
		{
			//Options::setDataFolder(*i);
			return path;
		}
	}

	// Give up
	return "";
}

/**
 * Checks a bunch of predefined paths for the User folder according
 * to the system and returns the full path.
 * @param exists Check if the folder actually exists.
 * @return Full path to User folder.
 */
std::string findUserFolder(bool exists)
{
#ifdef _WIN32
	char path[MAX_PATH];

	// Check in Documents folder
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path)))
	{
		PathAppendA(path, "OpenXcom\\");
		if (!exists || PathIsDirectoryA(path))
		{
			return path;
		}
	}

	// Check in binary directory
	if (GetModuleFileNameA(NULL, path, MAX_PATH) != 0)
	{
		PathRemoveFileSpecA(path);
		PathAppendA(path, "User\\");
		if (!exists || PathIsDirectoryA(path))
		{
			return path;
		}
	}

	// Check in working directory
	if (GetCurrentDirectoryA(MAX_PATH, path) != 0)
	{
		PathAppendA(path, "User\\");
		if (!exists || PathIsDirectoryA(path))
		{
			return path;
		}
	}
#else
	struct stat info;

	// Check HOME directory
	char *home = getenv("HOME");
	if (!home)
	{
		struct passwd* pwd = getpwuid(getuid());
		home = pwd->pw_dir;
	}
	else
	{
		char homePath[MAXPATHLEN];
#ifdef __APPLE__
		snprintf(homePath, MAXPATHLEN, "%s/Library/Application Support/OpenXcom/", home);
#else
		snprintf(homePath, MAXPATHLEN, "%s/.openxcom/", home);
#endif
		if (!exists || (stat(homePath, &info) == 0 && S_ISDIR(info.st_mode)))
		{
			return homePath;
		}
	}

	// Check working directory
	const char* working = "./user/";
	if (!exists || (stat(working, &info) == 0 && S_ISDIR(info.st_mode)))
	{
		return working;
	}
#endif
	return "";
}

/**
 * Creates a folder at the specified path.
 * @note Only creates the last folder on the path.
 * @param path Full path.
 * @return Error code.
 */
int createFolder(const char *path)
{
#ifdef _WIN32
	return _mkdir(path);
#else
	return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

/**
 * Adds an ending slash to a path if necessary.
 * @param path Folder path.
 * @return Terminated path.
 */
std::string endPath(const std::string &path)
{
	if (path.at(path.size()-1) != PATH_SEPARATOR)
		return path + PATH_SEPARATOR;
	return path;
}

/**
 * Gets the name of all the files
 * contained in a certain folder.
 * @param path Full path to folder.
 * @param ext Extension of files ("" if it doesn't matter).
 */
std::vector<std::string> getFolderContents(const std::string &path, const std::string &ext)
{
	std::vector<std::string> files;

	DIR *dp = opendir(path.c_str());
	if (dp == 0)
	{
		throw Exception("Failed to open saves directory");
	}

	struct dirent *dirp;
	while ((dirp = readdir(dp)) != 0)
	{
		std::string file = dirp->d_name;

		if (file == "." || file == "..")
		{
			continue;
		}
		if (!ext.empty())
		{
			if (file.length() >= ext.length() + 1)
			{
				std::string end = file.substr(file.length() - ext.length() - 1);
				if (end != "." + ext)
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}

		files.push_back(file);
	}
	closedir(dp);
#ifndef _WIN32
	std::sort(files.begin(), files.end());
#endif
	return files;
}

}
}
