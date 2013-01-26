/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "../Engine/Surface.h"
#include "../Engine/RNG.h"
#include "../Engine/Exception.h"
#include "BattleUnit.h"
#include "BattleItem.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Armor.h"

namespace OpenXcom
{

/**
* constructor
* @param pos Position.
*/
Tile::Tile(const Position& pos): _smoke(0), _fire(0),  _explosive(0), _pos(pos), _unit(0), _animationOffset(0), _markerColor(0), _visible(false)
{
	for (int i = 0; i < 4; ++i)
	{
		_objects[i] = 0;
		_mapDataID[i] = -1;
		_mapDataSetID[i] = -1;
		_currentFrame[i] = 0;
	}
	for (int layer = 0; layer < LIGHTLAYERS; layer++)
	{
		_light[layer] = 0;
		_lastLight[layer] = -1;
	}
	for (int i = 0; i < 3; ++i)
	{
		_discovered[i] = false;
	}
}

/**
 * destructor
 */
Tile::~Tile()
{
	_inventory.clear();
}

/**
 * Load the tile from a YAML node.
 * @param node YAML node.
 */
void Tile::load(const YAML::Node &node)
{
	//node["position"] >> _pos;
	for (int i =0; i < 4; i++)
	{
		node["mapDataID"][i] >> _mapDataID[i];
		node["mapDataSetID"][i] >> _mapDataSetID[i];
	}
	if(const YAML::Node *pName = node.FindValue("fire"))
	{
		*pName >> _fire;
	}
	else
	{
		_fire = 0;
	}
	if(const YAML::Node *pName = node.FindValue("smoke"))
	{
		*pName >> _smoke;
	}
	else
	{
		_smoke = 0;
	}
	if(const YAML::Node *pName = node.FindValue("discovered"))
	{
		node["discovered"][0] >> _discovered[0];
		node["discovered"][1] >> _discovered[1];
		node["discovered"][2] >> _discovered[2];
	}
}

/**
 * Load the tile from binary.
 * @param buffer pointer to buffer.
 */
void Tile::loadBinary(const unsigned char* buffer)
{
	_mapDataSetID[0] = buffer[4];
	_mapDataID[0] = buffer[5] + (buffer[6] << 8);
	_mapDataSetID[1] = buffer[7];
	_mapDataID[1] = buffer[8] + (buffer[9] << 8);
	_mapDataSetID[2] = buffer[10];
	_mapDataID[2] = buffer[11] + (buffer[10] << 8);
	_mapDataSetID[3] = buffer[13];
	_mapDataID[3] = buffer[14] + (buffer[15] << 8);

	_smoke = buffer[16];
	_fire = buffer[17];
	_discovered[0] = buffer[18] & 1;
	_discovered[1] = buffer[18] & 2;
	_discovered[2] = buffer[18] & 4;
}


/**
 * Saves the tile to a YAML node.
 * @param out YAML emitter.
 */
void Tile::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "position" << YAML::Value << _pos;
	out << YAML::Key << "mapDataID" << YAML::Value << YAML::Flow;
	out << YAML::BeginSeq << _mapDataID[0] << _mapDataID[1] << _mapDataID[2] << _mapDataID[3] << YAML::EndSeq;
	out << YAML::Key << "mapDataSetID" << YAML::Value << YAML::Flow;
	out << YAML::BeginSeq << _mapDataSetID[0] << _mapDataSetID[1] << _mapDataSetID[2] << _mapDataSetID[3] << YAML::EndSeq;
	if (_smoke)
		out << YAML::Key << "smoke" << YAML::Value << _smoke;
	if (_fire)
		out << YAML::Key << "fire" << YAML::Value << _fire;
	if (_discovered[0] || _discovered[0] || _discovered[0])
	{
		out << YAML::Key << "discovered" << YAML::Value << YAML::Flow;
		out << YAML::BeginSeq << _discovered[0] << _discovered[1] << _discovered[2] << YAML::EndSeq;
	}
	out << YAML::EndMap;
}
/**
 * Saves the tile to binary.
 * @param buffer pointer to buffer.
 */
