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
#include <assert.h>
#include <fstream>
#include <sstream>
#include "BattlescapeGenerator.h"
#include "TileEngine.h"
#include "Inventory.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Node.h"
#include "../Engine/RNG.h"
#include "../Engine/Exception.h"
#include "../Ruleset/MapBlock.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/RuleUfo.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleTerrain.h"
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/MapData.h"
#include "../Ruleset/MCDPatch.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/Unit.h"
#include "../Ruleset/AlienRace.h"
#include "../Ruleset/AlienDeployment.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Resource/XcomResourcePack.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Options.h"
#include "../Savegame/Vehicle.h"
#include "../Savegame/TerrorSite.h"
#include "../Savegame/AlienBase.h"
#include "../Savegame/EquipmentLayoutItem.h"
#include "CivilianBAIState.h"
#include "AlienBAIState.h"
#include "Pathfinding.h"

namespace OpenXcom
{

/**
 * Sets up a BattlescapeGenerator.
 * @param game pointer to Game object.
 */
BattlescapeGenerator::BattlescapeGenerator(Game *game) : _game(game), _save(game->getSavedGame()->getSavedBattle()), _res(_game->getResourcePack()), _craft(0), _ufo(0), _base(0), _terror(0), _alienBase(0), _terrain(0),
														 _mapsize_x(0), _mapsize_y(0), _mapsize_z(0), _worldTexture(0), _worldShade(0), _unitSequence(0), _craftInventoryTile(0), _alienRace(""), _alienItemLevel(0), _baseInventory(false), _generateFuel(true), _craftX(0), _craftY(0), _craftZ(0)
{
	_allowAutoLoadout = !Options::disableAutoEquip;
}

/**
 * Deletes the BattlescapeGenerator.
 */
BattlescapeGenerator::~BattlescapeGenerator()
{

}

/**
 * Sets the XCom craft involved in the battle.
 * @param craft Pointer to XCom craft.
 */
void BattlescapeGenerator::setCraft(Craft *craft)
{
	_craft = craft;
	_craft->setInBattlescape(true);
}

/**
 * Sets the ufo involved in the battle.
 * @param ufo Pointer to UFO.
 */
void BattlescapeGenerator::setUfo(Ufo *ufo)
{
	_ufo = ufo;
	_ufo->setInBattlescape(true);
}

/**
 * Sets the world texture where a ufo crashed. This is used to determine the terrain.
 * @param texture Texture id of the polygon on the globe.
 */
void BattlescapeGenerator::setWorldTexture(int texture)
{
	if (texture < 0) texture = 0;
	_worldTexture = texture;
}

/**
 * Sets the world shade where a ufo crashed. This is used to determine the battlescape light level.
 * @param shade Shade of the polygon on the globe.
 */
void BattlescapeGenerator::setWorldShade(int shade)
{
	if (shade > 15) shade = 15;
	if (shade < 0) shade = 0;
	_worldShade = shade;
}

/**
 * Sets the alien race on the mission. This is used to determine the various alien types to spawn.
 * @param alienRace Alien (main) race.
 */
void BattlescapeGenerator::setAlienRace(const std::string &alienRace)
{
	_alienRace = alienRace;
}

/**
 * Sets the alien item level. This is used to determine how advanced the equipment of the aliens will be.
 * note: this only applies to "New Battle" type games. we intentionally don't alter the month for those,
 * because we're using monthsPassed -1 for new battle in other sections of code.
 * - this value should be from 0 to the size of the itemLevel array in the ruleset (default 9).
 * - at a certain number of months higher item levels appear more and more and lower ones will gradually disappear
 * @param alienItemLevel AlienItemLevel.
 */
void BattlescapeGenerator::setAlienItemlevel(int alienItemLevel)
{
	_alienItemLevel = alienItemLevel;
}

/**
 * Sets the XCom base involved in the battle.
 * @param base Pointer to XCom base.
 */
void BattlescapeGenerator::setBase(Base *base)
{
	_base = base;
	_base->setInBattlescape(true);
}

/**
 * Sets the terror site involved in the battle.
 * @param terror Pointer to terror site.
 */
void BattlescapeGenerator::setTerrorSite(TerrorSite *terror)
{
	_terror = terror;
	_terror->setInBattlescape(true);
}


/**
 * Switches an existing battlescapesavegame to a new stage.
 */
void BattlescapeGenerator::nextStage()
{
	// kill all enemy units, or those not in endpoint area (if aborted)
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if ((_game->getSavedGame()->getSavedBattle()->isAborted() && !(*j)->isInExitArea(END_POINT))
			|| (*j)->getOriginalFaction() == FACTION_HOSTILE)
		{
			(*j)->instaKill();
		}
		if ((*j)->getTile())
		{
			(*j)->getTile()->setUnit(0);
		}
		(*j)->setTile(0);
		(*j)->setPosition(Position(-1,-1,-1), false);
	}

	while (_game->getSavedGame()->getSavedBattle()->getSide() != FACTION_PLAYER)
	{
		_game->getSavedGame()->getSavedBattle()->endTurn();
	}
	_save->resetTurnCounter();

	AlienDeployment *ruleDeploy = _game->getRuleset()->getDeployment(_ufo?_ufo->getRules()->getType():_save->getMissionType());

	ruleDeploy->getDimensions(&_mapsize_x, &_mapsize_y, &_mapsize_z);
	size_t pick = RNG::generate(0, ruleDeploy->getTerrains().size() -1);
	_terrain = _game->getRuleset()->getTerrain(ruleDeploy->getTerrains().at(pick));
	_game->getSavedGame()->getSavedBattle()->setTerrainType(_terrain->getName());
	_worldShade = ruleDeploy->getShade();

	_save->initMap(_mapsize_x, _mapsize_y, _mapsize_z);
	generateMap();
	int highestSoldierID = 0;
	bool selectedFirstSoldier = false;
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if ((*j)->getOriginalFaction() == FACTION_PLAYER)
		{
			if (!(*j)->isOut())
			{
				(*j)->convertToFaction(FACTION_PLAYER);
				(*j)->setTurnsSinceSpotted(255);
				(*j)->getVisibleTiles()->clear();
				if (!selectedFirstSoldier && (*j)->getGeoscapeSoldier())
				{
					_save->setSelectedUnit(*j);
					selectedFirstSoldier = true;
				}
				Node* node = _save->getSpawnNode(NR_XCOM, (*j));
				if (node || placeUnitNearFriend(*j))
				{
					if (node)
					{
						_save->setUnitPosition((*j), node->getPosition());
					}
					if (!_craftInventoryTile)
					{
						_craftInventoryTile = (*j)->getTile();
					}
					_craftInventoryTile->setUnit(*j);
					(*j)->setVisible(false);
					if ((*j)->getId() > highestSoldierID)
					{
						highestSoldierID = (*j)->getId();
					}
					(*j)->prepareNewTurn();
				}
			}
		}
	}

	// remove all items not belonging to our soldiers from the map.
	for (std::vector<BattleItem*>::iterator j = _save->getItems()->begin(); j != _save->getItems()->end(); ++j)
	{
		if (!(*j)->getOwner() || (*j)->getOwner()->getId() > highestSoldierID)
		{
			if (_game->getSavedGame()->getSavedBattle()->isAborted())
			{
				(*j)->setTile(0);
			}
			else if (_craftInventoryTile && (*j)->getRules()->isRecoverable() && !(*j)->getRules()->isFixed())
			{
				_craftInventoryTile->addItem((*j), _game->getRuleset()->getInventory("STR_GROUND"));
			}
		}
	}
	_unitSequence = _save->getUnits()->back()->getId() + 1;
	deployAliens(_game->getRuleset()->getAlienRace(_alienRace), ruleDeploy);

	deployCivilians(ruleDeploy->getCivilians());

	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
	{
		if (_save->getTiles()[i]->getMapData(MapData::O_FLOOR) &&
			(_save->getTiles()[i]->getMapData(MapData::O_FLOOR)->getSpecialType() == START_POINT ||
			(_save->getTiles()[i]->getPosition().z == 1 &&
			_save->getTiles()[i]->getMapData(MapData::O_FLOOR)->isGravLift() &&
			_save->getTiles()[i]->getMapData(MapData::O_OBJECT))))
				_save->getTiles()[i]->setDiscovered(true, 2);
	}
	_save->setGlobalShade(_worldShade);
	_save->getTileEngine()->calculateSunShading();
	_save->getTileEngine()->calculateTerrainLighting();
	_save->getTileEngine()->calculateUnitLighting();
	_save->getTileEngine()->recalculateFOV();
}

/**
 * Starts the generator; it fills up the battlescapesavegame with data.
 */
