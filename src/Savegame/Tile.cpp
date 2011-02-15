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
#include "Tile.h"
#include "../Ruleset/MapData.h"
#include "../Ruleset/MapDataSet.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/RNG.h"
#include "../Engine/Exception.h"

namespace OpenXcom
{

/**
* constructor
* @param pos Position.
*/
Tile::Tile(const Position& pos): _discovered(false), _smoke(0), _fire(0), _pos(pos), _cached(false), _unit(0), _explosive(0)
{
	for (int i = 0; i < 4; i++)
	{
		_objects[i] = 0;
		_currentFrame[i] = 0;
	}
	for (int layer = 0; layer < LIGHTLAYERS; layer++)
	{
		_light[layer] = 0;
		_lastLight[layer] = 0;
	}
}

/**
 * destructor
 */
Tile::~Tile()
{

}

/**
 * Get the MapData pointer of a part of the tile.
 * @param part the part 0-3.
 * @return pointer to terrainobject
 */
MapData *Tile::getMapData(int part)
{
	if (part < 0 || part > 3)
	{
		throw Exception("unkown MapDataID part");
	}
	return _objects[part];
}

/**
 * Set the MapData references of part 0 to 3.
 * @param dat pointer to the data object
 * @param part the part number
 */
void Tile::setMapData(MapData *dat, int part)
{
	_objects[part] = dat;
	setCached(false);
}

/**
 * Gets wether this tile has no objects.
 * @return bool
 */
bool Tile::isVoid()
{
	return _objects[0] == 0 && _objects[1] == 0 && _objects[2] == 0 && _objects[3] == 0;
}

/**
 * Get the TU cost to walk over a certain part of the tile.
 * @param part
 * @param movementType
 * @return TU cost
 */
int Tile::getTUCost(int part, MovementType movementType)
{
	if (_objects[part])
		return _objects[part]->getTUCost(movementType);
	else 
		return 0;
}

/**
 * Whether this tile has a floor or not. If no object defined as floor, it has no floor.
 * @return bool
 */
bool Tile::hasNoFloor()
{
	if (_objects[O_FLOOR])
		return _objects[O_FLOOR]->isNoFloor();
	else
		return true;
}

/**
 * Whether this tile has a big wall.
 * @return bool
 */
bool Tile::isBigWall()
{
	if (_objects[O_OBJECT])
		return _objects[O_OBJECT]->isBigWall();
	else
		return false;
}

/**
 * If an object stand on this tile, how high is it standing.
 * @return the level in pixels
 */
int Tile::getTerrainLevel()
{
	int level = 0;

	if (_objects[O_FLOOR])
		level = _objects[O_FLOOR]->getTerrainLevel();
	if (_objects[O_OBJECT])
		level += _objects[O_OBJECT]->getTerrainLevel();

	return level;
}

/**
 * Gets the tile's position.
 * @return position
 */
const Position& Tile::getPosition() const
{
	return _pos;
}


/**
 * Gets the tile's footstep sound.
 * @return position
 */
int Tile::getFootstepSound()
{
	int sound = 0;

	if (_objects[O_FLOOR])
		sound = _objects[O_FLOOR]->getFootstepSound();
	if (_objects[O_OBJECT])
		sound = _objects[O_OBJECT]->getFootstepSound();

	return sound;
}


/**
 * Open a door on this tile.
 * @param part
 * @return an ID, 0(normal), 1(ufo) or -1 if no door opened or 3 if ufo door is still opening
 */
int Tile::openDoor(int part)
{
	if (!_objects[part]) return -1;

	if (_objects[part]->isDoor())
	{
		setMapData(_objects[part]->getDataset()->getObjects()->at(_objects[part]->getAltMCD()),
				   _objects[part]->getDataset()->getObjects()->at(_objects[part]->getAltMCD())->getObjectType());
		setMapData(0, part);
		return 0;
	}
	if (_objects[part]->isUFODoor() && _currentFrame[part] == 0) // ufo door part 0 - door is closed
	{
		_currentFrame[part] = 1; // start opening door
		return 1;
	}
	if (_objects[part]->isUFODoor() && _currentFrame[part] != 7) // ufo door != part 7 - door is still opening
	{
		return 3;
	}
	return -1;
}

/**
 * Check if the ufo door is open or opening. Used for visibility/light blocking checks.
 * @param part
 * @return bool
 */
bool Tile::isUfoDoorOpen(int part)
{
	if (_objects[part] && _objects[part]->isUFODoor() && _currentFrame[part] != 0)
	{
		return true;
	}
	return false;
}

/**
 * Sets the tile's cache flag.
 * @param cached
 */
void Tile::setCached(bool cached)
{
	_cached = cached;
}

/**
 * Check if the tile is still cached in the Map cache.
 * When the tile changes (door/lighting/destroyed), it needs to be re-cached.
 * @return bool
 */
bool Tile::isCached()
{
	return _cached;
}

/**
 * Sets the tile's cache flag.
 * @param flag true/false
 */
void Tile::setDiscovered(bool flag)
{
	_discovered = flag;
}

bool Tile::isDiscovered()
{
	return _discovered;
}

/**
 * Add a light amount to the tile. If it was already lit more, it is ignored.
 * @param light Amount of light to add.
 * @param sessionID Unique number to avoid tiles are lit only once in a session.
 */
void Tile::addLight(int light, int sessionID, int layer)
{
	if (sessionID == _sessionID) return;
	_sessionID = sessionID;

	if (_light[layer] < light)
		_light[layer] = light;
}

bool Tile::isChecked(int sessionID)
{
	if (sessionID == _sessionID) return true;
	return false;
}


void Tile::isSeenBy(BattleUnit *unit, int sessionID)
{
	if (sessionID == _sessionID) return;
	if (!_discovered)
	{
		_discovered = true;
		setCached(false);
	}
	_sessionID = sessionID;
}

void Tile::resetLight(int layer)
{
	_lastLight[layer] = _light[layer];
	_light[layer] = 0;
	_sessionID = 0;
}

void Tile::setLight(int layer)
{
	if (_lastLight[layer] != _light[layer])
	{
		setCached(false);
	}
}

/**
 * Gets the tile's shade amount 0-15. It returns the brightest of all light layers.
 * And shade level is actually the inverse of light level.
 * @return shade
 */
int Tile::getShade()
{
	int light = 0;

	for (int layer = 0; layer < LIGHTLAYERS; layer++)
	{
		if (_light[layer] > light)
			light = _light[layer];
	}

	return 15 - light;
}

/**
 * Destroy a part on this tile. We first remove the old object, then replace it with the new one.
 * This is because the object type of the old and new one are not nescessarly the same.
 * @param part
 * @param sessionID
 */
void Tile::destroy(int part)
{
	if (_objects[part])
	{
		MapData *originalPart = _objects[part];
		setMapData(0, part);
		if (originalPart->getDieMCD())
		{
			MapData *dead = originalPart->getDataset()->getObjects()->at(originalPart->getDieMCD());
			setMapData(dead, dead->getObjectType());
		}
	}
}


/**

 * @param power
 * @param sessionID
 */
void Tile::setExplosive(int power, int sessionID)
{
	if (sessionID = _sessionID)
	{
		_explosive = (_explosive + power) / 2;
	}
	else
	{
		_sessionID = sessionID;
		_explosive = power;
	}
}

void Tile::detonate()
{
	int decrease;

	if (_explosive)
	{
		for (int i = 0; i < 4; i++)
		{
			if(_objects[i])
			{
				if ((_explosive) >= _objects[i]->getArmor())
				{
					decrease = _objects[i]->getArmor();
					destroy(i);
					// explosions create smoke which only stays 1 or 2 turns
					setSmoke(1);
					if (_objects[i] && (_explosive - decrease) >= _objects[i]->getArmor())
					{
						destroy(i);
						setSmoke(2);
					}
				}
			}
		}
		// flammable of the tile needs to be 20 or lower (lower is better chance of catching fire) to catch fire
		// note that when we get here, flammable objects can already be destroyed by the explosion, thus not catching fire.
		int flam = getFlammability();
		if (flam <= 20)
		{
			if (RNG::generate(0, 20) - flam >= 0)
			{
				ignite();
			}
		}
		_explosive = 0;
	}
}

/*
 * Flammability of a tile is the lowest flammability of it's objects
 */
int Tile::getFlammability()
{
	int flam = 255;

	for (int i=0; i < 4; i++)
	{
		if (_objects[i])
		{
			if (_objects[i]->getFlammable() < flam)
			{
				flam = _objects[i]->getFlammable();
			}
		}
	}
	return flam;
}

/*
 * Ignite starts fire on a tile, it will last <fuel> rounds. Fuel of a tile is the highest fuel of it's objects
 */
void Tile::ignite()
{
	int fuel = 0;

	for (int i=0; i < 4; i++)
	{
		if (_objects[i])
		{
			if (_objects[i]->getFuel() > fuel)
			{
				fuel = _objects[i]->getFuel();
			}
		}
	}
	setFire(fuel);
}
/**
 * Animate the tile. This means to advance the current frame for every object.
 */
void Tile::animate()
{
	int newframe;
	for (int i=0; i < 4; i++)
	{
		if (_objects[i])
		{
			if (_objects[i]->isUFODoor() && (_currentFrame[i] == 0 || _currentFrame[i] == 7)) // ufo door is static
			{
				continue;
			}
			newframe = _currentFrame[i] + 1;
			if (newframe == 8)
			{
				newframe = 0;
			}
			if (_objects[i]->getSprite(_currentFrame[i]) != _objects[i]->getSprite(newframe))
			{
				setCached(false);
			}
			_currentFrame[i] = newframe;
		}
	}
}

/**
 * Get the sprite of a certain part of the tile.
 * @param part
 * @return Pointer to the sprite.
 */
Surface *Tile::getSprite(int part)
{
	return _objects[part]->getDataset()->getSurfaceset()->getFrame(_objects[part]->getSprite(_currentFrame[part]));
}

/**
 * Set a unit on this tile.
 * @param unit
 */
void Tile::setUnit(BattleUnit *unit)
{
	_unit = unit;
}

/**
 * Get the unit on this tile.
 * @return BattleUnit.
 */
BattleUnit *Tile::getUnit()
{
	return _unit;
}

void Tile::setFire(int fire)
{
	_fire = fire;
	_smoke = 0; // fire stops smoke, like in the original game
}

int Tile::getFire()
{
	return _fire;
}

void Tile::setSmoke(int smoke)
{
	if (smoke > 40) smoke = 40;
	_smoke = smoke;
	_fire = 0; // smoke stops fire, like in the original game
}

int Tile::getSmoke()
{
	return _smoke;
}

}
