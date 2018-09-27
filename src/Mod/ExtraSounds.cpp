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

#include "ExtraSounds.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Engine/FileMap.h"
#include "../Engine/Logger.h"
#include "../Engine/Exception.h"

namespace OpenXcom
{

/**
 * Creates a blank set of extra sound data.
 */
ExtraSounds::ExtraSounds() : _modIndex(0)
{
}

/**
 * Cleans up the extra sound set.
 */
ExtraSounds::~ExtraSounds()
{
}

/**
 * Loads the extra sound set from YAML.
 * @param node YAML node.
 * @param modIndex The internal index of the associated mod.
 */
void ExtraSounds::load(const YAML::Node &node, int modIndex)
{
	_type = node["type"].as<std::string>(_type);
	_sounds = node["files"].as< std::map<int, std::string> >(_sounds);
	_modIndex = modIndex;
}

/**
 * Gets the filename that this sound set represents.
 * @return The sound name.
 */
std::string ExtraSounds::getType() const
{
	return _type;
}

/**
 * Gets the list of sounds defined by this mod.
 * @return The list of sounds defined my this mod.
 */
std::map<int, std::string> *ExtraSounds::getSounds()
{
	return &_sounds;
}

/**
 * Gets the mod index for this external sounds set.
 * @return The mod index for this external sounds set.
 */
int ExtraSounds::getModIndex() const
{
	return _modIndex;
}

/**
 * Loads the external sounds into a new or existing soundset.
 * @param set Existing soundset.
 * @return New soundset.
 */
SoundSet *ExtraSounds::loadSoundSet(SoundSet *set) const
{
	if (set == 0)
	{
		Log(LOG_WARNING) << "Creating new sound set: " << _type << ", this will likely have no in-game use.";
		set = new SoundSet();
	}
	else
	{
		Log(LOG_VERBOSE) << "Adding/Replacing items in sound set: " << _type;
	}
	for (std::map<int, std::string>::const_iterator j = _sounds.begin(); j != _sounds.end(); ++j)
	{
		int startSound = j->first;
		std::string fileName = j->second;
		if (fileName[fileName.length() - 1] == '/')
		{
			Log(LOG_VERBOSE) << "Loading sound set from folder: " << fileName << " starting at index: " << startSound;
			int offset = startSound;
			const std::set<std::string> &contents = FileMap::getVFolderContents(fileName);
			for (std::set<std::string>::iterator k = contents.begin(); k != contents.end(); ++k)
			{
				try
				{
					loadSound(set, offset, fileName + *k);
					offset++;
				}
				catch (Exception &e)
				{
					Log(LOG_WARNING) << e.what();
				}
			}
		}
		else
		{
			loadSound(set, startSound, fileName);
		}
	}
	return set;
}

void ExtraSounds::loadSound(SoundSet *set, int index, const std::string &fileName) const
{
	const std::string &fullPath = FileMap::getFilePath(fileName);
	Sound *sound = set->getSound(index);
	if (sound)
	{
		Log(LOG_VERBOSE) << "Replacing index: " << index;
	}
	else
	{
		Log(LOG_VERBOSE) << "Adding index: " << index;
		sound = set->addSound(index + _modIndex);
	}
	sound->load(fullPath);
}

}
