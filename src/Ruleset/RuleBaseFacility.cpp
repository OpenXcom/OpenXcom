/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "RuleBaseFacility.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of base facility.
 * @param type String defining the type.
 */
RuleBaseFacility::RuleBaseFacility(const std::string &type) : _type(type), _requires(), _spriteShape(-1), _spriteFacility(-1), _lift(false), _hyper(false), _mind(false), _grav(false), _size(1), _buildCost(0), _buildTime(0), _monthlyCost(0), _storage(0), _personnel(0), _aliens(0), _crafts(0), _labs(0), _workshops(0), _psiLabs(0), _radarRange(0), _radarChance(0), _defense(0), _hitRatio(0), _fireSound(0), _hitSound(0)
{
}

/**
 *
 */
RuleBaseFacility::~RuleBaseFacility()
{
}

/**
 * Loads the base facility type from a YAML file.
 * @param node YAML node.
 */
void RuleBaseFacility::load(const YAML::Node &node)
{
	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		else if (key == "requires")
		{
			i.second() >> _requires;
		}
		else if (key == "spriteShape")
		{
			i.second() >> _spriteShape;
		}
		else if (key == "spriteFacility")
		{
			i.second() >> _spriteFacility;
		}
		else if (key == "lift")
		{
			i.second() >> _lift;
		}
		else if (key == "hyper")
		{
			i.second() >> _hyper;
		}
		else if (key == "mind")
		{
			i.second() >> _mind;
		}
		else if (key == "grav")
		{
			i.second() >> _grav;
		}
		else if (key == "size")
		{
			i.second() >> _size;
		}
		else if (key == "buildCost")
		{
			i.second() >> _buildCost;
		}
		else if (key == "buildTime")
		{
			i.second() >> _buildTime;
		}
		else if (key == "monthlyCost")
		{
			i.second() >> _monthlyCost;
		}
		else if (key == "storage")
		{
			i.second() >> _storage;
		}
		else if (key == "personnel")
		{
			i.second() >> _personnel;
		}
		else if (key == "aliens")
		{
			i.second() >> _aliens;
		}
		else if (key == "crafts")
		{
			i.second() >> _crafts;
		}
		else if (key == "labs")
		{
			i.second() >> _labs;
		}
		else if (key == "workshops")
		{
			i.second() >> _workshops;
		}
		else if (key == "psiLabs")
		{
			i.second() >> _psiLabs;
		}
		else if (key == "radarRange")
		{
			i.second() >> _radarRange;
		}
		else if (key == "radarChance")
		{
			i.second() >> _radarChance;
		}
		else if (key == "defense")
		{
			i.second() >> _defense;
		}
		else if (key == "hitRatio")
		{
			i.second() >> _hitRatio;
		}
		else if (key == "fireSound")
		{
			i.second() >> _fireSound;
		}
		else if (key == "hitSound")
		{
			i.second() >> _hitSound;
		}
		else if (key == "mapName")
		{
			i.second() >> _mapName;
		}
	}
}

/**
 * Saves the base facility type to a YAML file.
 * @param out YAML emitter.
 */
void RuleBaseFacility::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "requires" << YAML::Value << _requires;
	out << YAML::Key << "spriteShape" << YAML::Value << _spriteShape;
	out << YAML::Key << "spriteFacility" << YAML::Value << _spriteFacility;
	out << YAML::Key << "lift" << YAML::Value << _lift;
	out << YAML::Key << "hyper" << YAML::Value << _hyper;
	out << YAML::Key << "mind" << YAML::Value << _mind;
	out << YAML::Key << "grav" << YAML::Value << _grav;
	out << YAML::Key << "size" << YAML::Value << _size;
	out << YAML::Key << "buildCost" << YAML::Value << _buildCost;
	out << YAML::Key << "buildTime" << YAML::Value << _buildTime;
	out << YAML::Key << "monthlyCost" << YAML::Value << _monthlyCost;
	out << YAML::Key << "storage" << YAML::Value << _storage;
	out << YAML::Key << "personnel" << YAML::Value << _personnel;
	out << YAML::Key << "aliens" << YAML::Value << _aliens;
	out << YAML::Key << "crafts" << YAML::Value << _crafts;
	out << YAML::Key << "labs" << YAML::Value << _labs;
	out << YAML::Key << "workshops" << YAML::Value << _workshops;
	out << YAML::Key << "psiLabs" << YAML::Value << _psiLabs;
	out << YAML::Key << "radarRange" << YAML::Value << _radarRange;
	out << YAML::Key << "radarChance" << YAML::Value << _radarChance;
	out << YAML::Key << "defense" << YAML::Value << _defense;
	out << YAML::Key << "hitRatio" << YAML::Value << _hitRatio;
	out << YAML::Key << "fireSound" << YAML::Value << _fireSound;
	out << YAML::Key << "hitSound" << YAML::Value << _hitSound;
	out << YAML::Key << "mapName" << YAML::Value << _mapName;
	out << YAML::EndMap;
}

/**
 * Returns the language string that names
 * this base facility. Each base facility type
 * has a unique name.
 * @return Facility name.
 */
std::string RuleBaseFacility::getType() const
{
	return _type;
}