void BattlescapeGenerator::run()
{
	AlienDeployment *ruleDeploy = _game->getRuleset()->getDeployment(_ufo && !_ufo->getScriptedEvent() ?_ufo->getRules()->getType():_save->getMissionType());

	ruleDeploy->getDimensions(&_mapsize_x, &_mapsize_y, &_mapsize_z);

	_unitSequence = BattleUnit::MAX_SOLDIER_ID; // geoscape soldier IDs should stay below this number

	if (ruleDeploy->getTerrains().empty())
	{
		double lat = 0;
		if (_ufo) lat = _ufo->getLatitude();
		_terrain = getTerrain(_worldTexture, lat);
	}
	else
	{
		size_t pick = RNG::generate(0, ruleDeploy->getTerrains().size() -1);
		_terrain = _game->getRuleset()->getTerrain(ruleDeploy->getTerrains().at(pick));
	}

	if (ruleDeploy->getShade() != -1)
	{
		_worldShade = ruleDeploy->getShade();
	}

	// creates the tile objects
	_save->setTerrainType(_terrain->getName());
	_save->initMap(_mapsize_x, _mapsize_y, _mapsize_z);
	_save->initUtilities(_res);

	// lets generate the map now and store it inside the tile objects
	generateMap();

	if (_craft != 0 || _base != 0)
	{
		deployXCOM();
	}

	deployAliens(_game->getRuleset()->getAlienRace(_alienRace), ruleDeploy);

	deployCivilians(ruleDeploy->getCivilians());

	if (_generateFuel)
	{
		fuelPowerSources();
	}

	if (_ufo && _ufo->getStatus() == Ufo::CRASHED)
	{
		explodePowerSources();
	}

	if (_save->getMissionType() == "STR_BASE_DEFENSE")
	{
		for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
		{
			_save->getTiles()[i]->setDiscovered(true, 2);
		}

		_save->calculateModuleMap();
	}

	if (_terrain->getName() == "UBASE")
	{
		for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
		{
			if (_save->getTiles()[i]->getMapData(MapData::O_FLOOR) &&
				(_save->getTiles()[i]->getMapData(MapData::O_FLOOR)->getSpecialType() == START_POINT ||
				(_save->getTiles()[i]->getPosition().z == 1 &&
				_save->getTiles()[i]->getMapData(MapData::O_FLOOR)->isGravLift() &&
				_save->getTiles()[i]->getMapData(MapData::O_OBJECT))))
				_save->getTiles()[i]->setDiscovered(true, 2);
		}
	}

	// set shade (alien bases are a little darker, sites depend on worldshade)
	_save->setGlobalShade(_worldShade);

	_save->getTileEngine()->calculateSunShading();
	_save->getTileEngine()->calculateTerrainLighting();
	_save->getTileEngine()->calculateUnitLighting();
	_save->getTileEngine()->recalculateFOV();
}

/**
* Deploys all the X-COM units and equipment based
* on the Geoscape base / craft.
* @param inventoryTile The tile to place all the extra equipment on.
*/
void BattlescapeGenerator::deployXCOM()
{
	RuleInventory *ground = _game->getRuleset()->getInventory("STR_GROUND");

	if (_craft != 0)
		_base = _craft->getBase();

	// add vehicles that are in the craft - a vehicle is actually an item, which you will never see as it is converted to a unit
	// however the item itself becomes the weapon it "holds".
	if (!_baseInventory)
	{
		if (_craft != 0)
		{
			for (std::vector<Vehicle*>::iterator i = _craft->getVehicles()->begin(); i != _craft->getVehicles()->end(); ++i)
			{
				BattleUnit *unit = addXCOMVehicle(*i);
				if (unit && !_save->getSelectedUnit())
					_save->setSelectedUnit(unit);
			}
		}
		else if (_base != 0)
		{
			// add vehicles that are in the base inventory
			for (std::vector<Vehicle*>::iterator i = _base->getVehicles()->begin(); i != _base->getVehicles()->end(); ++i)
			{
				BattleUnit *unit = addXCOMVehicle(*i);
				if (unit && !_save->getSelectedUnit())
					_save->setSelectedUnit(unit);
			}
		}
	}

	// add soldiers that are in the craft or base
	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
	{
		if ((_craft != 0 && (*i)->getCraft() == _craft) ||
			(_craft == 0 && (*i)->getWoundRecovery() == 0 && ((*i)->getCraft() == 0 || (*i)->getCraft()->getStatus() != "STR_OUT")))
		{
			BattleUnit *unit = addXCOMUnit(new BattleUnit(*i, FACTION_PLAYER));
			if (unit && !_save->getSelectedUnit())
				_save->setSelectedUnit(unit);
		}
	}

	// maybe we should assign all units to the first tile of the skyranger before the inventory pre-equip and then reassign them to their correct tile afterwards?
	// fix: make them invisible, they are made visible afterwards.
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if ((*i)->getFaction() == FACTION_PLAYER)
		{
			_craftInventoryTile->setUnit(*i);
			(*i)->setVisible(false);
		}
	}

	if (_craft != 0)
	{
		// add items that are in the craft
		for (std::map<std::string, int>::iterator i = _craft->getItems()->getContents()->begin(); i != _craft->getItems()->getContents()->end(); ++i)
		{
			for (int count = 0; count < i->second; count++)
			{
				_craftInventoryTile->addItem(new BattleItem(_game->getRuleset()->getItem(i->first), _save->getCurrentItemId()),	ground);
			}
		}
	}
	else
	{
		// add items that are in the base
		for (std::map<std::string, int>::iterator i = _base->getItems()->getContents()->begin(); i != _base->getItems()->getContents()->end();)
		{
			// only put items in the battlescape that make sense (when the item got a sprite, it's probably ok)
			RuleItem *rule = _game->getRuleset()->getItem(i->first);
			if (rule->getBigSprite() > -1 && rule->getBattleType() != BT_NONE && rule->getBattleType() != BT_CORPSE && !rule->isFixed() && _game->getSavedGame()->isResearched(rule->getRequirements()))
			{
				for (int count = 0; count < i->second; count++)
				{
					_craftInventoryTile->addItem(new BattleItem(_game->getRuleset()->getItem(i->first), _save->getCurrentItemId()), ground);
				}
				std::map<std::string, int>::iterator tmp = i;
				++i;
				_base->getItems()->removeItem(tmp->first, tmp->second);
			}
			else
			{
				++i;
			}
		}
		// add items from crafts in base
		for (std::vector<Craft*>::iterator c = _base->getCrafts()->begin(); c != _base->getCrafts()->end(); ++c)
		{
			if ((*c)->getStatus() == "STR_OUT")
				continue;
			for (std::map<std::string, int>::iterator i = (*c)->getItems()->getContents()->begin(); i != (*c)->getItems()->getContents()->end(); ++i)
			{
				for (int count = 0; count < i->second; count++)
				{
					_craftInventoryTile->addItem(new BattleItem(_game->getRuleset()->getItem(i->first), _save->getCurrentItemId()), ground);
				}
			}
		}
	}

	// equip soldiers based on equipment-layout
	for (std::vector<BattleItem*>::iterator i = _craftInventoryTile->getInventory()->begin(); i != _craftInventoryTile->getInventory()->end(); ++i)
	{
		// don't let the soldiers take extra ammo yet
		if ((*i)->getRules()->getBattleType() == BT_AMMO)
			continue;
		placeItemByLayout(*i);
	}

	// load weapons before loadouts take extra clips.
	loadWeapons();

	for (std::vector<BattleItem*>::iterator i = _craftInventoryTile->getInventory()->begin(); i != _craftInventoryTile->getInventory()->end(); ++i)
	{
		// we only need to distribute extra ammo at this point.
		if ((*i)->getRules()->getBattleType() != BT_AMMO)
			continue;
		placeItemByLayout(*i);
	}
	

	// auto-equip soldiers (only soldiers without layout)
	for (int pass = 0; pass != 4; ++pass)
	{
		for (std::vector<BattleItem*>::iterator j = _craftInventoryTile->getInventory()->begin(); j != _craftInventoryTile->getInventory()->end();)
		{
			if ((*j)->getSlot() == ground)
			{
				bool add = false;

				switch (pass)
				{
				// priority 1: rifles.
				case 0:
					add = (*j)->getRules()->isRifle();
					break;
				// priority 2: pistols (assuming no rifles were found).
				case 1:
					add = (*j)->getRules()->isPistol();
					break;
				// priority 3: ammunition.
				case 2:
					add = (*j)->getRules()->getBattleType() == BT_AMMO;
					break;
				// priority 4: leftovers.
				case 3:
					add = !(*j)->getRules()->isPistol() &&
							!(*j)->getRules()->isRifle() &&
							((*j)->getRules()->getBattleType() != BT_FLARE || _worldShade >= 9);
					break;
				default:
					break;
				}
				
				if (add)
				{
					for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
					{
						if (!(*i)->hasInventory() || !(*i)->getGeoscapeSoldier() || !(*i)->getGeoscapeSoldier()->getEquipmentLayout()->empty())
						{
							continue;
						}
						// let's not be greedy, we'll only take a second extra clip
						// if everyone else has had a chance to take a first.
						bool allowSecondClip = (pass == 3);

						if (addItem(*j, *i, allowSecondClip))
						{
							j = _craftInventoryTile->getInventory()->erase(j);
							add = false;
							break;
						}
					}
					if (!add)
					{
						continue;
					}
				}
			}
			++j;
		}
	}
	// clean up moved items
	for (std::vector<BattleItem*>::iterator i = _craftInventoryTile->getInventory()->begin(); i != _craftInventoryTile->getInventory()->end();)
	{
		if ((*i)->getSlot() != ground)
		{
			i = _craftInventoryTile->getInventory()->erase(i);
		}
		else
		{
			_save->getItems()->push_back(*i);
			++i;
		}
	}
}

/**
 * Adds an XCom vehicle to the game.
 * Sets the correct turret depending on the ammo type.
 * @param v Pointer to the Vehicle.
 * @return Pointer to the spawned unit.
 */
