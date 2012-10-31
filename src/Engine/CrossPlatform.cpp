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
#include "CrossPlatform.h"
#include <algorithm>
#include "../dirent.h"
#include "Logger.h"
#include "Exception.h"
#include "Options.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
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
#include <sys/stat.h>
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
#endif
	Log(LOG_FATAL) << error;
}

/**
 * Gets the user's home folder according to the system.
 * @return Absolute path to home folder.
 */
static char const *getHome()
{
	char const *home = getenv("HOME");
#ifndef _WIN32
	if (!home)
	{
		struct passwd *const pwd = getpwuid(getuid());
		home = pwd->pw_dir;
	}
#endif
	return home;
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
		PathAppendA(path, "OpenXcom\\data\\");
		list.push_back(path);
	}

	// Get binary directory
	if (GetModuleFileNameA(NULL, path, MAX_PATH) != 0)
	{
		PathRemoveFileSpecA(path);
		PathAppendA(path, "data\\");
		list.push_back(path);
	}

	// Get working directory
	if (GetCurrentDirectoryA(MAX_PATH, path) != 0)
	{
		PathAppendA(path, "data\\");
		list.push_back(path);
	}
#else
	char const *home = getHome();
	char path[MAXPATHLEN];

	// Get user-specific data folders
	if (char const *const xdg_data_home = getenv("XDG_DATA_HOME"))
 	{
		snprintf(path, MAXPATHLEN, "%s/openxcom/data/", xdg_data_home);
 	}
 	else
 	{
#ifdef __APPLE__
		snprintf(path, MAXPATHLEN, "%s/Library/Application Support/OpenXcom/data/", home);
#else
		snprintf(path, MAXPATHLEN, "%s/.local/share/openxcom/data/", home);
#endif
 	}
 	list.push_back(path);

	// Get global data folders
	if (char *xdg_data_dirs = getenv("XDG_DATA_DIRS"))
	{
		char *dir = strtok(xdg_data_dirs, ":");
		while (dir != 0)
		{
			snprintf(path, MAXPATHLEN, "%s/openxcom/data/", dir);
			list.push_back(path);
			dir = strtok(0, ":");
		}
	}
	else
	{
#ifdef __APPLE__
		snprintf(path, MAXPATHLEN, "%s/Users/Shared/OpenXcom/data/", home);
		list.push_back(path);
#else
		list.push_back("/usr/local/share/openxcom/data/");
		list.push_back("/usr/share/openxcom/data/");
#endif
	}
	
	// Get working directory
	list.push_back("./data/");
#endif

	return list;
}

/**
 * Builds a list of predefined paths for the User folder
 * according to the running system.
 * @return List of data paths.
 */
std::vector<std::string> findUserFolders()
{
	std::vector<std::string> list;
#ifdef _WIN32
	char path[MAX_PATH];

	// Get Documents folder
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path)))
	{
		PathAppendA(path, "OpenXcom\\");
		list.push_back(path);
	}

	// Get binary directory
	if (GetModuleFileNameA(NULL, path, MAX_PATH) != 0)
	{
		PathRemoveFileSpecA(path);
		PathAppendA(path, "user\\");
		list.push_back(path);
	}

	// Get working directory
	if (GetCurrentDirectoryA(MAX_PATH, path) != 0)
	{
		PathAppendA(path, "user\\");
		list.push_back(path);
	}
#else
	char const *home = getHome();
	char path[MAXPATHLEN];
	
	// Get user folders
	if (char const *const xdg_data_home = getenv("XDG_DATA_HOME"))
 	{
		snprintf(path, MAXPATHLEN, "%s/openxcom/", xdg_data_home);
 	}
 	else
 	{
#ifdef __APPLE__
		snprintf(path, MAXPATHLEN, "%s/Library/Application Support/OpenXcom/", home);
#else
		snprintf(path, MAXPATHLEN, "%s/.local/share/openxcom/", home);
#endif
 	}
	list.push_back(path);

	// Get old-style folder
	snprintf(path, MAXPATHLEN, "%s/.openxcom/", home);
	list.push_back(path);

	// Get working directory
	list.push_back("./user/");
#endif

	return list;
}

/**
 * Finds the Config folder according to the running system.
 * @return Config path.
 */
std::string findConfigFolder()
{
#if defined(_WIN32) || defined(__APPLE__)
	return "";
#else
	char const *home = getHome();
	char path[MAXPATHLEN];
	// Get config folders
	if (char const *const xdg_config_home = getenv("XDG_CONFIG_HOME"))
	{
		snprintf(path, MAXPATHLEN, "%s/openxcom/", xdg_config_home);
		return path;
	}
	else
	{
		snprintf(path, MAXPATHLEN, "%s/.config/openxcom/", home);
		return path;
	}
#endif
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
	if (fileExists(fullPath.c_str()))
	{
		return fullPath;
	}

	// UPPERCASE
	std::transform(newPath.begin(), newPath.end(), newPath.begin(), toupper);
	fullPath = base + newPath;
	if (fileExists(fullPath.c_str()))
	{
		return fullPath;
	}

	// lowercase
	std::transform(newPath.begin(), newPath.end(), newPath.begin(), tolower);
	fullPath = base + newPath;
	if (fileExists(fullPath.c_str()))
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
			Options::setDataFolder(*i);
			return path;
		}
	}

	// Give up
	return "";
}

/**
 * Creates a folder at the specified path.
 * @note Only creates the last folder on the path.
 * @param path Full path.
 * @return Folder created or not.
 */
bool createFolder(const std::string &path)
{
#ifdef _WIN32
	int result = CreateDirectoryA(path.c_str(), 0);
	if (result == 0)
		return false;
	else
		return true;
#else
	mode_t process_mask = umask(0);
	int result = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	umask(process_mask);
	if (result == 0)
		return true;
	else
		return false;
#endif
}

/**
 * Adds an ending slash to a path if necessary.
 * @param path Folder path.
 * @return Terminated path.
 */
std::string endPath(const std::string &path)
{
	if (!path.empty() && path.at(path.size()-1) != PATH_SEPARATOR)
		return path + PATH_SEPARATOR;
	return path;
}

/**
 * Gets the name of all the files
 * contained in a certain folder.
 * @param path Full path to folder.
 * @param ext Extension of files ("" if it doesn't matter).
 * @return Ordered list of all the files.
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

bool folderExists(const std::string &path)
{
#ifdef _WIN32
	return (PathIsDirectoryA(path.c_str()) != FALSE);
#else
	struct stat info;
	return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
#endif
}

bool fileExists(const std::string &path)
{
#ifdef _WIN32
	return (PathFileExistsA(path.c_str()) != FALSE);
#else
	struct stat info;
	return (stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode));
#endif
}

}
}
