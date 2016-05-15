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

#include "FileMap.h"
#include "Logger.h"
#include "CrossPlatform.h"
#include <map>
#include <algorithm>

namespace OpenXcom
{
namespace FileMap
{

static std::vector<std::pair<std::string, std::vector<std::string> > > _rulesets;
static std::map<std::string, std::string> _resources;
static std::map< std::string, std::set<std::string> > _vdirs;
static std::set<std::string> _emptySet;

static std::string _canonicalize(const std::string &in)
{
	std::string ret = in;
	std::transform(in.begin(), in.end(), ret.begin(), tolower);
	return ret;
}

const std::string &getFilePath(const std::string &relativeFilePath)
{
	std::string canonicalRelativeFilePath = _canonicalize(relativeFilePath);
	if (_resources.find(canonicalRelativeFilePath) == _resources.end())
	{
		Log(LOG_INFO) << "requested file not found: " << relativeFilePath;
		return relativeFilePath;
	}

	return _resources.at(canonicalRelativeFilePath);
}

const std::set<std::string> &getVFolderContents(const std::string &relativePath)
{
	std::string canonicalRelativePath = _canonicalize(relativePath);
	
	// trim of trailing '/' characters
	while (!canonicalRelativePath.empty() && '/' == canonicalRelativePath.at(canonicalRelativePath.length() - 1))
	{
		canonicalRelativePath.resize(canonicalRelativePath.length() - 1);
	}
	
	if (_vdirs.find(canonicalRelativePath) == _vdirs.end())
	{
		return _emptySet;
	}

	return _vdirs.at(canonicalRelativePath);
}

template <typename T>
std::set<std::string> _filterFiles(const T &files, const std::string &ext)
{
	std::set<std::string> ret;
	size_t extLen = ext.length() + 1; // +1 for the '.'
	std::string canonicalExt = _canonicalize(ext);
	for (typename T::const_iterator i = files.begin(); i != files.end(); ++i)
	{
		// less-than not less-than-or-equal since we should have at least
		// one character in the filename that is not part of the extension
		if (extLen < i->length() && 0 == _canonicalize(i->substr(i->length() - (extLen - 1))).compare(canonicalExt))
		{
			ret.insert(*i);
		}
	}
	return ret;
}

std::set<std::string> filterFiles(const std::vector<std::string> &files, const std::string &ext) { return _filterFiles(files, ext); }
std::set<std::string> filterFiles(const std::set<std::string>    &files, const std::string &ext) { return _filterFiles(files, ext); }

const std::vector<std::pair<std::string, std::vector<std::string> > > &getRulesets()
{
	return _rulesets;
}

static std::string _combinePath(const std::string &prefixPath, const std::string &appendPath)
{
	std::string ret;
	if (prefixPath.length())
	{
		ret += prefixPath + "/";
	}
	ret += appendPath;
	return ret;
}

static void _mapFiles(const std::string &modId, const std::string &basePath,
		      const std::string &relPath, bool ignoreMods)
{
	std::string fullDir = basePath + (relPath.length() ? "/" + relPath : "");
	std::vector<std::string> files = CrossPlatform::getFolderContents(fullDir);
	std::set<std::string> rulesetFiles = _filterFiles(files, "rul");

	if (!ignoreMods && rulesetFiles.size())
	{
		_rulesets.insert(_rulesets.begin(), std::pair<std::string, std::vector<std::string> >(modId, std::vector<std::string>()));
		for (std::set<std::string>::iterator i = rulesetFiles.begin(); i != rulesetFiles.end(); ++i)
		{
			std::string fullpath = fullDir + "/" + *i;
			Log(LOG_VERBOSE) << "  recording ruleset: " << fullpath;
			_rulesets.front().second.push_back(fullpath);
		}
	}

	for (std::vector<std::string>::iterator i = files.begin(); i != files.end(); ++i)
	{
		std::string fullpath = fullDir + "/" + *i;
		
		if (_canonicalize(*i) == "metadata.yml" || rulesetFiles.find(*i) != rulesetFiles.end())
		{
			// no need to map mod metadata files or ruleset files
			Log(LOG_VERBOSE) << "  ignoring non-resource file: " << fullpath;
			continue;
		}

		if (CrossPlatform::folderExists(fullpath))
		{
			Log(LOG_VERBOSE) << "  recursing into: " << fullpath;
			// allow old mod directory format -- if the top-level subdir
			// is named "Mod" and no top-level ruleset files were found,
			// record ruleset files in that subdirectory, otherwise ignore them
			bool ignoreModsRecurse = ignoreMods ||
				!rulesetFiles.empty() || !relPath.empty() || _canonicalize(*i) != "ruleset";
			_mapFiles(modId, basePath, _combinePath(relPath, *i), ignoreModsRecurse);
			continue;
		}

		// populate resource map
		std::string canonicalRelativeFilePath = _canonicalize(_combinePath(relPath, *i));
		if (_resources.insert(std::pair<std::string, std::string>(canonicalRelativeFilePath, fullpath)).second)
		{
			Log(LOG_VERBOSE) << "  mapped resource: " << canonicalRelativeFilePath << " -> " << fullpath;
		}
		else
		{
			Log(LOG_VERBOSE) << "  resource already mapped by higher-priority mod; ignoring: " << fullpath;
		}

		// populate vdir map
		std::string canonicalRelativePath = _canonicalize(relPath);
		std::string canonicalFile = _canonicalize(*i);
		if (_vdirs.find(canonicalRelativePath) == _vdirs.end())
		{
			_vdirs.insert(std::pair< std::string, std::set<std::string> >(canonicalRelativePath, std::set<std::string>()));
		}
		if (_vdirs.at(canonicalRelativePath).insert(canonicalFile).second)
		{
			Log(LOG_VERBOSE) << "  mapped file to virtual directory: " << canonicalRelativePath << " -> " << canonicalFile;
		}
	}
}

void clear()
{
	_rulesets.clear();
	_resources.clear();
	_vdirs.clear();
}

void load(const std::string &modId, const std::string &path, bool ignoreMods)
{
	Log(LOG_VERBOSE) << "  mapping resources in: " << path;
	_mapFiles(modId, path, "", ignoreMods);
}

}

}