BattleUnit *BattlescapeGenerator::addXCOMVehicle(Vehicle *v)
{
	std::string vehicle = v->getRules()->getType();
	Unit *rule = _game->getRuleset()->getUnit(vehicle);
	BattleUnit *unit = addXCOMUnit(new BattleUnit(rule, FACTION_PLAYER, _unitSequence++, _game->getRuleset()->getArmor(rule->getArmor()), 0));
	if (unit)
	{
		BattleItem *item = new BattleItem(_game->getRuleset()->getItem(vehicle), _save->getCurrentItemId());
		addItem(item, unit);
		if(!v->getRules()->getCompatibleAmmo()->empty())
		{
			std::string ammo = v->getRules()->getCompatibleAmmo()->front();
			BattleItem *ammoItem = new BattleItem(_game->getRuleset()->getItem(ammo), _save->getCurrentItemId());
			addItem(ammoItem, unit);
			ammoItem->setAmmoQuantity(v->getAmmo());
		}
		unit->setTurretType(v->getRules()->getTurretType());
	}
	return unit;
}

/**
 * Adds a soldier to the game and places him on a free spawnpoint.
 * Spawnpoints are either tiles in case of an XCom craft that landed.
 * Or they are mapnodes in case there's no craft.
 * @param soldier Pointer to the Soldier.
 * @return Pointer to the spawned unit.
 */
