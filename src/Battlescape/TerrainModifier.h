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
#ifndef OPENXCOM_TERRAINMODIFIER_H
#define OPENXCOM_TERRAINMODIFIER_H


#include "Position.h"
class SavedBattleGame;
class BattleUnit;

/**
 * A utility class that modifies tile properties on a battlescape map. This includes terrain destruction, smoke, fire, fog of war.
 */
class TerrainModifier
{
private:
	SavedBattleGame *_save;
public:
	/// Creates a new TerrainModifier class
	TerrainModifier(SavedBattleGame *save);
	/// Cleans up the TerrainModifier.
	~TerrainModifier();
	/// Clear fog of war around an xcom unit.
	void clearFogOfWar(BattleUnit *unit);
	/// Restore fog of war from an xcom unit (when it leaves a certain position, or he dies).
	void restoreFogOfWar(BattleUnit *unit);
};

#endif