void Tile::saveBinary(unsigned char* buffer) const
{
	buffer[4] = (unsigned char)_mapDataSetID[0];
	buffer[5] = (unsigned short)_mapDataID[0];
	buffer[7] = (unsigned char)_mapDataSetID[1];
	buffer[8] = (unsigned short)_mapDataID[1];
	buffer[10] = (unsigned char)_mapDataSetID[2];
	buffer[11] = (unsigned short)_mapDataID[2];
	buffer[13] = (unsigned char)_mapDataSetID[3];
	buffer[14] = (unsigned short)_mapDataID[3];

	buffer[16] = (unsigned short)_smoke;
	buffer[17] = (unsigned short)_fire;
	buffer[18] = (_discovered[0]?1:0) + (_discovered[1]?1:0) << 1 + (_discovered[2]?1:0) << 2;
}

/**
 * Get the MapData pointer of a part of the tile.
 * @param part the part 0-3.
 * @return pointer to mapdata
 */
MapData *Tile::getMapData(int part) const
{
	if (part < 0 || part > 3)
	{
		throw Exception("unknown MapDataID part");
	}
	return _objects[part];
}

/**
 * Set the MapData references of part 0 to 3.
 * @param dat pointer to the data object
 * @param newObjectID The ID in the total list of the objects of this battlegame.
 * @param part the part number
 */
void Tile::setMapData(MapData *dat, int mapDataID, int mapDataSetID, int part)
{
	_objects[part] = dat;
	_mapDataID[part] = mapDataID;
	_mapDataSetID[part] = mapDataSetID;
}

/**
 * get the MapData references of part 0 to 3.
 * @param part the part number
 * @return the object ID
 */
void Tile::getMapData(int *mapDataID, int *mapDataSetID, int part) const
{
	*mapDataID = _mapDataID[part];
	*mapDataSetID = _mapDataSetID[part];
}

/**
 * Gets whether this tile has no objects. Note that we can have a unit or smoke on this tile.
 * @return bool True if there is nothing but air on this tile.
 */
bool Tile::isVoid() const
{
	return _objects[0] == 0 && _objects[1] == 0 && _objects[2] == 0 && _objects[3] == 0 && _smoke == 0;
}

/**
 * Get the TU cost to walk over a certain part of the tile.
 * @param part
 * @param movementType
 * @return TU cost
 */
int Tile::getTUCost(int part, MovementType movementType) const
{
	if (_objects[part])
	{
		if (_objects[part]->isUFODoor() && _currentFrame[part] != 1)
			return 0;
		return _objects[part]->getTUCost(movementType);
	}
	else
		return 0;
}

/**
 * Whether this tile has a floor or not. If no object defined as floor, it has no floor.
 * @return bool
 */
bool Tile::hasNoFloor() const
{
	if (_objects[MapData::O_FLOOR])
		return _objects[MapData::O_FLOOR]->isNoFloor();
	else
		return true;
}

/**
 * Whether this tile has a big wall.
 * @return bool
 */
bool Tile::isBigWall() const
{
	if (_objects[MapData::O_OBJECT])
		return _objects[MapData::O_OBJECT]->isBigWall();
	else
		return false;
}

/**
 * If an object stand on this tile, this returns how high the unit is it standing.
 * @return the level in pixels
 */
