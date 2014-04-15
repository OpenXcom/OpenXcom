/*
 * Copyright 2010-2013 OpenXcom Developers.
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

namespace OpenXcom
{

class SavedBattleGame;
class Craft;
class Ufo;
class RuleTerrain;
class ResourcePack;
class BattleItem;
class MapBlock;
class Vehicle;
class Tile;
class RuleItem;
class Unit;
class AlienRace;
class AlienDeployment;
class Game;
class Base;
class TerrorSite;
class AlienBase;
class BattleUnit;

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
	bool _allowAutoLoadout;
	int _craftX, _craftY, _craftZ;

	/// Generates a new battlescape map.
	void generateMap();
	/// Adds a vehicle to the game.
	BattleUnit *addXCOMVehicle(Vehicle *v);
	/// Adds a soldier to the game.
	BattleUnit *addXCOMUnit(BattleUnit *unit);
	/// Adds an alien to the game.
	BattleUnit *addAlien(Unit *rules, int alienRank, bool outside);
	/// Adds a civlian to the game.
	BattleUnit *addCivilian(Unit *rules);
	/// Places an item on a soldier based on equipment layout.
	bool placeItemByLayout(BattleItem *item);
	/// Adds an item to a unit and the game.
	bool addItem(BattleItem *item, BattleUnit *unit, bool allowSecondClip = false);
	/// Loads an XCom MAP file.
	int loadMAP(MapBlock *mapblock, int xoff, int yoff, RuleTerrain *terrain, int objectIDOffset, bool discovered = false, bool craft = false);
	/// Loads an XCom RMP file.
	void loadRMP(MapBlock *mapblock, int xoff, int yoff, int segment);
	/// Fills power sources with an elerium-115 object.
	void fuelPowerSources();
	/// Possibly explodes ufo powersources.
	void explodePowerSources();
	/// Deploys the XCOM units on the mission.
	void deployXCOM();
	/// Runs necessary checks before physically setting the position.
	bool canPlaceXCOMUnit(Tile *tile);
	/// Deploys the aliens, according to the alien deployment rules.
	void deployAliens(AlienRace *race, AlienDeployment *deployment);
	/// Spawns civilians on a terror mission.
	void deployCivilians(int max);
	/// Gets battlescape terrain.
	RuleTerrain *getTerrain(int tex, double lat);
public:
	/// Creates a new BattlescapeGenerator class
	BattlescapeGenerator(Game *game);
	/// Cleans up the BattlescapeGenerator.
	~BattlescapeGenerator();
	/// Sets the XCom craft.
	void setCraft(Craft *craft);
	/// Sets the ufo.
	void setUfo(Ufo* ufo);
	/// Sets the polygon texture.
	void setWorldTexture(int texture);
	/// Sets the polygon shade.
	void setWorldShade(int shade);
	/// Sets the alien race.
	void setAlienRace(const std::string &alienRace);
	/// Sets the alien item level.
	void setAlienItemlevel(int alienItemLevel);
	/// Sets the XCom base.
	void setBase(Base *base);
	/// Sets the terror site.
	void setTerrorSite(TerrorSite* site);
	/// Sets the alien base
	void setAlienBase(AlienBase* base);
	/// Runs the generator.
	void run();
	/// Sets up the next stage (for cydonia/tftd terror missions).
	void nextStage();
	/// Finds a spot near a friend to spawn at.
	bool placeUnitNearFriend(BattleUnit *unit);
	/// Generates an inventory battlescape.
	void runInventory(Craft *craft);
};

}

#endif
