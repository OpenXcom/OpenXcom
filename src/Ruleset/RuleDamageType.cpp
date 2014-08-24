/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "RuleDamageType.h"
#include "../Engine/Options.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Function converting power to damage.
 * @param power Input power.
 * @return Final damage.
 */
int RuleDamageType::getRandomDamage(int power) const
{
	int dmgRng = 0;
	int randType = RandomType;
	if(randType == DRT_DEFAULT)
	{
		if(ResistType == DT_IN)
			randType = DRT_FIRE;
		else
			randType = (ResistType == DT_HE || Options::TFTDDamage) ? DRT_TFTD : DRT_UFO;
	}

	switch(randType)
	{
	case DRT_UFO: dmgRng = 100; break;
	case DRT_TFTD: dmgRng = 50; break;
	case DRT_FLAT: dmgRng = 0; break;
	case DRT_FIRE: return RNG::generate(5, 10);
	case DRT_NONE: return 0;
	default: return 0;
	}

	int min = power * (100 - dmgRng) / 100;
	int max = power * (100 + dmgRng) / 100;
	return RNG::generate(min, max);
}

/**
 * Is this damage single target.
 * @return True if it can only hit one target.
 */
bool RuleDamageType::isDirect() const
{
	return FixRadius == 0;
}

/**
 * Load rule from YAML.
 * @param node Node with data.
 */
void RuleDamageType::load(const YAML::Node& node)
{
	FixRadius = node["FixRadius"].as<int>(FixRadius);
	RandomType = node["RandomType"].as<int>(RandomType);
	ResistType = (ItemDamageType)node["ResistType"].as<int>(ResistType);
	FireBlastCalc = node["FireBlastCalc"].as<bool>(FireBlastCalc);
	IgnoreArmor = node["IgnoreArmor"].as<bool>(IgnoreArmor);

	ToUnit = node["ToUnit"].as<double>(ToUnit);
	ToItem = node["ToItem"].as<double>(ToItem);
	ToTile = node["ToTile"].as<double>(ToTile);
	ToStun = node["ToStun"].as<double>(ToStun);
	ToRadius = node["ToRadius"].as<double>(ToRadius);

	FireChance = node["FireChance"].as<double>(FireChance);
	SmokeChance = node["SmokeChance"].as<double>(SmokeChance);
}

} //namespace OpenXcom