BattleUnit *BattlescapeGenerator::addXCOMUnit(BattleUnit *unit)
{
//	unit->setId(_unitCount++);

	AlienDeployment *ruleDeploy = _game->getRuleset()->getDeployment(_ufo?_ufo->getRules()->getType():_save->getMissionType());

	if (_craft == 0 || !ruleDeploy->getShowCraft())
	{
		Node* node = _save->getSpawnNode(NR_XCOM, unit);
		if (node)
		{
			_save->setUnitPosition(unit, node->getPosition());
			_craftInventoryTile = _save->getTile(node->getPosition());
			unit->setDirection(RNG::generate(0,7));
			_save->getUnits()->push_back(unit);
			_save->getTileEngine()->calculateFOV(unit);
			unit->deriveRank();
			return unit;
		}
		else if (_save->getMissionType() != "STR_BASE_DEFENSE")
		{
			if (placeUnitNearFriend(unit))
			{
				_craftInventoryTile = _save->getTile(unit->getPosition());
				unit->setDirection(RNG::generate(0,7));
				_save->getUnits()->push_back(unit);
				_save->getTileEngine()->calculateFOV(unit);
				unit->deriveRank();
				return unit;
			}
		}
	}
	else if (_craft && !_craft->getRules()->getDeployment().empty() && !_baseInventory)
	{
		for (std::vector<std::vector<int> >::const_iterator i = _craft->getRules()->getDeployment().begin(); i != _craft->getRules()->getDeployment().end(); ++i)
		{
			Position pos = Position((*i)[0] + (_craftX * 10), (*i)[1] + (_craftY * 10), (*i)[2] + _craftZ);
			int dir = (*i)[3];

			if (canPlaceXCOMUnit(_save->getTile(pos)))
			{
				if (_save->setUnitPosition(unit, pos))
				{
					_save->getUnits()->push_back(unit);
					unit->setDirection(dir);
					unit->deriveRank();
					return unit;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < _mapsize_x * _mapsize_y * _mapsize_z; i++)
		{
			if (canPlaceXCOMUnit(_save->getTiles()[i]))
			{
				if (_save->setUnitPosition(unit, _save->getTiles()[i]->getPosition()))
				{
					_save->getUnits()->push_back(unit);
					unit->deriveRank();
					return unit;
				}
			}
		}
	}
	delete unit;
	return 0;
}

/**
 * Checks if a soldier/tank can be placed on a given tile.
 * @param tile the given tile.
 * @return whether the unit can be placed here.
 */
bool BattlescapeGenerator::canPlaceXCOMUnit(Tile *tile)
{
	// to spawn an xcom soldier, there has to be a tile, with a floor, with the starting point attribute and no object in the way
	if (tile && 
		tile->getMapData(MapData::O_FLOOR) && 
		tile->getMapData(MapData::O_FLOOR)->getSpecialType() == START_POINT && 
		!tile->getMapData(MapData::O_OBJECT) &&
		tile->getMapData(MapData::O_FLOOR)->getTUCost(MT_WALK) < 255)
	{
		if (_craftInventoryTile == 0)
			_craftInventoryTile = tile;

		return true;
	}
	return false;
}

/**
 * Deploys the aliens, according to the alien deployment rules.
 * @param race Pointer to the alien race.
 * @param deployment Pointer to the deployment rules.
 */
void BattlescapeGenerator::deployAliens(AlienRace *race, AlienDeployment *deployment)
{
	int month;
	if (_game->getSavedGame()->getMonthsPassed() != -1)
	{
		month =
		((size_t) _game->getSavedGame()->getMonthsPassed()) > _game->getRuleset()->getAlienItemLevels().size() - 1 ?  // if
		_game->getRuleset()->getAlienItemLevels().size() - 1 : // then
		_game->getSavedGame()->getMonthsPassed() ;  // else
	}
	else
	{
		month = _alienItemLevel;
	}
	for (std::vector<DeploymentData>::iterator d = deployment->getDeploymentData()->begin(); d != deployment->getDeploymentData()->end(); ++d)
	{
		std::string alienName = race->getMember((*d).alienRank);

		int quantity;
		
		if (_game->getSavedGame()->getDifficulty() < DIFF_VETERAN)
			quantity = (*d).lowQty + RNG::generate(0, (*d).dQty); // beginner/experienced
		else if (_game->getSavedGame()->getDifficulty() < DIFF_SUPERHUMAN)
			quantity = (*d).lowQty+(((*d).highQty-(*d).lowQty)/2) + RNG::generate(0, (*d).dQty); // veteran/genius
		else
			quantity = (*d).highQty + RNG::generate(0, (*d).dQty); // super (and beyond?)

		for (int i = 0; i < quantity; i++)
		{
			bool outside = RNG::generate(0,99) < (*d).percentageOutsideUfo;
			if (_ufo == 0)
				outside = false;
			Unit *rule = _game->getRuleset()->getUnit(alienName);
			BattleUnit *unit = addAlien(rule, (*d).alienRank, outside);
			int itemLevel = _game->getRuleset()->getAlienItemLevels().at(month).at(RNG::generate(0,9));
			if (unit)
			{
				// terrorist alien's equipment is a special case - they are fitted with a weapon which is the alien's name with suffix _WEAPON
				if (rule->isLivingWeapon())
				{
					std::string terroristWeapon = rule->getRace().substr(4);
					terroristWeapon += "_WEAPON";
					RuleItem *ruleItem = _game->getRuleset()->getItem(terroristWeapon);
					if (ruleItem)
					{
						BattleItem *item = new BattleItem(ruleItem, _save->getCurrentItemId());
						if (!addItem(item, unit))
						{
							delete item;
						}
						else
						{
							unit->setTurretType(item->getRules()->getTurretType());
						}
					}
				}
				else
				{
					for (std::vector<std::string>::iterator it = (*d).itemSets.at(itemLevel).items.begin(); it != (*d).itemSets.at(itemLevel).items.end(); ++it)
					{
						RuleItem *ruleItem = _game->getRuleset()->getItem((*it));
						if (ruleItem)
						{
							BattleItem *item = new BattleItem(ruleItem, _save->getCurrentItemId());
							if (!addItem(item, unit))
							{
								delete item;
							}
						}
					}
				}
			}
		}
	}
}



/**
 * Adds an alien to the game and places him on a free spawnpoint.
 * @param rules Pointer to the Unit which holds info about the alien .
 * @param alienRank The rank of the alien, used for spawn point search.
 * @param outside Whether the alien should spawn outside or inside the UFO.
 * @return Pointer to the created unit.
 */
BattleUnit *BattlescapeGenerator::addAlien(Unit *rules, int alienRank, bool outside)
{
	int difficulty = (int)(_game->getSavedGame()->getDifficulty());
	BattleUnit *unit = new BattleUnit(rules, FACTION_HOSTILE, _unitSequence++, _game->getRuleset()->getArmor(rules->getArmor()), difficulty);
	Node *node = 0;

	/* following data is the order in which certain alien ranks spawn on certain node ranks */
	/* note that they all can fall back to rank 0 nodes - which is scout (outside ufo) */

	for (int i = 0; i < 7 && node == 0; i++)
	{
		if (outside)
			node = _save->getSpawnNode(0, unit); // when alien is instructed to spawn outside, we only look for node 0 spawnpoints
		else
			node = _save->getSpawnNode(Node::nodeRank[alienRank][i], unit);
	}

	if (node && _save->setUnitPosition(unit, node->getPosition()))
	{
		unit->setAIState(new AlienBAIState(_game->getSavedGame()->getSavedBattle(), unit, node));
		unit->setRankInt(alienRank);
		int dir = _save->getTileEngine()->faceWindow(node->getPosition());
		Position craft = _game->getSavedGame()->getSavedBattle()->getUnits()->at(0)->getPosition();
		if (_save->getTileEngine()->distance(node->getPosition(), craft) <= 20 && RNG::percent(20 * difficulty))
			dir = unit->directionTo(craft);
		if (dir != -1)
			unit->setDirection(dir);
		else
			unit->setDirection(RNG::generate(0,7));

		if (!difficulty)
		{
			unit->halveArmor();
		}

		// we only add a unit if it has a node to spawn on.
		// (stops them spawning at 0,0,0)
		_save->getUnits()->push_back(unit);
	}
	else
	{
		delete unit;
		unit = 0;
	}

	return unit;
}

/**
 * Adds a civilian to the game and places him on a free spawnpoint.
 * @param rules Pointer to the Unit which holds info about the civilian.
 * @return Pointer to the created unit.
 */
BattleUnit *BattlescapeGenerator::addCivilian(Unit *rules)
{
	BattleUnit *unit = new BattleUnit(rules, FACTION_NEUTRAL, _unitSequence++, _game->getRuleset()->getArmor(rules->getArmor()), 0);
	Node *node = _save->getSpawnNode(0, unit);

	if (node)
	{
		_save->setUnitPosition(unit, node->getPosition());
		unit->setAIState(new CivilianBAIState(_game->getSavedGame()->getSavedBattle(), unit, node));
		unit->setDirection(RNG::generate(0,7));

		// we only add a unit if it has a node to spawn on.
		// (stops them spawning at 0,0,0)
		_save->getUnits()->push_back(unit);
	}
	else if (placeUnitNearFriend(unit))
	{
		unit->setAIState(new CivilianBAIState(_game->getSavedGame()->getSavedBattle(), unit, node));
		unit->setDirection(RNG::generate(0,7));
		_save->getUnits()->push_back(unit);
	}
	else
	{
		delete unit;
		unit = 0;
	}

	return unit;
}

/**
 * Places an item on an XCom soldier based on equipment layout.
 * @param item Pointer to the Item.
 * @return Pointer to the Item.
 */
bool BattlescapeGenerator::placeItemByLayout(BattleItem *item)
{
	RuleInventory *ground = _game->getRuleset()->getInventory("STR_GROUND");
	if (item->getSlot() == ground)
	{
		bool loaded;
		RuleInventory *righthand = _game->getRuleset()->getInventory("STR_RIGHT_HAND");

		// find the first soldier with a matching layout-slot
		for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			// skip the vehicles, we need only X-Com soldiers WITH equipment-layout
			if ((*i)->getArmor()->getSize() > 1 || !(*i)->getGeoscapeSoldier() || (*i)->getGeoscapeSoldier()->getEquipmentLayout()->empty())
			{
				continue;
			}

			// find the first matching layout-slot which is not already occupied
			std::vector<EquipmentLayoutItem*> *layoutItems = (*i)->getGeoscapeSoldier()->getEquipmentLayout();
			for (std::vector<EquipmentLayoutItem*>::iterator j = layoutItems->begin(); j != layoutItems->end(); ++j)
			{
				if (item->getRules()->getType() != (*j)->getItemType()
				|| (*i)->getItem((*j)->getSlot(), (*j)->getSlotX(), (*j)->getSlotY())) continue;

				if ((*j)->getAmmoItem() == "NONE")
				{
					loaded = true;
				}
				else
				{
					loaded = false;
					// maybe we find the layout-ammo on the ground to load it with
					for (std::vector<BattleItem*>::iterator k = _craftInventoryTile->getInventory()->begin(); (!loaded) && k != _craftInventoryTile->getInventory()->end(); ++k)
					{
						if ((*k)->getRules()->getType() == (*j)->getAmmoItem() && (*k)->getSlot() == ground
						&& item->setAmmoItem((*k)) == 0)
						{
							_save->getItems()->push_back(*k);
							(*k)->setSlot(righthand);
							loaded = true;
							// note: soldier is not owner of the ammo, we are using this fact when saving equipments
						}
					}
				}
				// only place the weapon onto the soldier when it's loaded with its layout-ammo (if any)
				if (loaded)
				{
					item->moveToOwner((*i));
					item->setSlot(_game->getRuleset()->getInventory((*j)->getSlot()));
					item->setSlotX((*j)->getSlotX());
					item->setSlotY((*j)->getSlotY());
					if (Options::includePrimeStateInSavedLayout &&
						(item->getRules()->getBattleType() == BT_GRENADE ||
						item->getRules()->getBattleType() == BT_PROXIMITYGRENADE))
					{
						item->setFuseTimer((*j)->getFuseTimer());
					}
					_save->getItems()->push_back(item);
					return true;
				}
			}
		}
	}
	return false;
}

/**
 * Adds an item to an XCom soldier (auto-equip).
 * @param item Pointer to the Item.
 * @param unit Pointer to the Unit.
 * @param allowSecondClip allow the unit to take a second clip or not. (only applies to xcom soldiers, aliens are allowed regardless of this flag)
 * @return if the item was placed or not.
 */
bool BattlescapeGenerator::addItem(BattleItem *item, BattleUnit *unit, bool allowSecondClip)
{
	RuleInventory *rightHand = _game->getRuleset()->getInventory("STR_RIGHT_HAND");
	bool placed = false;
	bool loaded = false;
	BattleItem *weapon = unit->getItem("STR_RIGHT_HAND");
	int weight = 0;

	// tanks and aliens don't care about weight or multiple items,
	// their loadouts are defined in the rulesets and more or less set in stone.
	if (unit->getFaction() == FACTION_PLAYER && unit->hasInventory())
	{
		weight = unit->getCarriedWeight() + item->getRules()->getWeight();
		if (item->getAmmoItem() && item->getAmmoItem() != item)
		{
			weight += item->getAmmoItem()->getRules()->getWeight();
		}
		// allow all weapons to be loaded by avoiding this check,
		// they'll return false later anyway if the unit has something in his hand.
		if (item->getRules()->getCompatibleAmmo()->empty())
		{
			int tally = 0;
			for (std::vector<BattleItem*>::iterator i = unit->getInventory()->begin(); i != unit->getInventory()->end(); ++i)
			{
				if (item->getRules()->getType() == (*i)->getRules()->getType())
				{
					if (allowSecondClip && item->getRules()->getBattleType() == BT_AMMO)
					{
						tally++;
						if (tally == 2)
						{
							return false;
						}
					}
					else
					{
						// we already have one, thanks.
						return false;
					}
				}
			}
		}
	}

	switch (item->getRules()->getBattleType())
	{
	case BT_FIREARM:
	case BT_MELEE:
		if (item->getAmmoItem() || unit->getFaction() != FACTION_PLAYER || !unit->hasInventory())
		{
			loaded = true;
		}

		if (loaded && (unit->getGeoscapeSoldier() == 0 || _allowAutoLoadout))
		{
			if (!unit->getItem("STR_RIGHT_HAND") && unit->getStats()->strength * 0.66 >= weight) // weight is always considered 0 for aliens
			{
				item->moveToOwner(unit);
				item->setSlot(rightHand);
				placed = true;
			}
		}
		break;
	case BT_AMMO:
		// no weapon, or our weapon takes no ammo, or this ammo isn't compatible.
		// we won't be needing this. move on.
		if (!weapon || weapon->getRules()->getCompatibleAmmo()->empty() ||
			std::find(weapon->getRules()->getCompatibleAmmo()->begin(),
			weapon->getRules()->getCompatibleAmmo()->end(),
			item->getRules()->getType()) == weapon->getRules()->getCompatibleAmmo()->end())
		{
			break;
		}
		// xcom weapons will already be loaded, aliens and tanks, however, get their ammo added afterwards.
		// so let's try to load them here.
		if ((weapon->getRules()->isFixed() || unit->getFaction() != FACTION_PLAYER) &&
			!weapon->getAmmoItem() &&
			weapon->setAmmoItem(item) == 0)
		{
			item->setSlot(rightHand);
			placed = true;
			break;
		}
	default:
		if ((unit->getGeoscapeSoldier() == 0 || _allowAutoLoadout))
		{
			if (unit->getStats()->strength >= weight) // weight is always considered 0 for aliens
			{
				for (std::vector<std::string>::const_iterator i = _game->getRuleset()->getInvsList().begin(); i != _game->getRuleset()->getInvsList().end() && !placed; ++i)
				{
					RuleInventory *slot = _game->getRuleset()->getInventory(*i);
					if (slot->getType() == INV_SLOT)
					{
						for (std::vector<RuleSlot>::iterator j = slot->getSlots()->begin(); j != slot->getSlots()->end() && !placed; ++j)
						{
							if (!Inventory::overlapItems(unit, item, slot, j->x, j->y) && slot->fitItemInSlot(item->getRules(), j->x, j->y))
							{
								item->moveToOwner(unit);
								item->setSlot(slot);
								item->setSlotX(j->x);
								item->setSlotY(j->y);
								placed = true;
								break;
							}
						}
					}
				}
			}
		}
	break;
	}

	if (placed)
	{
		_save->getItems()->push_back(item);
	}
	item->setXCOMProperty(unit->getFaction() == FACTION_PLAYER);

	return placed;
}

/**
 * Generates a map (set of tiles) for a new battlescape game.
 */
void BattlescapeGenerator::generateMap()
{
	int x = 0, y = 0;
	int blocksToDo = 0;
	std::vector< std::vector<MapBlock*> > blocks;
	std::vector< std::vector<bool> > storageBlocks;
	std::vector< std::vector<bool> > landingzone;
	std::vector< std::vector<int> > segments;
	int ufoX = 0, ufoY = 0;
	bool placed = false;

	MapBlock* dummy = new MapBlock("dummy", 0, 0, MT_DEFAULT);
	MapBlock* craftMap = 0;
	MapBlock* ufoMap = 0;

	int mapDataSetIDOffset = 0;
	int craftDataSetIDOffset = 0;

	_save->setAmbientSound(_terrain->getAmbience());

	blocks.resize((_mapsize_x / 10), std::vector<MapBlock*>((_mapsize_y / 10)));
	landingzone.resize((_mapsize_x / 10), std::vector<bool>((_mapsize_y / 10),false));
	storageBlocks.resize((_mapsize_x / 10), std::vector<bool>((_mapsize_y / 10),false));
	segments.resize((_mapsize_x / 10), std::vector<int>((_mapsize_y / 10),0));

	// The AlienDeployment rules hold terrain data for each mission type.
	AlienDeployment *ruleDeploy = _game->getRuleset()->getDeployment(_ufo?_ufo->getRules()->getType():_save->getMissionType());

	blocksToDo = (_mapsize_x / 10) * (_mapsize_y / 10);

	/* Determine UFO landingzone (do this first because ufo is generally bigger) */
	if (_ufo != 0)
	{
		// pick a random ufo mapblock, can have all kinds of sizes
		ufoMap = _ufo->getRules()->getBattlescapeTerrainData()->getRandomMapBlock(999, MT_DEFAULT);

		ufoX = RNG::generate(0, (_mapsize_x / 10) - ufoMap->getSizeX() / 10);
		ufoY = RNG::generate(0, (_mapsize_y / 10) - ufoMap->getSizeY() / 10);

		for (int i = 0; i < ufoMap->getSizeX() / 10; ++i)
		{
			for (int j = 0; j < ufoMap->getSizeY() / 10; ++j)
			{
				landingzone[ufoX + i][ufoY + j] = true;
				blocks[ufoX + i][ufoY + j] = _terrain->getRandomMapBlock(10, MT_LANDINGZONE);
				blocksToDo--;
			}
		}
	}

	/* Determine Craft landingzone */
	/* alien base assault has no craft landing zone */
	if (_craft != 0 && ruleDeploy->getShowCraft())
	{
		// pick a random craft mapblock, can have all kinds of sizes
		craftMap = _craft->getRules()->getBattlescapeTerrainData()->getRandomMapBlock(999, MT_DEFAULT);
		while (!placed)
		{
			_craftX = RNG::generate(0, (_mapsize_x/10)- craftMap->getSizeX() / 10);
			_craftY = RNG::generate(0, (_mapsize_y/10)- craftMap->getSizeY() / 10);
			placed = true;
			// check if this place is ok
			for (int i = 0; i < craftMap->getSizeX() / 10; ++i)
			{
				for (int j = 0; j < craftMap->getSizeY() / 10; ++j)
				{
					if (landingzone[_craftX + i][_craftY + j])
					{
						placed = false; // whoops the ufo is already here, try again
					}
				}
			}
			// if ok, allocate it
			if (placed)
			{
				for (int i = 0; i < craftMap->getSizeX() / 10; ++i)
				{
					for (int j = 0; j < craftMap->getSizeY() / 10; ++j)
					{
						landingzone[_craftX + i][_craftY + j] = true;
						blocks[_craftX + i][_craftY + j] = _terrain->getRandomMapBlock(10, MT_LANDINGZONE);
						blocksToDo--;
					}
				}
			}
		}
	}

	/* determine positioning of the urban terrain roads */
	if (_terrain->getName() == "URBAN")
	{
		int roadStyle = RNG::generate(0,99);
		std::vector<int> roadChances = _terrain->getRoadTypeOdds();
		bool EWRoad = roadStyle < roadChances.at(0);
		bool NSRoad = !EWRoad && roadStyle < roadChances.at(0) + roadChances.at(1);
		bool TwoRoads = !EWRoad && !NSRoad;
		int roadX = _craftX;
		int roadY = _craftY;
		// make sure the road(s) are not crossing the craft landing site
		while ((roadX >= _craftX && roadX < _craftX + (craftMap->getSizeX() / 10)) || (roadY >= _craftY && roadY < _craftY + (craftMap->getSizeY() / 10)))
		{
			roadX = RNG::generate(0, (_mapsize_x/10)- 1);
			roadY = RNG::generate(0, (_mapsize_y/10)- 1);
		}
		if (TwoRoads)
		{
			// put a crossing on the X,Y position and fill the rest with roads
			blocks[roadX][roadY] = _terrain->getRandomMapBlock(10, MT_CROSSING);
			blocksToDo--;
			EWRoad = true;
			NSRoad = true;
		}
		if (EWRoad)
		{
			while (x < (_mapsize_x / 10))
			{
				if (blocks[x][roadY] == 0)
				{
					blocks[x][roadY] = _terrain->getRandomMapBlock(10, MT_EWROAD);
					blocksToDo--;
				}
				x++;
			}
		}
		if (NSRoad)
		{
			while (y < (_mapsize_y / 10))
			{
				if (blocks[roadX][y] == 0)
				{
					blocks[roadX][y] = _terrain->getRandomMapBlock(10, MT_NSROAD);
					blocksToDo--;
				}
				y++;
			}
		}
	}

	/* determine positioning of base modules */
	if (_save->getMissionType() == "STR_BASE_DEFENSE")
	{
		for (std::vector<BaseFacility*>::const_iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); ++i)
		{
			if ((*i)->getBuildTime() == 0)
			{
				int num = 0;
				for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); ++y)
				{
					for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); ++x)
					{
						// lots of crazy stuff here, which is for the hangars (or other large base facilities one may create)
						std::string mapname = (*i)->getRules()->getMapName();
						std::ostringstream newname;
						newname << mapname.substr(0, mapname.size()-2); // strip of last 2 digits
						int mapnum = atoi(mapname.substr(mapname.size()-2, 2).c_str()); // get number
						mapnum += num;
						if (mapnum < 10) newname << 0;
						newname << mapnum;
						blocks[x][y] = _terrain->getMapBlock(newname.str());
						storageBlocks[x][y] = ((*i)->getRules()->getStorage() > 0);
						num++;
					}
				}
			}
		}

		// fill with dirt
		for (int i = 0; i < (_mapsize_x / 10); ++i)
		{
			for (int j = 0; j < (_mapsize_y / 10); ++j)
			{
				if (blocks[i][j] == 0)
				{
					blocks[i][j] = _terrain->getRandomMapBlock(10, MT_DIRT);
				}
			}
		}

		blocksToDo = 0;
	}

	/* determine positioning of base modules */

	// Iterate through the guaranteed terrain features on this map.
	for (std::vector<TerrainFeature>::iterator f = ruleDeploy->getTerrainFeature()->begin(); f != ruleDeploy->getTerrainFeature()->end(); ++f)
	{
		// Try to find a suitable location for each location in turn.
		// Each try is done by selecting a random X/Y and then checking that the map
		// has a suitable gap at that location.
		//
		// If the AlienDeployment has a small map with too many features this will give up,
		// and we'll get a map missing features.  
		int tries = 0;
		bool done = false;

		// Determine the size of the feature in terms of our 10x10 mapblocks.  
		int sizeX = ((*f).width + 9) / 10;
		int sizeY = ((*f).length + 9) / 10;

		while (!done && tries < 50)
		{
			// Get a random X/Y block, allowing size for our feature.
			int randX = RNG::generate(0, (_mapsize_x/10) - sizeX);
			int randY = RNG::generate(0, (_mapsize_y/10) - sizeY);

			// Check each mapblock that would be affected by our feature
			bool free = true;
			for (int i = 0; i < sizeX; ++i)
			{
				for (int j = 0; j < sizeY; ++j)
				{
					if (blocks[randX + i][randY + j] != NULL ||
						landingzone[randX + i][randY + j])
					{
						free = false;
					}
				}
			}

			if (free)
			{
				// Mark off used space on the map
				for (int i = 0; i < sizeX; i++)
				{
					for (int j = 0; j < sizeY; j++)
					{
						blocks[randX + i][randY + j] = dummy;
						--blocksToDo;					
					}
				}

				// Set a terrain feature at the free block we found
				blocks[randX][randY] = _terrain->getRandomMapBlock((*f).width, (*f).type);
				done = true;
			}

			++tries;
		}

	}

	x = 0;
	y = 0;
	int maxLarge = _terrain->getLargeBlockLimit();
	int curLarge = 0;
	int tries = 0;
	while (curLarge != maxLarge && tries <= 50)
	{
		int randX = RNG::generate(0, (_mapsize_x/10)- 2);
		int randY = RNG::generate(0, (_mapsize_y/10)- 2);
		if (!blocks[randX][randY] && !blocks[randX + 1][randY] && !blocks[randX + 1][randY + 1] && !blocks[randX][randY + 1]
		&& !landingzone[randX][randY] && !landingzone[randX + 1][randY] && !landingzone[randX][randY + 1] && !landingzone[randX + 1][randY + 1])
		{
			blocks[randX][randY] = _terrain->getRandomMapBlock(20, MT_DEFAULT, true);
			blocksToDo--;
			// mark mapblocks as used
			blocks[randX + 1][randY] = dummy;
			blocksToDo--;
			blocks[randX + 1][randY + 1] = dummy;
			blocksToDo--;
			blocks[randX][randY + 1] = dummy;
			blocksToDo--;
			curLarge++;
		}
		tries++;
	}
	/* Random map generation for crash/landing sites */
	while (blocksToDo)
	{
		if (blocks[x][y] == 0)
		{
			// Alien bases use special MT_CROSSING mapblocks as padding in 60% of blocks
			// others use MT_DEFAULT.
			if ((_terrain->getName() == "UBASE") && RNG::generate(0,100) > 60)
			{
				blocks[x][y] = _terrain->getRandomMapBlock(10, landingzone[x][y]?MT_LANDINGZONE:MT_CROSSING);
			}
			else
			{
				blocks[x][y] = _terrain->getRandomMapBlock(10, landingzone[x][y]?MT_LANDINGZONE:MT_DEFAULT);
			}
			blocksToDo--;
			x++;
		}
		else
		{
			x++;
		}

		if (x >= (_mapsize_x / 10)) // reach the end
		{
			x = 0;
			y++;
		}
	}

	//reset the "times used" fields.
	_terrain->resetMapBlocks();

	for (std::vector<MapDataSet*>::iterator i = _terrain->getMapDataSets()->begin(); i != _terrain->getMapDataSets()->end(); ++i)
	{
		(*i)->loadData();
		if (_game->getRuleset()->getMCDPatch((*i)->getName()))
		{
			_game->getRuleset()->getMCDPatch((*i)->getName())->modifyData(*i);
		}
		_save->getMapDataSets()->push_back(*i);
		mapDataSetIDOffset++;
	}

	/* now load them up */
	int segment = 0;
	for (int itY = 0; itY < (_mapsize_y / 10); itY++)
	{
		for (int itX = 0; itX < (_mapsize_x / 10); itX++)
		{
			segments[itX][itY] = segment;
			if (blocks[itX][itY] != 0 && blocks[itX][itY] != dummy)
			{
				loadMAP(blocks[itX][itY], itX * 10, itY * 10, _terrain, 0);
				if (!landingzone[itX][itY])
				{
					loadRMP(blocks[itX][itY], itX * 10, itY * 10, segment++);
				}
			}
		}
	}

	/* making passages between blocks in a base map */
	if (_save->getMissionType() == "STR_BASE_DEFENSE" || _terrain->getName() == "UBASE")
	{
		int ewallfix = 14;
		int swallfix = 13;
		int ewallfixSet = 1;
		int swallfixSet = 1;
		if (_terrain->getName() == "UBASE")
		{
			ewallfix = 17; //  north wall
			swallfix = 18; // west wall
			ewallfixSet = 2;
			swallfixSet = 2;
		}

		MapDataSet *mds = _terrain->getMapDataSets()->at(ewallfixSet);
		MapBlock *dirt = _terrain->getRandomMapBlock(10, MT_DIRT);
		for (int i = 0; i < (_mapsize_x / 10); ++i)
		{
			for (int j = 0; j < (_mapsize_y / 10); ++j)
			{
				if (blocks[i][j] == dirt)
					continue;

				// general stores - there is where the items are put
				if (storageBlocks[i][j])
				{
					for (int k = i * 10; k != (i + 1) * 10; ++k)
					{
						for (int l = j * 10; l != (j + 1) * 10; ++l)
						{
							// we only want every other tile, giving us a "checkerboard" pattern
							if ((k+l) % 2 == 0)
							{
								Tile *t = _save->getTile(Position(k,l,1));
								Tile *tEast = _save->getTile(Position(k+1,l,1));
								Tile *tSouth = _save->getTile(Position(k,l+1,1));
								if (t && t->getMapData(MapData::O_FLOOR) && !t->getMapData(MapData::O_OBJECT) &&
									tEast && !tEast->getMapData(MapData::O_WESTWALL) &&
									tSouth && !tSouth->getMapData(MapData::O_NORTHWALL))
								{
									_save->getStorageSpace().push_back(Position(k, l, 1));
								}
							}
						}
					}
					// let's put the inventory tile on the lower floor, just to be safe.
					_craftInventoryTile = _save->getTile(Position((i*10)+5,(j*10)+5,0));
				}

				// drill east
				if (i < (_mapsize_x / 10)-1
					&& blocks[i+1][j] != dirt
					&& _save->getTile(Position((i*10)+9,(j*10)+4,0))->getMapData(MapData::O_OBJECT)
					&& (!_save->getTile(Position((i*10)+8,(j*10)+4,0))->getMapData(MapData::O_OBJECT)
					|| (_save->getTile(Position((i*10)+9,(j*10)+4,0))->getMapData(MapData::O_OBJECT)
					!= _save->getTile(Position((i*10)+8,(j*10)+4,0))->getMapData(MapData::O_OBJECT))))
				{
					// remove stuff
					_save->getTile(Position((i*10)+9,(j*10)+3,0))->setMapData(0, -1, -1, MapData::O_WESTWALL);
					_save->getTile(Position((i*10)+9,(j*10)+3,0))->setMapData(0, -1, -1, MapData::O_OBJECT);
					_save->getTile(Position((i*10)+9,(j*10)+4,0))->setMapData(0, -1, -1, MapData::O_WESTWALL);
					_save->getTile(Position((i*10)+9,(j*10)+4,0))->setMapData(0, -1, -1, MapData::O_OBJECT);
					_save->getTile(Position((i*10)+9,(j*10)+5,0))->setMapData(0, -1, -1, MapData::O_WESTWALL);
					_save->getTile(Position((i*10)+9,(j*10)+5,0))->setMapData(0, -1, -1, MapData::O_OBJECT);
					if (_save->getTile(Position((i*10)+9,(j*10)+2,0))->getMapData(MapData::O_OBJECT))
					{
						//wallfix
						_save->getTile(Position((i*10)+9,(j*10)+3,0))->setMapData(mds->getObjects()->at(ewallfix), ewallfix, ewallfixSet, MapData::O_NORTHWALL);
						_save->getTile(Position((i*10)+9,(j*10)+6,0))->setMapData(mds->getObjects()->at(ewallfix), ewallfix, ewallfixSet, MapData::O_NORTHWALL);
					}
					if (_terrain->getName() == "UBASE")
					{
						//wallcornerfix
						if (!_save->getTile(Position((i*10)+10,(j*10)+3,0))->getMapData(MapData::O_NORTHWALL))
						{
							_save->getTile(Position(((i+1)*10),(j*10)+3,0))->setMapData(mds->getObjects()->at(swallfix+1), swallfix+1, swallfixSet, MapData::O_OBJECT);
						}
						//floorfix
						_save->getTile(Position((i*10)+9,(j*10)+3,0))->setMapData(_terrain->getMapDataSets()->at(1)->getObjects()->at(63), 63, 1, MapData::O_FLOOR);
						_save->getTile(Position((i*10)+9,(j*10)+4,0))->setMapData(_terrain->getMapDataSets()->at(1)->getObjects()->at(63), 63, 1, MapData::O_FLOOR);
						_save->getTile(Position((i*10)+9,(j*10)+5,0))->setMapData(_terrain->getMapDataSets()->at(1)->getObjects()->at(63), 63, 1, MapData::O_FLOOR);
					}
					// remove more stuff
					_save->getTile(Position(((i+1)*10),(j*10)+3,0))->setMapData(0, -1, -1, MapData::O_WESTWALL);
					_save->getTile(Position(((i+1)*10),(j*10)+4,0))->setMapData(0, -1, -1, MapData::O_WESTWALL);
					_save->getTile(Position(((i+1)*10),(j*10)+5,0))->setMapData(0, -1, -1, MapData::O_WESTWALL);
				}
				// drill south
				if (j < (_mapsize_y / 10)-1
					&& blocks[i][j+1] != dirt
					&& _save->getTile(Position((i*10)+4,(j*10)+9,0))->getMapData(MapData::O_OBJECT)
					&& (!_save->getTile(Position((i*10)+4,(j*10)+8,0))->getMapData(MapData::O_OBJECT)
					|| (_save->getTile(Position((i*10)+4,(j*10)+9,0))->getMapData(MapData::O_OBJECT)
					!= _save->getTile(Position((i*10)+4,(j*10)+8,0))->getMapData(MapData::O_OBJECT))))
				{
					// remove stuff
					_save->getTile(Position((i*10)+3,(j*10)+9,0))->setMapData(0, -1, -1, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+3,(j*10)+9,0))->setMapData(0, -1, -1, MapData::O_OBJECT);
					_save->getTile(Position((i*10)+4,(j*10)+9,0))->setMapData(0, -1, -1, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+4,(j*10)+9,0))->setMapData(0, -1, -1, MapData::O_OBJECT);
					_save->getTile(Position((i*10)+5,(j*10)+9,0))->setMapData(0, -1, -1, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+5,(j*10)+9,0))->setMapData(0, -1, -1, MapData::O_OBJECT);
					if (_save->getTile(Position((i*10)+2,(j*10)+9,0))->getMapData(MapData::O_OBJECT))
					{
						// wallfix
						_save->getTile(Position((i*10)+3,(j*10)+9,0))->setMapData(mds->getObjects()->at(swallfix), swallfix, swallfixSet, MapData::O_WESTWALL);
						_save->getTile(Position((i*10)+6,(j*10)+9,0))->setMapData(mds->getObjects()->at(swallfix), swallfix, swallfixSet, MapData::O_WESTWALL);
					}
					if (_terrain->getName() == "UBASE")
					{
						// wallcornerfix
						if (!_save->getTile(Position((i*10)+3,(j*10)+10,0))->getMapData(MapData::O_WESTWALL))
						{
							_save->getTile(Position((i*10)+3,((j+1)*10),0))->setMapData(mds->getObjects()->at(swallfix+1), swallfix+1, swallfixSet, MapData::O_OBJECT);
						}
						// floorfix
						_save->getTile(Position((i*10)+3,(j*10)+9,0))->setMapData(_terrain->getMapDataSets()->at(1)->getObjects()->at(63), 63, 1, MapData::O_FLOOR);
						_save->getTile(Position((i*10)+4,(j*10)+9,0))->setMapData(_terrain->getMapDataSets()->at(1)->getObjects()->at(63), 63, 1, MapData::O_FLOOR);
						_save->getTile(Position((i*10)+5,(j*10)+9,0))->setMapData(_terrain->getMapDataSets()->at(1)->getObjects()->at(63), 63, 1, MapData::O_FLOOR);
					}
					// remove more stuff
					_save->getTile(Position((i*10)+3,(j+1)*10,0))->setMapData(0, -1, -1, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+4,(j+1)*10,0))->setMapData(0, -1, -1, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+5,(j+1)*10,0))->setMapData(0, -1, -1, MapData::O_NORTHWALL);
				}
			}
		}
	}


	if (_ufo != 0)
	{
		for (std::vector<MapDataSet*>::iterator i = _ufo->getRules()->getBattlescapeTerrainData()->getMapDataSets()->begin(); i != _ufo->getRules()->getBattlescapeTerrainData()->getMapDataSets()->end(); ++i)
		{
			(*i)->loadData();
			if (_game->getRuleset()->getMCDPatch((*i)->getName()))
			{
				_game->getRuleset()->getMCDPatch((*i)->getName())->modifyData(*i);
			}
			_save->getMapDataSets()->push_back(*i);
			craftDataSetIDOffset++;
		}
		loadMAP(ufoMap, ufoX * 10, ufoY * 10, _ufo->getRules()->getBattlescapeTerrainData(), mapDataSetIDOffset);
		loadRMP(ufoMap, ufoX * 10, ufoY * 10, Node::UFOSEGMENT);
		for (int i = 0; i < ufoMap->getSizeX() / 10; ++i)
		{
			for (int j = 0; j < ufoMap->getSizeY() / 10; j++)
			{
				segments[ufoX + i][ufoY + j] = Node::UFOSEGMENT;
			}
		}
	}

	if (_craft != 0 && ruleDeploy->getShowCraft())
	{
		for (std::vector<MapDataSet*>::iterator i = _craft->getRules()->getBattlescapeTerrainData()->getMapDataSets()->begin(); i != _craft->getRules()->getBattlescapeTerrainData()->getMapDataSets()->end(); ++i)
		{
			(*i)->loadData();
			if (_game->getRuleset()->getMCDPatch((*i)->getName()))
			{
				_game->getRuleset()->getMCDPatch((*i)->getName())->modifyData(*i);
			}
			_save->getMapDataSets()->push_back(*i);
		}
		loadMAP(craftMap, _craftX * 10, _craftY * 10, _craft->getRules()->getBattlescapeTerrainData(), mapDataSetIDOffset + craftDataSetIDOffset, true, true);
		loadRMP(craftMap, _craftX * 10, _craftY * 10, Node::CRAFTSEGMENT);
		for (int i = 0; i < craftMap->getSizeX() / 10; ++i)
		{
			for (int j = 0; j < craftMap->getSizeY() / 10; j++)
			{
				segments[_craftX + i][_craftY + j] = Node::CRAFTSEGMENT;
			}
		}
	}

	/* attach nodelinks to each other */
	for (std::vector<Node*>::iterator i = _save->getNodes()->begin(); i != _save->getNodes()->end(); ++i)
	{
		Node *node = (*i);
		int segmentX = node->getPosition().x / 10;
		int segmentY = node->getPosition().y / 10;
		int neighbourSegments[4];
		int neighbourDirections[4] = { -2, -3, -4, -5 };
		int neighbourDirectionsInverted[4] = { -4, -5, -2, -3 };

		if (segmentX == (_mapsize_x / 10)-1)
			neighbourSegments[0] = -1;
		else
			neighbourSegments[0] = segments[segmentX+1][segmentY];
		if (segmentY == (_mapsize_y / 10)-1)
			neighbourSegments[1] = -1;
		else
			neighbourSegments[1] = segments[segmentX][segmentY+1];
		if (segmentX == 0)
			neighbourSegments[2] = -1;
		else
			neighbourSegments[2] = segments[segmentX-1][segmentY];
		if (segmentY == 0)
			neighbourSegments[3] = -1;
		else
			neighbourSegments[3] = segments[segmentX][segmentY-1];

		for (std::vector<int>::iterator j = node->getNodeLinks()->begin(); j != node->getNodeLinks()->end(); ++j )
		{
			for (int n = 0; n < 4; n++)
			{
				if (*j == neighbourDirections[n])
				{
					for (std::vector<Node*>::iterator k = _save->getNodes()->begin(); k != _save->getNodes()->end(); ++k)
					{
						if ((*k)->getSegment() == neighbourSegments[n])
						{
							for (std::vector<int>::iterator l = (*k)->getNodeLinks()->begin(); l != (*k)->getNodeLinks()->end(); ++l )
							{
								if (*l == neighbourDirectionsInverted[n])
								{
									*l = node->getID();
									*j = (*k)->getID();
								}
							}
						}
					}
				}
			}
		}
	}

	delete dummy;
}


