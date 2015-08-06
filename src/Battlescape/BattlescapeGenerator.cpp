/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/FileMap.h"
#include "../Engine/Options.h"
#include "../Engine/RNG.h"
#include "../Engine/Exception.h"
#include "../Ruleset/MapBlock.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/RuleUfo.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/MapData.h"
#include "../Ruleset/MCDPatch.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/Unit.h"
#include "../Ruleset/AlienRace.h"
#include "../Ruleset/AlienDeployment.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Ruleset/RuleGlobe.h"
#include "../Ruleset/Texture.h"
#include "../Resource/XcomResourcePack.h"
#include "../Savegame/Vehicle.h"
#include "../Savegame/MissionSite.h"
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
BattlescapeGenerator::BattlescapeGenerator(Game *game) : _game(game), _save(game->getSavedGame()->getSavedBattle()), _res(_game->getResourcePack()), _craft(0), _ufo(0), _base(0), _mission(0), _alienBase(0), _terrain(0),
														 _mapsize_x(0), _mapsize_y(0), _mapsize_z(0), _worldTexture(0), _worldShade(0), _unitSequence(0), _craftInventoryTile(0), _alienItemLevel(0), _baseInventory(false), _generateFuel(true), _craftDeployed(false), _craftZ(0)
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
 * Sets up all our various arrays and whatnot according to the size of the map.
 */
