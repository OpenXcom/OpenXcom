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
#define _USE_MATH_DEFINES
#include <cmath>
#include "TerrainModifier.h"
#include "SDL.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Engine/RNG.h"
#include "../Ruleset/MapDataSet.h"

namespace OpenXcom
{

/**
 * Sets up a TerrainModifier.
 * @param save pointer to SavedBattleGame object.
 */
TerrainModifier::TerrainModifier(SavedBattleGame *save) : _save(save)
{
	distances.resize(16);
	for (int x = 0; x < 16; x++)
	{
	    distances[x].resize(16);
		for (int y = 0; y < 16; y++)
		{
			distances[x][y] = int(floor(sqrt(float(x*x + y*y)) + 0.5));
		}
	}
}

/**
 * Deletes the TerrainModifier.
 */
TerrainModifier::~TerrainModifier()
{

}


/**
  * Calculate sun shading for the whole terrain.
  */
void TerrainModifier::calculateSunShading()
{
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
	{
		calculateSunShading(_save->getTiles()[i]);
	}	
}

/**
  * Calculate sun shading for 1 tile. Sun comes from above and is blocked by floors or objects.
  * @param tile The tile to calculate sun shading for.
  */
void TerrainModifier::calculateSunShading(Tile *tile)
{

	int power = 15 - _save->getGlobalShade();

	// At night/dusk sun isn't dropping shades
	if (_save->getGlobalShade() <= 5)
	{
		if (verticalBlockage(_save->getTile(Position(tile->getPosition().x, tile->getPosition().y, _save->getHeight() - 1)), tile, AFFECT_LIGHT))
		{
			power-=2;
		}
	}

	tile->addLight(power, 0, 0);
}

/**
  * Recalculate lighting for the terrain: objects,items,fire.
  */
void TerrainModifier::calculateTerrainLighting()
{
	const int layer = 1;

	// during daytime don't calculate lighting
	if (_save->getGlobalShade() < 1)
		return;

	// reset all light to 0 first
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
	{
		_save->getTiles()[i]->resetLight(layer);
	}

	// add lighting of terrain
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
	{
		// only floors and objects can light up
		if (_save->getTiles()[i]->getMapData(O_FLOOR)
			&& _save->getTiles()[i]->getMapData(O_FLOOR)->getLightSource())
		{
			addLight(_save->getTiles()[i]->getPosition(), _save->getTiles()[i]->getMapData(O_FLOOR)->getLightSource(), layer);
		}
		if (_save->getTiles()[i]->getMapData(O_OBJECT)
			&& _save->getTiles()[i]->getMapData(O_OBJECT)->getLightSource())
		{
			addLight(_save->getTiles()[i]->getPosition(), _save->getTiles()[i]->getMapData(O_OBJECT)->getLightSource(), layer);
		}

		// fires
		if (_save->getTiles()[i]->getFire())
		{
			addLight(_save->getTiles()[i]->getPosition(), 15, layer);
		}

	}

	// add lighting of items (flares)

	// indicate we have finished recalculating
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
	{
		_save->getTiles()[i]->setLight(layer);
	}
}

/**
  * Recalculate lighting for the units.
  */
void TerrainModifier::calculateUnitLighting()
{
	const int layer = 2;

	// during daytime don't calculate lighting
	if (_save->getGlobalShade() < 1)
		return;

	// reset all light to 0 first
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
	{
		_save->getTiles()[i]->resetLight(layer);
	}

	// add lighting of soldiers
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); i++)
	{
		if ((*i)->getFaction() == FACTION_PLAYER)
		{
			addLight((*i)->getPosition(), 15, layer);
		}
	}

	// indicate we have finished recalculating
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
	{
		_save->getTiles()[i]->setLight(layer);
	}
}


/**
 * Destroys all parts of a tile. This is used for testing purposes (press 'd' on battlescape)
 * @param tile
 */
