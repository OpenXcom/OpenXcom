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

#include "SavedBattleGame.h"
#include "SavedGame.h"
#include "Tile.h"
#include "Node.h"
#include "SDL.h"
#include "Unit.h"
#include "Alien.h"
#include "../Ruleset/MapDataSet.h"
#include "../Battlescape/Pathfinding.h"
#include "../Battlescape/TerrainModifier.h"
#include "../Battlescape/Position.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Ruleset.h"
#include "../Engine/Language.h"
#include "../Ruleset/RuleInventory.h"
#include "../Battlescape/PatrolBAIState.h"
#include "../Battlescape/AggroBAIState.h"

namespace OpenXcom
{

/**
 * Initializes a brand new battlescape saved game.
 */
SavedBattleGame::SavedBattleGame() : _tiles(), _selectedUnit(0), _nodes(), _units(), _items(), _pathfinding(0), _terrainModifier(0), _missionType(MISS_TERROR), _side(FACTION_PLAYER), _turn(1), _debugMode(false), _aborted(false), _itemId(0)
{
	_debriefingStats.push_back(new DebriefingStat("STR_ALIENS_KILLED", false));
	_debriefingStats.push_back(new DebriefingStat("STR_ALIEN_CORPSES_RECOVERED", false));
	_debriefingStats.push_back(new DebriefingStat("STR_LIVE_ALIENS_RECOVERED", false));
	_debriefingStats.push_back(new DebriefingStat("STR_ALIEN_ARTIFACTS_RECOVERED", false));
	_debriefingStats.push_back(new DebriefingStat("STR_ALIEN_BASE_CONTROL_DESTROYED", false));
	_debriefingStats.push_back(new DebriefingStat("STR_CIVILIANS_KILLED_BY_ALIENS", false));
	_debriefingStats.push_back(new DebriefingStat("STR_CIVILIANS_KILLED_BY_XCOM_OPERATIVES", false));
	_debriefingStats.push_back(new DebriefingStat("STR_CIVILIANS_SAVED", false));
	_debriefingStats.push_back(new DebriefingStat("STR_XCOM_OPERATIVES_KILLED", false));
	//_debriefingStats.push_back(new DebriefingStat("STR_XCOM_OPERATIVES_RETIRED_THROUGH_INJURY", false));
	_debriefingStats.push_back(new DebriefingStat("STR_XCOM_OPERATIVES_MISSING_IN_ACTION", false));
	_debriefingStats.push_back(new DebriefingStat("STR_TANKS_DESTROYED", false));
	_debriefingStats.push_back(new DebriefingStat("STR_XCOM_CRAFT_LOST", false));
	_debriefingStats.push_back(new DebriefingStat("STR_UFO_POWER_SOURCE", true));
	_debriefingStats.push_back(new DebriefingStat("STR_UFO_NAVIGATION", true));
	_debriefingStats.push_back(new DebriefingStat("STR_UFO_CONSTRUCTION", true));
	_debriefingStats.push_back(new DebriefingStat("STR_ALIEN_FOOD", true));
	_debriefingStats.push_back(new DebriefingStat("STR_ALIEN_REPRODUCTION", true));
	_debriefingStats.push_back(new DebriefingStat("STR_ALIEN_ENTERTAINMENT", true));
	_debriefingStats.push_back(new DebriefingStat("STR_ALIEN_SURGERY", true));
	_debriefingStats.push_back(new DebriefingStat("STR_EXAMINATION_ROOM", true));
	_debriefingStats.push_back(new DebriefingStat("STR_ALIEN_ALLOYS", true));
	_debriefingStats.push_back(new DebriefingStat("STR_ELERIUM_115", true));
}

/**
 * Deletes the game content from memory.
 */
SavedBattleGame::~SavedBattleGame()
{
	for (int i = 0; i < _height * _length * _width; ++i)
	{
		delete _tiles[i];
	}
	delete[] _tiles;

	for (std::vector<Node*>::iterator i = _nodes.begin(); i != _nodes.end(); ++i)
	{
		delete *i;
	}

	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		delete *i;
	}

	for (std::vector<BattleItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		delete *i;
	}

