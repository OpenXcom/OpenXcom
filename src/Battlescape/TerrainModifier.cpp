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
#include "TerrainModifier.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/BattleUnit.h"

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
  * Clear fog of war around an xcom unit. It is possible it discovers aliens.
  * @param unit 
  */
void TerrainModifier::clearFogOfWar(BattleUnit *unit)
{

}

/**
  * Restore fog of war from an xcom unit (when it leaves a certain position, or he dies).
  * @param unit 
  */
void TerrainModifier::restoreFogOfWar(BattleUnit *unit)
{

}

}