void TerrainModifier::destroyTile(Tile *tile)
{
	tile->destroy(0);
	tile->destroy(1);
	tile->destroy(2);
	tile->destroy(3);
	if (tile->getPosition().z > 0)
	{
		tile->setMapData(0, O_FLOOR);
	}
	// on bottom level, if floor is destroyed, it turns into burned ground from blanks.mcd
	if (tile->getPosition().z == 0 && tile->getMapData(O_FLOOR) == 0)
	{
		tile->setMapData(_save->getMapDataSets()->at(0)->getObjects()->at(0), O_FLOOR);
	}
	for (int i=0; i < _save->getHeight() - 1; i++)
	{
		calculateSunShading(_save->getTile(Position(tile->getPosition().x, tile->getPosition().y, i)));
	}

	// recalculate line of sight (to optimise: only units in range)
	calculateFOV(tile->getPosition());

}

/**
 * Calculates line of sight of a soldier. Every visible tile is marked + fog of war is removed.
 * @param unit
 */
void TerrainModifier::calculateFOV(BattleUnit *unit)
{
	double startAngle[8] = { 45, 0, -45, 270, 225, 180, 135, 90 };
	double endAngle[8] = { 135, 90, 45, 360, 315, 270, 225, 180 };

	double centerZ = (unit->getPosition().z * 2) + 1.5;
	double centerX = unit->getPosition().x + 0.5;
	double centerY = unit->getPosition().y + 0.5;
	int power_, objectFalloff;
	double startFi = -90;
	double endFi = 60;

	if (unit->getPosition().z == 0)
	{
		startFi = 0;
	}

	// a unique ID for this session, used to avoid tiles to be affected more than once.
	int sessionID = RNG::generate(1,65000);

	// obviously
	_save->getTile(unit->getPosition())->isSeenBy(unit, sessionID);

	// raytrace up and down
	for (double fi = startFi; fi <= endFi; fi += 6)
	{
		double cos_fi = cos(fi * M_PI / 180.0);
		double sin_fi = sin(fi * M_PI / 180.0);

		// raytrace every 3 degrees makes sure we cover all tiles in a circle.
		for (double te = startAngle[unit->getDirection()]; te <= endAngle[unit->getDirection()]; te += 3)
		{
			double cos_te = cos(te * M_PI / 180.0);
			double sin_te = sin(te * M_PI / 180.0);
			//double cos_te = _cosTable[(int)te/3];
			//double sin_te = _sinTable[(int)te/3];

			Tile *origin = _save->getTile(unit->getPosition());
			double l = 0;
			double vx, vy, vz;
			int tileX, tileY, tileZ;
			power_ = 20;

			while (power_ > 0)
			{
				l++;
				vx = centerX + l * cos_te * cos_fi;
				vy = centerY + l * sin_te * cos_fi;
				vz = centerZ + l * sin_fi;

				tileZ = int(floor(vz / 2.0));
				tileX = int(floor(vx));
				tileY = int(floor(vy));

				power_--;

				Tile *dest = _save->getTile(Position(tileX, tileY, tileZ));
				if (!dest) break; // out of map!

				// horizontal blockage by walls
				power_ -= horizontalBlockage(origin, dest, AFFECT_VISION);

				// vertical blockage by ceilings/floors
				power_ -= verticalBlockage(origin, dest, AFFECT_VISION);

				// objects on destination tile affect the ray after it has crossed this tile
				// but it has to be calculated before we affect the tile (it could have been blown up)
				if (dest->getMapData(O_OBJECT))
				{
					objectFalloff = dest->getMapData(O_OBJECT)->getBlock(AFFECT_VISION);
				}
				else
				{
					objectFalloff = 0;
				}

				// smoke decreases visibility
				if (dest->getSmoke())
				{
					objectFalloff += int(dest->getSmoke() / 3);
				}

				if (power_ > 0 && !dest->isChecked(sessionID) && dest->getShade() < 10)
				{
					dest->isSeenBy(unit, sessionID);
					// if there is a door to the east or south of a visible tile, we see that too
					Tile* t = _save->getTile(Position(tileX + 1, tileY, tileZ));
					if (t && t->getMapData(O_WESTWALL) && (t->getMapData(O_WESTWALL)->isDoor() || t->getMapData(O_WESTWALL)->isUFODoor()))
					{
						t->isSeenBy(unit, sessionID);
					}
					t = _save->getTile(Position(tileX, tileY - 1, tileZ));
					if (t && t->getMapData(O_NORTHWALL) && (t->getMapData(O_NORTHWALL)->isDoor() || t->getMapData(O_NORTHWALL)->isUFODoor()))
					{
						t->isSeenBy(unit, sessionID);
					}
				}
				power_ -= objectFalloff;
				origin = dest;
			}
		}
	}
}