	for (std::vector<DebriefingStat*>::iterator i = _debriefingStats.begin(); i != _debriefingStats.end(); ++i)
	{
		delete *i;
	}

	delete _pathfinding;
	delete _terrainModifier;
}

/**
 * Loads the saved battle game from a YAML file.
 * @param node YAML node.
 */
void SavedBattleGame::load(const YAML::Node &node, Ruleset *rule, SavedGame* savedGame)
{
	unsigned int size = 0;
	int a;
	int selectedUnit = 0;

	node["width"] >> _width;
	node["length"] >> _length;
	node["height"] >> _height;
	node["globalshade"] >> _globalShade;
	node["selectedUnit"] >> selectedUnit;

	for (YAML::Iterator i = node["mapdatasets"].begin(); i != node["mapdatasets"].end(); ++i)
	{
		std::string name;
		*i >> name;
		MapDataSet *mds = new MapDataSet(name);
		_mapDataSets.push_back(mds);
	}

	initMap(_width, _length, _height);

	for (YAML::Iterator i = node["tiles"].begin(); i != node["tiles"].end(); ++i)
	{
		Position pos;
		(*i)["position"][0] >> pos.x;
		(*i)["position"][1] >> pos.y;
		(*i)["position"][2] >> pos.z;
		getTile(pos)->load((*i));
	}

	for (YAML::Iterator i = node["nodes"].begin(); i != node["nodes"].end(); ++i)
	{
		Node *n = new Node();
		n->load(*i);
		_nodes.push_back(n);
	}

	for (YAML::Iterator i = node["units"].begin(); i != node["units"].end(); ++i)
	{
		UnitFaction faction;

		(*i)["faction"] >> a;
		faction = (UnitFaction)a;

		(*i)["soldierId"] >> a;

		Unit *unit;
		if (a != -1) // Unit is linked to a geoscape soldier
		{
			// look up the matching soldier
			unit = savedGame->getSoldier(a);
		}
		else
		{
			// create a new Alien Unit.
			unit = new Alien(rule->getAlien("SECTOID_SOLDIER"), rule->getArmor("SECTOID_ARMOR0"));
		}
		BattleUnit *b = new BattleUnit(unit, faction);
		b->load(*i);
		_units.push_back(b);
		if (faction == FACTION_PLAYER)
		{
			if (b->getId() == selectedUnit)
				_selectedUnit = b;
		}
		else
		{
			std::string state;
			BattleAIState *aiState;
			(*i)["AI"]["state"] >> state;
			if (state == "PATROL")
			{
				aiState = new PatrolBAIState(this, b, 0);
			}
			if (state == "AGGRO")
			{
				aiState = new AggroBAIState(this, b);
			}
			aiState->load((*i)["AI"]);
			b->setAIState(aiState);
		}
	}

	// matches up tiles and units
	resetUnitTiles();

	for (YAML::Iterator i = node["items"].begin(); i != node["items"].end(); ++i)
	{
		std::string type;
		(*i)["type"] >> type;
		if (type != "0")
		{
			BattleItem *item = new BattleItem(rule->getItem(type), &_itemId);
			item->load(*i);
			(*i)["inventoryslot"] >> type;
			if (type != "NULL")
				item->setSlot(rule->getInventory(type));
			(*i)["owner"] >> a;

			// match up items and units
			for (std::vector<BattleUnit*>::iterator bu = _units.begin(); bu != _units.end(); ++bu)
			{
				if ((*bu)->getId() == a)
				{
					item->moveToOwner(*bu);
					break;
				}
			}

			// match up items and tiles
			if (item->getSlot() && item->getSlot()->getType() == INV_GROUND)
			{
				Position pos;
				(*i)["position"][0] >> pos.x;
				(*i)["position"][1] >> pos.y;
				(*i)["position"][2] >> pos.z;
				if (pos.x != -1)
					getTile(pos)->addItem(item);
			}
			_items.push_back(item);
		}
	}

	// tie ammo items to their weapons, running through the items again
	std::vector<BattleItem*>::iterator weaponi = _items.begin();
	for (YAML::Iterator i = node["items"].begin(); i != node["items"].end(); ++i, ++weaponi)
	{
		(*i)["ammoItem"] >> a;
		if (a != -1)
		{
			for (std::vector<BattleItem*>::iterator ammoi = _items.begin(); ammoi != _items.end(); ++ammoi)
			{
				if ((*ammoi)->getId() == a)
				{
					(*weaponi)->setAmmoItem((*ammoi));
					break;
				}
			}
		}
	}

}

