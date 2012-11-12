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
#include <fstream>
#include <sstream>
#include "BattlescapeGenerator.h"
#include "TileEngine.h"
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
#include "../Savegame/NodeLink.h"
#include "../Engine/RNG.h"
#include "../Engine/Exception.h"
#include "../Ruleset/MapBlock.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/RuleUfo.h"
#include "../Ruleset/RuleCraft.h"
#include "../Ruleset/RuleTerrain.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/MapDataSet.h"
#include "../Ruleset/MapData.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/Unit.h"
#include "../Ruleset/AlienRace.h"
#include "../Ruleset/AlienDeployment.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Resource/XcomResourcePack.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/CrossPlatform.h"
#include "../Savegame/Vehicle.h"
#include "../Savegame/TerrorSite.h"
#include "PatrolBAIState.h"

namespace OpenXcom
{

/**
 * Sets up a BattlescapeGenerator.
 * @param game pointer to Game object.
 */
BattlescapeGenerator::BattlescapeGenerator(Game *game) : _game(game), _save(game->getSavedGame()->getBattleGame()), _res(_game->getResourcePack()), _craft(0), _ufo(0), _base(0), _terror(0), _terrain(0),
														 _width(0), _length(0), _height(0), _worldTexture(0), _worldShade(0), _unitSequence(0), _craftInventoryTile(0), _alienRace(""), _alienItemLevel(0)
{
}

/**
 * Deletes the BattlescapeGenerator.
 */
BattlescapeGenerator::~BattlescapeGenerator()
{

}

/**
 * Sets the x-com craft involved in the battle.
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
	if (texture > 12) texture = 12;
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
 * @param alienRace Alien (main)race.
 */
void BattlescapeGenerator::setAlienRace(const std::string &alienRace)
{
	_alienRace = alienRace;
}

/**
 * Sets the alien item level. This is used to determine how advanced the equipment of the aliens will be.
 * - this value should be from 0 to 3.
 * - at a certain number of months higher item levels appear more and more and lower ones will gradually disappear
 * - how quick a race evolves varies per race? TODO
 * @param alienItemLevel AlienItemLevel.
 */
void BattlescapeGenerator::setAlienItemlevel(int alienItemLevel)
{
	_alienItemLevel = alienItemLevel;
}

/**
 * Sets the x-com base involved in the battle.
 * @param base Pointer to XCom base.
 */
void BattlescapeGenerator::setBase(Base *base)
{
	_base = base;
}

/**
 * Sets the terror site involved in the battle.
 * @param base Pointer to terror site.
 */
void BattlescapeGenerator::setTerrorSite(TerrorSite *terror)
{
	_terror = terror;
	_terror->setInBattlescape(true);
}


/**
 * This will start the generator: it will fill up the battlescapesavegame with data.
 */
void BattlescapeGenerator::run()
{
	AlienDeployment *ruleDeploy = _game->getRuleset()->getDeployment(_ufo?_ufo->getRules()->getType():_save->getMissionType());

	ruleDeploy->getDimensions(&_width, &_length, &_height);

	_unitSequence = BattleUnit::MAX_SOLDIER_ID; // geoscape soldier IDs should stay below this number

	// find out the terrain type
	if (_save->getMissionType() == "STR_TERROR_MISSION")
	{
		_terrain = _game->getRuleset()->getTerrain("URBAN");
	}
	else
	if (_save->getMissionType() == "STR_BASE_DEFENSE")
	{
		_terrain = _game->getRuleset()->getTerrain("XBASE");
		_worldShade = 5;
	}
	else
	if (_save->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
	{
		_terrain = _game->getRuleset()->getTerrain("UBASE");
		_worldShade = 15;
	}
	else
	{
		switch (_worldTexture)
		{
		case 0:
		case 6:
		case 10:
		case 11:
			{
				if (_ufo != 0)
				{
					if (_ufo->getLatitude() < 0)
					{ // northern hemisphere
						_terrain = _game->getRuleset()->getTerrain("FOREST");
					}else
					{ // southern hemisphere
						_terrain = _game->getRuleset()->getTerrain("JUNGLE");
					}
				}
				else
				{
					_terrain = _game->getRuleset()->getTerrain("FOREST");
				}
				break;
			}
		case 1:
		case 2:
		case 3:
		case 4:
			{
				_terrain = _game->getRuleset()->getTerrain("CULTA");
				break;
			}
		case 5:
			{
				_terrain = _game->getRuleset()->getTerrain("MOUNT");
				break;
			}
		case 7:
		case 8:
			{
				_terrain = _game->getRuleset()->getTerrain("DESERT");
				break;
			}
		case 9:
		case 12:
			{
				_terrain = _game->getRuleset()->getTerrain("POLAR");
				break;
			}
		}
	}

	// creates the tile objects
	_save->initMap(_width, _length, _height);
	_save->initUtilities(_res);

	// lets generate the map now and store it inside the tile objects
	generateMap();
	BattleUnit *unit;

	if (_craft != 0 || _base != 0)
	{
		if (_craft !=0)
			_base = _craft->getBase();

		// add vehicles that are in the craft - a vehicle is actually an item, which you will never see as it is converted to a unit
		// however the item itself becomes the weapon it "holds".
		// TODO: Convert base vehicles
		if (_craft != 0)
		{
			for (std::vector<Vehicle*>::iterator i = _craft->getVehicles()->begin(); i != _craft->getVehicles()->end(); ++i)
			{
				std::string vehicle = (*i)->getRules()->getType();
				std::string ammo = (*i)->getRules()->getCompatibleAmmo()->front();
				Unit *rule = _game->getRuleset()->getUnit(vehicle.substr(4));
				unit = addXCOMUnit(new BattleUnit(rule, FACTION_PLAYER, _unitSequence++, _game->getRuleset()->getArmor(rule->getArmor())));
				addItem(_game->getRuleset()->getItem(vehicle), unit);
				addItem(_game->getRuleset()->getItem(ammo), unit)->setAmmoQuantity((*i)->getAmmo());
				unit->setTurretType(0);
			}
		}

		// add soldiers that are in the craft or base
		for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
		{
			if ((*i)->getCraft() == _craft ||
				(_craft == 0 && (*i)->getWoundRecovery() == 0 && ((*i)->getCraft() == 0 || (*i)->getCraft()->getStatus() != "STR_OUT")))
			{
				unit = addXCOMUnit(new BattleUnit(*i, FACTION_PLAYER));
				if (!_save->getSelectedUnit())
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
				for (int count=0; count < i->second; count++)
				{
					_craftInventoryTile->addItem(new BattleItem(_game->getRuleset()->getItem(i->first), _save->getCurrentItemId()),
						_game->getRuleset()->getInventory("STR_GROUND"));
				}
			}
		}
		else
		{
			// add items that are in the base
			for (std::map<std::string, int>::iterator i = _base->getItems()->getContents()->begin(); i != _base->getItems()->getContents()->end(); ++i)
			{
				// only put items in the battlescape that make sense (when the item got a sprite, it's probably ok)
				RuleItem *rule = _game->getRuleset()->getItem(i->first);
				if (rule->getBigSprite() > -1 && rule->getBattleType() != BT_NONE && rule->getBattleType() != BT_CORPSE && !rule->isFixed())
				{
					for (int count=0; count < i->second; count++)
					{
						_craftInventoryTile->addItem(new BattleItem(_game->getRuleset()->getItem(i->first), _save->getCurrentItemId()),
							_game->getRuleset()->getInventory("STR_GROUND"));
					}
					_base->getItems()->removeItem(i->first, i->second);
				}
			}
			// add items from crafts in base
			for (std::vector<Craft*>::iterator c = _base->getCrafts()->begin(); c != _base->getCrafts()->end(); ++c)
			{
				if ((*c)->getStatus() == "STR_OUT")
					continue;
				for (std::map<std::string, int>::iterator i = (*c)->getItems()->getContents()->begin(); i != (*c)->getItems()->getContents()->end(); ++i)
				{
					for (int count=0; count < i->second; count++)
					{
						_craftInventoryTile->addItem(new BattleItem(_game->getRuleset()->getItem(i->first), _save->getCurrentItemId()),
							_game->getRuleset()->getInventory("STR_GROUND"));
					}
				}
			}
		}

		// auto-equip soldiers
		for (std::vector<BattleItem*>::iterator i = _craftInventoryTile->getInventory()->begin(); i != _craftInventoryTile->getInventory()->end(); ++i)
		{
			addItem(*i);
		}
		// clean up moved items
		RuleInventory *ground = _game->getRuleset()->getInventory("STR_GROUND");
		for (std::vector<BattleItem*>::iterator i = _craftInventoryTile->getInventory()->begin(); i != _craftInventoryTile->getInventory()->end();)
		{
			if ((*i)->getSlot() != ground)
			{
				i = _craftInventoryTile->getInventory()->erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	deployAliens(_game->getRuleset()->getAlienRace(_alienRace), ruleDeploy);

	if (_save->getMissionType() ==  "STR_TERROR_MISSION")
	{
		deployCivilians(16);
	}


	fuelPowerSources();

	if (_save->getMissionType() ==  "STR_UFO_CRASH_RECOVERY")
	{
		explodePowerSources();
	}

	if (_save->getMissionType() == "STR_BASE_DEFENSE")
	{
		for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
		{
			_save->getTiles()[i]->setDiscovered(true, 2);
		}
	}

	if (_save->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
	{
		for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
		{
			if (_save->getTiles()[i]->getMapData(MapData::O_FLOOR) && _save->getTiles()[i]->getMapData(MapData::O_FLOOR)->getSpecialType() == START_POINT)
				_save->getTiles()[i]->setDiscovered(true, 2);
		}
	}


	// set shade (alien bases are a little darker, sites depend on worldshade)
	_save->setGlobalShade(_worldShade);

	_save->getTileEngine()->calculateSunShading();
	_save->getTileEngine()->calculateTerrainLighting();
	_save->getTileEngine()->calculateUnitLighting();
}

/**
 * Adds a soldier to the game and place him on a free spawnpoint.
 * Spawnpoints are either tiles in case of an xcom craft that landed.
 * Or they are mapnodes in case there's no craft.
 * @param soldier pointer to the Soldier
 */
BattleUnit *BattlescapeGenerator::addXCOMUnit(BattleUnit *unit)
{
//	unit->setId(_unitCount++);

	if (_craft == 0 || _save->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
	{
		Node* node = _save->getSpawnNode(NR_XCOM, unit);
		if (node)
		{
			_save->setUnitPosition(unit, node->getPosition());
		}
		_craftInventoryTile = _save->getTile(node->getPosition());
		unit->setDirection(RNG::generate(0,7));
	}
	else
	{
		for (int i = 0; i < _height * _length * _width; i++)
		{
			// to spawn an xcom soldier, there has to be a tile, with a floor, with the starting point attribute and no object in the way
			if (_save->getTiles()[i] && 
				_save->getTiles()[i]->getMapData(MapData::O_FLOOR) && 
				_save->getTiles()[i]->getMapData(MapData::O_FLOOR)->getSpecialType() == START_POINT && 
				!_save->getTiles()[i]->getMapData(MapData::O_OBJECT) &&
				_save->getTiles()[i]->getMapData(MapData::O_FLOOR)->getTUCost(MT_WALK) < 255)
			{
				if (_craftInventoryTile == 0)
					_craftInventoryTile = _save->getTiles()[i];

				// for bigger units, line them up with the first tile of the craft
				if (unit->getArmor()->getSize() == 1 || _craftInventoryTile == 0 || _save->getTiles()[i]->getPosition().x == _craftInventoryTile->getPosition().x)
				{
					if (_save->setUnitPosition(unit, _save->getTiles()[i]->getPosition()))
					{
						_save->getTileEngine()->calculateFOV(unit);
						break;
					}
				}
			}
		}
	}
	_save->getUnits()->push_back(unit);
	return unit;
}

/**
 * Deploy the aliens, according to the alien deployment rules.
 * @param race Pointer to the alien race.
 * @param deployment Pointer to the deployment rules.
 */
void BattlescapeGenerator::deployAliens(AlienRace *race, AlienDeployment *deployment)
{
	for (std::vector<DeploymentData>::iterator d = deployment->getDeploymentData()->begin(); d != deployment->getDeploymentData()->end(); ++d)
	{
		std::string alienName = race->getMember((*d).alienRank);

		int quantity = (*d).lowQty + RNG::generate(0, (*d).dQty); // beginner/experienced
		if( _game->getSavedGame()->getDifficulty() > DIFF_EXPERIENCED )
			quantity = (*d).lowQty+(((*d).highQty-(*d).lowQty)/2) + RNG::generate(0, (*d).dQty); // veteran/genius
		else if( _game->getSavedGame()->getDifficulty() > DIFF_GENIUS )
			quantity = (*d).highQty + RNG::generate(0, (*d).dQty); // super

		for (int i = 0; i < quantity; i++)
		{
			bool outside = RNG::generate(0,99) < (*d).percentageOutsideUfo;
			if (_ufo == 0)
				outside = false;
			BattleUnit *unit = addAlien(_game->getRuleset()->getUnit(alienName), (*d).alienRank, outside);
			for (std::vector<std::string>::iterator it = (*d).itemSets.at(_alienItemLevel).items.begin(); it != (*d).itemSets.at(_alienItemLevel).items.end(); ++it)
			{
				RuleItem *ruleItem = _game->getRuleset()->getItem((*it));
				if (ruleItem)
				{
					addItem(ruleItem, unit);
				}
			}
			// terrorist alien's equipment is a special case - they are fitted with a weapon which is the alien's name with suffix _WEAPON
			if ((*d).alienRank == AR_TERRORIST || (*d).alienRank == AR_TERRORIST2)
			{
				std::stringstream terroristWeapon;
				terroristWeapon << alienName;
				terroristWeapon << "_WEAPON";
				RuleItem *ruleItem = _game->getRuleset()->getItem(terroristWeapon.str());
				if (ruleItem)
				{
					addItem(ruleItem, unit);
				}
			}
		}
	}
}

/**
 * Adds an alien to the game and place him on a free spawnpoint.
 * @param rules pointer to the Unit which holds info about alien .
 * @param alienRank The rank of the alien, used for spawn point search.
 * @param outside Whether the alien should spawn outside or inside the UFO.
 * @return pointer to the created unit.
 */
BattleUnit *BattlescapeGenerator::addAlien(Unit *rules, int alienRank, bool outside)
{
	BattleUnit *unit = new BattleUnit(rules, FACTION_HOSTILE, _unitSequence++, _game->getRuleset()->getArmor(rules->getArmor()));
	Node *node = 0;

	/* following data is the order in which certain alien ranks spawn on certain node ranks */
	/* note that they all can fall back to rank 0 nodes - which is scout (outside ufo) */

	int nodeRank[8][7] = { { 4, 3, 5, 8, 7, 2, 0 }, // commander
	{ 4, 3, 5, 8, 7, 2, 0 }, // leader
	{ 5, 4, 3, 2, 7, 8, 0 }, //engineer
	{ 7, 6, 2, 8, 3, 4, 0 }, //medic
	{ 3, 4, 5, 2, 7, 8, 0 }, //navigator
	{ 2, 5, 3, 4, 6, 8, 0 }, //soldier
	{ 2, 5, 3, 4, 6, 8, 0 } }; //terrorist

	for (int i = 0; i < 7 && node == 0; i++)
	{
		if (outside)
			node = _save->getSpawnNode(0, unit); // when alien is instructed to spawn outside, we only look for node 0 spawnpoints
		else
			node = _save->getSpawnNode(nodeRank[alienRank][i], unit);
	}

	if (node)
	{
		_save->setUnitPosition(unit, node->getPosition());
		unit->setAIState(new PatrolBAIState(_game->getSavedGame()->getBattleGame(), unit, node));
		unit->setDirection(RNG::generate(0,7));
	}


	_save->getUnits()->push_back(unit);

	return unit;
}

/**
 * Adds a civilian to the game and place him on a free spawnpoint.
 * @param rules pointer to the Unit which holds info about civilian .
 * @return pointer to the created unit.
 */
BattleUnit *BattlescapeGenerator::addCivilian(Unit *rules)
{
	BattleUnit *unit = new BattleUnit(rules, FACTION_NEUTRAL, _unitSequence++, _game->getRuleset()->getArmor(rules->getArmor()));
	Node *node = _save->getSpawnNode(0, unit);

	if (node)
	{
		_save->setUnitPosition(unit, node->getPosition());
		unit->setAIState(new PatrolBAIState(_game->getSavedGame()->getBattleGame(), unit, node));
		unit->setDirection(RNG::generate(0,7));
	}

	_save->getUnits()->push_back(unit);

	return unit;
}

 /*** TODO - refactoring - the two below functions are very similar, should try to join them ***/

/**
 * Adds an item to an X-Com soldier.
 * @param item pointer to the Item
 */
BattleItem* BattlescapeGenerator::addItem(BattleItem *item)
{
	bool loaded = false;
	RuleInventory *righthand = _game->getRuleset()->getInventory("STR_RIGHT_HAND");

	switch (item->getRules()->getBattleType())
	{
	case BT_AMMO:
		break;
	case BT_GRENADE:
	case BT_PROXIMITYGRENADE:
	case BT_SCANNER:
		// find the first soldier with a free belt slot to equip grenades
		for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			if ((*i)->getArmor()->getSize() > 1) continue;

			if (!(*i)->getItem("STR_BELT"))
			{
				item->moveToOwner((*i));
				item->setSlot(_game->getRuleset()->getInventory("STR_BELT"));
				break;
			}
		}
		break;
	case BT_FIREARM:
	case BT_MELEE:
		// maybe we find ammo on the ground to load it with
		if (item->getRules()->getCompatibleAmmo()->empty())
		{
			loaded = true;
		}
		for (std::vector<BattleItem*>::iterator i = _craftInventoryTile->getInventory()->begin(); i != _craftInventoryTile->getInventory()->end() && !loaded; ++i)
		{
			if ((*i)->getSlot() != righthand && item->setAmmoItem((*i)) == 0)
			{
				(*i)->setSlot(righthand);
				loaded = true;
			}
		}
		if (loaded)
		{
			// find the first soldier with a free right hand to equip weapons
			for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
			{
				if ((*i)->getArmor()->getSize() > 1) continue;

				if (!(*i)->getItem("STR_RIGHT_HAND"))
				{
					item->moveToOwner((*i));
					item->setSlot(righthand);
					break;
				}
			}
		}
		break;
	case BT_MEDIKIT:
		// find the first soldier with a free belt for medikit (2 spaces)
		for (std::vector<BattleUnit*>::iterator i = _save->getUnits()->begin(); i != _save->getUnits()->end(); ++i)
		{
			if ((*i)->getArmor()->getSize() > 1) continue;

			if (!(*i)->getItem("STR_BELT",3,0))
			{
				item->moveToOwner((*i));
				item->setSlot(_game->getRuleset()->getInventory("STR_BELT"));
				item->setSlotX(3);
				item->setSlotY(0);
				break;
			}
		}
		break;
	default:
		break;
	}

	_save->getItems()->push_back(item);
	item->setXCOMProperty(true);

	return item;
}


/**
 * Adds an item to the game and assign it to a unit.
 * @param item pointer to the Item
 * @param unit pointer to the Unit
 */
BattleItem* BattlescapeGenerator::addItem(RuleItem *item, BattleUnit *unit)
{
	BattleItem *bi = new BattleItem(item, _save->getCurrentItemId());
	bool placed = false;

	switch (item->getBattleType())
	{
	case BT_AMMO:
		// find equipped weapons that can be loaded with this ammo
		if (unit->getItem("STR_RIGHT_HAND") && unit->getItem("STR_RIGHT_HAND")->getAmmoItem() == 0)
		{
			if (unit->getItem("STR_RIGHT_HAND")->setAmmoItem(bi) == 0)
			{
				placed = true;
			}
		}
		else if (!unit->getItem("STR_BELT"))
		{
			bi->moveToOwner(unit);
			bi->setSlot(_game->getRuleset()->getInventory("STR_BELT"));
			placed = true;
		}
		break;
	case BT_GRENADE:
	case BT_PROXIMITYGRENADE:
		if (!unit->getItem("STR_BELT"))
		{
			bi->moveToOwner(unit);
			bi->setSlot(_game->getRuleset()->getInventory("STR_BELT"));
			placed = true;
		}
		break;
	case BT_FIREARM:
	case BT_MELEE:
		if (!unit->getItem("STR_RIGHT_HAND"))
		{
			bi->moveToOwner(unit);
			bi->setSlot(_game->getRuleset()->getInventory("STR_RIGHT_HAND"));
			placed = true;
		}
		break;
	case BT_MEDIKIT:
	case BT_SCANNER:
		if (!unit->getItem("STR_BACK_PACK"))
		{
			bi->moveToOwner(unit);
			bi->setSlot(_game->getRuleset()->getInventory("STR_BACK_PACK"));
			placed = true;
		}
		break;
	default: break;
	}

	// if we could not equip the item, delete it
	if (!placed)
	{
		delete bi;
	}
	else
	{
		_save->getItems()->push_back(bi);
	}
	return bi;
}


/**
 * Generates a map (set of tiles) for a new battlescape game.
 */
void BattlescapeGenerator::generateMap()
{
	int x = 0, y = 0;
	int blocksToDo = 0;
	std::vector< std::vector<MapBlock*> > blocks;
	std::vector< std::vector<bool> > landingzone;
	std::vector< std::vector<int> > segments;
	int craftX = 0, craftY = 0;
	int ufoX = 0, ufoY = 0;
	bool placed = false;

	MapBlock* dummy = new MapBlock(_terrain, "dummy", 0, 0, MT_DEFAULT);
	MapBlock* craftMap = 0;
	MapBlock* ufoMap = 0;

	int mapDataSetIDOffset = 0;
	int craftDataSetIDOffset = 0;

	blocks.resize((_width / 10), std::vector<MapBlock*>((_length / 10)));
	landingzone.resize((_width / 10), std::vector<bool>((_length / 10),false));
	segments.resize((_width / 10), std::vector<int>((_length / 10),0));

	blocksToDo = (_width / 10) * (_length / 10);

	/* Determine UFO landingzone (do this first because ufo is generally bigger) */
	if (_ufo != 0)
	{
		// pick a random ufo mapblock, can have all kinds of sizes
		ufoMap = _ufo->getRules()->getBattlescapeTerrainData()->getRandomMapBlock(999, MT_DEFAULT);

		ufoX = RNG::generate(0, (_length / 10) - ufoMap->getWidth() / 10);
		ufoY = RNG::generate(0, (_width / 10) - ufoMap->getLength() / 10);

		for (int i = 0; i < ufoMap->getWidth() / 10; ++i)
		{
			for (int j = 0; j < ufoMap->getLength() / 10; ++j)
			{
				landingzone[ufoX + i][ufoY + j] = true;
			}
		}
	}

	/* Determine Craft landingzone */
	/* alien base assault has no craft landing zone */
	if (_craft != 0 && (_save->getMissionType() != "STR_ALIEN_BASE_ASSAULT"))
	{
		// pick a random craft mapblock, can have all kinds of sizes
		craftMap = _craft->getRules()->getBattlescapeTerrainData()->getRandomMapBlock(999, MT_DEFAULT);
		while (!placed)
		{
			craftX = RNG::generate(0, (_length/10)- craftMap->getWidth() / 10);
			craftY = RNG::generate(0, (_width/10)- craftMap->getLength() / 10);
			placed = true;
			// check if this place is ok
			for (int i = 0; i < craftMap->getWidth() / 10; ++i)
			{
				for (int j = 0; j < craftMap->getLength() / 10; ++j)
				{
					if (landingzone[craftX + i][craftY + j])
					{
						placed = false; // whoops the ufo is already here, try again
					}
				}
			}
			// if ok, allocate it
			if (placed)
			{
				for (int i = 0; i < craftMap->getWidth() / 10; ++i)
					for (int j = 0; j < craftMap->getLength() / 10; ++j)
						landingzone[craftX + i][craftY + j] = true;
			}
		}
	}

	/* determine positioning of the urban terrain roads */
	if (_save->getMissionType() == "STR_TERROR_MISSION")
	{
		bool EWRoad = RNG::generate(0,99) < 33;
		bool NSRoad = !EWRoad;
		bool TwoRoads = RNG::generate(0,99) < 25;
		int roadX = craftX;
		int roadY = craftY;
		// make sure the road(s) are not crossing the craftin landing site
		while (roadX == craftX || roadY == craftY)
		{
			roadX = RNG::generate(0, (_length/10)- 1);
			roadY = RNG::generate(0, (_width/10)- 1);
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
			while (x < (_width / 10))
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
			while (y < (_length / 10))
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
	else if (_save->getMissionType() == "STR_BASE_DEFENSE")
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
						std::stringstream newname;
						newname << mapname.substr(0, mapname.size()-2); // strip of last 2 digits
						int mapnum = atoi(mapname.substr(mapname.size()-2, 2).c_str()); // get number
						mapnum += num;
						if (mapnum < 10) newname << 0;
						newname << mapnum;
						blocks[x][y] = _terrain->getMapBlock(newname.str());
						num++;
					}
				}
			}
		}

		// fill with dirt
		for (int i = 0; i < (_width / 10); ++i)
		{
			for (int j = 0; j < (_length / 10); ++j)
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
	else if (_save->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
	{
		int randX = RNG::generate(0, (_length/10)- 2);
		int randY = RNG::generate(0, (_width/10)- 2);
		// add the command center
		blocks[randX][randY] = _terrain->getRandomMapBlock(20, MT_UBASECOMM);
		blocksToDo--;
		// mark mapblocks as used
		blocks[randX + 1][randY] = dummy;
		blocksToDo--;
		blocks[randX + 1][randY + 1] = dummy;
		blocksToDo--;
		blocks[randX][randY + 1] = dummy;
		blocksToDo--;
		// add two lifts (not on top of the command center)
		for (int i = 0; i < 2; i++)
		{
			while (blocks[randX][randY] != NULL)
			{
				randX = RNG::generate(0, (_length/10)- 1);
				randY = RNG::generate(0, (_width/10)- 1);
			}
			// add the lift
			blocks[randX][randY] = _terrain->getRandomMapBlock(10, MT_XCOMSPAWN);
			blocksToDo--;
		}
	}


	x = 0;
	y = 0;

	/* Random map generation for crash/landing sites */
	while (blocksToDo)
	{
		if (blocks[x][y] == 0)
		{
			// last block of this row or column or next block is not free or big block would block landingzone
			if (x == ((_width / 10) - 1) || y == ((_length / 10) - 1)
				|| landingzone[x + 1][y] || landingzone[x + 1][y + 1] || landingzone[x][y + 1]
				|| blocks[x + 1][y] || blocks[x + 1][y + 1] || blocks[x][y + 1] 
				|| blocksToDo == 1)
			{
				// only small block will fit
				blocks[x][y] = _terrain->getRandomMapBlock(10, landingzone[x][y]?MT_LANDINGZONE:MT_DEFAULT);
				blocksToDo--;
				x++;
			}
			else
			{
				blocks[x][y] = _terrain->getRandomMapBlock(20, landingzone[x][y]?MT_LANDINGZONE:MT_DEFAULT);
				blocksToDo--;
				if (blocks[x][y]->getWidth() == 20) // big block
				{
					// mark mapblocks as used
					blocks[x + 1][y] = dummy;
					blocksToDo--;
					blocks[x + 1][y + 1] = dummy;
					blocksToDo--;
					blocks[x][y + 1] = dummy;
					blocksToDo--;
					x++;
				}
				x++;
			}
		}
		else
		{
			x++;
		}

		if (x >= (_width / 10)) // reach the end
		{
			x = 0;
			y++;
		}
	}

	for (std::vector<MapDataSet*>::iterator i = _terrain->getMapDataSets()->begin(); i != _terrain->getMapDataSets()->end(); ++i)
	{
		(*i)->loadData();
		_save->getMapDataSets()->push_back(*i);
		mapDataSetIDOffset++;
	}

	/* now load them up */
	int segment = 0;
	for (int itY = 0; itY < (_length / 10); itY++)
	{
		for (int itX = 0; itX < (_width / 10); itX++)
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
	if (_save->getMissionType() == "STR_BASE_DEFENSE" || _save->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
	{
		int ewallfix = 14, swallfix = 13;
		int ewallfixSet = 1, swallfixSet = 1;
		if (_save->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
		{
			ewallfix = 17;
			swallfix = 18;
			ewallfixSet = 2;
			swallfixSet = 2;
		}

		MapDataSet *mds = _terrain->getMapDataSets()->at(ewallfixSet);
		for (int i = 0; i < (_width / 10); ++i)
		{
			for (int j = 0; j < (_length / 10); ++j)
			{
				MapBlock *dirt = _terrain->getRandomMapBlock(10, MT_DIRT);
				if (blocks[i][j] == dirt)
					continue;

				// general stores - there is where the items are put
				if (blocks[i][j] == _terrain->getMapBlock("XBASE_07"))
				{
					_craftInventoryTile = _save->getTile(Position(i*10,(j*10)+5,1));
				}

				// drill east
				if (i < (_width / 10)-1 && blocks[i+1][j] != dirt && _save->getTile(Position((i*10)+9,(j*10)+4,0))->getMapData(MapData::O_OBJECT))
				{
					// remove stuff
					_save->getTile(Position((i*10)+9,(j*10)+3,0))->setMapData(0, 0, 0, MapData::O_WESTWALL);
					_save->getTile(Position((i*10)+9,(j*10)+3,0))->setMapData(0, 0, 0, MapData::O_OBJECT);
					_save->getTile(Position((i*10)+9,(j*10)+4,0))->setMapData(0, 0, 0, MapData::O_WESTWALL);
					_save->getTile(Position((i*10)+9,(j*10)+4,0))->setMapData(0, 0, 0, MapData::O_OBJECT);
					_save->getTile(Position((i*10)+9,(j*10)+5,0))->setMapData(0, 0, 0, MapData::O_WESTWALL);
					_save->getTile(Position((i*10)+9,(j*10)+5,0))->setMapData(0, 0, 0, MapData::O_OBJECT);
					if (_save->getTile(Position((i*10)+9,(j*10)+2,0))->getMapData(MapData::O_OBJECT))
					{
						//wallfix
						_save->getTile(Position((i*10)+9,(j*10)+3,0))->setMapData(mds->getObjects()->at(ewallfix), ewallfix, ewallfixSet, MapData::O_NORTHWALL);
						_save->getTile(Position((i*10)+9,(j*10)+6,0))->setMapData(mds->getObjects()->at(ewallfix), ewallfix, ewallfixSet, MapData::O_NORTHWALL);
					}
					if (_save->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
					{
						//wallcornerfix
						_save->getTile(Position(((i+1)*10),(j*10)+3,0))->setMapData(mds->getObjects()->at(swallfix+1), swallfix+1, swallfixSet, MapData::O_OBJECT);
					}
					// remove more stuff
					_save->getTile(Position(((i+1)*10),(j*10)+3,0))->setMapData(0, 0, 0, MapData::O_WESTWALL);
					_save->getTile(Position(((i+1)*10),(j*10)+4,0))->setMapData(0, 0, 0, MapData::O_WESTWALL);
					_save->getTile(Position(((i+1)*10),(j*10)+5,0))->setMapData(0, 0, 0, MapData::O_WESTWALL);
				}
				// drill south
				if (j < (_length / 10)-1 && blocks[i][j+1] != dirt && _save->getTile(Position((i*10)+4,(j*10)+9,0))->getMapData(MapData::O_OBJECT))
				{
					// remove stuff
					_save->getTile(Position((i*10)+3,(j*10)+9,0))->setMapData(0, 0, 0, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+3,(j*10)+9,0))->setMapData(0, 0, 0, MapData::O_OBJECT);
					_save->getTile(Position((i*10)+4,(j*10)+9,0))->setMapData(0, 0, 0, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+4,(j*10)+9,0))->setMapData(0, 0, 0, MapData::O_OBJECT);
					_save->getTile(Position((i*10)+5,(j*10)+9,0))->setMapData(0, 0, 0, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+5,(j*10)+9,0))->setMapData(0, 0, 0, MapData::O_OBJECT);
					if (_save->getTile(Position((i*10)+2,(j*10)+9,0))->getMapData(MapData::O_OBJECT))
					{
						// wallfix
						_save->getTile(Position((i*10)+3,(j*10)+9,0))->setMapData(mds->getObjects()->at(swallfix), swallfix, swallfixSet, MapData::O_WESTWALL);
						_save->getTile(Position((i*10)+6,(j*10)+9,0))->setMapData(mds->getObjects()->at(swallfix), swallfix, swallfixSet, MapData::O_WESTWALL);
					}
					if (_save->getMissionType() == "STR_ALIEN_BASE_ASSAULT")
					{
						// wallcornerfix
						_save->getTile(Position((i*10)+3,((j+1)*10),0))->setMapData(mds->getObjects()->at(swallfix+1), swallfix+1, swallfixSet, MapData::O_OBJECT);
					}
					// remove more stuff
					_save->getTile(Position((i*10)+3,(j+1)*10,0))->setMapData(0, 0, 0, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+4,(j+1)*10,0))->setMapData(0, 0, 0, MapData::O_NORTHWALL);
					_save->getTile(Position((i*10)+5,(j+1)*10,0))->setMapData(0, 0, 0, MapData::O_NORTHWALL);
				}
			}
		}
	}


	if (_ufo != 0)
	{
		for (std::vector<MapDataSet*>::iterator i = _ufo->getRules()->getBattlescapeTerrainData()->getMapDataSets()->begin(); i != _ufo->getRules()->getBattlescapeTerrainData()->getMapDataSets()->end(); ++i)
		{
			(*i)->loadData();
			_save->getMapDataSets()->push_back(*i);
			craftDataSetIDOffset++;
		}
		loadMAP(ufoMap, ufoX * 10, ufoY * 10, _ufo->getRules()->getBattlescapeTerrainData(), mapDataSetIDOffset);
		loadRMP(ufoMap, ufoX * 10, ufoY * 10, Node::UFOSEGMENT);
		for (int i = 0; i < ufoMap->getWidth() / 10; ++i)
		{
			for (int j = 0; j < ufoMap->getLength() / 10; j++)
			{
				segments[ufoX + i][ufoY + j] = Node::UFOSEGMENT;
			}
		}
	}

	if (_craft != 0 && (_save->getMissionType() != "STR_ALIEN_BASE_ASSAULT"))
	{
		for (std::vector<MapDataSet*>::iterator i = _craft->getRules()->getBattlescapeTerrainData()->getMapDataSets()->begin(); i != _craft->getRules()->getBattlescapeTerrainData()->getMapDataSets()->end(); ++i)
		{
			(*i)->loadData();
			_save->getMapDataSets()->push_back(*i);
		}
		loadMAP(craftMap, craftX * 10, craftY * 10, _craft->getRules()->getBattlescapeTerrainData(), mapDataSetIDOffset + craftDataSetIDOffset, true);
		loadRMP(craftMap, craftX * 10, craftY * 10, Node::CRAFTSEGMENT);
		for (int i = 0; i < craftMap->getWidth() / 10; ++i)
		{
			for (int j = 0; j < craftMap->getLength() / 10; j++)
			{
				segments[craftX + i][craftY + j] = Node::CRAFTSEGMENT;
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

		if (segmentX == (_width / 10)-1)
			neighbourSegments[0] = -1;
		else
			neighbourSegments[0] = segments[segmentX+1][segmentY];
		if (segmentY == (_length / 10)-1)
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
			
		for (int j = 0; j < 5; j++)
		{
			for (int n = 0; n < 4; n++)
			{
				if (node->getNodeLink(j)->getConnectedNodeID() == neighbourDirections[n])
				{
					for (std::vector<Node*>::iterator k = _save->getNodes()->begin(); k != _save->getNodes()->end(); ++k)
					{
						if ((*k)->getSegment() == neighbourSegments[n])
						{
							for (int l = 0; l < 5; l++)
							{
								if ((*k)->getNodeLink(l)->getConnectedNodeID() == neighbourDirectionsInverted[n])
								{
									(*k)->getNodeLink(l)->setConnectedNodeID(node->getID());
									node->getNodeLink(j)->setConnectedNodeID((*k)->getID());
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
 * Loads a X-Com format MAP file into the tiles of the battlegame.
 * @param mapblock Pointer to MapBlock.
 * @param xoff Mapblock offset in X direction.
 * @param yoff Mapblock offset in Y direction.
 * @param save Pointer to the current SavedBattleGame.
 * @param terrain Pointer to the Terrain rule.
 * @param discovered Whether or not this mapblock is discovered (eg. landingsite of the x-com plane)
 * @return int Height of the loaded mapblock (this is needed for spawpoint calculation...)
 * @sa http://www.ufopaedia.org/index.php?title=MAPS
 * @note Y-axis is in reverse order
 */
int BattlescapeGenerator::loadMAP(MapBlock *mapblock, int xoff, int yoff, RuleTerrain *terrain, int mapDataSetOffset, bool discovered)
{
	int width, length, height;
	int x = xoff, y = yoff, z = 0;
	char size[3];
	unsigned char value[4];
	std::stringstream filename;
	filename << "MAPS/" << mapblock->getName() << ".MAP";
	int terrainObjectID;

	// Load file
	std::ifstream mapFile (CrossPlatform::getDataFile(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw Exception("Failed to load MAP");
	}

	mapFile.read((char*)&size, sizeof(size));
	length = (int)size[0];
	width = (int)size[1];
	height = (int)size[2];

	if (height > _save->getHeight())
	{
		throw Exception("Height of map too big for this mission");
	}

	z += height - 1;
	mapblock->setHeight(height);

	for (int i = _height-1; i >0; i--)
	{
		// check if there is already a layer - if so, we have to move Z up
		MapData *floor = _save->getTile(Position(x, y, i))->getMapData(MapData::O_FLOOR);
		if (floor != 0)
		{
			z += i;
			break;
		}
	}

	if (z > (_save->getHeight()-1))
	{
		throw Exception("Something is wrong in your map definitions");
	}

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		for (int part = 0; part < 4; part++)
		{
			terrainObjectID = (int)((unsigned char)value[part]);
			if (terrainObjectID>0)
			{
				int mapDataSetID = mapDataSetOffset;
				int mapDataID = terrainObjectID;
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
		_save->getTile(Position(x, y, z))->setDiscovered(discovered, 2);

		x++;

		if (x == (width + xoff))
		{
			x = xoff;
			y++;
		}
		if (y == (length + yoff))
		{
			y = yoff;
			z--;
		}
	}

	if (!mapFile.eof())
	{
		throw Exception("Invalid data from file");
	}

	mapFile.close();

	return height;
}

/**
 * Loads a X-Com format RMP file into the spawnpoints of the battlegame.
 * @param mapblock pointer to MapBlock.
 * @param xoff mapblock offset in X direction
 * @param yoff mapblock offset in Y direction
 * @param segment mapblock segment
 * @sa http://www.ufopaedia.org/index.php?title=ROUTES
 */
void BattlescapeGenerator::loadRMP(MapBlock *mapblock, int xoff, int yoff, int segment)
{
	int id = 0;
	char value[24];
	std::stringstream filename;
	filename << "ROUTES/" << mapblock->getName() << ".RMP";

	// Load file
	std::ifstream mapFile (CrossPlatform::getDataFile(filename.str()).c_str(), std::ios::in| std::ios::binary);
	if (!mapFile)
	{
		throw Exception("Failed to load RMP");
	}

	size_t nodeOffset = _save->getNodes()->size();

	while (mapFile.read((char*)&value, sizeof(value)))
	{
		if( (int)value[0] < mapblock->getLength() && (int)value[1] < mapblock->getWidth() && (int)value[2] < _height )
		{
			Node *node = new Node(nodeOffset + id, Position(xoff + (int)value[1], yoff + (int)value[0], mapblock->getHeight() - 1 - (int)value[2]), segment, (int)value[19], (int)value[20], (int)value[21], (int)value[22], (int)value[23]);
			for (int j=0;j<5;++j)
			{
				int connectID = (int)((signed char)value[4 + j*3]);
				if (connectID > -1)
				{
					connectID += nodeOffset;
				}
				node->assignNodeLink(new NodeLink(connectID, (int)value[5 + j*3], (int)value[6 + j*3]), j);
			}
			_save->getNodes()->push_back(node);
		}
		id++;
	}

	if (!mapFile.eof())
	{
		throw Exception("Invalid data from file");
	}

	mapFile.close();
}

/**
 * Fill power sources with an elerium-115 object.
 */
void BattlescapeGenerator::fuelPowerSources()
{
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
	{
		if (_save->getTiles()[i]->getMapData(MapData::O_OBJECT) 
			&& _save->getTiles()[i]->getMapData(MapData::O_OBJECT)->getSpecialType() == UFO_POWER_SOURCE)
		{
			BattleItem *elerium = new BattleItem(_game->getRuleset()->getItem("STR_ELERIUM_115"), _save->getCurrentItemId());
			_save->getItems()->push_back(elerium);
			_save->getTiles()[i]->addItem(elerium, _game->getRuleset()->getInventory("STR_GROUND"));
		}
	}
}


/**
 * When a UFO crashes, there is a 75% chance for each powersource to explode.
 */
void BattlescapeGenerator::explodePowerSources()
{
	for (int i = 0; i < _save->getWidth() * _save->getLength() * _save->getHeight(); ++i)
	{
		if (_save->getTiles()[i]->getMapData(MapData::O_OBJECT) 
			&& _save->getTiles()[i]->getMapData(MapData::O_OBJECT)->getSpecialType() == UFO_POWER_SOURCE && RNG::generate(0,100) < 75)
		{
			Position pos;
			pos.x = _save->getTiles()[i]->getPosition().x*16;
			pos.y = _save->getTiles()[i]->getPosition().y*16;
			pos.z = (_save->getTiles()[i]->getPosition().z*24) +12;
			_save->getTileEngine()->explode(pos, 180+RNG::generate(0,70), DT_HE, 11);
		}
	}
}

/**
 * spawn 1-16 civilians on a terror mission.
 */
void BattlescapeGenerator::deployCivilians(int max)
{
	int number = RNG::generate(1, max);

	for (int i = 0; i < number; ++i)
	{
		if (RNG::generate(0,100) < 50)
		{
			addCivilian(_game->getRuleset()->getUnit("MALE_CIVILIAN"));
		}
		else
		{
			addCivilian(_game->getRuleset()->getUnit("FEMALE_CIVILIAN"));
		}
	}
}

}
