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
#include "Armor.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain
 * type of armor.
 * @param type String defining the type.
 * @param spriteSheet Spritesheet used to render the unit.
 */
Armor::Armor(const std::string &type, std::string spriteSheet, int drawingRoutine, MovementType movementType, int size) : _type(type), _spriteSheet(spriteSheet), _spriteInv(""), _corpseItem(""), _storeItem(""), _frontArmor(0), _sideArmor(0), _rearArmor(0), _underArmor(0), _drawingRoutine(drawingRoutine), _movementType(movementType), _size(size)
{
	for (int i=0; i < DAMAGE_TYPES; i++)
		_damageModifier[i] = 1.0;
}

/**
 *
 */
Armor::~Armor()
{

}

/**
 * Loads the armor from a YAML file.
 * @param node YAML node.
 */
void Armor::load(const YAML::Node &node)
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
		else if (key == "spriteInv")
		{
			i.second() >> _spriteInv;
		}
		else if (key == "corpseItem")
		{
			i.second() >> _corpseItem;
		}
		else if (key == "storeItem")
		{
			i.second() >> _storeItem;
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
		else if (key == "damageModifier")
		{
			int dmg = 0;
			for (YAML::Iterator j = i.second().begin(); j != i.second().end(); ++j)
			{
				*j >> _damageModifier[dmg];
				++dmg;
			}
		}
		else if (key == "loftemps")
		{
			int a;
			i.second() >> a;
			_loftempsSet.push_back(a);
		}
		else if (key == "loftempsSet")
		{
			i.second() >> _loftempsSet;
		}
	}
}

/**
 * Saves the armor to a YAML file.
 * @param out YAML emitter.
 */
void Armor::save(YAML::Emitter &out) const
{

	out << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << _type;
	out << YAML::Key << "spriteSheet" << YAML::Value << _spriteSheet;
	out << YAML::Key << "spriteInv" << YAML::Value << _spriteInv;
	out << YAML::Key << "corpseItem" << YAML::Value << _corpseItem;
	out << YAML::Key << "storeItem" << YAML::Value << _storeItem;
	out << YAML::Key << "frontArmor" << YAML::Value << _frontArmor;
	out << YAML::Key << "sideArmor" << YAML::Value << _sideArmor;
	out << YAML::Key << "rearArmor" << YAML::Value << _rearArmor;
	out << YAML::Key << "underArmor" << YAML::Value << _underArmor;
	out << YAML::Key << "drawingRoutine" << YAML::Value << _drawingRoutine;
	out << YAML::Key << "movementType" << YAML::Value << _movementType;
	out << YAML::Key << "size" << YAML::Value << _size;
	out << YAML::Key << "damageModifier" << YAML::Value << YAML::BeginSeq;
	for (int i=0; i < 8; i++)
		out << _damageModifier[i];
	if (_loftempsSet.size() == 1)
	{
		out << YAML::Key << "loftemps" << YAML::Value << _loftempsSet.front();
	}
	else
	{
		out << YAML::Key << "loftempsSet" << YAML::Value << _loftempsSet;
	}
	out << YAML::EndSeq << YAML::EndMap;
}

/**
 * Returns the language string that names
 * this armor. Each armor has a unique name. Coveralls, Power Suit,...
 * @return Armor name.
 */
std::string Armor::getType() const
{
	return _type;
}

/**
 * Gets the unit's sprite sheet.
 * @return Sprite sheet name.
 */
std::string Armor::getSpriteSheet() const
{
	return _spriteSheet;
}

/**
 * Gets the unit's inventory sprite.
 * @return Inventory sprite name.
 */
std::string Armor::getSpriteInventory() const
{
	return _spriteInv;
}

/**
 * Get the front armor level.
 * @return Front armor level.
 */
int Armor::getFrontArmor() const
{
	return _frontArmor;
}

/**
 * Get the side armor level.
 * @return Side armor level. 
 */
int Armor::getSideArmor() const
{
	return _sideArmor;
}

/**
 * get the rear armor level.
 * @return Rear armor level. 
 */
int Armor::getRearArmor() const
{
	return _rearArmor;
}

/**
 * get the under armor level.
 * @return Under armor level. 
 */
int Armor::getUnderArmor() const
{
	return _underArmor;
}


/**
 * Get the corpse item.
 * @return Name of the corpse item.
 */
std::string Armor::getCorpseItem() const
{
	return _corpseItem;
}

/**
 * Get the storage item needed to equip this.
 * @return Name of the store item.
 */
std::string Armor::getStoreItem() const
{
	return _storeItem;
}

/**
 * get the drawing routine ID.
 * @return The drawing routine ID.
 */
int Armor::getDrawingRoutine() const
{
	return _drawingRoutine;
}

/**
 * Get whether the armor can fly.
 * @return Movementtype
 */
MovementType Armor::getMovementType() const
{
	return _movementType;
}

/**
 * Get the size of the unit. Normally this is 1(small) or 2(big).
 * @return size
 */
int Armor::getSize() const
{
	return _size;
}

/**
 * Gets damage modifier for a certain damage type.
 * @param dt DamageType
 * @return damage modifier 0->1
 */
float Armor::getDamageModifier(ItemDamageType dt)
{
	return _damageModifier[(int)dt];
}

/** Gets loftempSet
 * @return loftempsSet
 */
std::vector<int> Armor::getLoftempsSet() const
{
	return _loftempsSet;
}

}