/**
 * Loads an XCom format MAP file into the tiles of the battlegame.
 * @param mapblock Pointer to MapBlock.
 * @param xoff Mapblock offset in X direction.
 * @param yoff Mapblock offset in Y direction.
 * @param save Pointer to the current SavedBattleGame.
 * @param terrain Pointer to the Terrain rule.
 * @param discovered Whether or not this mapblock is discovered (eg. landingsite of the XCom plane).
 * @return int Height of the loaded mapblock (this is needed for spawpoint calculation...)
 * @sa http://www.ufopaedia.org/index.php?title=MAPS
 * @note Y-axis is in reverse order.
 */
int BattlescapeGenerator::loadMAP(MapBlock *mapblock, int xoff, int yoff, RuleTerrain *terrain, int mapDataSetOffset, bool discovered, bool craft)
{
	int sizex, sizey, sizez;
	int x = xoff, y = yoff, z = 0;
	char size[3];
	unsigned char value[4];
	std::ostringstream filename;
	filename << "MAPS/" << mapblock->getName() << ".MAP";
	unsigned int terrainObjectID;

	// Load file
	std::ifstream mapFile (CrossPlatform::getDataFile(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw Exception(filename.str() + " not found");
	}

	mapFile.read((char*)&size, sizeof(size));
	sizey = (int)size[0];
	sizex = (int)size[1];
	sizez = (int)size[2];

	if (sizez > _save->getMapSizeZ())
	{
		throw Exception("Height of map too big for this mission");
	}

	z += sizez - 1;
	mapblock->setSizeZ(sizez);

	for (int i = _mapsize_z-1; i >0; i--)
	{
		// check if there is already a layer - if so, we have to move Z up
		MapData *floor = _save->getTile(Position(x, y, i))->getMapData(MapData::O_FLOOR);
		if (floor != 0)
		{
			z += i;
			if (craft)
			{
				_craftZ = i;
			}
			break;
		}
	}

	if (z > (_save->getMapSizeZ()-1))
	{
		throw Exception("Something is wrong in your map definitions");
	}

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		for (int part = 0; part < 4; part++)
		{
			terrainObjectID = ((unsigned char)value[part]);
			if (terrainObjectID>0)
			{
				int mapDataSetID = mapDataSetOffset;
				unsigned int mapDataID = terrainObjectID;
				MapData *md = terrain->getMapData(&mapDataID, &mapDataSetID);
				_save->getTile(Position(x, y, z))->setMapData(md, mapDataID, mapDataSetID, part);
			}
			// if the part is empty and it's not a floor, remove it
			// it prevents growing grass in UFOs
			if (terrainObjectID == 0 && part > 0)
			{
				_save->getTile(Position(x, y, z))->setMapData(0, -1, -1, part);
			}
		}
		if (craft && _craftZ == z)
		{
			for (int z2 = _save->getMapSizeZ()-1; z2 >= _craftZ; --z2)
			{
				_save->getTile(Position(x, y, z2))->setDiscovered(true, 2);
			}
		}
		_save->getTile(Position(x, y, z))->setDiscovered(discovered, 2);

		x++;

		if (x == (sizex + xoff))
		{
			x = xoff;
			y++;
		}
		if (y == (sizey + yoff))
		{
			y = yoff;
			z--;
		}
	}

	if (!mapFile.eof())
	{
		throw Exception("Invalid MAP file");
	}

	mapFile.close();
	
	if (_generateFuel)
	{
		// if one of the mapBlocks has an items array defined, don't deploy fuel algorithmically
		_generateFuel = mapblock->getItems()->empty();
	}
	for (std::map<std::string, std::vector<Position> >::const_iterator i = mapblock->getItems()->begin(); i != mapblock->getItems()->end(); ++i)
	{
		RuleItem *rule = _game->getRuleset()->getItem((*i).first);
		for (std::vector<Position>::const_iterator j = (*i).second.begin(); j != (*i).second.end(); ++j)
		{
			BattleItem *item = new BattleItem(rule, _save->getCurrentItemId());
			_save->getItems()->push_back(item);
			_save->getTile((*j) + Position(xoff, yoff, 0))->addItem(item, _game->getRuleset()->getInventory("STR_GROUND"));
		}
	}
	return sizez;
}

