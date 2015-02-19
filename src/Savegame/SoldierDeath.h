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
#ifndef OPENXCOM_SOLDIERDEATH_H
#define OPENXCOM_SOLDIERDEATH_H

#include <yaml-cpp/yaml.h>
#include "GameTime.h"

namespace OpenXcom
{

class GameTime;

/**
 * Stores info about a soldier's death.
 */
class SoldierDeath
{
private:
	GameTime _time;
public:
	/// Creates a death.
	SoldierDeath();
	/// Cleans up the death.
	~SoldierDeath();
	/// Loads the death from YAML.
	void load(const YAML::Node& node);
	/// Saves the death to YAML.
	YAML::Node save() const;
	/// Gets the death time.
	const GameTime *getTime() const;
	/// Sets the death time.
	void setTime(GameTime time);
};

}

#endif
