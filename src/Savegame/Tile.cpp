/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include <algorithm>
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
#include "SerializationHelper.h"

namespace OpenXcom
{

/// How many bytes various fields use in a serialized tile. See header.
Tile::SerializationKey Tile::serializationKey =
{4, // index
 2, // _mapDataSetID, four of these
 2, // _mapDataID, four of these
 1, // _fire
 1, // _smoke
 1,	// one 8-bit bool field
 4 + 2*4 + 2*4 + 1 + 1 + 1 // total bytes to save one tile
};

/**
* constructor
* @param pos Position.
*/
Tile::Tile(const Position& pos): _smoke(0), _fire(0), _explosive(0), _pos(pos), _unit(0), _animationOffset(0), _markerColor(0), _visible(false), _preview(-1), _TUMarker(-1), _overlaps(0), _danger(false)
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
	//_position = node["position"].as<Position>(_position);
	for (int i = 0; i < 4; i++)
	{
		_mapDataID[i] = node["mapDataID"][i].as<int>(_mapDataID[i]);
		_mapDataSetID[i] = node["mapDataSetID"][i].as<int>(_mapDataSetID[i]);
	}
	_fire = node["fire"].as<int>(_fire);
	_smoke = node["smoke"].as<int>(_smoke);
	for (int i = 0; i < 3; i++)
	{
		_discovered[i] = node["discovered"][i].as<bool>();
	}
	if (node["openDoorWest"])
	{
		_currentFrame[1] = 7;
	}
	if (node["openDoorNorth"])
	{
		_currentFrame[2] = 7;
	}
}

/**
 * Load the tile from binary.
 * @param buffer Pointer to buffer.
 * @param serKey Serialization key.
 */
void Tile::loadBinary(Uint8 *buffer, Tile::SerializationKey& serKey)
{
	_mapDataID[0] = unserializeInt(&buffer, serKey._mapDataID);
	_mapDataID[1] = unserializeInt(&buffer, serKey._mapDataID);
	_mapDataID[2] = unserializeInt(&buffer, serKey._mapDataID);
	_mapDataID[3] = unserializeInt(&buffer, serKey._mapDataID);
	_mapDataSetID[0] = unserializeInt(&buffer, serKey._mapDataSetID);
	_mapDataSetID[1] = unserializeInt(&buffer, serKey._mapDataSetID);
	_mapDataSetID[2] = unserializeInt(&buffer, serKey._mapDataSetID);
	_mapDataSetID[3] = unserializeInt(&buffer, serKey._mapDataSetID);

	_smoke = unserializeInt(&buffer, serKey._smoke);
	_fire = unserializeInt(&buffer, serKey._fire);

    Uint8 boolFields = unserializeInt(&buffer, serKey.boolFields);
	_discovered[0] = (boolFields & 1) ? true : false;
	_discovered[1] = (boolFields & 2) ? true : false;
	_discovered[2] = (boolFields & 4) ? true : false;
	_currentFrame[1] = (boolFields & 8) ? 7 : 0;
	_currentFrame[2] = (boolFields & 0x10) ? 7 : 0;
}


/**
 * Saves the tile to a YAML node.
 * @return YAML node.
 */
YAML::Node Tile::save() const
{
	YAML::Node node;
	node["position"] = _pos;
	for (int i = 0; i < 4; i++)
	{
		node["mapDataID"].push_back(_mapDataID[i]);
		node["mapDataSetID"].push_back(_mapDataSetID[i]);
	}
	if (_smoke)
		node["smoke"] = _smoke;
	if (_fire)
		node["fire"] = _fire;
	if (_discovered[0] || _discovered[1] || _discovered[2])
	{
		for (int i = 0; i < 3; i++)
		{
			node["discovered"].push_back(_discovered[i]);
		}
	}
	if (isUfoDoorOpen(1))
	{
		node["openDoorWest"] = true;
	}
	if (isUfoDoorOpen(2))
	{
		node["openDoorNorth"] = true;
	}
	return node;
}

/**
 * Saves the tile to binary.
 * @param buffer pointer to buffer.
 */
void Tile::saveBinary(Uint8** buffer) const
{
	serializeInt(buffer, serializationKey._mapDataID, _mapDataID[0]);
	serializeInt(buffer, serializationKey._mapDataID, _mapDataID[1]);
	serializeInt(buffer, serializationKey._mapDataID, _mapDataID[2]);
	serializeInt(buffer, serializationKey._mapDataID, _mapDataID[3]);
	serializeInt(buffer, serializationKey._mapDataSetID, _mapDataSetID[0]);
	serializeInt(buffer, serializationKey._mapDataSetID, _mapDataSetID[1]);
	serializeInt(buffer, serializationKey._mapDataSetID, _mapDataSetID[2]);
	serializeInt(buffer, serializationKey._mapDataSetID, _mapDataSetID[3]);

	serializeInt(buffer, serializationKey._smoke, _smoke);
	serializeInt(buffer, serializationKey._fire, _fire);

	Uint8 boolFields = (_discovered[0]?1:0) + (_discovered[1]?2:0) + (_discovered[2]?4:0);
	boolFields |= isUfoDoorOpen(1) ? 8 : 0; // west
	boolFields |= isUfoDoorOpen(2) ? 0x10 : 0; // north?
	serializeInt(buffer, serializationKey.boolFields, boolFields);
}