void BattlescapeGenerator::init()
{
	_blocks.clear();
	_landingzone.clear();
	_segments.clear();
	_drillMap.clear();

	_blocks.resize((_mapsize_x / 10), std::vector<MapBlock*>((_mapsize_y / 10)));
	_landingzone.resize((_mapsize_x / 10), std::vector<bool>((_mapsize_y / 10),false));
	_segments.resize((_mapsize_x / 10), std::vector<int>((_mapsize_y / 10),0));
	_drillMap.resize((_mapsize_x / 10), std::vector<int>((_mapsize_y / 10),MD_NONE));

	_blocksToDo = (_mapsize_x / 10) * (_mapsize_y / 10);
	// creates the tile objects
	_save->initMap(_mapsize_x, _mapsize_y, _mapsize_z);
	_save->initUtilities(_res);
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
void BattlescapeGenerator::setWorldTexture(Texture *texture)
{
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
 * Sets the mission site involved in the battle.
 * @param mission Pointer to mission site.
 */
void BattlescapeGenerator::setMissionSite(MissionSite *mission)
{
	_mission = mission;
	_mission->setInBattlescape(true);
}


/**
 * Switches an existing battlescapesavegame to a new stage.
 */
void BattlescapeGenerator::nextStage()
{
	// kill all enemy units, or those not in endpoint area (if aborted)
	for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
	{
		if ((*i)->getStatus() != STATUS_DEAD                              // if they're not dead
			&& (((*i)->getOriginalFaction() == FACTION_PLAYER               // and they're a soldier
			&& _game->getSavedGame()->getSavedBattle()->isAborted()           // and you aborted
			&& !(*i)->isInExitArea(END_POINT))                                // and they're not on the exit
			|| (*i)->getOriginalFaction() != FACTION_PLAYER))               // or they're not a soldier
		{
			(*i)->goToTimeOut();
		}
		if ((*i)->getTile())
		{
			(*i)->getTile()->setUnit(0);
		}
		(*i)->setTile(0);
		(*i)->setPosition(Position(-1,-1,-1), false);
	}

	_save->resetTurnCounter();

	// remove all items not belonging to our soldiers from the map.
	// sort items into two categories:
	// the ones that we are guaranteed to be able to take home, barring complete failure (ie: stuff on the ship)
	// and the ones that are scattered about on the ground, that will be recovered ONLY on success.
	// this does not include items in your soldier's hands.
	std::vector<BattleItem*> *takeHomeGuaranteed = _save->getGuaranteedRecoveredItems();
	std::vector<BattleItem*> *takeHomeConditional = _save->getConditionalRecoveredItems();
	std::vector<BattleItem*> takeToNextStage;
	std::map<RuleItem*, int> guaranteedRounds, conditionalRounds;

	for (std::vector<BattleItem*>::iterator i = _save->getItems()->begin(); i != _save->getItems()->end();)
	{
		if (!(*i)->getOwner() || (*i)->getOwner()->getOriginalFaction() != FACTION_PLAYER)
		{
			bool isAmmo = false;

			for (std::vector<BattleItem*>::iterator j = _save->getItems()->begin(); j != _save->getItems()->end(); ++j)
			{
				if ((*j)->getAmmoItem() == *i)
				{
					if ((*j)->getOwner() && (*j)->getOwner()->getOriginalFaction() == FACTION_PLAYER)
					{
						isAmmo = true;
					}
					break;
				}
			}

			if (isAmmo)
			{
				++i;
				continue;
			}

			Tile *tile = (*i)->getTile();
			std::vector<BattleItem*> *toContainer = takeHomeConditional;
			if (tile)
			{
				tile->removeItem(*i);
				if (tile->getMapData(O_FLOOR))
				{
					if (tile->getMapData(O_FLOOR)->getSpecialType() == START_POINT)
					{
						toContainer = takeHomeGuaranteed;
					}
					else if (tile->getMapData(O_FLOOR)->getSpecialType() == END_POINT
					&& (*i)->getRules()->isRecoverable()
					&& !(*i)->getUnit())
					{
						takeToNextStage.push_back(*i);
						++i;
						continue;
					}
				}
			}
			if ((*i)->getRules()->isRecoverable() && !(*i)->getXCOMProperty())
			{
				toContainer->push_back(*i);
				i = _save->getItems()->erase(i);
				continue;
			}
		}
		++i;
	}

	AlienDeployment *ruleDeploy = _game->getRuleset()->getDeployment(_save->getMissionType());
	ruleDeploy->getDimensions(&_mapsize_x, &_mapsize_y, &_mapsize_z);
	size_t pick = RNG::generate(0, ruleDeploy->getTerrains().size() -1);
	_terrain = _game->getRuleset()->getTerrain(ruleDeploy->getTerrains().at(pick));
	_worldShade = ruleDeploy->getShade();

	const std::vector<MapScript*> *script = _game->getRuleset()->getMapScript(_terrain->getScript());
	if (_game->getRuleset()->getMapScript(ruleDeploy->getScript()))
	{
		script = _game->getRuleset()->getMapScript(ruleDeploy->getScript());
	}
	else if (ruleDeploy->getScript() != "")
	{
		throw Exception("Map generator encountered an error: " + ruleDeploy->getScript() + " script not found.");
	}
	if (script == 0)
	{
		throw Exception("Map generator encountered an error: " + _terrain->getScript() + " script not found.");
	}

	generateMap(script);

	setupObjectives(ruleDeploy);

	int highestSoldierID = 0;
	bool selectedFirstSoldier = false;
	for (std::vector<BattleUnit*>::iterator j = _save->getUnits()->begin(); j != _save->getUnits()->end(); ++j)
	{
		if ((*j)->getOriginalFaction() == FACTION_PLAYER)
		{
			if (!(*j)->isOut())
			{
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
					//reset TUs, regain energy, etc. but don't take damage or go berserk
					(*j)->prepareNewTurn(false);
				}
			}
		}
	}
	// tanks only i guess?
	if (_save->getSelectedUnit() == 0 || _save->getSelectedUnit()->getFaction() != FACTION_PLAYER)
	{
		_save->selectNextPlayerUnit();
	}
	RuleInventory *ground = _game->getRuleset()->getInventory("STR_GROUND");

	for (std::vector<BattleItem*>::iterator i = takeToNextStage.begin(); i != takeToNextStage.end(); ++i)
	{
		_craftInventoryTile->addItem(*i, ground);
	}

	_unitSequence = _save->getUnits()->back()->getId() + 1;

	size_t unitCount = _save->getUnits()->size();

	// Let's figure out what race we're up against.
	_alienRace = ruleDeploy->getRace();

	for (std::vector<MissionSite*>::iterator i = _game->getSavedGame()->getMissionSites()->begin();
		_alienRace == "" && i != _game->getSavedGame()->getMissionSites()->end(); ++i)
	{
		if ((*i)->isInBattlescape())
		{
			_alienRace = (*i)->getAlienRace();
		}
	}

	for (std::vector<AlienBase*>::iterator i = _game->getSavedGame()->getAlienBases()->begin();
		_alienRace == "" && i != _game->getSavedGame()->getAlienBases()->end(); ++i)
	{
		if ((*i)->isInBattlescape())
		{
			_alienRace = (*i)->getAlienRace();
		}
	}

	deployAliens(ruleDeploy);

	if (unitCount == _save->getUnits()->size())
	{
		throw Exception("Map generator encountered an error: no alien units could be placed on the map.");
	}

	deployCivilians(ruleDeploy->getCivilians());

	_save->setAborted(false);
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
	AlienDeployment *ruleDeploy = _game->getRuleset()->getDeployment(_ufo?_ufo->getRules()->getType():_save->getMissionType());

	ruleDeploy->getDimensions(&_mapsize_x, &_mapsize_y, &_mapsize_z);

	_unitSequence = BattleUnit::MAX_SOLDIER_ID; // geoscape soldier IDs should stay below this number

	if (_terrain == 0)
	{
		if (_worldTexture == 0 || _worldTexture->getTerrain()->empty() || !ruleDeploy->getTerrains().empty())
		{
			size_t pick = RNG::generate(0, ruleDeploy->getTerrains().size() - 1);
			_terrain = _game->getRuleset()->getTerrain(ruleDeploy->getTerrains().at(pick));
		}
		else
		{
			Target *target = _ufo;
			if (_mission) target = _mission;
			_terrain = _game->getRuleset()->getTerrain(_worldTexture->getRandomTerrain(target));
		}
	}

	// new battle menu will have set the depth already
	if (_save->getDepth() == 0)
	{
		if (ruleDeploy->getMaxDepth() > 0)
		{
			_save->setDepth(RNG::generate(ruleDeploy->getMinDepth(), ruleDeploy->getMaxDepth()));
		}
		else if (_terrain->getMaxDepth() > 0)
		{
			_save->setDepth(RNG::generate(_terrain->getMinDepth(), _terrain->getMaxDepth()));
		}
	}

	if (ruleDeploy->getShade() != -1)
	{
		_worldShade = ruleDeploy->getShade();
	}

	const std::vector<MapScript*> *script = _game->getRuleset()->getMapScript(_terrain->getScript());
	if (_game->getRuleset()->getMapScript(ruleDeploy->getScript()))
	{
		script = _game->getRuleset()->getMapScript(ruleDeploy->getScript());
	}
	else if (ruleDeploy->getScript() != "")
	{
		throw Exception("Map generator encountered an error: " + ruleDeploy->getScript() + " script not found.");
	}
	if (script == 0)
	{
		throw Exception("Map generator encountered an error: " + _terrain->getScript() + " script not found.");
	}

	generateMap(script);

	setupObjectives(ruleDeploy);

	deployXCOM();

	size_t unitCount = _save->getUnits()->size();

	deployAliens(ruleDeploy);

	if (unitCount == _save->getUnits()->size())
	{
		throw Exception("Map generator encountered an error: no alien units could be placed on the map.");
	}

	deployCivilians(ruleDeploy->getCivilians());

	if (_generateFuel)
	{
		fuelPowerSources();
	}

	if (_ufo && _ufo->getStatus() == Ufo::CRASHED)
	{
		explodePowerSources();
	}

	if (!ruleDeploy->getMusic().empty())
	{
		_save->setMusic(ruleDeploy->getMusic().at(RNG::generate(0, ruleDeploy->getMusic().size()-1)));
	}
	else if (!_terrain->getMusic().empty())
	{
		_save->setMusic(_terrain->getMusic().at(RNG::generate(0, _terrain->getMusic().size()-1)));
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
			// we only add vehicles from the craft in new battle mode,
			// otherwise the base's vehicle vector will already contain these
			// due to the geoscape calling base->setupDefenses()
			if (_game->getSavedGame()->getMonthsPassed() == -1)
			{
				for (std::vector<Craft*>::iterator i = _base->getCrafts()->begin(); i != _base->getCrafts()->end(); ++i)
				{
					for (std::vector<Vehicle*>::iterator j = (*i)->getVehicles()->begin(); j != (*i)->getVehicles()->end(); ++j)
					{
						BattleUnit *unit = addXCOMVehicle(*j);
						if (unit && !_save->getSelectedUnit())
							_save->setSelectedUnit(unit);
					}
				}
			}
		}
	}

	// add soldiers that are in the craft or base
	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
	{
		if ((_craft != 0 && (*i)->getCraft() == _craft) ||
			(_craft == 0 && (*i)->getWoundRecovery() == 0 && ((*i)->getCraft() == 0 || (*i)->getCraft()->getStatus() != "STR_OUT")))
		{
			BattleUnit *unit = addXCOMUnit(new BattleUnit(*i, _save->getDepth()));
			if (unit && !_save->getSelectedUnit())
				_save->setSelectedUnit(unit);
		}
	}

	if (_save->getUnits()->empty())
	{
		throw Exception("Map generator encountered an error: no xcom units could be placed on the map.");
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
		// only use the items in the craft in new battle mode.
		if (_game->getSavedGame()->getMonthsPassed() != -1)
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
	BattleUnit *unit = addXCOMUnit(new BattleUnit(rule, FACTION_PLAYER, _unitSequence++, _game->getRuleset()->getArmor(rule->getArmor()), 0, _save->getDepth()));
	if (unit)
	{
		BattleItem *item = new BattleItem(_game->getRuleset()->getItem(vehicle), _save->getCurrentItemId());
		if (!addItem(item, unit))
		{
			delete item;
		}
		if (!v->getRules()->getCompatibleAmmo()->empty())
		{
			std::string ammo = v->getRules()->getCompatibleAmmo()->front();
			BattleItem *ammoItem = new BattleItem(_game->getRuleset()->getItem(ammo), _save->getCurrentItemId());
			addItem(ammoItem, unit);
			ammoItem->setAmmoQuantity(v->getAmmo());
		}
		unit->setTurretType(v->getRules()->getTurretType());

		if (!rule->getBuiltInWeapons().empty())
		{
			for (std::vector<std::string>::const_iterator i = rule->getBuiltInWeapons().begin(); i != rule->getBuiltInWeapons().end(); ++i)
			{
				RuleItem *ruleItem = _game->getRuleset()->getItem(*i);
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

	if ((_craft == 0 || !_craftDeployed) && !_baseInventory)
	{
		Node* node = _save->getSpawnNode(NR_XCOM, unit);
		if (node)
		{
			_save->setUnitPosition(unit, node->getPosition());
			_craftInventoryTile = _save->getTile(node->getPosition());
			unit->setDirection(RNG::generate(0,7));
			_save->getUnits()->push_back(unit);
			_save->getTileEngine()->calculateFOV(unit);
			unit->setSpecialWeapon(_save, _game->getRuleset());
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
				unit->setSpecialWeapon(_save, _game->getRuleset());
				return unit;
			}
		}
	}
	else if (_craft && !_craft->getRules()->getDeployment().empty() && !_baseInventory)
	{
		for (std::vector<std::vector<int> >::const_iterator i = _craft->getRules()->getDeployment().begin(); i != _craft->getRules()->getDeployment().end(); ++i)
		{
			Position pos = Position((*i)[0] + (_craftPos.x * 10), (*i)[1] + (_craftPos.y * 10), (*i)[2] + _craftZ);
			int dir = (*i)[3];
			bool canPlace = true;
			for (int x = 0; x < unit->getArmor()->getSize(); ++x)
			{
				for (int y = 0; y < unit->getArmor()->getSize(); ++y)
				{
					canPlace = (canPlace && canPlaceXCOMUnit(_save->getTile(pos + Position(x,y,0))));
				}
			}
			if (canPlace)
			{
				if (_save->setUnitPosition(unit, pos))
				{
					_save->getUnits()->push_back(unit);
					unit->setDirection(dir);
					unit->setSpecialWeapon(_save, _game->getRuleset());
					return unit;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < _mapsize_x * _mapsize_y * _mapsize_z; ++i)
		{
			if (canPlaceXCOMUnit(_save->getTiles()[i]))
			{
				if (_save->setUnitPosition(unit, _save->getTiles()[i]->getPosition()))
				{
					_save->getUnits()->push_back(unit);
					unit->setSpecialWeapon(_save, _game->getRuleset());
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
		tile->getMapData(O_FLOOR) &&
		tile->getMapData(O_FLOOR)->getSpecialType() == START_POINT &&
		!tile->getMapData(O_OBJECT) &&
		tile->getMapData(O_FLOOR)->getTUCost(MT_WALK) < 255)
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
void BattlescapeGenerator::deployAliens(AlienDeployment *deployment)
{
	if (deployment->getRace() != "" && _alienRace == "")
	{
		_alienRace = deployment->getRace();
	}

	if (_save->getDepth() > 0 && _alienRace.find("_UNDERWATER") == std::string::npos)
	{
		_alienRace = _alienRace + "_UNDERWATER";
	}

	if (_game->getRuleset()->getAlienRace(_alienRace) == 0)
	{
		throw Exception("Map generator encountered an error: Unknown race: " + _alienRace + " defined in deployment: " + deployment->getType());
	}

	AlienRace *race = _game->getRuleset()->getAlienRace(_alienRace);

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

		quantity += RNG::generate(0, (*d).extraQty);

		for (int i = 0; i < quantity; ++i)
		{
			bool outside = RNG::generate(0,99) < (*d).percentageOutsideUfo;
			if (_ufo == 0)
				outside = false;
			Unit *rule = _game->getRuleset()->getUnit(alienName);
			BattleUnit *unit = addAlien(rule, (*d).alienRank, outside);
			size_t itemLevel = (size_t)(_game->getRuleset()->getAlienItemLevels().at(month).at(RNG::generate(0,9)));
			if (unit)
			{
				// Built in weapons: the unit has this weapon regardless of loadout or what have you.
				if (!rule->getBuiltInWeapons().empty())
				{
					for (std::vector<std::string>::const_iterator j = rule->getBuiltInWeapons().begin(); j != rule->getBuiltInWeapons().end(); ++j)
					{
						RuleItem *ruleItem = _game->getRuleset()->getItem(*j);
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
					if ((*d).itemSets.size() == 0)
					{
						throw Exception("Unit generator encountered an error: item set not defined");
					}
					if (itemLevel >= (*d).itemSets.size())
					{
						itemLevel = (*d).itemSets.size() - 1;
					}
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
	BattleUnit *unit = new BattleUnit(rules, FACTION_HOSTILE, _unitSequence++, _game->getRuleset()->getArmor(rules->getArmor()), difficulty, _save->getDepth());
	Node *node = 0;

	/* following data is the order in which certain alien ranks spawn on certain node ranks */
	/* note that they all can fall back to rank 0 nodes - which is scout (outside ufo) */

	for (int i = 0; i < 7 && node == 0; ++i)
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
		unit->setSpecialWeapon(_save, _game->getRuleset());
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
		// ASSASSINATION CHALLENGE SPECIAL: screw the player, just because we didn't find a node,
		// doesn't mean we can't ruin Tornis's day: spawn as many aliens as possible.
		if (_game->getSavedGame()->getDifficulty() >= DIFF_SUPERHUMAN && placeUnitNearFriend(unit))
		{
			unit->setAIState(new AlienBAIState(_game->getSavedGame()->getSavedBattle(), unit, 0));
			unit->setRankInt(alienRank);
			unit->setSpecialWeapon(_save, _game->getRuleset());
			int dir = _save->getTileEngine()->faceWindow(unit->getPosition());
			Position craft = _game->getSavedGame()->getSavedBattle()->getUnits()->at(0)->getPosition();
			if (_save->getTileEngine()->distance(unit->getPosition(), craft) <= 20 && RNG::percent(20 * difficulty))
				dir = unit->directionTo(craft);
			if (dir != -1)
				unit->setDirection(dir);
			else
				unit->setDirection(RNG::generate(0,7));

			_save->getUnits()->push_back(unit);
		}
		else
		{
			delete unit;
			unit = 0;
		}
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
	BattleUnit *unit = new BattleUnit(rules, FACTION_NEUTRAL, _unitSequence++, _game->getRuleset()->getArmor(rules->getArmor()), 0, _save->getDepth());
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
	RuleInventory *leftHand = _game->getRuleset()->getInventory("STR_LEFT_HAND");
	bool placed = false;
	bool loaded = false;
	BattleItem *rightWeapon = unit->getItem("STR_RIGHT_HAND");
	BattleItem *leftWeapon = unit->getItem("STR_LEFT_HAND");
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
	bool keep = true;
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
			if (!rightWeapon && unit->getBaseStats()->strength * 0.66 >= weight) // weight is always considered 0 for aliens
			{
				item->moveToOwner(unit);
				item->setSlot(rightHand);
				placed = true;
			}
			if (!placed && !leftWeapon && (unit->getFaction() != FACTION_PLAYER || item->getRules()->isFixed()))
			{
				item->moveToOwner(unit);
				item->setSlot(leftHand);
				placed = true;
			}
		}
		break;
	case BT_AMMO:
		// xcom weapons will already be loaded, aliens and tanks, however, get their ammo added afterwards.
		// so let's try to load them here.
		if (rightWeapon && (rightWeapon->getRules()->isFixed() || unit->getFaction() != FACTION_PLAYER) &&
			!rightWeapon->getRules()->getCompatibleAmmo()->empty() &&
			!rightWeapon->getAmmoItem() &&
			rightWeapon->setAmmoItem(item) == 0)
		{
			item->setSlot(rightHand);
			placed = true;
			break;
		}
		if (leftWeapon && (leftWeapon->getRules()->isFixed() || unit->getFaction() != FACTION_PLAYER) &&
			!leftWeapon->getRules()->getCompatibleAmmo()->empty() &&
			!leftWeapon->getAmmoItem() &&
			leftWeapon->setAmmoItem(item) == 0)
		{
			item->setSlot(leftHand);
			placed = true;
			break;
		}
		// don't take ammo for weapons we don't have.
		keep = (unit->getFaction() != FACTION_PLAYER);
		if (rightWeapon)
		{
			for (std::vector<std::string>::iterator i = rightWeapon->getRules()->getCompatibleAmmo()->begin(); i != rightWeapon->getRules()->getCompatibleAmmo()->end(); ++i)
			{
				if (*i == item->getRules()->getType())
				{
					keep = true;
					break;
				}
			}
		}
		if (leftWeapon)
		{
			for (std::vector<std::string>::iterator i = leftWeapon->getRules()->getCompatibleAmmo()->begin(); i != leftWeapon->getRules()->getCompatibleAmmo()->end(); ++i)
			{
				if (*i == item->getRules()->getType())
				{
					keep = true;
					break;
				}
			}
		}
		if (!keep)
		{
			break;
		}
	default:
		if ((unit->getGeoscapeSoldier() == 0 || _allowAutoLoadout))
		{
			if (unit->getBaseStats()->strength >= weight) // weight is always considered 0 for aliens
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
	std::ifstream mapFile (FileMap::getFilePath(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw Exception(filename.str() + " not found");
	}

	mapFile.read((char*)&size, sizeof(size));
	sizey = (int)size[0];
	sizex = (int)size[1];
	sizez = (int)size[2];

	mapblock->setSizeZ(sizez);

	std::stringstream ss;
	if (sizez > _save->getMapSizeZ())
	{
		ss <<"Height of map " + filename.str() + " too big for this mission, block is " << sizez << ", expected: " << _save->getMapSizeZ();
		throw Exception(ss.str());
	}

	if (sizex != mapblock->getSizeX() ||
		sizey != mapblock->getSizeY())
	{
		ss <<"Map block is not of the size specified " + filename.str() + " is " << sizex << "x" << sizey << " , expected: " << mapblock->getSizeX() << "x" << mapblock->getSizeY();
		throw Exception(ss.str());
	}

	z += sizez - 1;

	for (int i = _mapsize_z-1; i >0; i--)
	{
		// check if there is already a layer - if so, we have to move Z up
		MapData *floor = _save->getTile(Position(x, y, i))->getMapData(O_FLOOR);
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
		throw Exception("Something is wrong in your map definitions, craft/ufo map is too tall?");
	}

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		for (int part = 0; part < 4; ++part)
		{
			terrainObjectID = ((unsigned char)value[part]);
			if (terrainObjectID>0)
			{
				int mapDataSetID = mapDataSetOffset;
				unsigned int mapDataID = terrainObjectID;
				MapData *md = terrain->getMapData(&mapDataID, &mapDataSetID);
				if (mapDataSetOffset > 0) // ie: ufo or craft.
				{
					_save->getTile(Position(x, y, z))->setMapData(0, -1, -1, 3);
				}
				_save->getTile(Position(x, y, z))->setMapData(md, mapDataID, mapDataSetID, part);
			}
		}

		_save->getTile(Position(x, y, z))->setDiscovered((discovered || mapblock->isFloorRevealed(z)), 2);

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
		throw Exception("Invalid MAP file: " + filename.str());
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
	unsigned char value[24];
	std::ostringstream filename;
	filename << "ROUTES/" << mapblock->getName() << ".RMP";

	// Load file
	std::ifstream mapFile (FileMap::getFilePath(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw Exception(filename.str() + " not found");
	}

	size_t nodeOffset = _save->getNodes()->size();

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		int pos_x = value[1];
		int pos_y = value[0];
		int pos_z = value[2];
		if (pos_x < mapblock->getSizeX() && pos_y < mapblock->getSizeY() && pos_z < _mapsize_z)
		{
			Position pos = Position(xoff + pos_x, yoff + pos_y, mapblock->getSizeZ() - 1 - pos_z);
			int type     = value[19];
			int rank     = value[20];
			int flags    = value[21];
			int reserved = value[22];
			int priority = value[23];
			Node *node = new Node(_save->getNodes()->size(), pos, segment, type, rank, flags, reserved, priority);
			for (int j = 0; j < 5; ++j)
			{
				int connectID = value[4 + j * 3];
				// don't touch special values
				if (connectID <= 250)
				{
					connectID += nodeOffset;
				}
				// 255/-1 = unused, 254/-2 = north, 253/-3 = east, 252/-4 = south, 251/-5 = west
				else
				{
					connectID -= 256;
				}
				node->getNodeLinks()->push_back(connectID);
			}
			_save->getNodes()->push_back(node);
		}
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
		if (_save->getTiles()[i]->getMapData(O_OBJECT)
			&& _save->getTiles()[i]->getMapData(O_OBJECT)->getSpecialType() == UFO_POWER_SOURCE)
		{
			BattleItem *alienFuel = new BattleItem(_game->getRuleset()->getItem(_game->getRuleset()->getAlienFuelName()), _save->getCurrentItemId());
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
		if (_save->getTiles()[i]->getMapData(O_OBJECT)
			&& _save->getTiles()[i]->getMapData(O_OBJECT)->getSpecialType() == UFO_POWER_SOURCE && RNG::percent(75))
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
 * Spawns civilians on a terror mission.
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
	if (_save->getUnits()->empty())
	{
		return false;
	}
	for (int i = 0; i != 10; ++i)
	{
		Position entryPoint = Position(-1, -1, -1);
		int tries = 100;
		bool largeUnit = false;
		while (entryPoint == Position(-1, -1, -1) && tries)
		{
			BattleUnit* k = _save->getUnits()->at(RNG::generate(0, _save->getUnits()->size()-1));
			if (k->getFaction() == unit->getFaction() && k->getPosition() != Position(-1, -1, -1) && k->getArmor()->getSize() >= unit->getArmor()->getSize())
			{
				entryPoint = k->getPosition();
				largeUnit = (k->getArmor()->getSize() != 1);
			}
			--tries;
		}
		if (tries && _save->placeUnitNearPosition(unit, entryPoint, largeUnit))
		{
			return true;
		}
	}
	return false;
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
		tile->setMapData(data, 0, 0, O_FLOOR);
		tile->getMapData(O_FLOOR)->setSpecialType(START_POINT, 0);
		tile->getMapData(O_FLOOR)->setTUWalk(0);
		tile->getMapData(O_FLOOR)->setFlags(false, false, false, 0, false, false, false, false, false);
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

/**
 * Generates a map (set of tiles) for a new battlescape game.
 * @param script the script to use to build the map.
 */
void BattlescapeGenerator::generateMap(const std::vector<MapScript*> *script)
{
	// set our ambient sound
	_save->setAmbientSound(_terrain->getAmbience());

	// set up our map generation vars
	_dummy = new MapBlock("dummy");

	init();

	MapBlock* craftMap = 0;
	MapBlock* ufoMap = 0;

	int mapDataSetIDOffset = 0;
	int craftDataSetIDOffset = 0;

	// create an array to track command success/failure
	std::map<int, bool> conditionals;

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

	// lets generate the map now and store it inside the tile objects

	// this mission type is "hard-coded" in terms of map layout
	if (_save->getMissionType() == "STR_BASE_DEFENSE")
	{
		generateBaseMap();
	}

	//process script
	for (std::vector<MapScript*>::const_iterator i = script->begin(); i != script->end(); ++i)
	{
		MapScript *command = *i;

		if (command->getLabel() > 0 && conditionals.find(command->getLabel()) != conditionals.end())
		{
			throw Exception("Map generator encountered an error: multiple commands are sharing the same label.");
		}
		bool &success = conditionals[command->getLabel()] = false;


		// if this command runs conditionally on the failures or successes of previous commands
		if (!command->getConditionals()->empty())
		{
			bool execute = true;
			// compare the corresponding entries in the success/failure vector
			for (std::vector<int>::const_iterator condition = command->getConditionals()->begin(); condition != command->getConditionals()->end(); ++condition)
			{
				// positive numbers indicate conditional on success, negative means conditional on failure
				// ie: [1, -2] means this command only runs if command 1 succeeded and command 2 failed.
				if (conditionals.find(std::abs(*condition)) != conditionals.end())
				{
					if ((*condition > 0 && !conditionals[*condition]) || (*condition < 0 && conditionals[std::abs(*condition)]))
					{
						execute = false;
						break;
					}
				}
				else
				{
					throw Exception("Map generator encountered an error: conditional command expected a label that did not exist before this command.");
				}
			}
			if (!execute)
			{
				continue;
			}
		}

		// if there's a chance a command won't execute by design, take that into account here.
		if (RNG::percent(command->getChancesOfExecution()))
		{
			// initialize the block selection arrays
			command->init();

			// each command can be attempted multiple times, as randomization within the rects may occur
			for (int j = 0; j < command->getExecutions(); ++j)
			{
				int x, y;
				MapBlock *block = 0;
				switch (command->getType())
				{
				case MSC_ADDBLOCK:
					block = command->getNextBlock(_terrain);
					// select an X and Y position from within the rects, using an even distribution
					if (block && selectPosition(command->getRects(), x, y, block->getSizeX(), block->getSizeY()))
					{
						success = addBlock(x, y, block) || success;
					}
					break;
				case MSC_ADDLINE:
					success = addLine((MapDirection)(command->getDirection()), command->getRects());
					break;
				case MSC_ADDCRAFT:
					if (_craft)
					{
						craftMap = _craft->getRules()->getBattlescapeTerrainData()->getRandomMapBlock(999, 999, 0, false);
						if (addCraft(craftMap, command, _craftPos))
						{
							// by default addCraft adds blocks from group 1.
							// this can be overwritten in the command by defining specific groups or blocks
							// or this behaviour can be suppressed by leaving group 1 empty
							// this is intentional to allow for TFTD's cruise liners/etc
							// in this situation, you can end up with ANYTHING under your craft, so be careful
							for (x = _craftPos.x; x < _craftPos.x + _craftPos.w; ++x)
							{
								for (y = _craftPos.y; y < _craftPos.y + _craftPos.h; ++y)
								{
									if (_blocks[x][y])
									{
										loadMAP(_blocks[x][y], x * 10, y * 10, _terrain, 0);
									}
								}
							}
							_craftDeployed = true;
							success = true;
						}
					}
					break;
				case MSC_ADDUFO:
					// as above, note that the craft and the ufo will never be allowed to overlap.
					// TODO: make _ufopos a vector ;)
					if (_ufo)
					{
						ufoMap = _ufo->getRules()->getBattlescapeTerrainData()->getRandomMapBlock(999, 999, 0, false);
						if (addCraft(ufoMap, command, _ufoPos))
						{
							for (x = _ufoPos.x; x < _ufoPos.x + _ufoPos.w; ++x)
							{
								for (y = _ufoPos.y; y < _ufoPos.y + _ufoPos.h; ++y)
								{
									if (_blocks[x][y])
									{
										loadMAP(_blocks[x][y], x * 10, y * 10, _terrain, 0);
									}
								}
							}
							success = true;
						}
					}
					break;
				case MSC_DIGTUNNEL:
					drillModules(command->getTunnelData(), command->getRects(), command->getDirection());
					success = true; // this command is fail-proof
					break;
				case MSC_FILLAREA:
					block = command->getNextBlock(_terrain);
					while (block)
					{
						if (selectPosition(command->getRects(), x, y, block->getSizeX(), block->getSizeY()))
						{
							// fill area will succeed if even one block is added
							success = addBlock(x, y, block) || success;
						}
						else
						{
							break;
						}
						block = command->getNextBlock(_terrain);
					}
					break;
				case MSC_CHECKBLOCK:
					for (std::vector<SDL_Rect*>::const_iterator k = command->getRects()->begin(); k != command->getRects()->end() && !success; ++k)
					{
						for (x = (*k)->x; x != (*k)->x + (*k)->w && x != _mapsize_x / 10 && !success; ++x)
						{
							for (y = (*k)->y; y != (*k)->y + (*k)->h && y != _mapsize_y / 10 && !success; ++y)
							{
								if (!command->getGroups()->empty())
								{
									for (std::vector<int>::const_iterator z = command->getGroups()->begin(); z != command->getGroups()->end() && !success; ++z)
									{
										success = _blocks[x][y]->isInGroup((*z));
									}
								}
								else if (!command->getBlocks()->empty())
								{
									for (std::vector<int>::const_iterator z = command->getBlocks()->begin(); z != command->getBlocks()->end() && !success; ++z)
									{
										if ((size_t)(*z) < _terrain->getMapBlocks()->size())
										{
											success = (_blocks[x][y] == _terrain->getMapBlocks()->at(*z));
										}
									}
								}
								else
								{
									// wildcard, we don't care what block it is, we just wanna know if there's a block here
									success = (_blocks[x][y] != 0);
								}
							}
						}
					}
					break;
				case MSC_REMOVE:
					success = removeBlocks(command);
					break;
				case MSC_RESIZE:
					if (_save->getMissionType() == "STR_BASE_DEFENSE")
					{
						throw Exception("Map Generator encountered an error: Base defense map cannot be resized.");
					}
					if (_blocksToDo < (_mapsize_x / 10) * (_mapsize_y / 10))
					{
						throw Exception("Map Generator encountered an error: One does not simply resize the map after adding blocks.");
					}

					if (command->getSizeX() > 0 && command->getSizeX() != _mapsize_x / 10)
					{
						_mapsize_x = command->getSizeX() * 10;
					}
					if (command->getSizeY() > 0 && command->getSizeY() != _mapsize_y / 10)
					{
						_mapsize_y = command->getSizeY() * 10;
					}
					if (command->getSizeZ() > 0 && command->getSizeZ() != _mapsize_z)
					{
						_mapsize_z = command->getSizeZ();
					}
					init();
					break;
				default:
					break;
				}
			}
		}
	}

	if (_blocksToDo)
	{
		throw Exception("Map failed to fully generate.");
	}

	loadNodes();

	if (ufoMap)
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
		// TODO: put ufo positions in a vector rather than a single rect, and iterate here?
		// will probably need to make ufomap a vector too i suppose.
		loadMAP(ufoMap, _ufoPos.x * 10, _ufoPos.y * 10, _ufo->getRules()->getBattlescapeTerrainData(), mapDataSetIDOffset);
		loadRMP(ufoMap, _ufoPos.x * 10, _ufoPos.y * 10, Node::UFOSEGMENT);
		for (int i = 0; i < ufoMap->getSizeX() / 10; ++i)
		{
			for (int j = 0; j < ufoMap->getSizeY() / 10; j++)
			{
				_segments[_ufoPos.x + i][_ufoPos.y + j] = Node::UFOSEGMENT;
			}
		}
	}

	if (craftMap)
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
		loadMAP(craftMap, _craftPos.x * 10, _craftPos.y * 10, _craft->getRules()->getBattlescapeTerrainData(), mapDataSetIDOffset + craftDataSetIDOffset, true, true);
		loadRMP(craftMap, _craftPos.x * 10, _craftPos.y * 10, Node::CRAFTSEGMENT);
		for (int i = 0; i < craftMap->getSizeX() / 10; ++i)
		{
			for (int j = 0; j < craftMap->getSizeY() / 10; j++)
			{
				_segments[_craftPos.x + i][_craftPos.y + j] = Node::CRAFTSEGMENT;
			}
		}
		for (int i = (_craftPos.x * 10) - 1; i <= (_craftPos.x * 10) + craftMap->getSizeX(); ++i)
		{
			for (int j = (_craftPos.y * 10) - 1; j <= (_craftPos.y * 10) + craftMap->getSizeY(); j++)
			{
				for (int k = _mapsize_z-1; k >= _craftZ; --k)
				{
					if (_save->getTile(Position(i,j,k)))
					{
						_save->getTile(Position(i,j,k))->setDiscovered(true, 2);
					}
				}
			}
		}
	}

	delete _dummy;

	attachNodeLinks();
}

/**
 * Generates a map based on the base's layout.
 * this doesn't drill or fill with dirt, the script must do that.
 */
void BattlescapeGenerator::generateBaseMap()
{
	// add modules based on the base's layout
	for (std::vector<BaseFacility*>::const_iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); ++i)
	{
		if ((*i)->getBuildTime() == 0)
		{
			int num = 0;
			int xLimit = (*i)->getX() + (*i)->getRules()->getSize() -1;
			int yLimit = (*i)->getY() + (*i)->getRules()->getSize() -1;

			for (int y = (*i)->getY(); y <= yLimit; ++y)
			{
				for (int x = (*i)->getX(); x <= xLimit; ++x)
				{
					// lots of crazy stuff here, which is for the hangars (or other large base facilities one may create)
					// TODO: clean this mess up, make the mapNames a vector in the base module defs
					// also figure out how to do the terrain sets on a per-block basis.
					std::string mapname = (*i)->getRules()->getMapName();
					std::ostringstream newname;
					newname << mapname.substr(0, mapname.size()-2); // strip of last 2 digits
					int mapnum = atoi(mapname.substr(mapname.size()-2, 2).c_str()); // get number
					mapnum += num;
					if (mapnum < 10) newname << 0;
					newname << mapnum;
					addBlock(x, y, _terrain->getMapBlock(newname.str()));
					_drillMap[x][y] = MD_NONE;
					num++;
					if ((*i)->getRules()->getStorage() > 0)
					{
						int groundLevel;
						for (groundLevel = _mapsize_z -1; groundLevel >= 0; --groundLevel)
						{
							if (!_save->getTile(Position(x*10, y*10, groundLevel))->hasNoFloor(0))

								break;
						}
						// general stores - there is where the items are put
						for (int k = x * 10; k != (x + 1) * 10; ++k)
						{
							for (int l = y * 10; l != (y + 1) * 10; ++l)
							{
								// we only want every other tile, giving us a "checkerboard" pattern
								if ((k+l) % 2 == 0)
								{
									Tile *t = _save->getTile(Position(k,l,groundLevel));
									Tile *tEast = _save->getTile(Position(k+1,l,groundLevel));
									Tile *tSouth = _save->getTile(Position(k,l+1,groundLevel));
									if (t && t->getMapData(O_FLOOR) && !t->getMapData(O_OBJECT) &&
										tEast && !tEast->getMapData(O_WESTWALL) &&
										tSouth && !tSouth->getMapData(O_NORTHWALL))
									{
										_save->getStorageSpace().push_back(Position(k, l, groundLevel));
									}
								}
							}
						}
						// let's put the inventory tile on the lower floor, just to be safe.
						if (!_craftInventoryTile)
						{
							_craftInventoryTile = _save->getTile(Position((x*10)+5,(y*10)+5,groundLevel-1));
						}
					}
				}
			}
			for (int x = (*i)->getX(); x <= xLimit; ++x)
			{
				_drillMap[x][yLimit] = MD_VERTICAL;
			}
			for (int y = (*i)->getY(); y <= yLimit; ++y)
			{
				_drillMap[xLimit][y] = MD_HORIZONTAL;
			}
			_drillMap[xLimit][yLimit] = MD_BOTH;
		}
	}
	_save->calculateModuleMap();
}

/**
 * Clears a module from the map.
 * @param x the x offset.
 * @param y the y offset.
 * @param sizeX how far along the x axis to clear.
 * @param sizeY how far along the y axis to clear.
 */
void BattlescapeGenerator::clearModule(int x, int y, int sizeX, int sizeY)
{
	for (int z = 0; z != _mapsize_z; ++z)
	{
		for (int dx = x; dx != x + sizeX; ++dx)
		{
			for (int dy = y; dy != y + sizeY; ++dy)
			{
				Tile *tile = _save->getTile(Position(dx,dy,z));
				for (int i = 0; i < 4; ++i)
					tile->setMapData(0, -1, -1, i);
			}
		}
	}
}

/**
 * Loads all the nodes from the map modules.
 */
void BattlescapeGenerator::loadNodes()
{
	int segment = 0;
	for (int itY = 0; itY < (_mapsize_y / 10); itY++)
	{
		for (int itX = 0; itX < (_mapsize_x / 10); itX++)
		{
			_segments[itX][itY] = segment;
			if (_blocks[itX][itY] != 0 && _blocks[itX][itY] != _dummy)
			{
				if (!(_blocks[itX][itY]->isInGroup(MT_LANDINGZONE) && _landingzone[itX][itY]))
				{
					loadRMP(_blocks[itX][itY], itX * 10, itY * 10, segment++);
				}
			}
		}
	}
}

/**
 * Attaches all the nodes together in an intricate web of lies.
 */
void BattlescapeGenerator::attachNodeLinks()
{
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
			neighbourSegments[0] = _segments[segmentX+1][segmentY];
		if (segmentY == (_mapsize_y / 10)-1)
			neighbourSegments[1] = -1;
		else
			neighbourSegments[1] = _segments[segmentX][segmentY+1];
		if (segmentX == 0)
			neighbourSegments[2] = -1;
		else
			neighbourSegments[2] = _segments[segmentX-1][segmentY];
		if (segmentY == 0)
			neighbourSegments[3] = -1;
		else
			neighbourSegments[3] = _segments[segmentX][segmentY-1];

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
}

/**
 * Selects a position for a map block.
 * @param rects the positions to select from, none meaning the whole map.
 * @param X the x position for the block gets stored in this variable.
 * @param Y the y position for the block gets stored in this variable.
 * @param sizeX the x size of the block we want to add.
 * @param sizeY the y size of the block we want to add.
 * @return if a valid position was selected or not.
 */
bool BattlescapeGenerator::selectPosition(const std::vector<SDL_Rect *> *rects, int &X, int &Y, int sizeX, int sizeY)
{
	std::vector<SDL_Rect*> available;
	std::vector<std::pair<int, int> > valid;
	SDL_Rect wholeMap;
	wholeMap.x = 0;
	wholeMap.y = 0;
	wholeMap.w = (_mapsize_x / 10);
	wholeMap.h = (_mapsize_y / 10);
	sizeX = (sizeX / 10);
	sizeY = (sizeY / 10);
	if (rects->empty())
	{
		available.push_back(&wholeMap);
	}
	else
	{
		available = *rects;
	}
	for (std::vector<SDL_Rect*>::const_iterator i = available.begin(); i != available.end(); ++i)
	{
		if (sizeX > (*i)->w || sizeY > (*i)->h)
		{
			continue;
		}
		for (int x = (*i)->x; x + sizeX <= (*i)->x + (*i)->w && x + sizeX <= wholeMap.w; ++x)
		{
			for (int y = (*i)->y; y + sizeY <= (*i)->y + (*i)->h && y + sizeY <= wholeMap.h; ++y)
			{
				if (std::find(valid.begin(), valid.end(), std::make_pair(x,y)) == valid.end())
				{
					bool add = true;
					for (int xCheck = x; xCheck != x + sizeX; ++xCheck)
					{
						for (int yCheck = y; yCheck != y + sizeY; ++yCheck)
						{
							if (_blocks[xCheck][yCheck])
							{
								add = false;
							}
						}
					}
					if (add)
					{
						valid.push_back(std::make_pair(x,y));
					}
				}
			}
		}
	}
	if (valid.empty())
	{
		return false;
	}
	std::pair<int, int> selection = valid.at(RNG::generate(0, valid.size() - 1));
	X = selection.first;
	Y = selection.second;
	return true;
}

/**
 * Adds a craft (or UFO) to the map, and tries to add a landing zone type block underneath it.
 * @param craftMap the map for the craft in question.
 * @param command the script command to pull info from.
 * @param craftPos the position of the craft is stored here.
 * @return if the craft was placed or not.
 */
bool BattlescapeGenerator::addCraft(MapBlock *craftMap, MapScript *command, SDL_Rect &craftPos)
{
	craftPos.w = craftMap->getSizeX();
	craftPos.h = craftMap->getSizeY();
	bool placed = false;
	int x, y;

	placed = selectPosition(command->getRects(), x, y, craftPos.w, craftPos.h);
	// if ok, allocate it
	if (placed)
	{
		craftPos.x = x;
		craftPos.y = y;
		craftPos.w /= 10;
		craftPos.h /= 10;
		for (x = 0; x < craftPos.w; ++x)
		{
			for (y = 0; y < craftPos.h; ++y)
			{
				_landingzone[craftPos.x + x][craftPos.y + y] = true;
				MapBlock *block = command->getNextBlock(_terrain);
				if (block && !_blocks[craftPos.x + x][craftPos.y + y])
				{
					_blocks[craftPos.x + x][craftPos.y + y] = block;
					_blocksToDo--;
				}
			}
		}
	}

	return placed;
}

/**
 * draws a line along the map either horizontally, vertically or both.
 * @param direction the direction to draw the line
 * @param rects the positions to allow the line to be drawn in.
 * @return if the blocks were added or not.
 */
bool BattlescapeGenerator::addLine(MapDirection direction, const std::vector<SDL_Rect*> *rects)
{
	if (direction == MD_BOTH)
	{
		if (addLine(MD_VERTICAL, rects))
		{
			addLine(MD_HORIZONTAL, rects);
			return true;
		}
		return false;
	}

	int tries = 0;
	bool placed = false;

	int roadX, roadY;
	int *iteratorValue = &roadX;
	MapBlockType comparator = MT_NSROAD;
	MapBlockType typeToAdd = MT_EWROAD;
	int limit = _mapsize_x / 10;
	if (direction == MD_VERTICAL)
	{
		iteratorValue = &roadY;
		comparator = MT_EWROAD;
		typeToAdd = MT_NSROAD;
		limit = _mapsize_y / 10;
	}
	while (!placed)
	{
		selectPosition(rects, roadX, roadY, 10, 10);
		placed = true;
		for (*iteratorValue = 0; *iteratorValue < limit; *iteratorValue += 1)
		{
			if (_blocks[roadX][roadY] != 0 && _blocks[roadX][roadY]->isInGroup(comparator) == false)
			{
				placed = false;
				break;
			}
		}
		if (tries++ > 20)
		{
			return false;
		}
	}
	*iteratorValue = 0;
	while (*iteratorValue < limit)
	{
		if (_blocks[roadX][roadY] == 0)
		{
			addBlock(roadX, roadY, _terrain->getRandomMapBlock(10, 10, typeToAdd));
		}
		else if (_blocks[roadX][roadY]->isInGroup(comparator))
		{
			_blocks[roadX][roadY] = _terrain->getRandomMapBlock(10, 10, MT_CROSSING);
			clearModule(roadX * 10, roadY * 10, 10, 10);
			loadMAP(_blocks[roadX][roadY], roadX * 10, roadY * 10, _terrain, 0);
		}
		*iteratorValue += 1;
	}
	return true;
}

/**
 * Adds a single block to the map.
 * @param x the x position to add the block
 * @param y the y position to add the block
 * @param block the block to add.
 * @return if the block was added or not.
 */
bool BattlescapeGenerator::addBlock(int x, int y, MapBlock *block)
{
	int xSize = (block->getSizeX() - 1) / 10;
	int ySize = (block->getSizeY() - 1) / 10;

	for (int xd = 0; xd <= xSize; ++xd)
	{
		for (int yd = 0; yd != ySize; ++yd)
		{
			if (_blocks[x + xd][y + yd])
				return false;
		}
	}

	for (int xd = 0; xd <= xSize; ++xd)
	{
		for (int yd = 0; yd <= ySize; ++yd)
		{
			_blocks[x + xd][y + yd] = _dummy;
			_blocksToDo--;
		}
	}

	// mark the south edge of the block for drilling
	for (int xd = 0; xd <= xSize; ++xd)
	{
		_drillMap[x+xd][y + ySize] = MD_VERTICAL;
	}
	// then the east edge
	for (int yd = 0; yd <= ySize; ++yd)
	{
		_drillMap[x + xSize][y+yd] = MD_HORIZONTAL;
	}
	// then the far corner gets marked for both
	// this also marks 1x1 modules
	_drillMap[x + xSize][y+ySize] = MD_BOTH;

	_blocks[x][y] = block;
	bool visible = (_save->getMissionType() == "STR_BASE_DEFENSE"); // yes, i'm hard coding these, big whoop, wanna fight about it?

	loadMAP(_blocks[x][y], x * 10, y * 10, _terrain, 0, visible);
	return true;
}

/**
 * Drills a tunnel between existing map modules.
 * note that this drills all modules currently on the map,
 * so it should take place BEFORE the dirt is added in base defenses.
 * @param data the wall replacements and level to dig on.
 * @param rects the length/width of the tunnels themselves.
 * @param dir the direction to drill.
 */
void BattlescapeGenerator::drillModules(TunnelData* data, const std::vector<SDL_Rect *> *rects, MapDirection dir)
{
	const MCDReplacement *wWall = data->getMCDReplacement("westWall");
	const MCDReplacement *nWall = data->getMCDReplacement("northWall");
	const MCDReplacement *corner = data->getMCDReplacement("corner");
	const MCDReplacement *floor = data->getMCDReplacement("floor");
	SDL_Rect rect;
	rect.x = rect.y = rect.w = rect.h = 3;
	if (!rects->empty())
	{
		rect = *rects->front();
	}

	for (int i = 0; i < (_mapsize_x / 10); ++i)
	{
		for (int j = 0; j < (_mapsize_y / 10); ++j)
		{
			if (_blocks[i][j] == 0)
				continue;
			Tile *tile;
			MapData *md;

			if (dir != MD_VERTICAL)
			{
				// drill east
				if (i < (_mapsize_x / 10)-1 && (_drillMap[i][j] == MD_HORIZONTAL || _drillMap[i][j] == MD_BOTH) && _blocks[i+1][j] != 0)
				{
					// remove stuff
					for (int k = rect.y; k != rect.y + rect.h; ++k)
					{
						tile = _save->getTile(Position((i*10)+9, (j*10)+k, data->level));
						if (tile)
						{
							tile->setMapData(0, -1, -1, O_WESTWALL);
							tile->setMapData(0, -1, -1, O_OBJECT);
							if (floor)
							{
								md = _terrain->getMapDataSets()->at(floor->set)->getObjects()->at(floor->entry);
								tile->setMapData(md, floor->entry, floor->set, O_FLOOR);
							}

							tile = _save->getTile(Position((i+1)*10, (j*10)+k, data->level));
							tile->setMapData(0, -1, -1, O_WESTWALL);
							MapData* obj = tile->getMapData(O_OBJECT);
							if (obj && obj->getTUCost(MT_WALK) == 0)
							{
								tile->setMapData(0, -1, -1, O_OBJECT);
							}
						}
					}

					if (nWall)
					{
						md = _terrain->getMapDataSets()->at(nWall->set)->getObjects()->at(nWall->entry);
						tile = _save->getTile(Position((i*10)+9, (j*10)+rect.y, data->level));
						tile->setMapData(md, nWall->entry, nWall->set, O_NORTHWALL);
						tile = _save->getTile(Position((i*10)+9, (j*10)+rect.y+rect.h, data->level));
						tile->setMapData(md, nWall->entry, nWall->set, O_NORTHWALL);
					}

					if (corner)
					{
						md = _terrain->getMapDataSets()->at(corner->set)->getObjects()->at(corner->entry);
						tile = _save->getTile(Position((i+1)*10, (j*10)+rect.y, data->level));
						if (tile->getMapData(O_NORTHWALL) == 0)
							tile->setMapData(md, corner->entry, corner->set, O_NORTHWALL);
					}
				}
			}

			if (dir != MD_HORIZONTAL)
			{
				// drill south
				if (j < (_mapsize_y / 10)-1 && (_drillMap[i][j] == MD_VERTICAL || _drillMap[i][j] == MD_BOTH) && _blocks[i][j+1] != 0)
				{
					// remove stuff
					for (int k = rect.x; k != rect.x + rect.w; ++k)
					{
						Tile * tile = _save->getTile(Position((i*10)+k, (j*10)+9, data->level));
						if (tile)
						{
							tile->setMapData(0, -1, -1, O_NORTHWALL);
							tile->setMapData(0, -1, -1, O_OBJECT);
							if (floor)
							{
								md = _terrain->getMapDataSets()->at(floor->set)->getObjects()->at(floor->entry);
								tile->setMapData(md, floor->entry, floor->set, O_FLOOR);
							}

							tile = _save->getTile(Position((i*10)+k, (j+1)*10, data->level));
							tile->setMapData(0, -1, -1, O_NORTHWALL);
							MapData* obj = tile->getMapData(O_OBJECT);
							if (obj && obj->getTUCost(MT_WALK) == 0)
							{
								tile->setMapData(0, -1, -1, O_OBJECT);
							}
						}
					}

					if (wWall)
					{
						md = _terrain->getMapDataSets()->at(wWall->set)->getObjects()->at(wWall->entry);
						tile = _save->getTile(Position((i*10)+rect.x, (j*10)+9, data->level));
						tile->setMapData(md, wWall->entry, wWall->set, O_WESTWALL);
						tile = _save->getTile(Position((i*10)+rect.x+rect.w, (j*10)+9, data->level));
						tile->setMapData(md, wWall->entry, wWall->set, O_WESTWALL);
					}

					if (corner)
					{
						md = _terrain->getMapDataSets()->at(corner->set)->getObjects()->at(corner->entry);
						tile = _save->getTile(Position((i*10)+rect.x, (j+1)*10, data->level));
						if (tile->getMapData(O_WESTWALL) == 0)
							tile->setMapData(md, corner->entry, corner->set, O_WESTWALL);
					}
				}
			}
		}
	}
}

/**
 * Removes all blocks within a given set of rects, as defined in the command.
 * @param command contains all the info we need.
 * @return success of the removal.
 * @feel shame for having written this.
 */
bool BattlescapeGenerator::removeBlocks(MapScript *command)
{
	std::vector<std::pair<int, int> > deleted;
	bool success = false;

	for (std::vector<SDL_Rect*>::const_iterator k = command->getRects()->begin(); k != command->getRects()->end(); ++k)
	{
		for (int x = (*k)->x; x != (*k)->x + (*k)->w && x != _mapsize_x / 10; ++x)
		{
			for (int y = (*k)->y; y != (*k)->y + (*k)->h && y != _mapsize_y / 10; ++y)
			{
				if (_blocks[x][y] != 0 && _blocks[x][y] != _dummy)
				{
					std::pair<int, int> pos(x, y);
					if (!command->getGroups()->empty())
					{
						for (std::vector<int>::const_iterator z = command->getGroups()->begin(); z != command->getGroups()->end(); ++z)
						{
							if (_blocks[x][y]->isInGroup((*z)))
							{
								// the deleted vector should only contain unique entries
								if (std::find(deleted.begin(), deleted.end(), pos) == deleted.end())
								{
									deleted.push_back(pos);
								}
							}
						}
					}
					else if (!command->getBlocks()->empty())
					{
						for (std::vector<int>::const_iterator z = command->getBlocks()->begin(); z != command->getBlocks()->end(); ++z)
						{
							if ((size_t)(*z) < _terrain->getMapBlocks()->size())
							{
								// the deleted vector should only contain unique entries
								if (std::find(deleted.begin(), deleted.end(), pos) == deleted.end())
								{
									deleted.push_back(pos);
								}
							}
						}
					}
					else
					{
						// the deleted vector should only contain unique entries
						if (std::find(deleted.begin(), deleted.end(), pos) == deleted.end())
						{
							deleted.push_back(pos);
						}
					}
				}
			}
		}
	}
	for (std::vector<std::pair<int, int> >::const_iterator z = deleted.begin(); z != deleted.end(); ++z)
	{
		int x = (*z).first;
		int y = (*z).second;
		clearModule(x * 10, y * 10, _blocks[x][y]->getSizeX(), _blocks[x][y]->getSizeY());

		int delx = (_blocks[x][y]->getSizeX() / 10);
		int dely = (_blocks[x][y]->getSizeY() / 10);

		for (int dx = x; dx != x + delx; ++dx)
		{
			for (int dy = y; dy != y + dely; ++dy)
			{
				_blocks[dx][dy] = 0;
				_blocksToDo++;
			}
		}
		// this command succeeds if even one block is removed.
		success = true;
	}
	return success;
}

/**
 * Sets the terrain to be used in battle generation.
 * @param terrain Pointer to the terrain rules.
 */
void BattlescapeGenerator::setTerrain(RuleTerrain *terrain)
{
	_terrain = terrain;
}


/**
 * Sets up the objectives for the map.
 * @param ruleDeploy the deployment data we're gleaning data from.
 */
void BattlescapeGenerator::setupObjectives(AlienDeployment *ruleDeploy)
{
	int targetType = ruleDeploy->getObjectiveType();

	if (targetType > -1)
	{
		int objectives = ruleDeploy->getObjectivesRequired();
		int actualCount = 0;

		for (int i = 0; i < _save->getMapSizeXYZ(); ++i)
		{
			for (int j = 0; j != 4; ++j)
			{
				if (_save->getTiles()[i]->getMapData(j) && _save->getTiles()[i]->getMapData(j)->getSpecialType() == targetType)
				{
					actualCount++;
				}
			}
		}

		if (actualCount > 0)
		{
			_save->setObjectiveType(targetType);

			if (actualCount < objectives || objectives == 0)
			{
				_save->setObjectiveCount(actualCount);
			}
			else
			{
				_save->setObjectiveCount(objectives);
			}
		}
	}
}
}
