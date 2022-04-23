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

#include "../version.h"
#include "ModInfo.h"
#include "CrossPlatform.h"
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

ModInfo::ModInfo(const std::string &path) :
	 _path(path), _name(CrossPlatform::baseFilename(path)),
	_desc("No description."), _version("1.0"), _author("unknown author"),
	_id(_name), _master("xcom1"), _isMaster(false), _reservedSpace(1),
	_engineOk(false)
{
	// empty
}

namespace
{

struct EngineData
{
	std::string name;
	int version[4];
};

/**
 * List of engines that current version support.
 */
const EngineData supportedEngines[] = {
	{ OPENXCOM_VERSION_ENGINE, { OPENXCOM_VERSION_NUMBER }},
	{ "", { 0, 0, 0, 0 } }, // assume that every engine support mods from base game, remove if its not true.
};

template<int I>
bool findCompatibleEngine(const EngineData (&l)[I], const std::string& v)
{
	for (int i = 0; i < I; ++i)
	{
		if (l[i].name == v)
		{
			(void)l[i].version; //TODO: add check for version
			return true;
		}
	}
	return false;
}

} //namespace

void ModInfo::load(const std::string &filename)
{
	const YAML::Node doc = YAML::LoadFile(filename);

	_name     = doc["name"].as<std::string>(_name);
	_desc     = doc["description"].as<std::string>(_desc);
	_version  = doc["version"].as<std::string>(_version);
	_author   = doc["author"].as<std::string>(_author);
	_id       = doc["id"].as<std::string>(_id);
	_isMaster = doc["isMaster"].as<bool>(_isMaster);
	_reservedSpace = doc["reservedSpace"].as<int>(_reservedSpace);
	if (const YAML::Node& req = doc["requiredExtendedVersion"])
	{
		_requiredExtendedVersion = req.as<std::string>(_requiredExtendedVersion);
		_requiredExtendedEngine = "Extended"; //for backward compatibility
	}
	_requiredExtendedEngine = doc["requiredExtendedEngine"].as<std::string>(_requiredExtendedEngine);

	_engineOk = findCompatibleEngine(supportedEngines, _requiredExtendedEngine);

	if (_reservedSpace < 1)
	{
		_reservedSpace = 1;
	}
	else if (_reservedSpace > 100)
	{
		_reservedSpace = 100;
	}

	if (_isMaster)
	{
		// default a master's master to none.  masters can still have
		// masters, but they must be explicitly declared.
		_master = "";
		// only masters can load external resource dirs
		_externalResourceDirs = doc["loadResources"].as< std::vector<std::string> >(_externalResourceDirs);
		// or basic resource definition
		_resourceConfigFile = doc["resourceConfig"].as<std::string>(_resourceConfigFile);
	}

	_master = doc["master"].as<std::string>(_master);
	if (_master == "*")
	{
		_master = "";
	}
}

const std::string &ModInfo::getPath()                    const { return _path;                    }
const std::string &ModInfo::getName()                    const { return _name;                    }
const std::string &ModInfo::getDescription()             const { return _desc;                    }
const std::string &ModInfo::getVersion()                 const { return _version;                 }
const std::string &ModInfo::getAuthor()                  const { return _author;                  }
const std::string &ModInfo::getId()                      const { return _id;                      }
const std::string &ModInfo::getMaster()                  const { return _master;                  }
bool               ModInfo::isMaster()                   const { return _isMaster;                }
bool               ModInfo::isEngineOk()                 const { return _engineOk;                }
const std::string &ModInfo::getRequiredExtendedEngine()  const { return _requiredExtendedEngine;  }
const std::string &ModInfo::getRequiredExtendedVersion() const { return _requiredExtendedVersion; }
const std::string &ModInfo::getResourceConfigFile()      const { return _resourceConfigFile;      }
int                ModInfo::getReservedSpace()           const { return _reservedSpace;           }

/**
 * Checks if a given mod can be activated.
 * It must either be:
 * - a Master mod
 * - a standalone mod (no master)
 * - depend on the current Master mod
 * @param curMaster Id of the active master mod.
 * @return True if it's activable, false otherwise.
*/
bool ModInfo::canActivate(const std::string &curMaster) const
{
	return (isMaster() || getMaster().empty() || getMaster() == curMaster);
}


const std::vector<std::string> &ModInfo::getExternalResourceDirs() const { return _externalResourceDirs; }
}