/**
 * Loads an XCom format RMP file into the spawnpoints of the battlegame.
 * @param mapblock Pointer to MapBlock.
 * @param xoff Mapblock offset in X direction.
 * @param yoff Mapblock offset in Y direction.
 * @param segment Mapblock segment.
 * @sa http://www.ufopaedia.org/index.php?title=ROUTES
 */
void BattlescapeGenerator::loadRMP(MapBlock *mapblock, int xoff, int yoff, int segment)
{
	int id = 0;
	char value[24];
	std::ostringstream filename;
	filename << "ROUTES/" << mapblock->getName() << ".RMP";

	// Load file
	std::ifstream mapFile (CrossPlatform::getDataFile(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw Exception(filename.str() + " not found");
	}

	size_t nodeOffset = _save->getNodes()->size();

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		if( (int)value[0] < mapblock->getSizeY() && (int)value[1] < mapblock->getSizeX() && (int)value[2] < _mapsize_z )
		{
			Node *node = new Node(nodeOffset + id, Position(xoff + (int)value[1], yoff + (int)value[0], mapblock->getSizeZ() - 1 - (int)value[2]), segment, (int)value[19], (int)value[20], (int)value[21], (int)value[22], (int)value[23]);
			for (int j=0;j<5;++j)
			{
				int connectID = (int)((signed char)value[4 + j*3]);
				if (connectID > -1)
				{
					connectID += nodeOffset;
				}
				node->getNodeLinks()->push_back(connectID);
			}
			_save->getNodes()->push_back(node);
		}
		id++;
	}

	if (!mapFile.eof())
	{
		throw Exception("Invalid RMP file");
	}

	mapFile.close();
}

