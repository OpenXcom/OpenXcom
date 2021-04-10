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
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "CrossPlatform.h"
#include <exception>
#include <algorithm>
#include <sstream>
#include <string>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include "../dirent.h"
#include "Logger.h"
#include "Exception.h"
#include "Options.h"
#include "Unicode.h"
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#ifndef __NO_DBGHELP
#include <dbghelp.h>
#endif
#ifdef __MINGW32__
#include <cxxabi.h>
#endif
#define EXCEPTION_CODE_CXX 0xe06d7363
#ifndef __GNUC__
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#ifndef __NO_DBGHELP
#pragma comment(lib, "dbghelp.lib")
#endif
#endif
#else
#include <iostream>
#include <fstream>
#include <locale>
#include <SDL_image.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <pwd.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include "Unicode.h"
#endif
#include <SDL.h>
#include <SDL_syswm.h>
#ifdef __HAIKU__
#include <FindDirectory.h>
#include <StorageDefs.h>
#endif

namespace OpenXcom
{
namespace CrossPlatform
{
	std::string errorDlg;

/**
 * Determines the available Linux error dialogs.
 */
void getErrorDialog()
{
#ifndef _WIN32
	if (system(NULL))
	{
		if (getenv("KDE_SESSION_UID") && system("which kdialog 2>&1 > /dev/null") == 0)
			errorDlg = "kdialog --error ";
		else if (system("which zenity 2>&1 > /dev/null") == 0)
			errorDlg = "zenity --no-wrap --error --text=";
		else if (system("which kdialog 2>&1 > /dev/null") == 0)
			errorDlg = "kdialog --error ";
		else if (system("which gdialog 2>&1 > /dev/null") == 0)
			errorDlg = "gdialog --msgbox ";
		else if (system("which xdialog 2>&1 > /dev/null") == 0)
			errorDlg = "xdialog --msgbox ";
	}
#endif
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
	if (errorDlg.empty())
	{
		std::cerr << error << std::endl;
	}
	else
	{
		std::string nError = '"' + error + '"';
		Unicode::replace(nError, "\n", "\\n");
		std::string cmd = errorDlg + nError;
		if (system(cmd.c_str()) != 0)
			std::cerr << error << std::endl;
	}
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
	if (!home)
	{
		struct passwd *const pwd = getpwuid(getuid());
		home = pwd->pw_dir;
	}
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
	list.push_back("PROGDIR:");
	return list;
#endif

#ifdef _WIN32
	char path[MAX_PATH];

	// Get Documents folder
	if (SHGetSpecialFolderPathA(NULL, path, CSIDL_PERSONAL, FALSE))
	{
		PathAppendA(path, "OpenXcom\\");
		list.push_back(path);
	}

	// Get binary directory
	if (GetModuleFileNameA(NULL, path, MAX_PATH) != 0)
	{
		PathRemoveFileSpecA(path);
		list.push_back(path);
	}

	// Get working directory
	if (GetCurrentDirectoryA(MAX_PATH, path) != 0)
	{
		list.push_back(path);
	}
#else
	char const *home = getHome();
#ifdef __HAIKU__
	char data_path[B_PATH_NAME_LENGTH];
	find_directory(B_SYSTEM_SETTINGS_DIRECTORY, 0, true, data_path, sizeof(data_path)-strlen("/OpenXcom/"));
	strcat(data_path,"/OpenXcom/");
	list.push_back(data_path);
#endif
	char path[MAXPATHLEN];

	// Get user-specific data folders
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

	// Get global data folders
	if (char const *const xdg_data_dirs = getenv("XDG_DATA_DIRS"))
	{
		char xdg_data_dirs_copy[strlen(xdg_data_dirs)+1];
		strcpy(xdg_data_dirs_copy, xdg_data_dirs);
		char *dir = strtok(xdg_data_dirs_copy, ":");
		while (dir != 0)
		{
			snprintf(path, MAXPATHLEN, "%s/openxcom/", dir);
			list.push_back(path);
			dir = strtok(0, ":");
		}
	}
#ifdef __APPLE__
	list.push_back("/Users/Shared/OpenXcom/");
#else
	list.push_back("/usr/local/share/openxcom/");
	list.push_back("/usr/share/openxcom/");
#ifdef DATADIR
	snprintf(path, MAXPATHLEN, "%s/", DATADIR);
	list.push_back(path);
#endif

#endif
	// Get working directory
	list.push_back("./");
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
	if (SHGetSpecialFolderPathA(NULL, path, CSIDL_PERSONAL, FALSE))
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
	char user_path[B_PATH_NAME_LENGTH];
	find_directory(B_USER_SETTINGS_DIRECTORY, 0, true, user_path, sizeof(user_path)-strlen("/OpenXcom/"));
	strcat(user_path,"/OpenXcom/");
	list.push_back(user_path);
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
	char settings_path[B_PATH_NAME_LENGTH];
	find_directory(B_USER_SETTINGS_DIRECTORY, 0, true, settings_path, sizeof(settings_path)-strlen("/OpenXcom/"));
	strcat(settings_path,"/OpenXcom/");
	return settings_path;
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

std::string searchDataFile(const std::string &filename)
{
	// Correct folder separator
	std::string name = filename;
#ifdef _WIN32
	std::replace(name.begin(), name.end(), '/', PATH_SEPARATOR);
#endif

	// Check current data path
	std::string path = Options::getDataFolder() + name;
	if (fileExists(path))
	{
		return path;
	}

	// Check every other path
	for (std::vector<std::string>::const_iterator i = Options::getDataList().begin(); i != Options::getDataList().end(); ++i)
	{
		path = *i + name;
		if (fileExists(path))
		{
			Options::setDataFolder(*i);
			return path;
		}
	}

	// Give up
	return filename;
}

std::string searchDataFolder(const std::string &foldername)
{
	// Correct folder separator
	std::string name = foldername;
#ifdef _WIN32
	std::replace(name.begin(), name.end(), '/', PATH_SEPARATOR);
#endif

	// Check current data path
	std::string path = Options::getDataFolder() + name;
	if (folderExists(path))
	{
		return path;
	}

	// Check every other path
	for (std::vector<std::string>::const_iterator i = Options::getDataList().begin(); i != Options::getDataList().end(); ++i)
	{
		path = *i + name;
		if (folderExists(path))
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

		if (!file.empty() && file[0] == '.')
		{
			//skip ".", "..", ".git", ".svn", ".bashrc", ".ssh" etc.
			continue;
		}
		if (!compareExt(file, ext))
		{
			continue;
		}

		files.push_back(file);
	}
	closedir(dp);
	std::sort(files.begin(), files.end());
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
	if ( l != NULL )
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
	if ( l != NULL )
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
 * Returns only the filename from a specified path.
 * @param path Full path.
 * @return Filename component.
 */
std::string baseFilename(const std::string &path)
{
	size_t sep = path.find_last_of("/\\");
	std::string filename;
	if (sep == std::string::npos)
	{
		filename = path;
	}
	else if (sep == path.size() - 1)
	{
		return baseFilename(path.substr(0, path.size() - 1));
	}
	else
	{
		filename = path.substr(sep + 1);
	}
	return filename;
}

/**
 * Replaces invalid filesystem characters with _.
 * @param filename Original filename.
 * @return Filename without invalid characters.
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
 * Removes the extension from a filename. Only the
 * last dot is considered.
 * @param filename Original filename.
 * @return Filename without the extension.
 */
std::string noExt(const std::string &filename)
{
	size_t dot = filename.find_last_of('.');
	if (dot == std::string::npos)
	{
		return filename;
	}
	return filename.substr(0, dot);
}

/**
 * Returns the extension from a filename. Only the
 * last dot is considered.
 * @param filename Original filename.
 * @return Extension component, includes dot.
 */
std::string getExt(const std::string &filename)
{
	size_t dot = filename.find_last_of('.');
	if (dot == std::string::npos)
	{
		return "";
	}
	return filename.substr(dot);
}

/**
 * Compares the extension in a filename (case-insensitive).
 * @param filename Filename to compare.
 * @param extension Extension to compare to.
 * @return If the extensions match.
 */
bool compareExt(const std::string &filename, const std::string &extension)
{
	if (extension.empty())
		return true;
	int j = filename.length() - extension.length();
	if (j <= 0)
		return false;
	if (filename[j - 1] != '.')
		return false;
	for (size_t i = 0; i < extension.length(); ++i)
	{
		if (::tolower(filename[j + i]) != ::tolower(extension[i]))
			return false;
	}
	return true;
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
#else
	std::locale l;
	try
	{
		l = std::locale("");
	}
	catch (const std::runtime_error &)
	{
		return "x-";
	}
	std::string name = l.name();
	size_t dash = name.find_first_of('_'), dot = name.find_first_of('.');
	if (dot != std::string::npos)
	{
		name = name.substr(0, dot - 1);
	}
	if (dash != std::string::npos)
	{
		std::string language = name.substr(0, dash - 1);
		std::string country = name.substr(dash - 1);
		std::ostringstream locale;
		locale << language << "-" << country;
		return locale.str();
	}
	else
	{
		return name + "-";
	}
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
    (void)ev;
	return false;
#endif
}

/**
 * Gets the last modified date of a file.
 * @param path Full path to file.
 * @return The timestamp in integral format.
 */
time_t getDateModified(const std::string &path)
{
/*#ifdef _WIN32
	WIN32_FILE_ATTRIBUTE_DATA info;
	if (GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &info))
	{
		FILETIME ft = info.ftLastWriteTime;
		LARGE_INTEGER li;
		li.HighPart = ft.dwHighDateTime;
		li.LowPart = ft.dwLowDateTime;
		return li.QuadPart;
	}
	else
	{
		return 0;
	}
#endif*/
	struct stat info;
	if (stat(path.c_str(), &info) == 0)
	{
		return info.st_mtime;
	}
	else
	{
		return 0;
	}
}

/**
 * Converts a date/time into a human-readable string
 * using the ISO 8601 standard.
 * @param time Value in timestamp format.
 * @return String pair with date and time.
 */
std::pair<std::string, std::string> timeToString(time_t time)
{
	char localDate[25], localTime[25];

/*#ifdef _WIN32
	LARGE_INTEGER li;
	li.QuadPart = time;
	FILETIME ft;
	ft.dwHighDateTime = li.HighPart;
	ft.dwLowDateTime = li.LowPart;
	SYSTEMTIME st;
	FileTimeToLocalFileTime(&ft, &ft);
	FileTimeToSystemTime(&ft, &st);

	GetDateFormatW(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, localDate, 25);
	GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, localTime, 25);
#endif*/

	struct tm *timeinfo = localtime(&(time));
	strftime(localDate, 25, "%Y-%m-%d", timeinfo);
	strftime(localTime, 25, "%H:%M", timeinfo);

	return std::make_pair(localDate, localTime);
}

/**
 * Moves a file from one path to another,
 * replacing any existing file.
 * @param src Source path.
 * @param dest Destination path.
 * @return True if the operation succeeded, False otherwise.
 */
bool moveFile(const std::string &src, const std::string &dest)
{
#ifdef _WIN32
	return (MoveFileExA(src.c_str(), dest.c_str(), MOVEFILE_REPLACE_EXISTING) != 0);
#else
	//return (rename(src.c_str(), dest.c_str()) == 0);
	std::ifstream srcStream;
	std::ofstream destStream;
	srcStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	destStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	try
	{
		srcStream.open(src.c_str(), std::ios::binary);
		destStream.open(dest.c_str(), std::ios::binary);
		destStream << srcStream.rdbuf();
		srcStream.close();
		destStream.close();
	}
	catch (const std::fstream::failure &)
	{
		return false;
	}
	return deleteFile(src);
#endif
}

/**
 * Notifies the user that maybe he should have a look.
 */
void flashWindow(SDL_Window *window)
{
#ifdef _WIN32
	SDL_SysWMinfo wminfo;
	SDL_VERSION(&wminfo.version)
	if (SDL_GetWindowWMInfo(window, &wminfo))
	{
		HWND hwnd = wminfo.info.win.window;
		FlashWindow(hwnd, true);
	}
#endif
}

/**
 * Gets the executable path in DOS-style (short) form.
 * For non-Windows systems, just use a dummy path.
 * @return Executable path.
 */
std::string getDosPath()
{
#ifdef _WIN32
	std::string path, bufstr;
	char buf[MAX_PATH];
	if (GetModuleFileNameA(0, buf, MAX_PATH) != 0)
	{
		bufstr = buf;
		size_t c1 = bufstr.find_first_of('\\');
		path += bufstr.substr(0, c1+1);
		size_t c2 = bufstr.find_first_of('\\', c1+1);
		while (c2 != std::string::npos)
		{
			std::string dirname = bufstr.substr(c1+1, c2-c1-1);
			if (dirname == "..")
			{
				path = path.substr(0, path.find_last_of('\\', path.length()-2));
			}
			else
			{
				if (dirname.length() > 8)
					dirname = dirname.substr(0, 6) + "~1";
				std::transform(dirname.begin(), dirname.end(), dirname.begin(), toupper);
				path += dirname;
			}
			c1 = c2;
			c2 = bufstr.find_first_of('\\', c1+1);
			if (c2 != std::string::npos)
				path += '\\';
		}
	}
	else
	{
		path = "C:\\GAMES\\OPENXCOM";
	}
	return path;
#else
	return "C:\\GAMES\\OPENXCOM";
#endif
}

/**
 * Sets the window titlebar icon.
 * For Windows, use the embedded resource icon.
 * For other systems, use a PNG icon.
 * @param winResource ID for Windows icon.
 * @param unixPath Path to PNG icon for Unix.
 */
#ifdef _WIN32
void setWindowIcon(SDL_Window *window, int winResource, const std::string &)
{
	HINSTANCE handle = GetModuleHandle(NULL);
	HICON icon = LoadIcon(handle, MAKEINTRESOURCE(winResource));
		
	SDL_SysWMinfo wminfo;
	SDL_VERSION(&wminfo.version)
	if (SDL_GetWindowWMInfo(window, &wminfo))
	{
		HWND hwnd = wminfo.info.win.window;
		SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)icon);
	}
}
#else
void setWindowIcon(SDL_Window *window, int, const std::string &unixPath)
{
	std::string utf8 = Unicode::convPathToUtf8(unixPath);
	SDL_Surface *icon = IMG_Load(utf8.c_str());
	if (icon != 0)
	{
		SDL_SetWindowIcon(window, icon);
		SDL_FreeSurface(icon);
	}
}
#endif

/**
 * Logs the stack back trace leading up to this function call.
 * @param ctx Pointer to stack context (PCONTEXT on Windows), NULL to use current context.
 */
void stackTrace(void *ctx)
{
#ifdef _WIN32
#ifndef __NO_DBGHELP
	const int MAX_SYMBOL_LENGTH = 1024;
	CONTEXT context;
	if (ctx != 0)
	{
		context = *((PCONTEXT)ctx);
	}
	else
	{
#ifdef _M_IX86
		memset(&context, 0, sizeof(CONTEXT));
		context.ContextFlags = CONTEXT_CONTROL;
#ifdef __MINGW32__
		asm("Label:\n\t"
			"movl %%ebp,%0;\n\t"
			"movl %%esp,%1;\n\t"
			"movl $Label,%%eax;\n\t"
			"movl %%eax,%2;\n\t"
			: "=r" (context.Ebp), "=r" (context.Esp), "=r" (context.Eip)
			: //no input
			: "eax");
#else
		_asm {
		Label:
			mov[context.Ebp], ebp;
			mov[context.Esp], esp;
			mov eax, [Label];
			mov[context.Eip], eax;
		}
#endif
#else
		RtlCaptureContext(&context);
#endif
	}
	HANDLE thread = GetCurrentThread();
	HANDLE process = GetCurrentProcess();
	STACKFRAME64 frame;
	memset(&frame, 0, sizeof(STACKFRAME64));
	DWORD image;
#ifdef _M_IX86
	image = IMAGE_FILE_MACHINE_I386;
	frame.AddrPC.Offset = context.Eip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.Ebp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.Esp;
	frame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	image = IMAGE_FILE_MACHINE_AMD64;
	frame.AddrPC.Offset = context.Rip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.Rbp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.Rsp;
	frame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	image = IMAGE_FILE_MACHINE_IA64;
	frame.AddrPC.Offset = context.StIIP;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.IntSp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrBStore.Offset = context.RsBSP;
	frame.AddrBStore.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.IntSp;
	frame.AddrStack.Mode = AddrModeFlat;
#else
	Log(LOG_FATAL) << "Unfortunately, no stack trace information is available";
	return;
#endif
	SYMBOL_INFO *symbol = (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO) + (MAX_SYMBOL_LENGTH - 1) * sizeof(TCHAR));
	symbol->MaxNameLen = MAX_SYMBOL_LENGTH;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
	line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	DWORD displacement;
	SymInitialize(process, NULL, TRUE);
	while (StackWalk64(image, process, thread, &frame, &context, NULL, NULL, NULL, NULL))
	{
		if (SymFromAddr(process, frame.AddrPC.Offset, NULL, symbol))
		{
			std::string symname = symbol->Name;
#ifdef __MINGW32__
			symname = "_" + symname;
			int status = 0;
			size_t outSz = 0;
			char* demangled = abi::__cxa_demangle(symname.c_str(), 0, &outSz, &status);
			if (status == 0)
			{
				symname = demangled;
				if (outSz > 0)
					free(demangled);
			}
			else
			{
				symname = symbol->Name;
			}
#endif
			if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &displacement, line))
			{
				std::string filename = line->FileName;
				size_t n = filename.find_last_of(PATH_SEPARATOR);
				if (n != std::string::npos)
				{
					filename = filename.substr(n + 1);
				}
				Log(LOG_FATAL) << "0x" << std::hex << symbol->Address << std::dec << " " << symname << " (" << filename << ":" << line->LineNumber << ")";
			}
			else
			{
				Log(LOG_FATAL) << "0x" << std::hex << symbol->Address << std::dec << " " << symname;
			}
		}
		else
		{
			Log(LOG_FATAL) << "??";
		}
	}
	DWORD err = GetLastError();
	if (err)
	{
		Log(LOG_FATAL) << "Unfortunately, no stack trace information is available";
	}
	SymCleanup(process);
