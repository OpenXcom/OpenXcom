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
#include "TerrainModifier.h"
#include <cmath>
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Sets up a TerrainModifier.
 * @param save pointer to SavedBattleGame object.
 */
TerrainModifier::TerrainModifier(SavedBattleGame *save) : _save(save)
{

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

	// At night don't check for sun shading.
	if (_save->getGlobalShade() > 5)
	{
		tile->setShadeSun(_save->getGlobalShade());
		return;
	}

	if (tile->getPosition().z < _save->getHeight() - 1
		&& ((_save->getTile(tile->getPosition() + Position(0,0,1))
		&& (_save->getTile(tile->getPosition() + Position(0,0,1))->getTerrainObject(O_FLOOR)
		|| _save->getTile(tile->getPosition() + Position(0,0,1))->getTerrainObject(O_OBJECT)))
			|| (_save->getTile(tile->getPosition() + Position(0,0,2))
		&& (_save->getTile(tile->getPosition() + Position(0,0,2))->getTerrainObject(O_FLOOR)
		|| _save->getTile(tile->getPosition() + Position(0,0,2))->getTerrainObject(O_OBJECT))))
		)
	{
		tile->setShadeSun(_save->getGlobalShade() + (4 - (_save->getGlobalShade()/2)));
	}
	else
	{
		tile->setShadeSun(_save->getGlobalShade());
	}
}

/**
  * Recalculate lighting for the complete battlescape: units,terrain,items,fire.
  */
void TerrainModifier::calculateLighting()
{
	// reset all light to 0 first
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
	{
		_save->getTiles()[i]->resetLight();
	}

	// add lighting of terrain
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
	{
		// only floors and objects can light up
		if (_save->getTiles()[i]->getTerrainObject(O_FLOOR)
			&& _save->getTiles()[i]->getTerrainObject(O_FLOOR)->getLightSource())
		{
			circularAffector(_save->getTiles()[i]->getPosition(), AFFECT_LIGHT, _save->getTiles()[i]->getTerrainObject(O_FLOOR)->getLightSource());
		}
		if (_save->getTiles()[i]->getTerrainObject(O_OBJECT)
			&& _save->getTiles()[i]->getTerrainObject(O_OBJECT)->getLightSource())
		{
			circularAffector(_save->getTiles()[i]->getPosition(), AFFECT_LIGHT, _save->getTiles()[i]->getTerrainObject(O_OBJECT)->getLightSource());
		}
	}

	// add lighting of items (flares)

	// add lighting of fire

	// add lighting of soldiers + recalculate line of sight
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); i++)
	{
		if ((*i)->getFaction() == FACTION_PLAYER)
		{
			circularAffector((*i)->getPosition(), AFFECT_LIGHT, 15);
			calculateLineOfSight(*i);
		}
	}

	// indicate we have finished recalculating
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); i++)
	{
		_save->getTiles()[i]->setLight();
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
		tile->setTerrainObject(0, O_FLOOR);
	}
	for (int i=0; i < _save->getHeight() - 1; i++)
	{
		calculateSunShading(_save->getTile(Position(tile->getPosition().x, tile->getPosition().y, i)));
	}
	calculateLighting();
}

/**
 * Calculates line of sight of a soldier. Every visible tile is marked + fog of war is removed.
 * @param unit
 */
