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
#ifndef OPENXCOM_BATTLESCAPEGENERATOR_H
#define OPENXCOM_BATTLESCAPEGENERATOR_H

#include "../Savegame/Node.h"
#include "../Savegame/SavedBattleGame.h"

namespace OpenXcom
{

class SavedBattleGame;
class Craft;
class Ufo;
class RuleTerrain;
class ResourcePack;
class RuleSet;
class Soldier;
class RuleUnitSprite;
class Item;
class Game;

/**
 * A utility class that generates the initial battlescape data. Taking into account mission type, craft and ufo involved, terrain type,...
 */
class BattlescapeGenerator
{
private:
	Game *_game;
	SavedBattleGame *_save;
	ResourcePack *_res;
	Craft *_craft;
	Ufo *_ufo;
	RuleTerrain *_terrain;
	int _width, _length, _height;
	int _worldTexture, _worldShade;
	MissionType _missionType;
	int _unitCount;

	/// Generate a new battlescape map.
	void generateMap();
	/// links tiles with terrainobjects, for easier/faster lookup
	void linkTilesWithMapDatas();
	/// Add a soldier to the game
	void addSoldier(Soldier *soldier, RuleUnitSprite *rules);
	/// Add an alien to the game
	void addAlien(RuleUnitSprite *rules, NodeRank rank, const std::wstring &name);
	/// Add an item to the game
	void addItem(Item *item);
	/// loads an XCOM MAP file
	int loadMAP(MapBlock *mapblock, int xoff, int yoff, RuleTerrain *terrain, bool discovered = false);
	/// loads an XCOM RMP file
	void loadRMP(MapBlock *mapblock, int xoff, int yoff);
public:
	/// Creates a new BattlescapeGenerator class
	BattlescapeGenerator(Game *game);
	/// Cleans up the BattlescapeGenerator.
	~BattlescapeGenerator();
	/// Sets the xcom craft.
	void setCraft(Craft *craft);
	/// Sets the ufo.
	void setUfo(Ufo* ufo);
	/// Sets the polygon texture.
	void setWorldTexture(int texture);
	/// Sets the polygon shade.
	void setWorldShade(int shade);
	/// Set the mission type.
	void setMissionType(MissionType missionType);
	/// Runs the generator.
	void run();

};

}

#endif