/**
 * Fill power sources with an alien fuel object.
 */
void BattlescapeGenerator::fuelPowerSources()
{
	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
	{
		if (_save->getTiles()[i]->getMapData(MapData::O_OBJECT) 
			&& _save->getTiles()[i]->getMapData(MapData::O_OBJECT)->getSpecialType() == UFO_POWER_SOURCE)
		{
			BattleItem *alienFuel = new BattleItem(_game->getRuleset()->getItem(_game->getRuleset()->getAlienFuel()), _save->getCurrentItemId());
			_save->getItems()->push_back(alienFuel);
			_save->getTiles()[i]->addItem(alienFuel, _game->getRuleset()->getInventory("STR_GROUND"));
		}
	}
}


/**
 * When a UFO crashes, there is a 75% chance for each powersource to explode.
 */
void BattlescapeGenerator::explodePowerSources()
{
	for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
	{
		if (_save->getTiles()[i]->getMapData(MapData::O_OBJECT)
			&& _save->getTiles()[i]->getMapData(MapData::O_OBJECT)->getSpecialType() == UFO_POWER_SOURCE && RNG::percent(75))
		{
			Position pos;
			pos.x = _save->getTiles()[i]->getPosition().x*16;
			pos.y = _save->getTiles()[i]->getPosition().y*16;
			pos.z = (_save->getTiles()[i]->getPosition().z*24) +12;
			_save->getTileEngine()->explode(pos, 180+RNG::generate(0,70), DT_HE, 10);
		}
	}
	Tile *t = _save->getTileEngine()->checkForTerrainExplosions();
	while (t)
	{
		Position p = Position(t->getPosition().x * 16, t->getPosition().y * 16, t->getPosition().z * 24);
		p += Position(8,8,0);
		_save->getTileEngine()->explode(p, t->getExplosive(), DT_HE, t->getExplosive() / 10);
		t = _save->getTileEngine()->checkForTerrainExplosions();
	}
}

