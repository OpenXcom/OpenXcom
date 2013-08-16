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
#include "RuleItem.h"
#include "RuleInventory.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Surface.h"

namespace OpenXcom
{

/**
 * Creates a blank ruleset for a certain type of item.
 * @param type String defining the type.
 */
RuleItem::RuleItem(const std::string &type) : _type(type), _name(type), _size(0.0), _costBuy(0), _costSell(0), _transferTime(24), _weight(999), _bigSprite(0), _floorSprite(-1), _handSprite(120), _bulletSprite(-1),
											_fireSound(-1), _hitSound(-1), _hitAnimation(0), _power(0), _compatibleAmmo(), _damageType(DT_NONE),
											_accuracyAuto(0), _accuracySnap(0), _accuracyAimed(0), _tuAuto(0), _tuSnap(0), _tuAimed(0), _clipSize(0), _accuracyMelee(0), _tuMelee(0),
											_battleType(BT_NONE), _twoHanded(false), _waypoint(false), _fixedWeapon(false), _invWidth(1), _invHeight(1),
											_painKiller(0), _heal(0), _stimulant(0), _healAmount(0), _healthAmount(0), _stun(0), _energy(0), _tuUse(0), _recoveryPoints(0), _armor(20), _turretType(-1),
											_recover(true), _liveAlien(false), _blastRadius(-1), _attraction(0), _flatRate(false), _arcingShot(false), _listOrder(0), _range(0), _bulletSpeed(0)
{
}

/**
 *
 */
RuleItem::~RuleItem()
{
}

/**
 * Loads the item from a YAML file.
 * @param node YAML node.
 * @param modIndex offsets the sounds and sprite values to avoid conflicts.
 * @param listOrder the list weight for this item.
 */
void RuleItem::load(const YAML::Node &node, int modIndex, int listOrder)
{
	_type = node["type"].as<std::string>(_type);
	_name = node["name"].as<std::string>(_name);
	_requires = node["requires"].as<std::vector<std::string>>(_requires);
	_size = node["size"].as<float>(_size);
	_costBuy = node["costBuy"].as<int>(_costBuy);
	_costSell = node["costSell"].as<int>(_costSell);
	_transferTime = node["transferTime"].as<int>(_transferTime);
	_weight = node["weight"].as<int>(_weight);
	_bigSprite = node["bigSprite"].as<int>(_bigSprite);
	// BIGOBS.PCK: 57 entries
	if (_bigSprite > 56)
		_bigSprite += modIndex;
	_floorSprite = node["floorSprite"].as<int>(_floorSprite);
	// FLOOROB.PCK: 73 entries
	if (_floorSprite > 72)
		_floorSprite += modIndex;
	_handSprite = node["handSprite"].as<int>(_handSprite);
	// HANDOBS.PCK: 128 entries
	if (_handSprite > 127)
		_handSprite += modIndex;
	_bulletSprite = node["bulletSprite"].as<int>(_bulletSprite);
	// Projectiles: 385 entries ((105*33) / (3*3)) (35 sprites per projectile(0-34), 11 projectiles (0-10))
	_bulletSprite *= 35;
	if (_bulletSprite >= 385)
		_bulletSprite += modIndex;
	_fireSound = node["fireSound"].as<int>(_fireSound);
	// BATTLE.CAT: 55 entries
	if (_fireSound > 54)
		_fireSound += modIndex;
	_hitSound = node["hitSound"].as<int>(_hitSound);
	// BATTLE.CAT: 55 entries
	if (_hitSound > 54)
		_hitSound += modIndex;
	_hitAnimation = node["hitAnimation"].as<int>(_hitAnimation);
	// SMOKE.PCK: 56 entries
	if (_hitAnimation > 55)
		_hitAnimation += modIndex;
	_power = node["power"].as<int>(_power);
	_compatibleAmmo = node["compatibleAmmo"].as<std::vector<std::string>>(_compatibleAmmo);
	_damageType = (ItemDamageType)node["damageType"].as<int>(_damageType);
	_accuracyAuto = node["accuracyAuto"].as<int>(_accuracyAuto);
	_accuracySnap = node["accuracySnap"].as<int>(_accuracySnap);
	_accuracyAimed = node["accuracyAimed"].as<int>(_accuracyAimed);
	_tuAuto = node["tuAuto"].as<int>(_tuAuto);
	_tuSnap = node["tuSnap"].as<int>(_tuSnap);
	_tuAimed = node["tuAimed"].as<int>(_tuAimed);
	_clipSize = node["clipSize"].as<int>(_clipSize);
	_accuracyMelee = node["accuracyMelee"].as<int>(_accuracyMelee);
	_tuMelee = node["tuMelee"].as<int>(_tuMelee);
	_battleType = (BattleType)node["battleType"].as<int>(_battleType);
	_twoHanded = node["twoHanded"].as<bool>(_twoHanded);
	_waypoint = node["waypoint"].as<bool>(_waypoint);
	_fixedWeapon = node["fixedWeapon"].as<bool>(_fixedWeapon);
	_invWidth = node["invWidth"].as<int>(_invWidth);
	_invHeight = node["invHeight"].as<int>(_invHeight);
	_painKiller = node["painKiller"].as<int>(_painKiller);
	_heal = node["heal"].as<int>(_heal);
	_stimulant = node["stimulant"].as<int>(_stimulant);
	_healAmount = node["healAmount"].as<int>(_healAmount);
	_healthAmount = node["healthAmount"].as<int>(_healthAmount);
	_stun = node["stun"].as<int>(_stun);
	_energy = node["energy"].as<int>(_energy);
	_tuUse = node["tuUse"].as<int>(_tuUse);
	_recoveryPoints = node["recoveryPoints"].as<int>(_recoveryPoints);
	_armor = node["armor"].as<int>(_armor);
	_turretType = node["turretType"].as<int>(_turretType);
	_recover = node["recover"].as<bool>(_recover);
	_liveAlien = node["liveAlien"].as<bool>(_liveAlien);
	_blastRadius = node["blastRadius"].as<int>(_blastRadius);
	_attraction = node["attraction"].as<int>(_attraction);
	_flatRate = node["flatRate"].as<bool>(_flatRate);
	_arcingShot = node["arcingShot"].as<bool>(_arcingShot);
	_listOrder = node["listOrder"].as<int>(_listOrder);
	_range = node["maxRange"].as<int>(_range);
	_bulletSpeed = node["bulletSpeed"].as<int>(_bulletSpeed);
	if (!_listOrder)
	{
		_listOrder = listOrder;
	}
}

/**
 * Returns the item type. Each item has a unique type.
 * @return Item name.
 */
std::string RuleItem::getType() const
{
	return _type;
}

/**
 * Returns the language string that names
 * this item. This is not necessarily unique.
 * @return Item name.
 */
std::string RuleItem::getName() const
{
	return _name;
}

/**
 * Returns the list of research required to
 * use this item.
 * @return List of research IDs.
 */
const std::vector<std::string> &RuleItem::getRequirements() const
{
	return _requires;
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
 * Returns the amount of money this item
 * costs to purchase (0 if not purchasable).
 * @return Cost.
 */
int RuleItem::getBuyCost() const
{
	return _costBuy;
}

/**
 * Returns the amount of money this item
 * is worth to sell.
 * @return Cost.
 */
int RuleItem::getSellCost() const
{
	return _costSell;
}

/**
 * Returns the amount of time this item
 * takes to arrive at a base.
 * @return Time in hours.
 */
int RuleItem::getTransferTime() const
{
	return _transferTime;
}

/**
 * Returns the weight of the item.
 * @return Weight in strength units.
 */
int RuleItem::getWeight() const
{
	return _weight;
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
 * Returns the reference in FLOOROB.PCK for use in inventory.
 * @return Sprite reference.
 */
int RuleItem::getFloorSprite() const
{
	return _floorSprite;
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
 * Returns whether this item is held with two hands.
 * @return Is it two-handed?
 */
bool RuleItem::isTwoHanded() const
{
	return _twoHanded;
}

/**
 * Returns whether this uses waypoints.
 * @return Uses waypoints?
 */
bool RuleItem::isWaypoint() const
{
	return _waypoint;
}

/**
 * Returns whether this item is a fixed weapon.
 * You can't move/throw/drop fixed weapons - ie. HWP turrets.
 * @return Is it fixed weapon?
 */
bool RuleItem::isFixed() const
{
	return _fixedWeapon;
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
 * Returns the item's fire sound.
 * @return Sound id.
 */
int RuleItem::getFireSound() const
{
	return _fireSound;
}

/**
 * Returns the item's hit sound.
 * @return Sound id.
 */
int RuleItem::getHitSound() const
{
	return _hitSound;
}

/**
 * Returns the item's hit sound.
 * @return Sound id.
 */
int RuleItem::getHitAnimation() const
{
	return _hitAnimation;
}

/**
 * Returns the item's power.
 * @return Teh powah.
 */
int RuleItem::getPower() const
{
	return _power;
}

/**
 * Returns the item's accuracy for snapshots.
 * @return item's accuracy for snapshots.
 */
int RuleItem::getAccuracySnap() const
{
	return _accuracySnap;
}

/**
 * Returns the item's accuracy for autoshots.
 * @return item's accuracy for autoshots.
 */
int RuleItem::getAccuracyAuto() const
{
	return _accuracyAuto;
}

/**
 * Returns the item's accuracy for aimed shots.
 * @return item's accuracy for aimed shots.
 */
int RuleItem::getAccuracyAimed() const
{
	return _accuracyAimed;
}

/**
 * Returns the item's accuracy for melee.
 * @return item's accuracy for melee.
 */
int RuleItem::getAccuracyMelee() const
{
	return _accuracyMelee;
}

/**
 * Returns the item's time unit percentage for snapshots.
 * @return item's time unit percentage for snapshots.
 */
int RuleItem::getTUSnap() const
{
	return _tuSnap;
}

/**
 * Returns the item's time unit percentage for autoshots.
 * @return item's time unit percentage for autoshots.
 */
int RuleItem::getTUAuto() const
{
	return _tuAuto;
}

/**
 * Returns the item's time unit percentage for aimed shots.
 * @return item's time unit percentage for aimed shots.
 */
int RuleItem::getTUAimed() const
{
	return _tuAimed;
}

/**
 * Returns the item's time unit percentage for melee.
 * @return item's time unit percentage for melee.
 */
int RuleItem::getTUMelee() const
{
	return _tuMelee;
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
 * Returns the item's type.
 * @return type the item's type.
 */
BattleType RuleItem::getBattleType() const
{
	return _battleType;
}

/**
 * Returns the item's width in a soldier's inventory.
 * @return the item's width.
 */
int RuleItem::getInventoryWidth() const
{
	return _invWidth;
}

/**
 * Returns the item's height in a soldier's inventory.
 * @return the item's height.
 */
int RuleItem::getInventoryHeight() const
{
	return _invHeight;
}

/**
 * Returns the item's ammo clip size.
 * @return the item's ammo clip size.
 */
int RuleItem::getClipSize() const
{
	return _clipSize;
}

/**
 * Draws and centers the hand sprite on a surface
 * according to its dimensions.
 * @param texture Pointer to surface set to get the sprite from.
 * @param surface Pointer to surface to draw to.
 */
void RuleItem::drawHandSprite(SurfaceSet *texture, Surface *surface) const
{
	Surface *frame = texture->getFrame(this->getBigSprite());
	frame->setX((RuleInventory::HAND_W - this->getInventoryWidth()) * RuleInventory::SLOT_W/2);
	frame->setY((RuleInventory::HAND_H - this->getInventoryHeight()) * RuleInventory::SLOT_H/2);
	texture->getFrame(this->getBigSprite())->blit(surface);
}

/**
 * Get the heal quantity of item
 * @return The new heal quantity
 */
int RuleItem::getHealQuantity () const
{
	return _heal;
}

/**
 * Get the pain killer quantity of item
 * @return The new pain killer quantity
 */
int RuleItem::getPainKillerQuantity () const
{
	return _painKiller;
}

/**
 * Get the stimulant quantity of item
 * @return The new stimulant quantity
 */
int RuleItem::getStimulantQuantity () const
{
	return _stimulant;
}

/**
 * Get the amount of fatal wound healed per usage
 * @return The amount of fatal wound healed
 */
int RuleItem::getHealAmount () const
{
	return _healAmount;
}

/**
 * Get the amount of health added to wounded soldier health
 * @return The amount of health to add
 */
int RuleItem::getHealthAmount () const
{
	return _healthAmount;
}

/**
 * Get the amount of energy added to soldier energy
 * @return The amount of energy to add
 */
int RuleItem::getEnergy () const
{
	return _energy;
}

/**
 * Get the amount of stun removed to soldier stun level
 * @return The amount of stun to remove
 */
int RuleItem::getStun () const
{
	return _stun;
}

/**
 * Get the amount of Time Unit needed to use this item
 * @return The amount of Time Unit needed to use this item
 */
int RuleItem::getTUUse() const
{
	return _tuUse;
}

/**
 * Returns the item's max explosion radius. Small explosions don't have a restriction.
 * Larger explosions are restricted using a formula, with a maximum of radius 10 no matter how large the explosion is.
 * @return radius.
 */
int RuleItem::getExplosionRadius() const
{
	int radius = 0;
	if (_blastRadius == -1)
	{
		if (_damageType == DT_IN)
		{
			radius = (_power / 30) + 1;
		}
		else if (_damageType == DT_HE || _damageType == DT_STUN)
		{
			radius = _power / 20;
		}
	}
	else
	{
		radius = _blastRadius;
	}

	if (radius > 11)
	{
		radius = 11;
	}

	return radius;
}

/**
 * Returns the item's recovery points.
 * This is used at battlescape debriefing score calculation.
 * @return points.
 */
int RuleItem::getRecoveryPoints() const
{
	return _recoveryPoints;
}

/**
 * Returns the item's armor.
 * Item is destroyed when an explosion power -bigger than it's armor- hits the item.
 * @return armor.
 */
int RuleItem::getArmor() const
{
	return _armor;
}

/**
 * Returns if the item should be recoverable
 * from the battlescape.
 * @return Is it recoverable.
 */
bool RuleItem::isRecoverable() const
{
	return _recover;
}


/**
 * Returns the item's Turret Type
 * @return turret index (-1 for no turret)
 */
int RuleItem::getTurretType() const
{
	return _turretType;
}

/**
 * Returns if this is a live alien.
 * @return alien
 */
bool RuleItem::getAlien() const
{
	return _liveAlien;
}

/**
 * @return if this charges a flat TU rate.
 */
bool RuleItem::getFlatRate() const
{
	return _flatRate;
}

/**
 * @return if this weapon should arc it's shots.
 */
bool RuleItem::getArcingShot() const
{
	return _arcingShot;
}

/**
 * @return the attraction value for this item (for AI)
 */
int RuleItem::getAttraction() const
{
	return _attraction;
}

/**
 * @return the list weight for this research item.
 */
int RuleItem::getListOrder() const
{
	 return _listOrder;
}

/*
 * get the max range of this weapon (0 = unlimited)
 * @return max range
 */
int RuleItem::getRange() const
{
	return _range;
}

/**
 * get the speed at which this bullet travels.
 * @return the speed.
 */
int RuleItem::getBulletSpeed() const
{
	return _bulletSpeed;
}

}