void TerrainModifier::calculateLineOfSight(BattleUnit *unit)
{
	int impactz = unit->getPosition().z;
	int impactx = unit->getPosition().x;
	int impacty = unit->getPosition().y;
	int visibility, maxVisibility = 19; // ideally 20 tiles of visibility (including own tile, so actually 19)

	double startAngle[8] = { 45, 0, -45, 270, 225, 180, 135, 90 };
	double endAngle[8] = { 135, 90, 45, 360, 315, 270, 225, 180 };
	Tile *t = _save->getTile(Position(impactx, impacty, impactz));

	// a unique ID for this session, used to avoid tiles to be affected more than once.
	int sessionID = RNG::generate(1, 65000);

	t->isSeenBy(unit, sessionID); // the tile we are on, is already certainly seen

	// raytrace every 2 degrees and affect tiles on our way
	for (double te = startAngle[unit->getDirection()]-1; te <= endAngle[unit->getDirection()]+1; te += 1)
	{
		double cos_te = cos(te * M_PI / 180);
		double sin_te = sin(te * M_PI / 180);

		int oz = impactz, ox = impactx, oy = impacty;
		int l = 0;
		int vz, vx, vy;
		visibility = maxVisibility;
		while (visibility > 0)
		{
			l++;
			vz = impactz;
			vx = impactx +(int)((double)l*cos_te+0.5f);
			vy = impacty +(int)((double)l*sin_te+0.5f);

			t = _save->getTile(Position(vx,vy,vz));
			if (!t) break;
			visibility -= blockage(_save->getTile(Position(ox,oy,oz)), t, AFFECT_VISION);
			if (visibility > 0) 
			{
				t->isSeenBy(unit, sessionID);
				// if there is a door to the east or south, we see it too
				t = _save->getTile(Position(vx+1,vy,vz));
				if (t && t->getTerrainObject(O_WESTWALL) && (t->getTerrainObject(O_WESTWALL)->isDoor() || t->getTerrainObject(O_WESTWALL)->isUFODoor()))
				{
					t->isSeenBy(unit, sessionID);
				}
				t = _save->getTile(Position(vx,vy-1,vz));
				if (t && t->getTerrainObject(O_NORTHWALL) && (t->getTerrainObject(O_NORTHWALL)->isDoor() || t->getTerrainObject(O_NORTHWALL)->isUFODoor()))
				{
					t->isSeenBy(unit, sessionID);
				}
			}
			visibility--; // visibility drops by 1 each tile
			oz=vz;ox=vx;oy=vy;
		}
	}
}

/**
 * Affect tiles in a circular pattern. Can be used for light and all kinds of explosions (HE,smoke,fire,stun).
 * @param pointOfImpact The position where the circular pattern starts.
 * @param affector The kind of affector.
 * @param power The initial power at the point of impact.
 */
void TerrainModifier::circularAffector(const Position &pointOfImpact, Affector affector, int power)
{
	int impactz = pointOfImpact.z;
	int impactx = pointOfImpact.x;
	int impacty = pointOfImpact.y;
	int power_;

	// a unique ID for this session, used to avoid tiles to be affected more than once.
	int sessionID = RNG::generate(1,65000);

	// raytrace every 2 degrees and affect tiles on our way
	for (double te = 0; te <= 360; te += 2)
	{
		double cos_te = cos(te * M_PI / 180);
		double sin_te = sin(te * M_PI / 180);

		int oz = impactz, ox = impactx, oy = impacty;
		int l = 0;
		int vz, vx, vy;
		power_ = power;
		while (power_ > 0)
		{
			l++;
			vz = impactz;
			vx = impactx +(int)((double)l*cos_te+0.5f);
			vy = impacty +(int)((double)l*sin_te+0.5f);

			Tile *t = _save->getTile(Position(vx,vy,vz));
			if (!t) break;
			power_ -= blockage(_save->getTile(Position(ox,oy,oz)), t, affector);
			if (power_ > 0)
			{
				if (affector == AFFECT_LIGHT)
					t->addLight(power_, sessionID);
			}
			power_--;
			oz=vz;ox=vx;oy=vy;
		}
	}

}
/*
 * The amount this part of the tile blocks.
 * @param tile
 * @param part
 * @param affector
 * @return amount of blockage
 */
int TerrainModifier::blockage(Tile *tile, const int part, Affector affector)
{
	int blockage = 0;
	if (tile == 0) return 0; // probably outside the map here
	if (tile->getTerrainObject(part))
		blockage += tile->getTerrainObject(part)->getBlock((int)affector);

	// open ufo doors are actually still closed behind the scenes
	// so a special trick is needed to see if they are open, if they are, they obviously don't block anything
	if (tile->isUfoDoorOpen(part)) 
		blockage = 0;

	if (tile->getTerrainObject(O_OBJECT))
		blockage += tile->getTerrainObject(O_OBJECT)->getBlock((int)affector);
	return blockage;
}

/**
 * The amount of power that is blocked going from one tile to another.
 * @param startTile
 * @param endTile
 * @param affector
 * @return amount of blockage
 */
int TerrainModifier::blockage(Tile *startTile, Tile *endTile, Affector affector)
{
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

}
