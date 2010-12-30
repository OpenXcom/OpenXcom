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
#include "../Resource/TerrainObject.h"

namespace OpenXcom
{

/**
* constructor
* @param pos Position.
*/
Tile::Tile(const Position& pos): _discovered(false), _shadeSun(0), _light(0), _pos(pos), _cached(false), _unit(0)
{
	for (int i = 0; i < 4; i++)
	{
		_terrainObjects[i] = 0;
		_currentFrame[i] = 0;
	}
}

/**
 * destructor
 */
Tile::~Tile()
{

}

/**
 * Get the TerrainObject pointer of a part of the tile.
 * @param part the part 0-3.
 * @return pointer to terrainobject
 */
TerrainObject *Tile::getTerrainObject(int part)
{
	if (part < 0 || part > 3)
	{
		throw "unkown TerrainObjectID part";
	}
	return _terrainObjects[part];
}

/**
 * Set the TerrainObject references of part 0 to 3.
 * @param tob pointer to the terrain object
 * @param part the part number
 */
void Tile::setTerrainObject(TerrainObject *tob, int part)
{
	_terrainObjects[part] = tob;
	setCached(false);
}

/**
 * Get mapdatafilename.
 * @param part the part number 0-3
 * @return a string which contains the MCD filename
 */
std::string Tile::getName(int part)
{
	return _name[part];
}

/**
 * Sets mapdatafilename.
 * @param name the name of the specific MCD file
 * @param part the part number 0-3
 */
void Tile::setName(std::string name, int part)
{
	_name[part] = name;
}

/**
 * Get the TU cost to walk over a certain part of the tile.
 * @param part
 * @param movementType
 * @return TU cost
 */
int Tile::getTUCost(int part, MovementType movementType)
{
	if (_terrainObjects[part])
		return _terrainObjects[part]->getTUCost(movementType);
	else 
		return 0;
}

/**
 * Whether this tile has a floor or not. If no object defined as floor, it has no floor.
 * @return bool
 */
bool Tile::hasNoFloor()
{
	if (_terrainObjects[O_FLOOR])
		return _terrainObjects[O_FLOOR]->isNoFloor();
	else
		return true;
}

/**
 * Whether this tile has a big wall.
 * @return bool
 */
bool Tile::isBigWall()
{
	if (_terrainObjects[O_OBJECT])
		return _terrainObjects[O_OBJECT]->isBigWall();
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

	if (_terrainObjects[O_FLOOR])
		level = _terrainObjects[O_FLOOR]->getTerrainLevel();
	if (_terrainObjects[O_OBJECT])
		level += _terrainObjects[O_OBJECT]->getTerrainLevel();

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

	if (_terrainObjects[O_FLOOR])
		sound = _terrainObjects[O_FLOOR]->getFootstepSound();
	if (_terrainObjects[O_OBJECT])
		sound = _terrainObjects[O_OBJECT]->getFootstepSound();

	return sound;
}


/**
 * Open a door on this tile.
 * @param part
 * @return an ID, 0(normal), 1(ufo) or -1 if no door opened or 3 if ufo door is still opening
 */
int Tile::openDoor(int part)
{
	if (!_terrainObjects[part]) return -1;

	if (_terrainObjects[part]->isDoor())
	{
		setTerrainObject(_terrainObjects[part]->getAltObject(1), _terrainObjects[part]->getAltObject(1)->getObjectType());
		setTerrainObject(0, part);
		return 0;
	}
	if (_terrainObjects[part]->isUFODoor() && _currentFrame[part] == 0) // ufo door part 0 - door is closed
	{
		_currentFrame[part] = 1; // start opening door
		return 1;
	}
	if (_terrainObjects[part]->isUFODoor() && _currentFrame[part] != 7) // ufo door != part 7 - door is still opening
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
	if (_terrainObjects[part] && _terrainObjects[part]->isUFODoor() && _currentFrame[part] != 0)
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
 * Sets the tile's shade amount 0-15.
 * Determined by the sun. Absolute amount.
 * @param shade
 */
void Tile::setShadeSun(int shade)
{
	if (_shadeSun != shade)
	{
		_shadeSun = shade;
		setCached(false);
	}
}

/**
 * Change a light amount (+ ligther / - dimmer). Relative to the existing light amount.
 * Can be caused by adding/removing objects, fire, personal light.
 * Normally resulting light level should not go below 0. (lights normally can not darken a place...)
 * @param light Amount of light to add.
 * @param sessionID Unique number to avoid tiles are lit only once in a session.
 */
void Tile::addLight(int light, int sessionID)
{
	if (sessionID == _sessionID) return;
	_sessionID = sessionID;

	if (light != 0)
	{
		_light += light;
	}
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
	_visibleByUnit.push_back(unit);
}

void Tile::resetLight()
{
	_lastLight = _light;
	_light = 0;
	_sessionID = 0;
	_visibleByUnit.clear();
}

void Tile::setLight()
{
	if (_lastLight != _light)
	{
		setCached(false);
	}
}

/**
 * Gets the tile's shade amount 0-15.
 * @return shade
 */
int Tile::getShade()
{
	int shade = _shadeSun - _light;
	if (shade < 0) shade = 0;
	return shade;
}

/**
 * Destroy a part on this tile.
 * @param part
 */
void Tile::destroy(int part)
{
	if (_terrainObjects[part])
	{
		TerrainObject *tob = _terrainObjects[part];
		setTerrainObject(0, part);
		if (tob->getAltObject(0))
			setTerrainObject(tob->getAltObject(0), tob->getAltObject(0)->getObjectType());
	}
}

/**
 * Animate the tile. This means to advance the current frame for every object.
 */
void Tile::animate()
{
	int newframe;
	for (int i=0; i < 4; i++)
	{
		if (_terrainObjects[i])
		{
			if (_terrainObjects[i]->isUFODoor() && (_currentFrame[i] == 0 || _currentFrame[i] == 7)) // ufo door is static
			{
				continue;
			}
			newframe = _currentFrame[i] + 1;
			if (newframe == 8)
			{
				newframe = 0;
			}
			if (_terrainObjects[i]->getSprite(_currentFrame[i]) != _terrainObjects[i]->getSprite(newframe))
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
	return _terrainObjects[part]->getSprite(_currentFrame[part]);
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


}