/**
 * Spawns 1-16 civilians on a terror mission.
 * @param max Maximum number of civilians to spawn.
 */
void BattlescapeGenerator::deployCivilians(int max)
{
	if (max)
	{
		// inevitably someone will point out that ufopaedia says 0-16 civilians.
		// to that person: i looked at the code and it says otherwise.
		// 0 civilians would only be a possibility if there were already 80 units,
		// or no spawn nodes for civilians.
		int number = RNG::generate(max/2, max);

		if (number > 0)
		{
			for (int i = 0; i < number; ++i)
			{
				size_t pick = RNG::generate(0, _terrain->getCivilianTypes().size() -1);
				addCivilian(_game->getRuleset()->getUnit(_terrain->getCivilianTypes().at(pick)));
			}
		}
	}
}

/**
 * Sets the alien base involved in the battle.
 * @param base Pointer to alien base.
 */
void BattlescapeGenerator::setAlienBase(AlienBase *base)
{
	_alienBase = base;
	_alienBase->setInBattlescape(true);
}

/**
 * Places a unit near a friendly unit.
 * @param unit Pointer to the unit in question.
 * @return If we successfully placed the unit.
 */
bool BattlescapeGenerator::placeUnitNearFriend(BattleUnit *unit)
{
	Position entryPoint = Position(-1, -1, -1);
	int tries = 100;
	while (entryPoint == Position(-1, -1, -1) && tries)
	{
		BattleUnit* k = _save->getUnits()->at(RNG::generate(0, _save->getUnits()->size()-1));
		if (k->getFaction() == unit->getFaction() && k->getPosition() != Position(-1, -1, -1) && k->getArmor()->getSize() == 1)
		{
			entryPoint = k->getPosition();
		}
		--tries;
	}
	if (tries && _save->placeUnitNearPosition(unit, entryPoint))
	{
		return true;
	}
	return false;
}


/**
 * Gets battlescape terrain using globe texture and latitude.
 * @param tex Globe texture.
 * @param lat Latitude.
 * @return Pointer to ruleterrain.
 */
RuleTerrain *BattlescapeGenerator::getTerrain(int tex, double lat)
{
	RuleTerrain *t =  0;
	const std::vector<std::string> &terrains = _game->getRuleset()->getTerrainList();
	for (std::vector<std::string>::const_iterator i = terrains.begin(); i != terrains.end(); ++i)
	{
		t =  _game->getRuleset()->getTerrain(*i);
		for (std::vector<int>::iterator j = t->getTextures()->begin(); j != t->getTextures()->end(); ++j )
		{
			if (*j == tex && (t->getHemisphere() == 0 || (t->getHemisphere() < 0 && lat < 0) || (t->getHemisphere() > 0 && lat >= 0)))
			{
				return t;
			}
		}
	}

	assert(0 && "No matching terrain for globe texture");
	return t;
}

/**
* Creates a mini-battle-save for managing inventory from the Geoscape.
* Kids, don't try this at home!
* @param craft Pointer to craft to manage.
*/
void BattlescapeGenerator::runInventory(Craft *craft)
{
	// we need to fake a map for soldier placement
	_baseInventory = true;
	int soldiers = craft->getNumSoldiers();
	_mapsize_x = soldiers;
	_mapsize_y = 1;
	_mapsize_z = 1;
	_save->initMap(_mapsize_x, _mapsize_y, _mapsize_z);
	MapDataSet *set = new MapDataSet("dummy");
	MapData *data = new MapData(set);
	for (int i = 0; i < soldiers; ++i)
	{
		Tile *tile = _save->getTiles()[i];
		tile->setMapData(data, 0, 0, MapData::O_FLOOR);
		tile->getMapData(MapData::O_FLOOR)->setSpecialType(START_POINT, 0);
		tile->getMapData(MapData::O_FLOOR)->setTUWalk(0);
		tile->getMapData(MapData::O_FLOOR)->setFlags(false, false, false, 0, false, false, false, false, false);
	}

	// ok now generate the battleitems for inventory
	setCraft(craft);
	deployXCOM();
	delete data;
	delete set;
}

/**
 * Loads all XCom weaponry before anything else is distributed.
 */
void BattlescapeGenerator::loadWeapons()
{
	// let's try to load this weapon, whether we equip it or not.
	for (std::vector<BattleItem*>::iterator i = _craftInventoryTile->getInventory()->begin(); i != _craftInventoryTile->getInventory()->end(); ++i)
	{
		if (!(*i)->getRules()->isFixed() &&
			!(*i)->getRules()->getCompatibleAmmo()->empty() &&
			(*i)->getAmmoItem() == 0 &&
			((*i)->getRules()->getBattleType() == BT_FIREARM || (*i)->getRules()->getBattleType() == BT_MELEE))
		{
			bool loaded = false;
			for (std::vector<BattleItem*>::iterator j = _craftInventoryTile->getInventory()->begin(); j != _craftInventoryTile->getInventory()->end() && !loaded; ++j)
			{
				if ((*j)->getSlot() == _game->getRuleset()->getInventory("STR_GROUND") && (*i)->setAmmoItem((*j)) == 0)
				{
					_save->getItems()->push_back(*j);
					(*j)->setXCOMProperty(true);
					(*j)->setSlot(_game->getRuleset()->getInventory("STR_RIGHT_HAND"));
					loaded = true;
				}
			}
		}
	}
	for (std::vector<BattleItem*>::iterator i = _craftInventoryTile->getInventory()->begin(); i != _craftInventoryTile->getInventory()->end();)
	{
		if ((*i)->getSlot() != _game->getRuleset()->getInventory("STR_GROUND"))
		{
			i = _craftInventoryTile->getInventory()->erase(i);
			continue;
		}
		++i;
	}

}
}
