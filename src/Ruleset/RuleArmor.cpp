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
#include "RuleArmor.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of armor.
 * @param type String defining the type.
 * @param spriteSheet Spritesheet used to render the unit.
 */
RuleArmor::RuleArmor(const std::string &type, std::string spriteSheet, int drawingRoutine, MovementType movementType, int size) : _type(type), _spriteSheet(spriteSheet), _corpseItem(""), _frontArmor(0), _sideArmor(0), _rearArmor(0), _underArmor(0), _drawingRoutine(drawingRoutine), _movementType(movementType), _size(size)
{

}

/**
 *
 */
RuleArmor::~RuleArmor()
{

}

/**
 * Loads the armor from a YAML file.
 * @param node YAML node.
 */
void RuleArmor::load(const YAML::Node &node)
{
	int a = 0;

	for (YAML::Iterator i = node.begin(); i != node.end(); ++i)
	{
		std::string key;
		i.first() >> key;
		if (key == "type")
		{
			i.second() >> _type;
		}
		else if (key == "spriteSheet")
		{
			i.second() >> _spriteSheet;
		}
		else if (key == "corpseItem")
		{
			i.second() >> _corpseItem;
		}
		else if (key == "frontArmor")
		{
			i.second() >> _frontArmor;
		}
		else if (key == "sideArmor")
		{
			i.second() >> _sideArmor;
		}
		else if (key == "rearArmor")
		{
			i.second() >> _rearArmor;
		}
		else if (key == "underArmor")
		{
			i.second() >> _underArmor;
		}
		else if (key == "drawingRoutine")
		{
			i.second() >> _drawingRoutine;
		}
		else if (key == "movementType")
		{
			i.second() >> a;
			_movementType = (MovementType)a;
		}
		else if (key == "size")
		{
			i.second() >> _size;
		}
	}
}

/**
 * Saves the armor to a YAML file.
 * @param out YAML emitter.
 */
void RuleArmor::save(YAML::Emitter &out) const
{

	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "spriteSheet" << YAML::Value << _spriteSheet;
	out << YAML::Key << "corpseItem" << YAML::Value << _corpseItem;
	out << YAML::Key << "frontArmor" << YAML::Value << _frontArmor;
	out << YAML::Key << "sideArmor" << YAML::Value << _sideArmor;
	out << YAML::Key << "rearArmor" << YAML::Value << _rearArmor;
	out << YAML::Key << "underArmor" << YAML::Value << _underArmor;
	out << YAML::Key << "drawingRoutine" << YAML::Value << _drawingRoutine;
	out << YAML::Key << "movementType" << YAML::Value << _movementType;
	out << YAML::Key << "size" << YAML::Value << _size;
	out << YAML::EndMap;
}

/**
 * Returns the language string that names
 * this armor. Each armor has a unique name. Coveralls, Power Suit,...
 * @return Armor name.
 */
std::string RuleArmor::getType() const
{
	return _type;
}

/// Gets the unit's sprite sheet.
std::string RuleArmor::getSpriteSheet() const
{
	return _spriteSheet;
}

/// Sets the different armor levels.
void RuleArmor::setArmor(int front, int side, int rear, int under)
{
	_frontArmor = front;
	_sideArmor = side;
	_rearArmor = rear;
	_underArmor = under;
}

/// Get the front armor level.
int RuleArmor::getFrontArmor() const
{
	return _frontArmor;
}
/// Get the side armor level.
int RuleArmor::getSideArmor() const
{
	return _sideArmor;
}
/// get the rear armor level.
int RuleArmor::getRearArmor() const
{
	return _rearArmor;
}
/// get the under armor level.
int RuleArmor::getUnderArmor() const
{
	return _underArmor;
}
/// Set the corpse item.
void RuleArmor::setCorpseItem(const std::string &corpseItem)
{
	_corpseItem = corpseItem;
}
/// Get the corpse item.
std::string RuleArmor::getCorpseItem() const
{
	return _corpseItem;
}
/// get the drawing routine ID.
int RuleArmor::getDrawingRoutine() const
{
	return _drawingRoutine;
}
/// Get whether the armor can fly.
MovementType RuleArmor::getMovementType() const
{
	return _movementType;
}

/// Get whether the armor can fly.
int RuleArmor::getSize() const
{
	return _size;
}

}