/**
 * Returns the list of research required to
 * build this base facility.
 * @return List of research IDs..
 */
const std::vector<std::string> &RuleBaseFacility::getRequirements() const
{
	return _requires;
}

/**
 * Returns the ID of the sprite used to draw the
 * base structure of the facility that defines its shape.
 * @return Sprite ID.
 */
int RuleBaseFacility::getSpriteShape() const
{
	return _spriteShape;
}

/**
 * Returns the ID of the sprite used to draw the
 * facility's contents inside the base shape.
 * @return Sprite ID.
 */
int RuleBaseFacility::getSpriteFacility() const
{
	return _spriteFacility;
}

/**
 * Returns the size of the facility on the base grid.
 * @return Length in grid squares.
 */
int RuleBaseFacility::getSize() const
{
	return _size;
}

/**
 * Returns whether this facility is the core access lift
 * of a base. Every base has an access lift and all
 * facilities have to be connected to it.
 * @return True if it's a lift, False otherwise.
 */
bool RuleBaseFacility::isLift() const
{
	return _lift;
}

/**
 * Returns whether this facility has hyperwave detection
 * capabilities. This allows it to get extra details about UFOs.
 * @return Hyperwave flag.
 */
bool RuleBaseFacility::isHyperwave() const
{
	return _hyper;
}

/**
 * Returns whether this facility has a mind shield,
 * which covers your base from alien detection.
 * @return Mind Shield flag.
 */
bool RuleBaseFacility::isMindShield() const
{
	return _mind;
}

/**
 * Returns whether this facility has a grav shield,
 * which doubles base defense's fire ratio.
 * @return Grav Shield flag.
 */
bool RuleBaseFacility::isGravShield() const
{
	return _grav;
}

/**
 * Returns the amount of funds that this facility costs
 * to build on a base.
 * @return Building cost.
 */
int RuleBaseFacility::getBuildCost() const
{
	return _buildCost;
}

/**
 * Returns the amount of time that this facility takes
 * to be constructed since placement.
 * @return Time in days.
 */
int RuleBaseFacility::getBuildTime() const
{
	return _buildTime;
}

/**
 * Returns the amount of funds this facility costs monthly
 * to maintain once it's fully built.
 * @return Monthly cost.
 */
int RuleBaseFacility::getMonthlyCost() const
{
	return _monthlyCost;
}

/**
 * Returns the amount of storage space this facility provides
 * for base equipment.
 * @return Storage space.
 */
int RuleBaseFacility::getStorage() const
{
	return _storage;
}

/**
 * Returns the amount of base personnel (soldiers, scientists,
 * engineers) const this facility can contain.
 * @return Amount of personnel.
 */
int RuleBaseFacility::getPersonnel() const
{
	return _personnel;
}

/**
 * Returns the amount of captured live aliens this facility
 * can contain.
 * @return Amount of aliens.
 */
int RuleBaseFacility::getAliens() const
{
	return _aliens;
}

/**
 * Returns the amount of base craft this facility can contain.
 * @return Amount of craft.
 */
int RuleBaseFacility::getCrafts() const
{
	return _crafts;
}

/**
 * Returns the amount of laboratory space this facility provides
 * for research projects.
 * @return Laboratory space.
 */
int RuleBaseFacility::getLaboratories() const
{
	return _labs;
}

/**
 * Returns the amount of workshop space this facility provides
 * for manufacturing projects.
 * @return Workshop space.
 */
int RuleBaseFacility::getWorkshops() const
{
	return _workshops;
}

/**
 * Returns the amount of soldiers this facility can contain
 * for monthly psi-training.
 * @return Amount of soldiers.
 */
int RuleBaseFacility::getPsiLaboratories() const
{
	return _psiLabs;
}

/**
 * Returns the radar range this facility provides for the
 * detection of UFOs.
 * @return Range in nautical miles.
 */
int RuleBaseFacility::getRadarRange() const
{
	return _radarRange;
}

/**
 * Returns the chance of UFOs that come within the facility's
 * radar range to be detected.
 * @return Chance in percentage.
 */
int RuleBaseFacility::getRadarChance() const
{
	return _radarChance;
}

/**
 * Returns the defense value of this facility's weaponry
 * against UFO invasions on the base.
 * @return Defense value.
 */
int RuleBaseFacility::getDefenseValue() const
{
	return _defense;
}

/**
 * Returns the hit ratio of this facility's weaponry
 * against UFO invasions on the base.
 * @return Ratio in percentage.
 */
int RuleBaseFacility::getHitRatio() const
{
	return _hitRatio;
}

/**
 * Returns the battlescape map block name for this facility
 * to construct the base defense mission map.
 * @return Map name.
 */
std::string RuleBaseFacility::getMapName() const
{
	return _mapName;
}

/**
 * Returns the hit sound of this facility's weaponry
 * @return Sound index number.
 */
int RuleBaseFacility::getHitSound() const
{
	return _hitSound;
}

/**
 * Returns the fire sound of this facility's weaponry
 * @return Sound index number.
 */
int RuleBaseFacility::getFireSound() const
{
	return _fireSound;
}
}