int Tile::getTerrainLevel() const
{
	int level = 0;

	if (_objects[MapData::O_FLOOR])
		level = _objects[MapData::O_FLOOR]->getTerrainLevel();
	if (_objects[MapData::O_OBJECT])
		level += _objects[MapData::O_OBJECT]->getTerrainLevel();

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
 * @return sound ID
 */
int Tile::getFootstepSound() const
{
	int sound = 0;

	if (_objects[MapData::O_FLOOR])
		sound = _objects[MapData::O_FLOOR]->getFootstepSound();
	if (_objects[MapData::O_OBJECT])
		sound = _objects[MapData::O_OBJECT]->getFootstepSound();

	return sound;
}


/**
 * Open a door on this tile.
 * @param part
 * @return a value: 0(normal door), 1(ufo door) or -1 if no door opened or 3 if ufo door(=animated) is still opening 4 if not enough TUs
 */
int Tile::openDoor(int part, BattleUnit *unit, bool debug)
{
	if (!_objects[part]) return -1;

	if (_objects[part]->isDoor())
	{
		if (unit && unit->getTimeUnits() < _objects[part]->getTUCost(unit->getArmor()->getMovementType()) && !debug)
			return 4;
		setMapData(_objects[part]->getDataset()->getObjects()->at(_objects[part]->getAltMCD()), _objects[part]->getAltMCD(), _mapDataSetID[part],
				   _objects[part]->getDataset()->getObjects()->at(_objects[part]->getAltMCD())->getObjectType());
		setMapData(0, -1, -1, part);
		return 0;
	}
	if (_objects[part]->isUFODoor() && _currentFrame[part] == 0) // ufo door part 0 - door is closed
	{
		if (unit && unit->getTimeUnits() < _objects[part]->getTUCost(unit->getArmor()->getMovementType()) && !debug)
			return 4;
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
 * This function assumes that there never are 2 doors on 1 tile or a door and another wall on 1 tile.
 * @param part
 * @return bool
 */
bool Tile::isUfoDoorOpen(int part) const
{
	if (_objects[part] && _objects[part]->isUFODoor() && _currentFrame[part] != 0)
	{
		return true;
	}
	return false;
}

int Tile::closeUfoDoor()
{
	int retval = 0;

	for (int part = 0; part < 4; part++)
	{
		if (isUfoDoorOpen(part))
		{
			_currentFrame[part] = 0;
			retval = 1;
		}
	}

	return retval;
}

/**
 * Sets the tile's cache flag. - TODO: set this for each object separately?
 * @param flag true/false
 * @param part 0-2 westwall/northwall/content+floor
 */
void Tile::setDiscovered(bool flag, int part)
{
	if (_discovered[part] != flag)
	{
		_discovered[part] = flag;
		if (part == 2 && flag == true)
		{
			_discovered[0] = true;
			_discovered[1] = true;
		}
		// if light on tile changes, units and objects on it change light too
		if (_unit != 0)
		{
			_unit->setCache(0);
		}
	}
}

/**
 * Get the black fog of war state of this tile.
 * @param part 0-2 westwall/northwall/content+floor
 * @return bool True = discovered the tile.
 */
bool Tile::isDiscovered(int part) const
{
	return _discovered[part];
}


/**
 * Reset the light amount on the tile. This is done before a light level recalculation.
 * @param layer Light is separated in 3 layers: Ambient, Static and Dynamic.
 */
void Tile::resetLight(int layer)
{
	_light[layer] = 0;
	_lastLight[layer] = _light[layer];
}

/**
 * Add the light amount on the tile. Only add light if the current light is lower.
 * @param light Amount of light to add.
 * @param layer Light is separated in 3 layers: Ambient, Static and Dynamic.
 */
void Tile::addLight(int light, int layer)
{
	if (_light[layer] < light)
		_light[layer] = light;
}

/**
 * Gets the tile's shade amount 0-15. It returns the brightest of all light layers.
 * Shade level is the inverse of light level. So a maximum amount of light (15) returns shade level 0.
 * @return shade
 */
int Tile::getShade() const
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
 * Destroy a part on this tile. We first remove the old object, then replace it with the destroyed one.
 * This is because the object type of the old and new one are not necessarily the same.
 * If the destroyed part is an explosive, set the tile's explosive value, which will trigger a chained explosion.
 * @param part
 * @return bool Return true objective was destroyed
 */
bool Tile::destroy(int part)
{
	bool _objective = false;
	if (_objects[part])
	{
		_objective = _objects[part]->getSpecialType() == MUST_DESTROY;
		MapData *originalPart = _objects[part];
		int originalMapDataSetID = _mapDataSetID[part];
		setMapData(0, -1, -1, part);
		if (originalPart->getDieMCD())
		{
			MapData *dead = originalPart->getDataset()->getObjects()->at(originalPart->getDieMCD());
			setMapData(dead, originalPart->getDieMCD(), originalMapDataSetID, dead->getObjectType());
		}
		if (originalPart->getExplosive())
		{
			setExplosive(originalPart->getExplosive());
		}
	}
	/* check if the floor on the lowest level is gone */
	if (part == MapData::O_FLOOR && getPosition().z == 0 && _objects[MapData::O_FLOOR] == 0)
	{
		/* replace with scorched earth */
		setMapData(MapDataSet::getScourgedEarthTile(), 1, 0, MapData::O_FLOOR);
	}
	return _objective;
}

/* damage terrain  - check against armor
 * @return bool Return true objective was destroyed
 */
bool Tile::damage(int part, int power)
{
	bool objective = false;
	if (power >= _objects[part]->getArmor())
		objective = destroy(part);
	return objective;
}


/**
 * Set a "virtual" explosive on this tile. We mark a tile this way to detonate it later.
 * We do it this way, because the same tile can be visited multiple times by an "explosion ray".
 * The explosive power on the tile is some kind of moving average of the explosive rays that passes it.
 * @param power
 */
void Tile::setExplosive(int power)
{
	if (_explosive)
	{
		_explosive = (_explosive + power) / 2;
	}
	else
	{
		_explosive = power;
	}
}

int Tile::getExplosive() const
{
	return _explosive;
}

/**
 * Apply the explosive power to the tile parts. This is where the actual destruction takes place.
 * @return bool Return true objective was destroyed
 */
bool Tile::detonate()
{
	int explosive = _explosive;
	_explosive = 0;
	bool objective = false;

	if (explosive)
	{
		// explosions create smoke which only stays 1 or 2 turns
		addSmoke(1);
		for (int i = 0; i < 4; ++i)
		{
			if(_objects[i])
			{
				if ((explosive) >= _objects[i]->getArmor())
				{
					int decrease = _objects[i]->getArmor();
					objective = destroy(i);
					addSmoke(2);
					if (_objects[i] && (explosive - decrease) >= _objects[i]->getArmor())
					{
						objective = destroy(i);
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
	}

	return objective;
}

/*
 * Flammability of a tile is the lowest flammability of it's objects.
 * @return Flammability : the lower the value, the higher the chance the tile/object catches fire.
 */
int Tile::getFlammability() const
{
	int flam = 255;

	for (int i=0; i < 4; ++i)
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
 * Ignite starts fire on a tile, it will burn <fuel> rounds. Fuel of a tile is the highest fuel of it's objects.
 * NOT the sum of the fuel of the objects! TODO: check if this is like in the original.
 */
void Tile::ignite()
{
	int fuel = 0;

	for (int i=0; i < 4; ++i)
	{
		if (_objects[i])
		{
			if (_objects[i]->getFuel() > fuel)
			{
				fuel = _objects[i]->getFuel();
			}
		}
	}
	setFire(fuel + 1);
	if (fuel > 1)
	{
		addSmoke(fuel * 2); // not sure
	}
}

/**
 * Animate the tile. This means to advance the current frame for every object.
 * Ufo doors are a bit special, they animated only when triggered.
 * When ufo doors are on frame 0(closed) or frame 7(open) they are not animated further.
 */
void Tile::animate()
{
	int newframe;
	for (int i=0; i < 4; ++i)
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
			_currentFrame[i] = newframe;
		}
	}
}

/**
 * Get the sprite of a certain part of the tile.
 * @param part
 * @return Pointer to the sprite.
 */
Surface *Tile::getSprite(int part) const
{
	if (_objects[part] == 0)
		return 0;

	return _objects[part]->getDataset()->getSurfaceset()->getFrame(_objects[part]->getSprite(_currentFrame[part]));
}

/**
 * Set a unit on this tile.
 * @param unit
 */
void Tile::setUnit(BattleUnit *unit)
{
	if (unit != 0)
	{
		unit->setTile(this);
	}
	_unit = unit;
}

/**
 * Get the (alive) unit on this tile.
 * @return BattleUnit.
 */
BattleUnit *Tile::getUnit() const
{
	return _unit;
}

/**
 * Set the amount of turns this tile is on fire. 0 = no fire.
 * @param fire : amount of turns this tile is on fire.
 */
void Tile::setFire(int fire)
{
	_fire = fire;
	_animationOffset = RNG::generate(0,3);
}

/**
 * Get the amount of turns this tile is on fire. 0 = no fire.
 * @return fire : amount of turns this tile is on fire.
 */
int Tile::getFire() const
{
	return _fire;
}

/**
 * Set the amount of turns this tile is smoking. 0 = no smoke.
 * @param smoke : amount of turns this tile is smoking.
 */
void Tile::addSmoke(int smoke)
{
	_smoke += smoke;
	if (_smoke > 40) _smoke = 40;
	_animationOffset = RNG::generate(0,3);
}

/**
 * Get the amount of turns this tile is smoking. 0 = no smoke.
 * @return smoke : amount of turns this tile is smoking.
 */
int Tile::getSmoke() const
{
	return _smoke;
}

/**
 * Get the number of frames the fire or smoke animation is off-sync.
 * To void fire and smoke animations of different tiles moving nice in sync - it looks fake.
 * @return offset
 */
int Tile::getAnimationOffset() const
{
	return _animationOffset;
}

/**
 * Add an item on the tile.
 * @param item
 * @param ground
 */
void Tile::addItem(BattleItem *item, RuleInventory *ground)
{
	item->setSlot(ground);
	_inventory.push_back(item);
	item->setTile(this);
}

/**
 * Remove an item from the tile.
 * @param item
 */
void Tile::removeItem(BattleItem *item)
{
	for (std::vector<BattleItem*>::iterator i = _inventory.begin(); i != _inventory.end(); ++i)
	{
		if ((*i) == item)
		{
			_inventory.erase(i);
			break;
		}
	}
	item->setTile(0);
}

/**
 * Get the topmost item sprite to draw on the battlescape.
 * @return item sprite ID in floorob, or -1 when no item
 */
int Tile::getTopItemSprite()
{
	int biggestWeight = -1;
	int biggestItem = -1;
	for (std::vector<BattleItem*>::iterator i = _inventory.begin(); i != _inventory.end(); ++i)
	{
		if ((*i)->getRules()->getWeight() > biggestWeight)
		{
			biggestWeight = (*i)->getRules()->getWeight();
			biggestItem = (*i)->getRules()->getFloorSprite();
		}
	}
	return biggestItem;
}

/**
 * New turn preparations. Decrease smoke and fire timers.
 * @return bool Return true objective was destroyed
 */
bool Tile::prepareNewTurn()
{
	bool objective = false;

	_smoke--;
	if (_smoke < 0) _smoke = 0;

	if (_fire == 1)
	{
		// fire will be finished in this turn
		// destroy all objects that burned, and try to ignite again
		for (int i = 0; i < 4; ++i)
		{
			if(_objects[i])
			{
				if (_objects[i]->getFlammable() < 255)
				{
					objective = destroy(i);
				}
			}
		}
		if (getFlammability() < 255)
		{
			ignite();
		}
		else
		{
			_fire = 0;
		}
	}
	else
	{
		_fire--;
		if (_fire < 0) _fire = 0;
	}

	return objective;
}

/**
 * Get the inventory on this tile.
 * @return pointer to a vector of battleitems.
 */
std::vector<BattleItem *> *Tile::getInventory()
{
	return &_inventory;
}

/**
 * Set the marker color on this tile.
 * @param color
 */
void Tile::setMarkerColor(int color)
{
	_markerColor = color;
}

/**
 * Get the marker color on this tile.
 * @return color
 */
int Tile::getMarkerColor()
{
	return _markerColor;
}

/**
 * Set the tile visible flag.
 */
void Tile::setVisible(int visibility)
{
	_visible += visibility;
}

/**
 * Get the tile visible flag.
 */
int Tile::getVisible()
{
	return _visible;
}

}
