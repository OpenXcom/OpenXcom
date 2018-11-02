#pragma once
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
#include <yaml-cpp/yaml.h>

namespace OpenXcom
{

class MapDataSet;
/**
 * An MCD data Patch.
 */
class MCDPatch
{
private:
	std::vector<std::pair<size_t, int> > _bigWalls, _TUWalks, _TUFlys, _TUSlides, _deathTiles, _terrainHeight, _specialTypes, _armors, _explosives, _flammabilities, _fuels, _HEBlocks, _footstepSounds, _objectTypes;
	std::vector<std::pair<size_t, bool> > _noFloors, _stopLOSses;
	std::vector<std::pair<size_t, std::vector<int> > > _LOFTS;
public:
	/// Creates an MCD Patch.
	MCDPatch();
	/// Cleans up the MCD Patch.
	~MCDPatch();
	/// Loads the MCD Patch from YAML.
	void load(const YAML::Node& node);
	/// Applies an MCD patch to a mapDataSet
	void modifyData(MapDataSet *dataSet) const;
};

}
