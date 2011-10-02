/*
 * Copyright 2010 OpenXcom Developers.
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
#ifndef OPENXCOM_XCOMRULESET_H
#define OPENXCOM_XCOMRULESET_H

#include "Ruleset.h"
#include "yaml.h"

namespace OpenXcom
{

/**
 * Rule set for the X-Com: UFO Defense game.
 */
class XcomRuleset : public Ruleset
{
private:
	// loads the name files from config
	void initNames(YAML::Node &doc);
	// loads the countries from config
	void initCountries(YAML::Node &doc);
	// loads the regions from config
	void initRegions(YAML::Node &doc);
	// loads the regions from config
	void initBaseFacilities(YAML::Node &doc);
	// 
	void initMapDataFiles(YAML::Node &doc);
	// initCrafts
	void initCrafts(YAML::Node &doc);
	//
	void initCraftWeapons(YAML::Node &doc);
	//
	void initItems(YAML::Node &doc);
	//
	void initUFOs(YAML::Node &doc);

	//
	RuleTerrain *getTerrainFromYAML(YAML::Iterator &terrains);
public:
	/// Creates the X-Com ruleset.
	XcomRuleset();
	/// Cleans up the X-Com ruleset.
	~XcomRuleset();
	/// Generates a saved game with starting equipment.
	SavedGame *newSave(GameDifficulty diff) const;
};

}

#endif
