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
	_type = node["type"].as<std::string>(_type);
	_spriteSheet = node["spriteSheet"].as<std::string>(_spriteSheet);
	_spriteInv = node["spriteInv"].as<std::string>(_spriteInv);
	_corpseItem = node["corpseItem"].as<std::string>(_corpseItem);
	_storeItem = node["storeItem"].as<std::string>(_storeItem);
	_frontArmor = node["frontArmor"].as<int>(_frontArmor);
	_sideArmor = node["sideArmor"].as<int>(_sideArmor);
	_rearArmor = node["rearArmor"].as<int>(_rearArmor);
	_underArmor = node["underArmor"].as<int>(_underArmor);
	_drawingRoutine = node["drawingRoutine"].as<int>(_drawingRoutine);
	_movementType = (MovementType)node["movementType"].as<int>(_movementType);
	_size = node["size"].as<int>(_size);
	if (const YAML::Node &dmg = node["damageModifier"])
	{
		for (size_t i = 0; i < dmg.size() && i < DAMAGE_TYPES; ++i)
		{
			_damageModifier[i] = dmg[i].as<float>();
		}
	}
	_loftempsSet = node["loftempsSet"].as<std::vector<int>>(_loftempsSet);
	if (const YAML::Node &loftemps = node["loftemps"])
	{
		_loftempsSet.push_back(loftemps.as<int>());
	}
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
