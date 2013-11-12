/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include <iostream>
#include <sstream>
#include <string>
#include <locale>
#include "../dirent.h"
#include "Logger.h"
#include "Exception.h"
#include "Options.h"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <direct.h>
#ifndef SHGFP_TYPE_CURRENT
#define SHGFP_TYPE_CURRENT 0
#endif
#ifndef __GNUC__
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#endif
#else
#include <sys/stat.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
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
	std::cerr << error << std::endl;
#endif
	Log(LOG_FATAL) << error;
}

#ifndef _WIN32
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
#endif

/**
 * Builds a list of predefined paths for the Data folder
 * according to the running system.
 * @return List of data paths.
 */
std::vector<std::string> findDataFolders()
{
	std::vector<std::string> list;
#ifdef __MORPHOS__
	list.push_back("PROGDIR:data/");
	return list;
#endif
	
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
#ifdef __HAIKU__
	list.push_back("/boot/apps/OpenXcom/data/");
#endif
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
#ifdef __APPLE__
	snprintf(path, MAXPATHLEN, "%s/Users/Shared/OpenXcom/data/", home);
	list.push_back(path);
#else
	list.push_back("/usr/local/share/openxcom/data/");
#ifndef __FreeBSD__
	list.push_back("/usr/share/openxcom/data/");
#endif
#ifdef DATADIR
	snprintf(path, MAXPATHLEN, "%s/data/", DATADIR);
	list.push_back(path);
#endif

#endif
	
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
	
#ifdef __MORPHOS__
	list.push_back("PROGDIR:");
	return list;
#endif

	
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
#ifdef __HAIKU__
	list.push_back("/boot/apps/OpenXcom/");
#endif
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
#ifdef __MORPHOS__
	return "PROGDIR:";
#endif

#if defined(_WIN32) || defined(__APPLE__)
	return "";
#elif defined (__HAIKU__)
	return "/boot/home/config/settings/openxcom/";
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
 * Takes a path and tries to find it based on the
 * system's case-sensitivity.
 * @param base Base unaltered path.
 * @param path Full path to check for casing.
 * @return Correct foldername or "" if it doesn't exist.
 * @note There's no actual method for figuring out the correct
 * foldername on case-sensitive systems, this is just a workaround.
 */
std::string caseInsensitiveFolder(const std::string &base, const std::string &path)
{
	std::string fullPath = base + path, newPath = path;

	// Try all various case mutations
	// Normal unmangled
	if (folderExists(fullPath.c_str()))
	{
		return fullPath;
	}

	// UPPERCASE
	std::transform(newPath.begin(), newPath.end(), newPath.begin(), toupper);
	fullPath = base + newPath;
	if (folderExists(fullPath.c_str()))
	{
		return fullPath;
	}

	// lowercase
	std::transform(newPath.begin(), newPath.end(), newPath.begin(), tolower);
	fullPath = base + newPath;
	if (folderExists(fullPath.c_str()))
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
	return filename;
}

/**
 * Takes a foldername and tries to find it in the game's Data folders,
 * accounting for the system's case-sensitivity and path style.
 * @param foldername Original foldername.
 * @return Correct foldername or "" if it doesn't exist.
 */
std::string getDataFolder(const std::string &foldername)
{
	// Correct folder separator
	std::string name = foldername;
#ifdef _WIN32
	std::replace(name.begin(), name.end(), '/', PATH_SEPARATOR);
#endif

	// Check current data path
	std::string path = caseInsensitiveFolder(Options::getDataFolder(), name);
	if (path != "")
	{
		return path;
	}

	// Check every other path
	for (std::vector<std::string>::iterator i = Options::getDataList()->begin(); i != Options::getDataList()->end(); ++i)
	{
		std::string path = caseInsensitiveFolder(*i, name);
		if (path != "")
		{
			Options::setDataFolder(*i);
			return path;
		}
	}

	// Give up
	return foldername;
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
	std::string extl = ext;
	std::transform(extl.begin(), extl.end(), extl.begin(), ::tolower);

	DIR *dp = opendir(path.c_str());
	if (dp == 0)
	{
	#ifdef __MORPHOS__
		return files;
	#else
		std::string errorMessage("Failed to open directory: " + path);
		throw Exception(errorMessage);
	#endif
	}

	struct dirent *dirp;
	while ((dirp = readdir(dp)) != 0)
	{
		std::string file = dirp->d_name;

		if (file == "." || file == "..")
		{
			continue;
		}
		if (!extl.empty())
		{
			if (file.length() >= extl.length() + 1)
			{
				std::string end = file.substr(file.length() - extl.length() - 1);
				std::transform(end.begin(), end.end(), end.begin(), ::tolower);
				if (end != "." + extl)
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

/**
 * Checks if a certain path exists and is a folder.
 * @param path Full path to folder.
 * @return Does it exist?
 */
bool folderExists(const std::string &path)
{
#ifdef _WIN32
	return (PathIsDirectoryA(path.c_str()) != FALSE);
#elif __MORPHOS__
	BPTR l = Lock( path.c_str(), SHARED_LOCK );
	if( l != NULL )
	{
		UnLock( l );
		return 1;
	}
	return 0;
#else
	struct stat info;
	return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
#endif
}

/**
 * Checks if a certain path exists and is a file.
 * @param path Full path to file.
 * @return Does it exist?
 */
bool fileExists(const std::string &path)
{
#ifdef _WIN32
	return (PathFileExistsA(path.c_str()) != FALSE);
#elif __MORPHOS__
	BPTR l = Lock( path.c_str(), SHARED_LOCK );
	if( l != NULL )
	{
		UnLock( l );
		return 1;
	}
	return 0;
#else 
	struct stat info;
	return (stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode));
#endif
}

/**
 * Removes a file from the specified path.
 * @param path Full path to file.
 * @return True if the operation succeeded, False otherwise.
 */
bool deleteFile(const std::string &path)
{
#ifdef _WIN32
	return (DeleteFileA(path.c_str()) != 0);
#else
	return (remove(path.c_str()) == 0);
#endif
}

/**
* Gets the base filename of a path.
* @param path Full path to file.
* @return Base filename.
*/
std::string baseFilename(const std::string &path, int (*transform)(int))
{
	size_t sep = path.find_last_of(PATH_SEPARATOR);
	std::string filename;
	if (sep == std::string::npos)
	{
		filename = path;
	}
	else
	{
		filename = path.substr(0, sep + 1);
	}
	if (transform != 0)
	{
		std::transform(filename.begin(), filename.end(), filename.begin(), transform);
	}
	return filename;
}

/**
* Replaces invalid filesystem characters with _.
* @param filename Original filename.
* @return Filename without invalid characters
*/
std::string sanitizeFilename(const std::string &filename)
{
	std::string newFilename = filename;
	for (std::string::iterator i = newFilename.begin(); i != newFilename.end(); ++i)
	{
		if ((*i) == '<' ||
			(*i) == '>' ||
			(*i) == ':' ||
			(*i) == '"' || 
			(*i) == '/' ||
			(*i) == '?' ||
			(*i) == '\\')
		{
			*i = '_';
		}
	}
	return newFilename;
}

/**
 * Removes the extension from a filename.
 * @param filename Original filename.
 * @return Filename without the extension.
 */
std::string noExt(const std::string &filename)
{
	return filename.substr(0, filename.find_last_of('.'));
}

/**
 * Gets the current locale of the system in language-COUNTRY format.
 * @return Locale string.
 */
std::string getLocale()
{
#ifdef _WIN32
	char language[9], country[9];

	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, language, 9);
	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, country, 9);

	std::ostringstream locale;
	locale << language << "-" << country;
	return locale.str();
	/*
	wchar_t locale[LOCALE_NAME_MAX_LENGTH];
	LCIDToLocaleName(GetUserDefaultUILanguage(), locale, LOCALE_NAME_MAX_LENGTH, 0);

	return Language::wstrToUtf8(locale);
	*/
#else
	std::locale l("");
	std::string name = l.name();
	std::string language = name.substr(0, name.find_first_of('_')-1);
	std::string country = name.substr(name.find_first_of('_')-1, name.find_first_of(".")-1);
	
	std::ostringstream locale;
	locale << language << "-" << country;
	return locale.str();
#endif
}

/**
 * Checks if the system's default quit shortcut was pressed.
 * @param ev SDL event.
 * @return Is quitting necessary?
 */
bool isQuitShortcut(const SDL_Event &ev)
{
#ifdef _WIN32
	// Alt + F4
	return (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_F4 && ev.key.keysym.mod & KMOD_ALT);
#elif __APPLE__
	// Command + Q
	return (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_q && ev.key.keysym.mod & KMOD_LMETA);
#else
	//TODO add other OSs shortcuts.
	return false;
#endif
}

}
}
