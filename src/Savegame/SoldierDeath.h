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
#include "GameTime.h"

namespace OpenXcom
{

struct BattleUnitKills;

/**
 * Stores info about a soldier's death.
 */
class SoldierDeath
{
private:
	GameTime _time;
	BattleUnitKills *_cause;
public:
	SoldierDeath() : _time(0, 0, 0, 0, 0, 0, 0), _cause(0) {}
	/// Creates a death.
	SoldierDeath(GameTime time, BattleUnitKills *cause);
	/// Cleans up the death.
	~SoldierDeath();
	/// Loads the death from YAML.
	void load(const YAML::Node& node);
	/// Saves the death to YAML.
	YAML::Node save() const;
	/// Gets the death time.
	const GameTime *getTime() const;
	/// Gets the death cause.
	const BattleUnitKills *getCause() const;
};

}
