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
 * Default constructor
 */
RuleDamageType::RuleDamageType() :
	FixRadius(0), RandomType(DRT_DEFAULT), ResistType(DT_AP), FireBlastCalc(false),
	IgnoreDirection(false), IgnoreSelfDestruct(false), IgnorePainImmunity(false),
	ArmorEffectiveness(1.0f), RadiusEffectiveness(0.0f),
	ToHealth(1.0f), ToArmor(0.1f), ToWound(1.0f), ToItem(0.0f), ToTile(0.5f), ToStun(0.25f)
{

}
/**
 * Function converting power to damage.
 * @param power Input power.
 * @return Random damage based on power.
 */
int RuleDamageType::getRandomDamage(int power) const
{
	int dmgRng = 0;
	ItemDamageRandomType randType = RandomType;
	if (randType == DRT_DEFAULT)
	{
		switch (ResistType)
		{
		case DT_IN: randType = DRT_FIRE; break;
		case DT_HE: randType = DRT_TFTD; break;
		case DT_SMOKE: randType = DRT_NONE; break;
		default: randType = Options::TFTDDamage ? DRT_TFTD : DRT_UFO; break;
		}
	}

	switch (randType)
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
	RandomType = (ItemDamageRandomType)node["RandomType"].as<int>(RandomType);
	ResistType = (ItemDamageType)node["ResistType"].as<int>(ResistType);
	FireBlastCalc = node["FireBlastCalc"].as<bool>(FireBlastCalc);
	IgnoreDirection = node["IgnoreDirection"].as<bool>(IgnoreDirection);
	IgnoreSelfDestruct = node["IgnoreSelfDestruct"].as<bool>(IgnoreSelfDestruct);
	IgnorePainImmunity = node["IgnorePainImmunity"].as<bool>(IgnorePainImmunity);
	ArmorEffectiveness = node["ArmorEffectiveness"].as<float>(ArmorEffectiveness);
	RadiusEffectiveness = node["RadiusEffectiveness"].as<float>(RadiusEffectiveness);

	ToHealth = node["ToHealth"].as<float>(ToHealth);
	ToArmor = node["ToArmor"].as<float>(ToArmor);
	ToWound = node["ToWound"].as<float>(ToWound);
	ToItem = node["ToItem"].as<float>(ToItem);
	ToTile = node["ToTile"].as<float>(ToTile);
	ToStun = node["ToStun"].as<float>(ToStun);
}

} //namespace OpenXcom
