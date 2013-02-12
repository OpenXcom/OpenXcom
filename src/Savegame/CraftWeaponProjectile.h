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
#ifndef OPENXCOM_WEAPONPROJECTILE_H
#define OPENXCOM_WEAPONPROJECTILE_H

#include <string>

namespace OpenXcom {

class Surface;

// Do not change the order of these enums because they are related to blob order.
enum CraftWeaponProjectileType { CWPT_STINGRAY_MISSILE, CWPT_AVALANCHE_MISSILE, CWPT_CANNON_ROUND, CWPT_FUSION_BALL, CWPT_LASER_BEAM, CWPT_PLASMA_BEAM };
enum CraftWeaponProjectileGlobalType { CWPGT_MISSILE, CWPGT_BEAM };
enum Directions { D_NONE, D_UP, D_DOWN };
const int HP_LEFT = -1;
const int HP_CENTER = 0;
const int HP_RIGHT = 1;

class CraftWeaponProjectile
{
private:
	int _type;
	int _globalType;
	int _speed;
	int _direction;
	int _currentPosition; // relative to interceptor, apparently, which is a problem when the interceptor disengages while projectile is in flight
	int _horizontalPosition;
	int _state;
	int _accuracy;
	int _damage;
	int _range;
	bool _toBeRemoved;
	bool _missed;

	int _distanceCovered;
	
public:
	CraftWeaponProjectile();
	~CraftWeaponProjectile(void);

	/// Sets projectile type. This determines it's speed.
	void setType(const std::string &type);
	/// Returns projectile type.
	int getType() const;
	/// Returns projectile global type.
	int getGlobalType() const;
	/// Sets projectile direction. This determines it's initial position.
	void setDirection(const int &directon);
	/// Gets projectile direction.
	int getDirection() const;
	/// Moves projectile in _direction with _speed.
	void move();
	/// Gets projectile position.
	int getPosition() const;
	/// Sets projectile position.
	void setPosition(const int &position);
	/// Sets horizontal position. This determines from which weapon projectile has been fired.
	void setHorizontalPosition(int position);
	/// Gets horizontal position.
	int getHorizontalPosition() const;
	/// Marks projectile to be removed.
	void remove();
	/// Returns true if the projectile should be removed.
	bool toBeRemoved() const;
	/// Retruns state of the beam.
	int getState() const;
	/// Sets power of the projectile.
	void setDamage(const int &damage);
	/// Gets power of the projectile.
	int getDamage() const;
	/// Sets accuracy of the projectile.
	void setAccuracy(const int &accuracy);
	/// Gets accuracy of the projectile.
	int getAccuracy() const;
	/// Sets the projectile to missed status.
	void setMissed(const bool &missed);
	/// Gets the projectile missed status.
	bool getMissed() const;
	/// Sets maximum range of projectile.
	void setRange(const int &range);
	/// Gets maximum range of projectile.
	int getRange() const;
};

}

#endif
