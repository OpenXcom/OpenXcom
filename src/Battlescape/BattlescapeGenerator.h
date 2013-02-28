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
class Vehicle;
class Armor;
class RuleItem;
class Unit;
class AlienRace;
class AlienDeployment;
class Game;
class Base;
class TerrorSite;
class AlienBase;

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
	TerrorSite *_terror;
	AlienBase *_alienBase;
	RuleTerrain *_terrain;
	int _mapsize_x, _mapsize_y, _mapsize_z;
	int _worldTexture, _worldShade;
	int _unitSequence;
	Tile *_craftInventoryTile;
	std::string _alienRace;
	int _alienItemLevel;

	/// Generate a new battlescape map.
	void generateMap();
	/// links tiles with terrainobjects, for easier/faster lookup
	void linkTilesWithMapDatas();
	/// Add a vehicle to the game
	void addXCOMVehicle(Vehicle *v);
	/// Add a soldier to the game
	BattleUnit *addXCOMUnit(BattleUnit *unit);
	/// Add an alien to the game
	BattleUnit *addAlien(Unit *rules, int alienRank, bool outside);
	BattleUnit *addCivilian(Unit *rules);
	/// Place an item to a soldier based on equipment layout
	BattleItem* placeItemByLayout(BattleItem *item);
	/// Add an item to the game
	BattleItem* addItem(BattleItem *item);
	// Add an item to a unit
	BattleItem* addItem(RuleItem *item, BattleUnit *unit);
	/// loads an XCOM MAP file
	int loadMAP(MapBlock *mapblock, int xoff, int yoff, RuleTerrain *terrain, int objectIDOffset, bool discovered = false);
	/// loads an XCOM RMP file
	void loadRMP(MapBlock *mapblock, int xoff, int yoff, int segment);
	void fuelPowerSources();
	void explodePowerSources();
	void deployAliens(AlienRace *race, AlienDeployment *deployment);
	void deployCivilians(int max);
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
	void setAlienRace(const std::string &alienRace);
	/// Set the alien item level.
	void setAlienItemlevel(int alienItemLevel);
	/// Sets the xcom base.
	void setBase(Base *base);
	/// Sets the terror site.
	void setTerrorSite(TerrorSite* site);
	/// Sets the alien base
	void setAlienBase(AlienBase* base);
	/// Runs the generator.
	void run();
	void nextStage();

};

}

#endif