/**
 * Loads the resources required by the map in the battle save.
 * @param res Pointer to resource pack.
 */
void SavedBattleGame::loadMapResources(ResourcePack *res)
{
	for (std::vector<MapDataSet*>::const_iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		(*i)->load(res);
	}

	int mdsID, mdID;

	for (int i = 0; i < _height * _length * _width; ++i)
	{
		for (int part = 0; part < 4; part++)
		{
			_tiles[i]->getMapData(&mdID, &mdsID, part);
			if (mdID != -1 && mdsID != -1)
			{
				_tiles[i]->setMapData(_mapDataSets[mdsID]->getObjects()->at(mdID), mdID, mdsID, part);
			}
		}
	}

	initUtilities(res);
	getTerrainModifier()->calculateSunShading();
	getTerrainModifier()->calculateTerrainLighting();
	getTerrainModifier()->calculateUnitLighting();
	for (std::vector<BattleUnit*>::iterator bu = _units.begin(); bu != _units.end(); ++bu)
	{
		_terrainModifier->calculateFOV(*bu);
	}
}

/**
 * Saves the saved battle game to a YAML file.
 * @param out YAML emitter.
 */
void SavedBattleGame::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;

	out << YAML::Key << "width" << YAML::Value << _width;
	out << YAML::Key << "length" << YAML::Value << _length;
	out << YAML::Key << "height" << YAML::Value << _height;
	out << YAML::Key << "globalshade" << YAML::Value << _globalShade;
	out << YAML::Key << "selectedUnit" << YAML::Value << (_selectedUnit?_selectedUnit->getId():-1);

	out << YAML::Key << "mapdatasets" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<MapDataSet*>::const_iterator i = _mapDataSets.begin(); i != _mapDataSets.end(); ++i)
	{
		out << (*i)->getName();
	}
	out << YAML::EndSeq;

	out << YAML::Key << "tiles" << YAML::Value;
	out << YAML::BeginSeq;
	for (int i = 0; i < _height * _length * _width; ++i)
	{
		if (!_tiles[i]->isVoid())
		{
			_tiles[i]->save(out);
		}
	}
	out << YAML::EndSeq;

	out << YAML::Key << "nodes" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<Node*>::const_iterator i = _nodes.begin(); i != _nodes.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "units" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<BattleUnit*>::const_iterator i = _units.begin(); i != _units.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "items" << YAML::Value;
	out << YAML::BeginSeq;
	for (std::vector<BattleItem*>::const_iterator i = _items.begin(); i != _items.end(); ++i)
	{
		(*i)->save(out);
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;
}

/**
 * Gets a pointer to the array of tiles.
 * @return A pointer to Tile array.
 */
Tile **SavedBattleGame::getTiles() const
{
	return _tiles;
}

/**
 * Initializes the array of tiles + creates a pathfinding object.
 * @param width
 * @param length
 * @param height
 */
void SavedBattleGame::initMap(int width, int length, int height)
{
	_width = width;
	_length = length;
	_height = height;
	_tiles = new Tile*[_height * _length * _width];
	/* create tile objects */
	for (int i = 0; i < _height * _length * _width; ++i)
	{
		Position pos;
		getTileCoords(i, &pos.x, &pos.y, &pos.z);
		_tiles[i] = new Tile(pos);
	}

}

/**
 * Initializes the map utilities.
 * @param res Pointer to resource pack.
 */
void SavedBattleGame::initUtilities(ResourcePack *res)
{
	_pathfinding = new Pathfinding(this);
	_terrainModifier = new TerrainModifier(this, res->getVoxelData());
}

/**
 * Sets the mission type.
 * @param missionType
 */
void SavedBattleGame::setMissionType(MissionType missionType)
{
	_missionType = missionType;
}

/**
 * Gets the mission type.
 * @return missionType
 */
MissionType SavedBattleGame::getMissionType() const
{
	return _missionType;
}