/**
 * Set the MapData references of part 0 to 3.
 * @param dat pointer to the data object
 * @param mapDataID
 * @param mapDataSetID
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
 * @param mapDataID
 * @param mapDataSetID
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
	return _objects[0] == 0 && _objects[1] == 0 && _objects[2] == 0 && _objects[3] == 0 && _smoke == 0 && _inventory.empty();
}

/**
 * Gets the TU cost to walk over a certain part of the tile.
 * @param part The part number.
 * @param movementType The movement type.
 * @return TU cost.
 */
int Tile::getTUCost(int part, MovementType movementType) const
{
	if (_objects[part])
	{
		if (_objects[part]->isUFODoor() && _currentFrame[part] > 1)
			return 0;
		if (_objects[part]->getBigWall() >= 4)
			return 0;
		return _objects[part]->getTUCost(movementType);
	}
	else
		return 0;
}

/**
 * Whether this tile has a floor or not. If no object defined as floor, it has no floor.
 * @param tileBelow
 * @return bool
 */
bool Tile::hasNoFloor(Tile *tileBelow) const
{
	if (tileBelow != 0 && tileBelow->getTerrainLevel() == -24)
		return false;
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
		return (_objects[MapData::O_OBJECT]->getBigWall() != 0);
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
 * Gets the tile's footstep sound.
 * @param tileBelow
 * @return sound ID
 */
int Tile::getFootstepSound(Tile *tileBelow) const
{
	int sound = 0;

	if (_objects[MapData::O_FLOOR])
		sound = _objects[MapData::O_FLOOR]->getFootstepSound();
	if (_objects[MapData::O_OBJECT] && _objects[MapData::O_OBJECT]->getBigWall() == 0)
		sound = _objects[MapData::O_OBJECT]->getFootstepSound();
	if (!_objects[MapData::O_FLOOR] && !_objects[MapData::O_OBJECT] && tileBelow != 0 && tileBelow->getTerrainLevel() == -24)
		sound = tileBelow->getMapData(MapData::O_OBJECT)->getFootstepSound();

	return sound;
}


/**
 * Open a door on this tile.
 * @param part
 * @param unit
 * @param reserve
 * @return a value: 0(normal door), 1(ufo door) or -1 if no door opened or 3 if ufo door(=animated) is still opening 4 if not enough TUs
 */
int Tile::openDoor(int part, BattleUnit *unit, BattleActionType reserve)
{
	if (!_objects[part]) return -1;

	if (_objects[part]->isDoor())
	{
		if (unit && unit->getTimeUnits() < _objects[part]->getTUCost(unit->getArmor()->getMovementType()) + unit->getActionTUs(reserve, unit->getMainHandWeapon(false)))
			return 4;
		if (_unit && _unit != unit && _unit->getPosition() != getPosition())
			return -1;
		setMapData(_objects[part]->getDataset()->getObjects()->at(_objects[part]->getAltMCD()), _objects[part]->getAltMCD(), _mapDataSetID[part],
				   _objects[part]->getDataset()->getObjects()->at(_objects[part]->getAltMCD())->getObjectType());
		setMapData(0, -1, -1, part);
		return 0;
	}
	if (_objects[part]->isUFODoor() && _currentFrame[part] == 0) // ufo door part 0 - door is closed
	{
		if (unit &&	unit->getTimeUnits() < _objects[part]->getTUCost(unit->getArmor()->getMovementType()) + unit->getActionTUs(reserve, unit->getMainHandWeapon(false)))
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

	return std::max(0, 15 - light);
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
		if (_objects[part]->isGravLift())
			return false;
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
		setMapData(MapDataSet::getScorchedEarthTile(), 1, 0, MapData::O_FLOOR);
	}
	return _objective;
}

/**
 * damage terrain - check against armor
 * @param part Part to check.
 * @param power Power of the damage.
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
 * The explosive power on the tile is some kind of moving MAXIMUM of the explosive rays that passes it.
 * @param power Power of the damage.
 * @param force Force damage.
 */
void Tile::setExplosive(int power, bool force)
{
	if (force || _explosive < power)
	{
		_explosive = power;
	}
}

/**
 * Get explosive on this tile.
 * @return explosive
 */
int Tile::getExplosive() const
{
	return _explosive;
}

/*
 * Flammability of a tile is the lowest flammability of it's objects.
 * @return Flammability : the lower the value, the higher the chance the tile/object catches fire.
 */
int Tile::getFlammability() const
{
	int flam = 255;

	for (int i=0; i<4; ++i)
		if (_objects[i] && (_objects[i]->getFlammable() < flam))
			flam = _objects[i]->getFlammable();

	return flam;
}

/*
 * Fuel of a tile is the highest fuel of it's objects.
 * @return how long to burn.
 */
int Tile::getFuel() const
{
	int fuel = 0;

	for (int i=0; i<4; ++i)
		if (_objects[i] && (_objects[i]->getFuel() > fuel))
			fuel = _objects[i]->getFuel();

	return fuel;
}


/*
 * Flammability of the particular part of the tile
 * @return Flammability : the lower the value, the higher the chance the tile/object catches fire.
 */
int Tile::getFlammability(int part) const
{
	return _objects[part]->getFlammable();
}

/*
 * Fuel of particular part of the tile
 * @return how long to burn.
 */
int Tile::getFuel(int part) const
{
	return _objects[part]->getFuel();
}

/*
 * Ignite starts fire on a tile, it will burn <fuel> rounds. Fuel of a tile is the highest fuel of its objects.
 * NOT the sum of the fuel of the objects!
 */
void Tile::ignite(int power)
{
	if (getFlammability() != 255)
	{
		power = power - (getFlammability() / 10) + 15;
		if (power < 0)
		{
			power = 0;
		}
		if (RNG::percent(power) && getFuel())
		{
			if (_fire == 0)
			{
				_smoke = 15 - std::max(1, std::min((getFlammability() / 10), 12));
				_overlaps = 1;
				_fire = getFuel() + 1;
				_animationOffset = RNG::generate(0,3);
			}
		}
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
			if (_objects[i]->isUFODoor() && _objects[i]->getSpecialType() == START_POINT && newframe == 3)
			{
				newframe = 7;
			}
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
 * @param tileBelow
 */
void Tile::setUnit(BattleUnit *unit, Tile *tileBelow)
{
	if (unit != 0)
	{
		unit->setTile(this, tileBelow);
	}
	_unit = unit;
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
	if (_fire == 0)
	{
		if (_overlaps == 0)
		{
			_smoke = std::max(1, std::min(_smoke + smoke, 15));
		}
		else
		{
			_smoke += smoke;
		}
		_animationOffset = RNG::generate(0,3);
		addOverlap();
	}
}

/**
 * Set the amount of turns this tile is smoking. 0 = no smoke.
 * @param smoke : amount of turns this tile is smoking.
 */
void Tile::setSmoke(int smoke)
{
	_smoke = smoke;
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
 * New turn preparations.
 * average out any smoke added by the number of overlaps.
 * apply fire/smoke damage to units as applicable.
 */
void Tile::prepareNewTurn()
{
	// we've recieved new smoke in this turn, but we're not on fire, average out the smoke.
	if ( _overlaps != 0 && _smoke != 0 && _fire == 0)
	{
		_smoke = std::max(0, std::min((_smoke / _overlaps)- 1, 15));
	}
	// if we still have smoke/fire
	if (_smoke)
	{
		if (_unit && !_unit->isOut())
		{
			if (_fire)
			{
				// this is how we avoid hitting the same unit multiple times.
				if (_unit->getArmor()->getSize() == 1 || !_unit->tookFireDamage())
				{
					_unit->toggleFireDamage();
					// _smoke becomes our damage value
					_unit->damage(Position(0, 0, 0), _smoke, DT_IN, true);
					// try to set the unit on fire.
					if (RNG::percent(40 * _unit->getArmor()->getDamageModifier(DT_IN)))
					{
						int burnTime = RNG::generate(0, int(5 * _unit->getArmor()->getDamageModifier(DT_IN)));
						if (_unit->getFire() < burnTime)
						{
							_unit->setFire(burnTime);
						}
					}
				}
			}
			// no fire: must be smoke
			else
			{
				// try to knock this guy out.
				if (_unit->getArmor()->getDamageModifier(DT_SMOKE) > 0.0 && _unit->getArmor()->getSize() == 1)
				{
					_unit->damage(Position(0,0,0), (_smoke / 4) + 1, DT_SMOKE, true);
				}
			}
		}
	}
	_overlaps = 0;
	_danger = false;
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
 * @param visibility
 */
void Tile::setVisible(int visibility)
{
	_visible += visibility;
}

/**
 * Get the tile visible flag.
 * @return visibility
 */
int Tile::getVisible()
{
	return _visible;
}

/**
 * set the direction used for path previewing.
 * @param dir
 */
void Tile::setPreview(int dir)
{
	_preview = dir;
}

/**
 * retrieve the direction stored by the pathfinding.
 * @return preview
 */
int Tile::getPreview() const
{
	return _preview;
}

/**
 * set the number to be displayed for pathfinding preview.
 * @param tu
 */
void Tile::setTUMarker(int tu)
{
	_TUMarker = tu;
}

/**
 * get the number to be displayed for pathfinding preview.
 * @return marker
 */
int Tile::getTUMarker() const
{
	return _TUMarker;
}

/**
 * get the overlap value of this tile.
 * @return overlap
 */
int Tile::getOverlaps() const
{
	return _overlaps;
}

/**
 * increment the overlap value on this tile.
 */
void Tile::addOverlap()
{
	++_overlaps;
}

/**
 * set the danger flag on this tile.
 */
void Tile::setDangerous()
{
	_danger = true;
}

/**
 * get the danger flag on this tile.
 * @return the danger flag for this tile.
 */
bool Tile::getDangerous()
{
	return _danger;
}

}
