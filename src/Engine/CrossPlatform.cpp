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

/**
 * Takes a filename and tries to find it in the game's DATA folder,
 * accounting for the system's case-sensitivity and path style.
 * @param filename Original filename.
 * @return Correct filename or exception if it doesn't exist.
 * @note There's no actual method for figuring out the correct
 * filename on case-sensitive systems, this is just a workaround.
 */
std::string getDataFile(const std::string &filename)
{
	// Correct folder separator
	std::string newName = filename;
#ifdef _WIN32
	std::replace(newName.begin(), newName.end(), '/', '\\');
#endif
	std::string newPath = Options::getDataFolder() + filename;

	// Try lowercase and uppercase names
	struct stat info;
	if (stat(newPath.c_str(), &info) == 0)
	{
		return newPath;
	}
	else
	{
		std::transform(newName.begin(), newName.end(), newName.begin(), toupper);
		newPath = Options::getDataFolder() + newName;
		if (stat(newPath.c_str(), &info) == 0)
		{
			return newPath;
		}
		else
		{
			std::transform(newName.begin(), newName.end(), newName.begin(), tolower);
			newPath = Options::getDataFolder() + newName;
			if (stat(newPath.c_str(), &info) == 0)
			{
				return newPath;
			}
			else
			{
				//throw Exception("Couldn't find " + filename);
				return "";
			}
		}
	}
}

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
 * Checks a bunch of predefined paths for the Data folder according
 * to the system and returns the full path.
 * @param exists Check if the folder actually exists.
 * @return Full path to Data folder.
 */
std::string findDataFolder(bool exists)
{
#ifdef _WIN32
	char path[MAX_PATH];

	// Check in AppData folder
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, path)))
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
		PathAppendA(path, "DATA\\");
		if (!exists || PathIsDirectoryA(path))
		{
			return path;
		}
	}

	// Check in working directory
	if (GetCurrentDirectoryA(MAX_PATH, path) != 0)
	{
		PathAppendA(path, "DATA\\");
		if (!exists || PathIsDirectoryA(path))
		{
			return path;
		}
	}
#else
	struct stat info;

	// Check shared directory
#ifdef __APPLE__
	const char* shared = "/Users/Shared/OpenXcom/";
#else
	const char* shared = DATADIR;
#endif
	if (!exists || (stat(shared, &info) == 0 && S_ISDIR(info.st_mode)))
	{
		return shared;
	}

	// Check working directory
	const char* working = "./DATA/";
	if (!exists || (stat(working, &info) == 0 && S_ISDIR(info.st_mode)))
	{
		return working;
	}
#endif
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
		PathAppendA(path, "USER\\");
		if (!exists || PathIsDirectoryA(path))
		{
			return path;
		}
	}

	// Check in working directory
	if (GetCurrentDirectoryA(MAX_PATH, path) != 0)
	{
		PathAppendA(path, "USER\\");
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
	if (home)
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
	const char* working = "./USER/";
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
