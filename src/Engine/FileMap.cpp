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

#include "FileMap.h"
#include "Logger.h"
#include "CrossPlatform.h"
#include <map>
#include <algorithm>

namespace OpenXcom
{
namespace FileMap
{

static std::vector<std::string>                       _rulesets;
static std::map<std::string, std::string>             _resources;
static std::map< std::string, std::set<std::string> > _vdirs;
static std::set<std::string>                          _emptySet;

static std::string _lcase(const std::string &in)
{
	std::string ret;
	std::transform(in.begin(), in.end(), ret.begin(), tolower);
	return ret;
}

const std::string &getFilePath(const std::string &relativeFilePath)
{
	std::string canonicalRelativeFilePath = _lcase(relativeFilePath);
	if (_resources.find(canonicalRelativeFilePath) == _resources.end())
	{
		return relativeFilePath;
	}

	return _resources.at(canonicalRelativeFilePath);
}

const std::set<std::string> &getVFolderContents(const std::string &relativePath)
{
	std::string canonicalRelativePath = _lcase(relativePath);
	if (_vdirs.find(canonicalRelativePath) == _vdirs.end())
	{
		return _emptySet;
	}

	return _vdirs.at(canonicalRelativePath);
}

std::set<std::string> filterFiles(const std::set<std::string> &files, const std::string &ext)
{
	std::set<std::string> ret;
	size_t extLen = ext.length() + 1; // +1 for the '.'
	for (std::set<std::string>::const_iterator i = files.begin(); i != files.end(); ++i)
	{
		// < not <= since we should have at least one character in the filename that is
		// not part of the extention
		if (extLen < i->length() && 0 == i->substr(i->length() - extLen).compare(ext))
		{
			ret.insert(*i);
		}

	}
	return ret;
}

const std::vector<std::string> &getRulesets()
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

static void _mapFiles(const std::string &basePath, const std::string &relPath, bool ignoreRulesets)
{
	std::string fullDir = basePath + (relPath.length() ? "/" + relPath : "");
	std::vector<std::string> files = CrossPlatform::getFolderContents(fullDir);
	for (std::vector<std::string>::iterator i = files.begin(); i != files.end(); ++i)
	{
		if (*i == "metadata.yaml")
		{
			// no need to map mod metadata files
			continue;
		}

		std::string fullpath = fullDir + "/" + *i;
		if (CrossPlatform::folderExists(fullpath))
		{
			Log(LOG_INFO) << "  recursing into: " << fullpath;
			_mapFiles(basePath, _combinePath(relPath, *i), true);
			continue;
		}

		if (5 <= i->length() && 0 == i->substr(i->length() - 4).compare(".rul"))
		{
			if (ignoreRulesets)
			{
				Log(LOG_INFO) << "  ignoring ruleset: " << fullpath;
			}
			else
			{
				Log(LOG_INFO) << "  recording ruleset: " << fullpath;
				_rulesets.insert(_rulesets.begin(), fullpath);
			}
		}
		else
		{
			// populate resource map
			std::string canonicalRelativeFilePath = _lcase(_combinePath(relPath, *i));
			if (_resources.insert(std::pair<std::string, std::string>(canonicalRelativeFilePath, fullpath)).second)
			{
				Log(LOG_INFO) << "  mapped resource: " << canonicalRelativeFilePath << " -> " << fullpath;
			}
			else
			{
				Log(LOG_INFO) << "  resource already mapped by higher-priority mod; ignoring: " << fullpath;
			}

			// populate vdir map
			std::string canonicalRelativePath = _lcase(relPath);
			std::string lcaseFile = _lcase(*i);
			if (_vdirs.find(canonicalRelativePath) == _vdirs.end())
			{
				_vdirs.insert(std::pair< std::string, std::set<std::string> >(canonicalRelativePath, std::set<std::string>()));
			}
			if (_vdirs.at(canonicalRelativePath).insert(lcaseFile).second)
			{
				Log(LOG_INFO) << "  mapped file to virtual directory: " << canonicalRelativePath << " -> " << lcaseFile;
			}
		}
	}
}

void load(const std::string &path, bool ignoreRulesets)
{
	Log(LOG_INFO) << "mapping resources in: " << path;
	_mapFiles(path, "", ignoreRulesets);
}

}
}