/**
 * Sets the global shade.
 * @param shade
 */
void SavedBattleGame::setGlobalShade(int shade)
{
	_globalShade = shade;
}

/**
 * Gets the global shade.
 * @return int
 */
int SavedBattleGame::getGlobalShade() const
{
	return _globalShade;
}

/**
 * Gets the map width.
 * @return Width in tiles.
 */
int SavedBattleGame::getWidth() const
{
	return _width;
}

/**
 * Gets the map length.
 * @return Length in tiles.
 */
int SavedBattleGame::getLength() const
{
	return _length;
}

/**
 * Gets the map height.
 * @return Height in layers.
 */
int SavedBattleGame::getHeight() const
{
	return _height;
}

/**
 * This method converts coordinates into a unique index.
 * @param pos position
 * @return Unique index.
 */
int SavedBattleGame::getTileIndex(const Position& pos) const
{
	return pos.z * _length * _width + pos.y * _width + pos.x;
}

/**
 * This method converts an index to coords.
 * @param index tileindex
 * @param x pointer to X coordinate.
 * @param y pointer to Y coordinate.
 * @param z pointer to Z coordinate.
 * @return Unique index.
 */
void SavedBattleGame::getTileCoords(int index, int *x, int *y, int *z) const
{
	*z = index / (_length * _width);
	*y = (index % (_length * _width)) / _width;
	*x = (index % (_length * _width)) % _width;
}

/**
 * Gets the Tile on a given position on the map.
 * @param pos position
 * @return Pointer to tile.
 */
Tile *SavedBattleGame::getTile(const Position& pos) const
{
	if (pos.x < 0 || pos.y < 0 || pos.z < 0
		|| pos.x >= _width || pos.y >= _length || pos.z >= _height)
		return 0;

	return _tiles[getTileIndex(pos)];
}

/**
 * Gets the currently selected unit
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::getSelectedUnit() const
{
	return _selectedUnit;
}

/**
 * Sets the currently selected unit.
 * @param unit pointer to BattleUnit.
 */
void SavedBattleGame::setSelectedUnit(BattleUnit *unit)
{
	_selectedUnit = unit;
}

/**
 * Select the previous player unit TODO move this to BattlescapeState ?
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::selectPreviousPlayerUnit()
{
	std::vector<BattleUnit*>::iterator i = _units.begin();
	bool bPrev = false;
	int wraps = 0;

	if (_selectedUnit == 0)
	{
		bPrev = true;
	}

	do
	{
		if (bPrev && (*i)->getFaction() == _side && !(*i)->isOut())
		{
			break;
		}
		if ((*i) == _selectedUnit)
		{
			bPrev = true;
		}
		if (i == _units.begin())
		{
			i = _units.end();
			wraps++;
		}
		--i;
		// back to where we started... no more units found
		if (wraps == 2)
		{
			_selectedUnit = 0;
			return _selectedUnit;
		}
	}
	while (true);

	_selectedUnit = (*i);

	return _selectedUnit;
}

/**
 * Select the next player unit.
 * TODO move this to BattlescapeState ?
 * @param checkReselect Don't reselect unit.
 * @return pointer to BattleUnit.
 */
BattleUnit *SavedBattleGame::selectNextPlayerUnit(bool checkReselect)
{
	std::vector<BattleUnit*>::iterator i = _units.begin();
	bool bNext = false;
	int wraps = 0;

	if (_selectedUnit == 0)
	{
		bNext = true;
	}
	else
	if (checkReselect)
	{
		_selectedUnit->dontReselect();
	}

	do
	{
		if (bNext && (*i)->getFaction() == _side && !(*i)->isOut() && ((checkReselect && (*i)->reselectAllowed())||!checkReselect))
		{
			break;
		}
		if ((*i) == _selectedUnit)
		{
			bNext = true;
		}
		++i;
		if (i == _units.end())
		{
			i = _units.begin();
			wraps++;
		}
		// back to where we started... no more units found
		if (wraps == 2)
		{
			_selectedUnit = 0;
			return _selectedUnit;
		}
	}
	while (true);

	_selectedUnit = (*i);

	return _selectedUnit;
}