#else
	Log(LOG_FATAL) << "Unfortunately, no stack trace information is available";
#endif
#elif __CYGWIN__
	Log(LOG_FATAL) << "Unfortunately, no stack trace information is available";
#else
	void *frames[32];
	char buf[1024];
	int  frame_count = backtrace(frames, 32);
	char *demangled = NULL;
	const char *mangled = NULL;
	int status;
	size_t sym_offset;

	for (int i = 0; i < frame_count; i++) {
		Dl_info dl_info;
		if (dladdr(frames[i], &dl_info )) {
			demangled = NULL;
			mangled = dl_info.dli_sname;
			if ( mangled != NULL) {
				sym_offset = (char *)frames[i] - (char *)dl_info.dli_saddr;
				demangled = abi::__cxa_demangle( dl_info.dli_sname, NULL, 0, &status);
				snprintf(buf, sizeof(buf), "%s(%s+0x%zx) [%p]",
						dl_info.dli_fname,
						status == 0 ? demangled : mangled,
						sym_offset, frames[i] );
			} else { // symbol not found
				sym_offset = (char *)frames[i] - (char *)dl_info.dli_fbase;
				snprintf(buf, sizeof(buf), "%s(+0x%zx) [%p]", dl_info.dli_fname, sym_offset, frames[i]);
			}
			free(demangled);
			Log(LOG_FATAL) << buf;
		} else { // object not found
			snprintf(buf, sizeof(buf), "? ? [%p]", frames[i]);
			Log(LOG_FATAL) << buf;
		}
	}