/**
 * Calculates line of sight of a soldiers within range of the Position.
 * @param position
 */
void TerrainModifier::calculateFOV(const Position &position)
{
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); i++)
	{
		if ((*i)->getFaction() == FACTION_PLAYER)
		{
			calculateFOV(*i);
		}
	}
}

/**
 * Adds simple light overlay.
 * @param center
 * @param power
 * @param layer
 */
void TerrainModifier::addLight(const Position &center, int power, int layer)
{
	int sessionID = RNG::generate(1,65000);
	for (int x = 0; x <= power; x++)
	{
		for (int y = 0; y <= power; y++)
		{
			for (int z = 0; z < _save->getHeight(); z++)
			{
				//int distance = sqrt(float(x*x + y*y));
				if (_save->getTile(Position(center.x + x,center.y + y, z)))
				_save->getTile(Position(center.x + x,center.y + y, z))->addLight(power - distances[x][y], sessionID, layer);
				if (_save->getTile(Position(center.x - x,center.y - y, z)))
				_save->getTile(Position(center.x - x,center.y - y, z))->addLight(power - distances[x][y], sessionID, layer);
				if (_save->getTile(Position(center.x - x,center.y + y, z)))
				_save->getTile(Position(center.x - x,center.y + y, z))->addLight(power - distances[x][y], sessionID, layer);
				if (_save->getTile(Position(center.x + x,center.y - y, z)))
				_save->getTile(Position(center.x + x,center.y - y, z))->addLight(power - distances[x][y], sessionID, layer);
			}
		}
	}
}

/*
 * The amount this certain wall or floor-part of the tile blocks.
 * @param tile
 * @param part
 * @param affector
 * @return amount of blockage
 */
int TerrainModifier::blockage(Tile *tile, const int part, Affector affector)
{
	int blockage = 0;
	
	if (tile == 0) return 0; // probably outside the map here

	if (part == O_FLOOR && tile->getMapData(O_FLOOR))
	{
		// blockage modifiers of floors in ufo only counted for horizontal stuff, so this is kind of an experiment
		if (affector == AFFECT_LIGHT)
			blockage += 2;	
		else if (affector == AFFECT_HE)
			blockage += 15;	
		else
			blockage += 255;
	}
	else
	{
		if (tile->getMapData(part))
			blockage += tile->getMapData(part)->getBlock(affector);

		// open ufo doors are actually still closed behind the scenes
		// so a special trick is needed to see if they are open, if they are, they obviously don't block anything
		if (tile->isUfoDoorOpen(part)) 
			blockage = 0;
	}

	return blockage;
}

/**
 * HE, smoke and fire explodes in a circular pattern on 1 level only. HE however damages floor tiles of the above level. Not the units on it.
 * HE destroys an object if its armor is lower than the explosive power, then it's HE blockage is applied for further propagation.
 * See http://www.ufopaedia.org/index.php?title=Explosions for more info.
 * @param center
 * @param power
 * @param affector
 */