/**
 * Select unit with position on map.
 * @param pos Position
 * @return pointer to BattleUnit - 0 when nothing found
 */
BattleUnit *SavedBattleGame::selectUnit(const Position& pos)
{
	BattleUnit *bu = 0;

	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if ((*i)->getPosition() == pos && !(*i)->isOut())
		{
			bu = *i;
			break;
		}
	}

	return bu;
}

/**
 * Gets the list of nodes.
 * @return pointer to the list of nodes
 */
std::vector<Node*> *const SavedBattleGame::getNodes()
{
	return &_nodes;
}

/**
 * Gets the list of units.
 * @return pointer to the list of units
 */
std::vector<BattleUnit*> *const SavedBattleGame::getUnits()
{
	return &_units;
}

/**
 * Gets the list of items.
 * @return pointer to the list of items
 */
std::vector<BattleItem*> *const SavedBattleGame::getItems()
{
	return &_items;
}

/**
 * Get the pathfinding object.
 * @return pointer to the pathfinding object
 */
Pathfinding *const SavedBattleGame::getPathfinding() const
{
	return _pathfinding;
}

/**
 * Get the terrain modifier object.
 * @return pointer to the terrain modifier object
 */
TerrainModifier *const SavedBattleGame::getTerrainModifier() const
{
	return _terrainModifier;
}

/**
* gets a pointer to the array of mapblock
* @return pointer to the array of mapblocks
*/
std::vector<MapDataSet*> *const SavedBattleGame::getMapDataSets()
{
	return &_mapDataSets;
}

/**
 * Gets the side currently playing.
 * @return Unit faction.
 */
UnitFaction SavedBattleGame::getSide() const
{
	return _side;
}

/**
 * Gets the current turn number.
 * @return Turn.
 */
int SavedBattleGame::getTurn() const
{
	return _turn;
}

/**
 * Ends the current turn and progresses to the next one.
 */
void SavedBattleGame::endTurn()
{
	if (_side == FACTION_PLAYER)
	{
		_side = FACTION_HOSTILE;
		// hide all aliens
		for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
		{
			if ((*i)->getFaction() == _side)
			{
				(*i)->setVisible(false);
			}
		}
	}
	else if (_side == FACTION_HOSTILE)
	{
		_terrainModifier->prepareNewTurn();
		_turn++;
		_side = FACTION_PLAYER;
	}

	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if ((*i)->getFaction() == _side)
		{
			(*i)->prepareNewTurn();
		}
		_terrainModifier->calculateFOV(*i);
	}

	_selectedUnit = 0;
	selectNextPlayerUnit();
}

/**
 * Turns on debug mode.
 */
void SavedBattleGame::setDebugMode()
{
	for (int i = 0; i < _height * _length * _width; ++i)
	{
		_tiles[i]->setDiscovered(true, 2);
	}

	_debugMode = true;
}

/**
 * Gets the current debug mode.
 * @return Debug mode.
 */
bool SavedBattleGame::getDebugMode() const
{
	return _debugMode;
}

/**
 * Resets all the units to their current standing tile.
 */
void SavedBattleGame::resetUnitTiles()
{
	for (std::vector<BattleUnit*>::iterator i = _units.begin(); i != _units.end(); ++i)
	{
		if (!(*i)->isOut())
			getTile((*i)->getPosition())->setUnit(*i);
	}
}

/**
 * Removes an item from the game. Eg. when ammo item is depleted.
 * @param item The Item to remove.
 */
void SavedBattleGame::removeItem(BattleItem *item)
{
	for (std::vector<BattleItem*>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		if (*i == item)
		{
			_items.erase(i);
			break;
		}
	}
}


/**
 * Add to debriefing stats.
 * @param name The untranslated name of the stat.
 * @param quantity The quantity to add.
 * @param score The score to add.
 */
void SavedBattleGame::addStat(const std::string &name, int quantity, int score)
{
	for (std::vector<DebriefingStat*>::iterator i = _debriefingStats.begin(); i != _debriefingStats.end(); ++i)
	{
		if ((*i)->item == name)
		{
			(*i)->qty = (*i)->qty + quantity;
			(*i)->score = (*i)->score + score;
			break;
		}
	}
}

