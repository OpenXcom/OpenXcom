/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "CraftWeaponProjectile.h"

namespace OpenXcom {

CraftWeaponProjectile::CraftWeaponProjectile() : _type(CWPT_CANNON_ROUND), _globalType(CWPGT_MISSILE), _speed(0), _direction(D_NONE), _currentPosition(0), _horizontalPosition(0), _state(0), _accuracy(0), _damage(0), _range(0), _toBeRemoved(false), _missed(false), _distanceCovered(0)
{
}


CraftWeaponProjectile::~CraftWeaponProjectile(void)
{
}

/*
 * Sets the type of projectile according to the type of
 * weapon it was shot from. This is used for drawing the
 * projectiles.
 */
void CraftWeaponProjectile::setType(CraftWeaponProjectileType type)
{
	_type = type;
	if (type >= CWPT_LASER_BEAM)
	{
		_globalType = CWPGT_BEAM;
		_state = 8;
	}
}

/*
 * Returns the type of projectile.
 * @return Projectile type as an integer value.
 */
CraftWeaponProjectileType CraftWeaponProjectile::getType() const
{
	return _type;
}

/*
 * Returns the global type of projectile.
 * @return 0 - if it's a missile, 1 if beam.
 */
CraftWeaponProjectileGlobalType CraftWeaponProjectile::getGlobalType() const
{
	return _globalType;
}

/*
 * Sets the direction of the projectile.
 */
void CraftWeaponProjectile::setDirection(const int &directon)
{
	_direction = directon;
	if (_direction == D_UP)
	{
		_currentPosition = 0;
	}
}

/*
 * Gets the direction of the projectile.
 */
int CraftWeaponProjectile::getDirection() const
{
	return _direction;
}

/*
 * Moves the projectile according to it's speed
 * or changes the phase of beam animation.
 */
void CraftWeaponProjectile::move()
{
	if (_globalType == CWPGT_MISSILE)
	{
		int positionChange = _speed;

		// Check if projectile would reach its maximum range this tick.
		if ((_distanceCovered / 8) < getRange() && ((_distanceCovered + _speed)/ 8) >= getRange())
			positionChange = getRange() * 8 - _distanceCovered;

		// Check if projectile passed its maximum range on previous tick. 
		if ((_distanceCovered / 8) >= getRange())
			setMissed(true);
		
		if (_direction == D_UP)
		{
			_currentPosition += positionChange;
		}
		else if (_direction == D_DOWN)
		{
			_currentPosition -= positionChange;
		}
		
		_distanceCovered += positionChange;
	}
	else if (_globalType == CWPGT_BEAM)
	{
		_state /= 2;
		if (_state == 1)
		{
			_toBeRemoved = true;
		}
	}
}

/*
 * Sets the y position of the projectile on the radar.
 */
void CraftWeaponProjectile::setPosition(const int &position)
{
	_currentPosition = position;
}

/*
 * Gets the y position of the projectile on the radar.
 */
int CraftWeaponProjectile::getPosition() const
{
	return _currentPosition;
}

/*
 * Sets the x position of the projectile on the radar.
 * It's used only once for each projectile during firing.
 */
void CraftWeaponProjectile::setHorizontalPosition(int position)
{
	_horizontalPosition = position;
}

/*
 * Gets the x position of the projectile.
 */
int CraftWeaponProjectile::getHorizontalPosition() const
{
	return _horizontalPosition;
}

/*
 * Marks the projectile to be removed.
 */
void CraftWeaponProjectile::remove()
{
	_toBeRemoved = true;
}

/*
 * Returns if a projectile should be removed.
 */
bool CraftWeaponProjectile::toBeRemoved() const
{
	return _toBeRemoved;
}

/*
 * Returns animation state of a beam.
 */
int CraftWeaponProjectile::getState() const
{
	return _state;
}

/*
 * Sets the amount of damage the projectile can do
 * when hitting it's target.
 */
void CraftWeaponProjectile::setDamage(const int &damage)
{
	_damage = damage;
}

/*
 * Gets the amount of damage the projectile can do
 * when hitting it's target.
 */
int CraftWeaponProjectile::getDamage() const
{
	return _damage;
}

/*
 * Sets the accuracy of the projectile.
 */
void CraftWeaponProjectile::setAccuracy(const int &accuracy)
{
	_accuracy = accuracy;
}

/*
 * Gets the accuracy of the projectile.
 */
int CraftWeaponProjectile::getAccuracy() const
{
	return _accuracy;
}

/*
 * Marks the projectile as a one which missed it's target.
 */
void CraftWeaponProjectile::setMissed(const bool &missed)
{
	_missed = missed;
}

/*
 * Returns true if the projectile missed it's target.
 * Otherwise returns false.
 */
bool CraftWeaponProjectile::getMissed() const
{
	return _missed;
}

/*
 * Sets maximum range of projectile.
 */
void CraftWeaponProjectile::setRange(const int &range)
{
	_range = range;
}

/*
 * Returns maximum range of projectile.
 */
int CraftWeaponProjectile::getRange() const
{
	return _range;
}

/*
 * Sets the speed of the projectile.
 */
void CraftWeaponProjectile::setSpeed(int speed)
{
	_speed = speed;
}

}
