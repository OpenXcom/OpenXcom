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

#include "ModInfo.h"
#include "CrossPlatform.h"
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

ModInfo::ModInfo(const std::string &path) :
	 _path(path), _name(CrossPlatform::baseFilename(path)),
	_desc("No description."), _version("1.0"), _author("unknown author"),
	_id(_name), _isMaster(false)
{
	// if not specified, assume a UFO mod
	_masters.insert("xcom1");
}

ModInfo::~ModInfo()
{
}

void ModInfo::load(const std::string &filename)
{
	YAML::Node doc = YAML::LoadFile(filename);

	_name     = doc["name"].as<std::string>(_name);
	_desc     = doc["description"].as<std::string>(_desc);
	_version  = doc["version"].as<std::string>(_version);
	_author   = doc["author"].as<std::string>(_author);
	_id       = doc["id"].as<std::string>(_id);
	_isMaster = doc["isMaster"].as<bool>(_isMaster);

	if (_isMaster)
	{
		// masters can't have masters
		_masters.clear();

		// but they can load external resource dirs
		_externalResourceDirs = doc["loadResources"].as< std::vector<std::string> >(_externalResourceDirs);
	}
	else if (doc["masters"].IsDefined())
	{
		_masters.clear();
		std::vector<std::string> masterVector;
		masterVector = doc["masters"].as< std::vector<std::string> >(masterVector);
		_masters.insert(masterVector.begin(), masterVector.end());
	}
	else if (doc["master"].IsDefined())
	{
		_masters.clear();
		_masters.insert(doc["master"].as<std::string>(""));
	}
}

const std::string &ModInfo::getPath()        const { return _path;     }
const std::string &ModInfo::getName()        const { return _name;     }
const std::string &ModInfo::getDescription() const { return _desc;     }
const std::string &ModInfo::getVersion()     const { return _version;  }
const std::string &ModInfo::getAuthor()      const { return _author;   }
const std::string &ModInfo::getId()          const { return _id;       }
bool               ModInfo::isMaster()       const { return _isMaster; }

const std::set<std::string>    &ModInfo::getMasters()              const { return _masters;              }
const std::vector<std::string> &ModInfo::getExternalResourceDirs() const { return _externalResourceDirs; }
}