#endif
	ctx = (void*)ctx;
}

/**
 * Generates a timestamp of the current time.
 * @return String in D-M-Y_H-M-S format.
 */
std::string now()
{
	const int MAX_LEN = 25, MAX_RESULT = 80;
	char result[MAX_RESULT] = { 0 };
#ifdef _WIN32
	char date[MAX_LEN], time[MAX_LEN];
	if (GetDateFormatA(LOCALE_INVARIANT, 0, 0, "dd'-'MM'-'yyyy", date, MAX_LEN) == 0)
		return "00-00-0000";
	if (GetTimeFormatA(LOCALE_INVARIANT, TIME_FORCE24HOURFORMAT, 0, "HH'-'mm'-'ss", time, MAX_LEN) == 0)
		return "00-00-00";
	sprintf(result, "%s_%s", date, time);
#else
	char buffer[MAX_LEN];
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, MAX_LEN, "%d-%m-%Y_%H-%M-%S", timeinfo);
	sprintf(result, "%s", buffer);
#endif
	return result;
}

/**
 * Logs the details of this crash and shows an error.
 * @param ex Pointer to exception data (PEXCEPTION_POINTERS on Windows, signal int on Unix)
 * @param err Exception message, if any.
 */
void crashDump(void *ex, const std::string &err)
{
	std::ostringstream error;
#ifdef _MSC_VER
	PEXCEPTION_POINTERS exception = (PEXCEPTION_POINTERS)ex;
	std::exception *cppException = 0;
	switch (exception->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_CODE_CXX:
		cppException = (std::exception *)exception->ExceptionRecord->ExceptionInformation[1];
		error << cppException->what();
		break;
	case EXCEPTION_ACCESS_VIOLATION:
		error << "Memory access violation. This usually indicates something missing in a mod.";
		break;
	default:
		error << "code 0x" << std::hex << exception->ExceptionRecord->ExceptionCode;
		break;
	}
	Log(LOG_FATAL) << "A fatal error has occurred: " << error.str();
	stackTrace(exception->ContextRecord);
	std::string dumpName = Options::getUserFolder();
	dumpName += now() + ".dmp";
	HANDLE dumpFile = CreateFileA(dumpName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	MINIDUMP_EXCEPTION_INFORMATION exceptionInformation;
	exceptionInformation.ThreadId = GetCurrentThreadId();
	exceptionInformation.ExceptionPointers = exception;
	exceptionInformation.ClientPointers = FALSE;
	if (MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dumpFile, MiniDumpNormal, exception ? &exceptionInformation : NULL, NULL, NULL))
	{
		Log(LOG_FATAL) << "Crash dump generated at " << dumpName;
	}
	else
	{
		Log(LOG_FATAL) << "No crash dump generated: " << GetLastError();
	}
#else
	if (ex == 0)
	{
		error << err;
	}
	else
	{
		int signal = *((int*)ex);
		switch (signal)
		{
		case SIGSEGV:
			error << "Segmentation fault. This usually indicates something missing in a mod.";
			break;
		default:
			error << "signal " << signal;
			break;
		}
	}
	Log(LOG_FATAL) << "A fatal error has occurred: " << error.str();
	stackTrace(0);
#endif
	std::ostringstream msg;
	msg << "OpenXcom has crashed: " << error.str() << std::endl;
	msg << "More details here: " << Logger::logFile() << std::endl;
	msg << "If this error was unexpected, please report it to the developers.";
	showError(msg.str());
}

/**
 * Opens a file or web path in the system default browser.
 */
bool openExplorer(const std::string &url)
{
#ifdef _WIN32
	HINSTANCE ret = ShellExecuteW(NULL, L"open", Unicode::convMbToWc(url, CP_UTF8).c_str(), NULL, NULL, SW_SHOWNORMAL);
	// The return value is not a true HINSTANCE. If the function succeeds, it returns a value greater than 32.
	return (static_cast<int>(reinterpret_cast<uintptr_t>(ret)) > 32);
#elif __APPLE__
	std::string cmd = "open \"" + url + "\"";
	return (system(cmd.c_str()) == 0);
#else
	std::string cmd = "xdg-open \"" + url + "\"";
	return (system(cmd.c_str()) == 0);
#endif
}

}

}
