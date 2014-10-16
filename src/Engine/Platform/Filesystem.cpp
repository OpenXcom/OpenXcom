#include <boost/filesystem.hpp>

namespace OpenXcom
{
namespace CrossPlatform
{

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

} // namespace CrossPlatform
} // namespace OpenXcom