/**
 * Get a list of debriefing stats.
 * @return A map of debriefing stats.
 */
std::vector<DebriefingStat*> *SavedBattleGame::getDebriefingStats()
{
	return &_debriefingStats;
}

/** 
 * Prepares debriefing: gathers Aliens, Corpses, Artefacts, UFO Components.
 * Adds the items to the craft.
 * Also calculates the soldiers experience, and possible promotions.
 * If aborted, only the things on the exit area are recovered.
 *
 */
void SavedBattleGame::prepareDebriefing(bool aborted)
{
	_aborted = aborted;
	int playerInExitArea = 0;

	// lets see what happens with units
	for (std::vector<BattleUnit*>::iterator j = getUnits()->begin(); j != getUnits()->end(); ++j)
	{
		UnitStatus status = (*j)->getStatus();
		UnitFaction faction = (*j)->getFaction();
		int value = (*j)->getUnit()->getValue();

		if (status == STATUS_DEAD)
		{
			if (faction == FACTION_HOSTILE)
			{
				addStat("STR_ALIENS_KILLED", 1, value);
			}
			if (faction == FACTION_PLAYER)
			{
				addStat("STR_XCOM_OPERATIVES_KILLED", 1, -value);
			}
		}
		else if (status == STATUS_UNCONSCIOUS)
		{
			if (faction == FACTION_HOSTILE && (!aborted || (*j)->isInExitArea()))
			{
				addStat("STR_LIVE_ALIENS_RECOVERED", 1, value);
			}
		}
		else if (faction == FACTION_PLAYER)
		{
			if ((*j)->isInExitArea() || !aborted)
				playerInExitArea++;
			else
				addStat("STR_XCOM_OPERATIVES_MISSING_IN_ACTION", 1, -value);
		}
	}
	if (playerInExitArea == 0 && aborted)
	{
		addStat("STR_XCOM_CRAFT_LOST", 1, -200);
	}

	// run through all tiles to recover UFO components and items
	if (!aborted)
	{
		for (int i = 0; i < _height * _length * _width; ++i)
		{
			for (int part = 0; part < 4; part++)
			{
				if (_tiles[i]->getMapData(part))
				{
					switch (_tiles[i]->getMapData(part)->getSpecialType())
					{
					case UFO_POWER_SOURCE:
						addStat("STR_UFO_POWER_SOURCE", 1, 1); break;
					case DESTROY_OBJECTIVE:break; // this is the brain
					case UFO_NAVIGATION:
						addStat("STR_UFO_NAVIGATION", 1, 1); break;
					case ALIEN_FOOD:
						addStat("STR_ALIEN_FOOD", 1, 1); break;
					case ALIEN_REPRODUCTION:
						addStat("STR_ALIEN_REPRODUCTION", 1, 1); break;
					case ALIEN_ENTERTAINMENT:
						addStat("STR_ALIEN_ENTERTAINMENT", 1, 1); break;
					case ALIEN_SURGERY:
						addStat("STR_ALIEN_SURGERY", 1, 1); break;
					case UNKNOWN09:
						addStat("STR_UFO_CONSTRUCTION", 1, 1); break;
					case ALIEN_ALLOYS:
						addStat("STR_ALIEN_ALLOYS", 1, 1); break;
					case EXAM_ROOM:
						addStat("STR_EXAMINATION_ROOM", 1, 1); break;
					}

				}
			}
		}

		// alien alloys recovery values are divided by 10 or devided by 150 in case of an alien base
		int divider = _missionType==MISS_ALIENBASE?150:10;
		for (std::vector<DebriefingStat*>::iterator i = _debriefingStats.begin(); i != _debriefingStats.end(); ++i)
		{
			if ((*i)->item == "STR_ALIEN_ALLOYS")
			{
				(*i)->qty = (*i)->qty / divider;
				(*i)->score = (*i)->score / divider;
				break;
			}
		}
	}
}

/**
 * Is the mission aborted or succesful.
 * @return bool.
 */
bool SavedBattleGame::isAborted()
{
	return _aborted;
}


/**
 * Gets the current item ID.
 * @return Current item ID pointer.
 */
int *SavedBattleGame::getCurrentItemId()
{
	return &_itemId;
}


}