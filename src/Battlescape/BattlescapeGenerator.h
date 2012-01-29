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
class RuleArmor;
class RuleItem;
class RuleGenUnit;
class RuleAlienRace;
class RuleAlienDeployment;
class Game;
class Base;

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
	Base *_base;
	RuleTerrain *_terrain;
	int _width, _length, _height;
	int _worldTexture, _worldShade;
	int _unitCount;
	Tile *_craftInventoryTile;
	std::string _alienRace;
	int _alienItemLevel;

	/// Generate a new battlescape map.
	void generateMap();
	/// links tiles with terrainobjects, for easier/faster lookup
	void linkTilesWithMapDatas();
	/// Add a soldier to the game
	BattleUnit *addXCOMUnit(BattleUnit *unit);
	/// Add an alien to the game
	BattleUnit *addAlien(RuleGenUnit *rules, int alienRank, bool outside);
	BattleUnit *addCivilian(RuleGenUnit *rules);
	/// Add an item to the game
	void addItem(RuleItem *item);
	// Add an item to a unit
	void addItem(RuleItem *item, BattleUnit *unit);
	/// loads an XCOM MAP file
	int loadMAP(MapBlock *mapblock, int xoff, int yoff, RuleTerrain *terrain, int objectIDOffset, bool discovered = false);
	/// loads an XCOM RMP file
	void loadRMP(MapBlock *mapblock, int xoff, int yoff, int segment);
	void explodePowerSources();
	void deployAliens(RuleAlienRace *race, RuleAlienDeployment *deployment);
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
	/// Set the alien race.
	void setAlienRace(std::string alienRace);
	/// Set the alien item level.
	void setAlienItemlevel(int alienItemLevel);
	/// Sets the xcom base.
	void setBase(Base *base);
	/// Runs the generator.
	void run();

};

}

#endif
