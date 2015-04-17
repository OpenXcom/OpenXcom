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
#ifndef OPENXCOM_FILEMAP_H
#define OPENXCOM_FILEMAP_H

#include <set>
#include <string>
#include <vector>

namespace OpenXcom
{

/**
 * Maps canonical names to file paths and maintains the virtual file system
 * for resource files.
 */
namespace FileMap
{
	/// Gets the path for a data file when given a relative (case insensitive) path, like "units/zombie.pck".
	const std::string &getFilePath(const std::string &relativeFilePath);

	/// Returns the set of files in a virtual folder.  The virtual folder contains files from all active mods
	/// that are in similarly-named subdirectories.  The returned file names can then be translated to real
	/// filesystem paths via getFilePath()
	const std::set<std::string> &getVFolderContents(const std::string &relativePath);

	/// Returns a the subset of the given files that matches the given extention
	std::set<std::string> filterFiles(const std::set<std::string> &files, const std::string &ext);

	/// Returns the list of ruleset files found, in reverse order, while mapping resources.
	const std::vector<std::string> &getRulesets();

	/// Scans a directory tree rooted at the specified filesystem path.  Any files it encounters that have already
	/// been mapped will be ignored.  Therefore, load files from mods with the highest priority first.  If
	/// ignoreRulesets is false (the default), it will add any rulesets it finds to the front of the vector
	/// returned by getRulesets().
	void load(const std::string &path, bool ignoreRulesets = false);
}

}

#endif
