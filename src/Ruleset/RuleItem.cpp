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
#include "RuleItem.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain type of item.
 * @param type String defining the type.
 */
RuleItem::RuleItem(std::string type) : _type(type), _size(0.0), _cost(0), _time(24), _damageType(DT_NONE), _accuracyAuto(0), _accuracySnap(0), _accuracyAimed(0), _battleType(BT_NONE), _twoHanded(false)
{
}

/**
 *
 */
RuleItem::~RuleItem()
{
}

/**
 * Returns the language string that names
 * this item. Each item type has a unique name.
 * @return Item name.
 */
std::string RuleItem::getType() const
{
	return _type;
}

/**
 * Returns the amount of space this item
 * takes up in a storage facility.
 * @return Storage size.
 */
float RuleItem::getSize() const
{
	return _size;
}

/**
 * Changes the amount of space this item
 * takes up in a storage facility.
 * @param size Storage size.
 */
void RuleItem::setSize(float size)
{
	_size = size;
}

/**
 * Returns the amount of money this item
 * costs in purchase/sale.
 * @return Cost.
 */
int RuleItem::getCost() const
{
	return _cost;
}

/**
 * Changes the amount of money this item
 * costs in purchase/sale.
 * @param cost Cost.
 */
void RuleItem::setCost(int cost)
{
	_cost = cost;
}

/**
 * Returns the amount of time this item
 * takes to arrive at a base.
 * @return Time in hours.
 */
int RuleItem::getTransferTime() const
{
	return _time;
}

/**
 * Changes the amount of time this item
 * takes to arrive at a base.
 * @param time Time in hours.
 */
void RuleItem::setTransferTime(int time)
{
	_time = time;
}

/**
 * Returns the reference in BIGOBS.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getBigSprite() const
{
	return _bigSprite;
}

/**
 * Changes the reference in BIGOBS.PCK for use in inventory.
 * @param sprite Sprite reference.
 */
void RuleItem::setBigSprite(int sprite)
{
	_bigSprite = sprite;
	_floorSprite = sprite;
}

/**
 * Returns the reference in FLOOROB.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getFloorSprite() const
{
	return _floorSprite;
}

/**
 * Changes the reference in FLOOROB.PCK for use in inventory.
 * @param sprite Sprite reference.
 */
void RuleItem::setFloorSprite(int sprite)
{
	_floorSprite = sprite;
}

/**
 * Returns the reference in HANDOB.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getHandSprite() const
{
	return _handSprite;
}

/**
 * Changes the reference in HANDOB.PCK for use in inventory.
 * @param sprite Sprite reference.
 */
void RuleItem::setHandSprite(int sprite)
{
	_handSprite = sprite;
}

/**
 * Returns whether this item is held with two hands.
 * @return Is it two-handed?
 */
bool RuleItem::getTwoHanded() const
{
	return _twoHanded;
}

/**
 * Changes whether this item is held with two hands.
 * @param equip Is it two-handed?
 */
void RuleItem::setTwoHanded(bool flag)
{
	_twoHanded = flag;
}

/**
 * Returns the item's bullet sprite reference.
 * @return Sprite reference.
 */
int RuleItem::getBulletSprite() const
{
	return _bulletSprite;
}

/**
 * Changes the item's bulet sprite reference.
 * @param sprite Sprite reference.
 */
void RuleItem::setBulletSprite(int sprite)
{
	_bulletSprite = sprite;
}

/**
 * Returns the item's fire sound.
 * @return Sprite reference.
 */
int RuleItem::getFireSound() const
{
	return _fireSound;
}

/**
 * Changes the item's fire sound..
 * @param sprite Sprite reference.
 */
void RuleItem::setFireSound(int sound)
{
	_fireSound = sound;
}

/**
 * Returns the item's hit sound.
 * @return Sprite reference.
 */
int RuleItem::getHitSound() const
{
	return _hitSound;
}

/**
 * Changes the item's fire sound..
 * @param sprite Sprite reference.
 */
void RuleItem::setHitSound(int sound)
{
	_hitSound = sound;
}

/**
 * Returns the item's hit sound.
 * @return Sprite reference.
 */
int RuleItem::getHitAnimation() const
{
	return _hitAnimation;
}

/**
 * Changes the item's fire sound.
 * @param sprite Sprite reference.
 */
void RuleItem::setHitAnimation(int animation)
{
	_hitAnimation = animation;
}

/**
 * Returns the item's power.
 * @return power Teh powah.
 */
int RuleItem::getPower() const
{
	return _power;
}

/**
 * Sets the item's power.
 * @param power the item's power.
 */
void RuleItem::setPower(int power)
{
	_power = power;
}

/**
 * Returns the item's accuracy for snapshots.
 * @return Accuracy the item's accuracy for snapshots.
 */
int RuleItem::getAccuracySnap() const
{
	return _accuracySnap;
}

/**
 * Sets the item's accuracy for snapshots.
 * @param accuracy item's accuracy for snapshots.
 */
void RuleItem::setAccuracySnap(int accuracy)
{
	_accuracySnap = accuracy;
}

/**
 * Returns the item's accuracy for autoshots.
 * @return Accuracy the item's accuracy for autoshots.
 */
int RuleItem::getAccuracyAuto() const
{
	return _accuracyAuto;
}

/**
 * Sets the item's accuracy for autoshots.
 * @param accuracy item's accuracy for autoshots.
 */
void RuleItem::setAccuracyAuto(int accuracy)
{
	_accuracyAuto = accuracy;
}

/**
 * Returns the item's accuracy for aimed shots.
 * @return Accuracy the item's accuracy for aimed sthos.
 */
int RuleItem::getAccuracyAimed() const
{
	return _accuracyAimed;
}

/**
 * Sets the item's accuracy for aimed shot.
 * @param accuracy item's accuracy for aimed shots.
 */
void RuleItem::setAccuracyAimed(int accuracy)
{
	_accuracyAimed = accuracy;
}


/**
 * Returns a list of compatible ammo.
 * @return pointer to a list of compatible ammo.
 */
std::vector<std::string> *RuleItem::getCompatibleAmmo()
{
	return &_compatibleAmmo;
}

/**
 * Returns the item's damage type.
 * @return damagetype the item's damage type.
 */
ItemDamageType RuleItem::getDamageType() const
{
	return _damageType;
}

/**
 * Sets the item's damage type.
 * @param damagetype the item's damage type.
 */
void RuleItem::setDamageType(ItemDamageType damageType)
{
	_damageType = damageType;
}

/**
 * Returns the item's type.
 * @return type the item's type.
 */
BattleType RuleItem::getBattleType() const
{
	return _battleType;
}

/**
 * Sets the item's type.
 * @param type the item's type.
 */
void RuleItem::setBattleType(BattleType type)
{
	_battleType = type;
}

/**
 * Returns the item's X size.
 * @return the item's X size.
 */
int RuleItem::getSizeX() const
{
	return _sizeX;
}

/**
 * Sets the item's size X.
 * @param size the item's size X.
 */
void RuleItem::setSizeX(int size)
{
	_sizeX = size;
}

/**
 * Returns the item's Y size.
 * @return the item's Y size.
 */
int RuleItem::getSizeY() const
{
	return _sizeY;
}

/**
 * Sets the item's size Y.
 * @param size the item's size Y.
 */
void RuleItem::setSizeY(int size)
{
	_sizeY = size;
}
}
