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

namespace OpenXcom
{
namespace CrossPlatform
{

/**
 * Displays a message box with an error message.
 * @param error Error message.
 */
void showError(const std::string &error)
{
    MessageBoxA(NULL, error.c_str(), "OpenXcom Error", MB_ICONERROR | MB_OK);
    Log(LOG_FATAL) << error;
}

/**
 * Builds a list of predefined paths for the Data folder
 * according to the running system.
 * @return List of data paths.
 */
std::vector<std::string> findDataFolders()
{
    std::vector<std::string> list; // change to boost::filesystem::path
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

    return list;
}

/**
 * Finds the Config folder according to the running system.
 * @return Config path.
 */
std::string findConfigFolder()
{
    return "";
}

/** @shared
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

/** @shared
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

/** @shared sans Windows specific path separators
 * Takes a filename and tries to find it in the game's Data folders,
 * accounting for the system's case-sensitivity and path style.
 * @param filename Original filename.
 * @return Correct filename or "" if it doesn't exist.
 */
std::string getDataFile(const std::string &filename)
{
    // Correct folder separator
    std::string name = filename;
    std::replace(name.begin(), name.end(), '/', PATH_SEPARATOR);

    // Check current data path
    std::string path = caseInsensitive(Options::getDataFolder(), name);
    if (path != "")
    {
        return path;
    }

    // Check every other path
    for (std::vector<std::string>::const_iterator i = Options::getDataList().begin(); i != Options::getDataList().end(); ++i)
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

/** @shared sans Windows specific path separators
 * Takes a foldername and tries to find it in the game's Data folders,
 * accounting for the system's case-sensitivity and path style.
 * @param foldername Original foldername.
 * @return Correct foldername or "" if it doesn't exist.
 */
std::string getDataFolder(const std::string &foldername)
{
    // Correct folder separator
    std::string name = foldername;
    std::replace(name.begin(), name.end(), '/', PATH_SEPARATOR);

    // Check current data path
    std::string path = caseInsensitiveFolder(Options::getDataFolder(), name);
    if (path != "")
    {
        return path;
    }

    // Check every other path
    for (std::vector<std::string>::const_iterator i = Options::getDataList().begin(); i != Options::getDataList().end(); ++i)
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

/** @TODO replace with portable boost functions
 * Creates a folder at the specified path.
 * @note Only creates the last folder on the path.
 * @param path Full path.
 * @return Folder created or not.
 */
bool createFolder(const std::string &path)
{
    int result = CreateDirectoryA(path.c_str(), 0);
    if (result == 0)
        return false;
    else
        return true;
}

/** @should be not needed
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
        std::string errorMessage("Failed to open directory: " + path);
        throw Exception(errorMessage);
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
    std::sort(files.begin(), files.end());
    return files;
}

/**
 * Gets the name of all the files
 * contained in a Data subfolder.
 * Repeated files are ignored.
 * @param folder Path to the data folder.
 * @param ext Extension of files ("" if it doesn't matter).
 * @return Ordered list of all the files.
 */
std::vector<std::string> getDataContents(const std::string &folder, const std::string &ext)
{
    std::set<std::string> unique;
    std::vector<std::string> files;

    // Check current data path
    std::string current = caseInsensitiveFolder(Options::getDataFolder(), folder);
    if (current != "")
    {
        std::vector<std::string> contents = getFolderContents(current, ext);
        for (std::vector<std::string>::const_iterator file = contents.begin(); file != contents.end(); ++file)
        {
            unique.insert(*file);
        }
    }

    // Check every other path
    for (std::vector<std::string>::const_iterator i = Options::getDataList().begin(); i != Options::getDataList().end(); ++i)
    {
        std::string path = caseInsensitiveFolder(*i, folder);
        if (path == current)
        {
            continue;
        }
        if (path != "")
        {
            std::vector<std::string> contents = getFolderContents(path, ext);
            for (std::vector<std::string>::const_iterator file = contents.begin(); file != contents.end(); ++file)
            {
                unique.insert(*file);
            }
        }
    }

    files = std::vector<std::string>(unique.begin(), unique.end());
    return files;
}

/**
 * Checks if a certain path exists and is a folder.
 * @param path Full path to folder.
 * @return Does it exist?
 */
bool folderExists(const std::string &path)
{
    return (PathIsDirectoryA(path.c_str()) != FALSE);
}

/**
 * Checks if a certain path exists and is a file.
 * @param path Full path to file.
 * @return Does it exist?
 */
bool fileExists(const std::string &path)
{
    return (PathFileExistsA(path.c_str()) != FALSE);
}

/**
 * Removes a file from the specified path.
 * @param path Full path to file.
 * @return True if the operation succeeded, False otherwise.
 */
bool deleteFile(const std::string &path)
{
    return (DeleteFileA(path.c_str()) != 0);
}

/**
 * Gets the base filename of a path.
 * @param path Full path to file.
 * @param transform Optional function to transform the filename.
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
    size_t dot = filename.find_last_of('.');
    if (dot == std::string::npos)
    {
        return filename;
    }
    return filename.substr(0, filename.find_last_of('.'));
}

/**
 * Gets the current locale of the system in language-COUNTRY format.
 * @return Locale string.
 */
std::string getLocale()
{
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
}

/**
 * Checks if the system's default quit shortcut was pressed.
 * @param ev SDL event.
 * @return Is quitting necessary?
 */
bool isQuitShortcut(const SDL_Event &ev)
{
    // Alt + F4
    return (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_F4 && ev.key.keysym.mod & KMOD_ALT);
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
std::pair<std::wstring, std::wstring> timeToString(time_t time)
{
    wchar_t localDate[25], localTime[25];

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
    wcsftime(localDate, 25, L"%Y-%m-%d", timeinfo);
    wcsftime(localTime, 25, L"%H:%M", timeinfo);

    return std::make_pair(localDate, localTime);
}

/**
 * Compares two Unicode strings using natural human ordering.
 * @param a String A.
 * @param b String B.
 * @return String A comes before String B.
 */
bool naturalCompare(const std::wstring &a, const std::wstring &b)
{
#if (!defined(__MINGW32__) || defined(__MINGW64_VERSION_MAJOR))
    return (StrCmpLogicalW(a.c_str(), b.c_str()) < 0);
#else
    // sorry unix users you get ASCII sort
    std::wstring::const_iterator i, j;
    for (i = a.begin(), j = b.begin(); i != a.end() && j != b.end() && tolower(*i) == tolower(*j); i++, j++);
    return (i != a.end() && j != b.end() && tolower(*i) < tolower(*j));
#endif
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
    return (MoveFileExA(src.c_str(), dest.c_str(), MOVEFILE_REPLACE_EXISTING) != 0);
}

/**
 * Notifies the user that maybe he should have a look.
 */
void flashWindow()
{
    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version)
    if (SDL_GetWMInfo(&wminfo))
    {
        HWND hwnd = wminfo.window;
        FlashWindow(hwnd, true);
    }
}

/**
 * Gets the executable path in DOS-style (short) form.
 * For non-Windows systems, just use a dummy path.
 * @return Executable path.
 */
std::string getDosPath()
{
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
}

void setWindowIcon(int winResource, const std::string &unixPath)
{
    HINSTANCE handle = GetModuleHandle(NULL);
    HICON icon = LoadIcon(handle, MAKEINTRESOURCE(winResource));

    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version)
    if (SDL_GetWMInfo(&wminfo))
    {
        HWND hwnd = wminfo.window;
        SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)icon);
    }
}

} // namespace CrossPlatform
} // namespace OpenXcom