void TerrainModifier::explode(const Position &center, int power, Affector affector, int maxRadius)
{
	double centerZ = center.z;
	double centerX = center.x + 0.5;
	double centerY = center.y + 0.5;
	int power_;

	// a unique ID for this session, used to avoid tiles to be affected more than once.
	int sessionID = RNG::generate(1,65000);

	// raytrace every 3 degrees makes sure we cover all tiles in a circle.
	for (double te = 0; te <= 360; te += 3)
	{
		double cos_te = cos(te * M_PI / 180.0);
		double sin_te = sin(te * M_PI / 180.0);
		//double cos_te = _cosTable[(int)te/3];
		//double sin_te = _sinTable[(int)te/3];

		Tile *origin = _save->getTile(center);
		double l = 0;
		double vx, vy, vz;
		int tileX, tileY, tileZ;
		power_ = power;

		while (power_ > 0 && l <= maxRadius)
		{
			vx = centerX + l * cos_te;
			vy = centerY + l * sin_te;
			vz = centerZ;

			tileZ = int(floor(vz));
			tileX = int(floor(vx));
			tileY = int(floor(vy));

			Tile *dest = _save->getTile(Position(tileX, tileY, tileZ));
			if (!dest) break; // out of map!

			// horizontal blockage by walls
			power_ -= horizontalBlockage(origin, dest, affector);

			if (power_ > 0/* && !dest->isChecked(sessionID)*/)
			{
				if (affector == AFFECT_HE)
				{
					// explosives do 1/2 damage to terrain and 1/2 up to 3/2 random damage to units
					dest->setExplosive(power_ / 2, sessionID);
					// TODO: destroy floors above
				
				}
				if (affector == AFFECT_SMOKE)
				{
					// smoke from explosions always stay 15 to 20 turns
					dest->setSmoke(RNG::generate(15, 20));
				}
			}

			power_ -= 10; // explosive damage decreases by 10

			// objects on destination tile affect the ray after it has crossed this tile
			// but it has to be calculated before we affect the tile (it could have been blown up)
			if (dest->getMapData(O_OBJECT))
			{
				power_ -= dest->getMapData(O_OBJECT)->getBlock(affector);
			}
			origin = dest;
			l++;
		}
	}

	// indicate we have finished recalculating
	if (affector == AFFECT_HE)
	{
		for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
		{
			_save->getTiles()[i]->detonate();
		}
	}

	// recalculate line of sight (to optimise: only units in range)
	calculateFOV(center);
}

/**
 * The amount of power that is blocked going from one tile to another on a different level.
 * Can cross more than one level.
 * @param startTile
 * @param endTile
 * @param affector
 * @return amount of blockage
 */
int TerrainModifier::verticalBlockage(Tile *startTile, Tile *endTile, Affector affector)
{
	int block = 0;

	// safety check
	if (startTile == 0 || endTile == 0) return 0;

	int direction = endTile->getPosition().z - startTile->getPosition().z;
	int x = startTile->getPosition().x;
	int y = startTile->getPosition().y;

	if (direction < 0) // down
	{
		for (int z = startTile->getPosition().z; z > endTile->getPosition().z; z--)
		{
			block += blockage(_save->getTile(Position(x, y, z)), O_FLOOR, affector);
		}
	}
	else if (direction > 0) // up
	{
		for (int z = startTile->getPosition().z + 1; z <= endTile->getPosition().z; z++)
		{
			block += blockage(_save->getTile(Position(x, y, z)), O_FLOOR, affector);
		}
	}

	return block;
}

/**
 * The amount of power that is blocked going from one tile to another on the same level.
 * @param startTile
 * @param endTile
 * @param affector
 * @return amount of blockage
 */
