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
#include "Exception.h"
#include "Options.h"
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#ifndef SHGFP_TYPE_CURRENT
	#define SHGFP_TYPE_CURRENT 0
#endif
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#else
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
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
 * @return Full path to Data folder.
 */
std::wstring findDataFolder()
{
#ifdef _WIN32
	WCHAR path[MAX_PATH];

	// Check in AppData folder
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, path)))
	{
		PathAppendW(path, L"OpenXcom");
		if ((GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			return path;
		}
	}

	// Check in binary directory
	if (GetModuleFileNameW(NULL, path, MAX_PATH) != 0)
	{
		PathRemoveFileSpecW(path);
		PathAppendW(path, L"DATA");
		if ((GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			return path;
		}
	}

	// Check in working directory
	if (GetCurrentDirectoryW(MAX_PATH, path) != 0)
	{
		PathAppendW(path, L"DATA");
		if ((GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			return path;
		}
	}
#else
	wchar_t path[MAXPATHLEN];
	struct stat info;

	// Check shared directory
	const char* shared = "/usr/share/openxcom/";
	if (stat(shared, &info) == 0 && S_ISDIR(info.st_mode))
	{
		mbstowcs(path, shared, strlen(shared)+1);
		return path;
	}

	// Check working directory
	const char* working = "./DATA/";
	if (stat(working, &info) == 0 && S_ISDIR(info.st_mode))
	{
		mbstowcs(path, working, strlen(working)+1);
		return path;
	}
#endif
	return L"";
}

/**
 * Checks a bunch of predefined paths for the User folder according
 * to the system and returns the full path.
 * @return Full path to User folder.
 */
std::wstring findUserFolder()
{
#ifdef _WIN32
	WCHAR path[MAX_PATH];

	// Check in Documents folder
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path)))
	{
		PathAppendW(path, L"OpenXcom");
		if ((GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			return path;
		}
	}

	// Check in binary directory
	if (GetModuleFileNameW(NULL, path, MAX_PATH) != 0)
	{
		PathRemoveFileSpecW(path);
		PathAppendW(path, L"USER");
		if ((GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			return path;
		}
	}

	// Check in working directory
	if (GetCurrentDirectoryW(MAX_PATH, path) != 0)
	{
		PathAppendW(path, L"USER");
		if ((GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			return path;
		}
	}
#else
	wchar_t path[MAXPATHLEN];
	struct stat info;
	
	// Check HOME directory
	char *homedir = getenv("HOME");
	if (homedir)
	{
		char homePath[MAXPATHLEN];
		snprintf(homePath, MAXPATHLEN, "%s/.openxcom/", homedir);
		if (stat(homePath, &info) == 0 && S_ISDIR(info.st_mode))
		{
			mbstowcs(path, homePath, strlen(homePath)+1);
			return path;
		}
	}

	// Check working directory
	const char* working = "./USER/";
	if (stat(working, &info) == 0 && S_ISDIR(info.st_mode))
	{
		mbstowcs(path, working, strlen(working)+1);
		return path;
	}
#endif
	return L"";
}

}
}
