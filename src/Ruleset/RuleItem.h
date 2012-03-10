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
#ifndef OPENXCOM_RULEITEM_H
#define OPENXCOM_RULEITEM_H

#include <string>
#include <vector>
#include "yaml.h"

enum ItemDamageType { DT_NONE, DT_AP, DT_IN, DT_HE, DT_LASER, DT_PLASMA, DT_STUN, DT_MELEE, DT_ACID, DT_SMOKE };
enum BattleType { BT_NONE, BT_FIREARM, BT_AMMO, BT_MELEE, BT_GRENADE, BT_PROXIMITYGRENADE, BT_MEDIKIT, BT_SCANNER, BT_MINDPROBE, BT_PSIAMP, BT_FLARE };

namespace OpenXcom
{

class SurfaceSet;
class Surface;
class RuleManufactureInfo;

/**
 * Represents a specific type of item.
 * Contains constant info about an item like
 * storage size, sell price, etc.
 * @sa Item
 */
class RuleItem
{
private:
	std::string _type, _name; // two types of objects can have the same name
	float _size;
	int _cost, _time, _weight;
	int _bigSprite, _floorSprite, _handSprite, _bulletSprite;
	int _fireSound, _hitSound, _hitAnimation;
	int _power, _priority;
	std::vector<std::string> _compatibleAmmo;
	ItemDamageType _damageType;
	int _accuracyAuto, _accuracySnap, _accuracyAimed, _tuAuto, _tuSnap, _tuAimed;
	int _clipSize, _accuracyMelee, _tuMelee;
	BattleType _battleType;
	bool _twoHanded, _waypoint, _fixedWeapon;
	int _invWidth, _invHeight;
	int _painKiller, _heal, _stimulant;
	int _healAmount, _healthAmount;
	int _stun, _energy;
	int _tuUse;
public:
	/// Creates a blank item ruleset.
	RuleItem(const std::string &type);
	/// Cleans up the item ruleset.
	~RuleItem();
	/// Loads item data from YAML.
	void load(const YAML::Node& node);
	/// Saves the item data to YAML.
	void save(YAML::Emitter& out) const;
	/// Gets the item's type.
	std::string getType() const;
	/// Sets the item's name.
	void setName(const std::string &name);
	/// Gets the item's name.
	std::string getName() const;
	/// Gets the item's size.
	float getSize() const;
	/// Sets the item's size.
	void setSize(float size);
	/// Gets the item's cost.
	int getCost() const;
	/// Sets the item's cost.
	void setCost(int cost);
	/// Gets the item's transfer time.
	int getTransferTime() const;
	/// Sets the item's transfer time.
	void setTransferTime(int time);
	/// Gets the item's weight.
	int getWeight() const;
	/// Sets the item's weight.
	void setWeight(int cost);
	/// Gets the item's reference in BIGOBS.PCK for use in inventory.
	int getBigSprite() const;
	/// Sets the item's reference in BIGOBS.PCK for use in inventory.
	void setBigSprite(int sprite);
	/// Gets the item's reference in FLOOROB.PCK for use in inventory.
	int getFloorSprite() const;
	/// Sets the item's reference in FLOOROB.PCK for use in inventory.
	void setFloorSprite(int sprite);
	/// Gets the item's reference in HANDOB.PCK for use in inventory.
	int getHandSprite() const;
	/// Sets the item's reference in HANDOB.PCK for use in inventory.
	void setHandSprite(int sprite);
	/// Gets if the item is two-handed.
	bool getTwoHanded() const;
	/// Sets if the item is two-handed.
	void setTwoHanded(bool flag);
	/// Gets if the item is a launcher.
	bool getWaypoint() const;
	/// Sets if the item is a launcher.
	void setWaypoint(bool flag);
	/// Gets if the item is fixed.
	bool getFixed() const;
	/// Sets if the item is fixed.
	void setFixed(bool flag);
	/// Gets the item's bullet sprite reference.
	int getBulletSprite() const;
	/// Sets the item's bulet sprite reference.
	void setBulletSprite(int sprite);
	/// Gets the item's fire sound.
	int getFireSound() const;
	/// Sets the item's fire sound.
	void setFireSound(int sound);
	/// Gets the item's hit sound.
	int getHitSound() const;
	/// Sets the item's hit sound.
	void setHitSound(int sound);
	/// Gets the item's hit animation.
	int getHitAnimation() const;
	/// Sets the item's hit animation.
	void setHitAnimation(int animation);
	/// Gets the item's power.
	int getPower() const;
	/// Sets the item's power.
	void setPower(int power);
	/// Gets the item's accuracy.
	int getAccuracySnap() const;
	/// Sets the item's accuracy.
	void setAccuracySnap(int accuracy);
	/// Gets the item's accuracy.
	int getAccuracyAuto() const;
	/// Sets the item's accuracy.
	void setAccuracyAuto(int accuracy);
	/// Gets the item's accuracy.
	int getAccuracyAimed() const;
	/// Sets the item's accuracy.
	void setAccuracyAimed(int accuracy);
	/// Gets the item's tu.
	int getTUSnap() const;
	/// Sets the item's tu.
	void setTUSnap(int tu);
	/// Gets the item's tu.
	int getTUAuto() const;
	/// Sets the item's tu.
	void setTUAuto(int tu);
	/// Gets the item's tu.
	int getTUAimed() const;
	/// Sets the item's tu.
	void setTUAimed(int tu);
	/// Gets list of compatible ammo.
	std::vector<std::string> *getCompatibleAmmo();
	/// Gets the item's damage type.
	ItemDamageType getDamageType() const;
	/// Sets the item's damage type.
	void setDamageType(ItemDamageType damageType);
	/// Gets the item's type.
	BattleType getBattleType() const;
	/// Sets the item's type.
	void setBattleType(BattleType type);
	/// Gets the item's inventory width.
	int getInventoryWidth() const;
	/// Sets the item's inventory width.
	void setInventoryWidth(int width);
	/// Gets the item's inventory height.
	int getInventoryHeight() const;
	/// Sets the item's inventory height.
	void setInventoryHeight(int height);
	/// Gets the ammo amount.
	int getClipSize() const;
	/// Sets the item's ammo amount.
	void setClipSize(int size);
	/// Draws the item's hand sprite onto a surface.
	void drawHandSprite(SurfaceSet *texture, Surface *surface) const;
	/// Set medikit heal quantity
	void setHealQuantity (int heal);
	/// Get medikit heal quantity
	int getHealQuantity () const;
	/// Set medikit pain killer quantity
	void setPainKillerQuantity (int pk);
	/// Get medikit pain killer quantity
	int getPainKillerQuantity () const;
	/// Set medikit stimulant quantity
	void setStimulantQuantity (int stimulant);
	/// Get medikit stimulant quantity
	int getStimulantQuantity () const;
	/// Get medikit heal amount per shot
	int getHealAmount () const;
	/// Get medikit health amount per shot
	int getHealthAmount () const;
	/// Set medikit heal amount per shot
	void setHealAmount (int h);
	/// Set medikit health amount per shot
	void setHealthAmount (int h);
	/// Get medikit energy amount per shot
	int getEnergy () const;
	/// Get medikit stun amount per shot
	int getStun () const;
	/// Set medikit energy amount per shot
	void setEnergy (int e);
	/// Set medikit stun amount per shot
	void setStun (int s);
	/// Get use Time Unit use
	int getTUUse() const;
	/// Set use Time Unit use
	void setTUUse(int tu);
	/// Gets the max explosion radius
	int getExplosionRadius() const;
};

}

#endif