int TerrainModifier::horizontalBlockage(Tile *startTile, Tile *endTile, Affector affector)
{
	// safety check
	if (startTile == 0 || endTile == 0) return 0;

	int direction = vectorToDirection(endTile->getPosition() - startTile->getPosition());
	if (direction == -1) return 0;

	switch(direction)
	{
	case 0:	// north
		return blockage(startTile, O_NORTHWALL, affector);
		break;
	case 1: // north east
		return (blockage(startTile,O_NORTHWALL, affector) + blockage(endTile,O_WESTWALL, affector))/2
			+ (blockage(_save->getTile(startTile->getPosition() + Position(1, 0, 0)),O_WESTWALL, affector)
			+ blockage(_save->getTile(startTile->getPosition() + Position(1, 0, 0)),O_NORTHWALL, affector))/2;
		break;
	case 2: // east
		return blockage(endTile,O_WESTWALL, affector);
		break;
	case 3: // south east
		return (blockage(endTile,O_WESTWALL, affector) + blockage(endTile,O_NORTHWALL, affector))/2
			+ (blockage(_save->getTile(startTile->getPosition() + Position(1, 0, 0)),O_WESTWALL, affector)
			+ blockage(_save->getTile(startTile->getPosition() + Position(0, -1, 0)),O_NORTHWALL, affector))/2;
		break;
	case 4: // south
		return blockage(endTile,O_NORTHWALL, affector);
		break;
	case 5: // south west
		return (blockage(endTile,O_NORTHWALL, affector) + blockage(startTile,O_WESTWALL, affector))/2
			+ (blockage(_save->getTile(startTile->getPosition() + Position(0, -1, 0)),O_WESTWALL, affector)
			+ blockage(_save->getTile(startTile->getPosition() + Position(0, -1, 0)),O_NORTHWALL, affector))/2;
		break;
	case 6: // west
		return blockage(startTile,O_WESTWALL, affector);
		break;
	case 7: // north west
		return (blockage(startTile,O_WESTWALL, affector) + blockage(startTile,O_NORTHWALL, affector))/2
			+ (blockage(_save->getTile(startTile->getPosition() + Position(0, 1, 0)),O_WESTWALL, affector)
			+ blockage(_save->getTile(startTile->getPosition() + Position(-1, 0, 0)),O_NORTHWALL, affector))/2;
		break;
	}

	return false;
}

/*
 * Converts direction to a vector. Direction starts north = 0 and goes clockwise.
 * @param vector pointer to a position (which acts as a vector)
 * @return direction
 */
int TerrainModifier::vectorToDirection(const Position &vector)
{
	int x[8] = {0, 1, 1, 1, 0, -1, -1, -1};
	int y[8] = {1, 1, 0, -1, -1, -1, 0, 1};
	for (int i=0;i<9;i++)
	{
		if (x[i] == vector.x && y[i] == vector.y)
			return i;
	}
	return -1;
}

/**
 * Soldier opens a door (if any) by rightclick, or by walking through it.
 * Normal door changes the tile objects. We need to make a sound 3 here.
 * Ufo door updates the ufodooropened flag of the tile. We need to make a sound 20 or 21 and start the animation.
 * An ufo door takes a few animation frames: while this animation is running this function returns true.
 * @param unit
 * @return 0 normal door opened, 1 ufo door is starting to open, 3 ufo door is still opening, -1 there is no door.
 */
int TerrainModifier::unitOpensDoor(BattleUnit *unit)
{
	int door = -1;
	Tile *tile = _save->getTile(unit->getPosition());
	switch(unit->getDirection())
	{
	case 0:	// north
		door = tile->openDoor(O_NORTHWALL);
		break;
	case 2: // east
		tile = _save->getTile(tile->getPosition() + Position(1, 0, 0));
		if (tile) door = tile->openDoor(O_WESTWALL);
		break;
	case 4: // south
		tile = _save->getTile(tile->getPosition() + Position(0, -1, 0));
		if (tile) door = tile->openDoor(O_NORTHWALL);
		break;
	case 6: // west
		door = tile->openDoor(O_WESTWALL);
		break;
	}

	if (door == 0 || door == 1)
	{
		_save->getTerrainModifier()->calculateFOV(tile->getPosition());
	}

	return door;
}


}
