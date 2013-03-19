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
#include "SearchForWeaponBAIState.h"
#include "BattleScapeState.h"
#include "TileEngine.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/Game.h"
#include "../Ruleset/RuleSet.h"
#include "../Ruleset/RuleInventory.h"
#include "../Ruleset/RuleItem.h"
#include "../Savegame/Tile.h"

namespace OpenXcom
{

/**
 * Sets up a SearchForWeaponBAIState.
 */
SearchForWeaponBAIState::SearchForWeaponBAIState(SavedBattleGame *game, BattleUnit *unit) : BattleAIState(game, unit)
{

}

/**
 * Deletes the SearchForWeaponBAIState.
 */
SearchForWeaponBAIState::~SearchForWeaponBAIState()
{

}

/**
 * Loads the AI state from a YAML file.
 * @param node YAML node.
 */
void SearchForWeaponBAIState::load(const YAML::Node &node)
{
	// not sure what to put here to be honest, this state doesn't store the item it's looking for
	// or the action, or anything really, it has no member variables and everything is calculated on the fly.
}

/**
 * Saves the AI state to a YAML file.
 * @param out YAML emitter.
 */
void SearchForWeaponBAIState::save(YAML::Emitter &out) const
{
	out << YAML::BeginMap;
	out << YAML::Key << "state" << YAML::Value << "SEARCH";
	out << YAML::EndMap;
}

/**
 * Enters the current AI state.
 */
void SearchForWeaponBAIState::enter()
{

}


/**
 * Exits the current AI state.
 */
void SearchForWeaponBAIState::exit()
{
}

/**
 * Runs any code the state needs to keep updating every
 * AI cycle.
 * @param action a pointer to the action being performed.
 */
void SearchForWeaponBAIState::think(BattleAction *action)
{
	// terrorists don't have hands.
	if (action->actor->getRankString() != "STR_TERRORIST")
	{
		// pick the best available item
		BattleItem *targetItem = surveyItems(action);
		// make sure it's worth taking
		if (targetItem && worthTaking(targetItem, action))
		{
			action->target = targetItem->getTile()->getPosition();
			// if we're already standing on it...
			if (action->target == action->actor->getPosition())
			{
				// try to pick it up
				if (takeItemFromGround(targetItem, action) == 0)
				{
					// if it isn't loaded or it is ammo
					if (!targetItem->getAmmoItem())
					{
						// try to load our weapon
						action->actor->checkAmmo();
					}
				}
			}
			else
			{
				// if we're not standing on it, we should try to get to it.
				action->type = BA_WALK;
			}
		}
	}
}


/**
 * Search through items on the map that were dropped on an alien turn, then pick the most "attractive" one
 * @param action a pointer to the action being performed.
 * @return the item to attempt to take.
 */
BattleItem *SearchForWeaponBAIState::surveyItems(BattleAction *action)
{
	std::vector<BattleItem*> droppedItems;

	// first fill a vector with items on the ground that were dropped on the alien turn, and have an attraction value.
	for (std::vector<BattleItem*>::iterator i = _game->getItems()->begin(); i != _game->getItems()->end(); ++i)
	{
		if ((*i)->getSlot()->getId() == "STR_GROUND" && (*i)->getTile() && (*i)->getTurnFlag() && (*i)->getRules()->getAttraction())
		{
			droppedItems.push_back(*i);
		}
	}

	BattleItem *targetItem;
	int maxWorth = 0;

	// now select the most suitable candidate depending on attractiveness and distance
	// (are we still talking about items?)
	for (std::vector<BattleItem*>::iterator i = droppedItems.begin(); i != droppedItems.end(); ++i)
	{
		int currentWorth = (*i)->getRules()->getAttraction() / (_game->getTileEngine()->distance(action->actor->getPosition(), (*i)->getTile()->getPosition()) + 1);
		if (currentWorth > maxWorth)
		{
			maxWorth = currentWorth;
			targetItem = *i;
		}
	}

	return targetItem;
}


/**
 * Assess whether this item is worth trying to pick up, taking into account how many units we see,
 * whether or not the Weapon has ammo, and if we have ammo FOR it,
 * or if it's ammo, check if we have the weapon to go with it,
 * assess the attraction value of the item and compare it with the distance to the object,
 * then return false anyway.
 * @param item the item to attempt to take.
 * @param action a pointer to the action being performed.
 * @return false.
 */
bool SearchForWeaponBAIState::worthTaking(BattleItem* item, BattleAction *action)
{
	int worthToTake = 0;

	// don't even think about making a move for that gun if you can see a target, for some reason 
	// (maybe this should check for enemies spotting the tile the item is on?)
	if (action->actor->getVisibleUnits()->size() == 0)
	{
		// retrieve an insignificantly low value from the ruleset.
		worthToTake = item->getRules()->getAttraction();

		// it's always going to be worth while to try and take a blaster launcher, apparently
		if (!item->getRules()->isWaypoint() && item->getRules()->getBattleType() != BT_AMMO)
		{
			// we only want weapons that HAVE ammo, or weapons that we have ammo FOR
			bool ammoFound = true;
			if (!item->getAmmoItem())
			{
				ammoFound = false;
				for (std::vector<BattleItem*>::iterator i = action->actor->getInventory()->begin(); i != action->actor->getInventory()->end() && !ammoFound; ++i)
				{
					if ((*i)->getRules()->getBattleType() == BT_AMMO)
					{
						for (std::vector<std::string>::iterator j = item->getRules()->getCompatibleAmmo()->begin(); j != item->getRules()->getCompatibleAmmo()->end() && !ammoFound; ++j)
						{
							if ((*i)->getRules()->getName() == *j)
							{
								ammoFound = true;
								break;
							}
						}
					}
				}
			}
			if (!ammoFound)
			{
				return false;
			}
		}

		if ( item->getRules()->getBattleType() == BT_AMMO)
		{
			// similar to the above, but this time we're checking if the ammo is suitable for a weapon we have.
			bool weaponFound = false;
			for (std::vector<BattleItem*>::iterator i = action->actor->getInventory()->begin(); i != action->actor->getInventory()->end() && !weaponFound; ++i)
			{
				if ((*i)->getRules()->getBattleType() == BT_FIREARM)
				{
					for (std::vector<std::string>::iterator j = (*i)->getRules()->getCompatibleAmmo()->begin(); j != (*i)->getRules()->getCompatibleAmmo()->end() && !weaponFound; ++j)
					{
						if ((*i)->getRules()->getName() == *j)
						{
							weaponFound = true;
							break;
						}
					}
				}
			}
			if (!weaponFound)
			{
				return false;
			}
		}
	}

    if (worthToTake)
    {
		// use bad logic to determine if we'll have room for the item
		int freeSlots = 25;
		for (std::vector<BattleItem*>::iterator i = action->actor->getInventory()->begin(); i != action->actor->getInventory()->end(); ++i)
		{
			freeSlots -= (*i)->getRules()->getInventoryHeight() * (*i)->getRules()->getInventoryWidth();
		}
		int size = item->getRules()->getInventoryHeight() * item->getRules()->getInventoryWidth();
		if (freeSlots < size)
		{
			return false;
		}
	}

	// return false for any item that we aren't standing directly on top of with an attraction value less than 6 (aka always)
	return (worthToTake - _game->getTileEngine()->distance(action->actor->getPosition(), item->getTile()->getPosition())) > 5;
}


/**
 * at this point we've decided it's worth our while to grab this item, so we try to do just that.
 * first we check to make sure we have time units, then that we have space (using horrifying logic)
 * then we attempt to actually recover the item.
 * @param item the item to attempt to take.
 * @param action a pointer to the action being performed.
 * @return 0 if successful, 1 for no TUs, 2 for not enough room, 3 for "won't fit" and -1 for "something went horribly wrong"
 */
int SearchForWeaponBAIState::takeItemFromGround(BattleItem* item, BattleAction *action)
{
	const int unhandledError = -1;
	const int success = 0;
	const int notEnoughTimeUnits = 1;
	const int notEnoughSpace = 2;
	const int couldNotFit = 3;
	int freeSlots = 25;

	// make sure we have time units
	if (action->actor->getTimeUnits() < 6)
	{
		return notEnoughTimeUnits;
	}
	else
	{
		// check to make sure we have enough space by checking all the sizes of items in our inventory 
		for (std::vector<BattleItem*>::iterator i = action->actor->getInventory()->begin(); i != action->actor->getInventory()->end(); ++i)
		{
			freeSlots -= (*i)->getRules()->getInventoryHeight() * (*i)->getRules()->getInventoryWidth();
		}
		if (freeSlots < item->getRules()->getInventoryHeight() * item->getRules()->getInventoryWidth())
		{
			return notEnoughSpace;
		}
		else
		{
			// check that the item will fit in our inventory, and if so, take it
			if (takeItem(item, action))
			{
				action->actor->spendTimeUnits(6);
				return success;
			}
			else
			{
				return couldNotFit;
			}
		}
	}
	// shouldn't ever end up here
	return unhandledError;
}


/**
 * try to fit an item into the unit's inventory, return false if you can't.
 * @param item the item to attempt to take.
 * @param action a pointer to the action being performed.
 * @return whether or not the item was successfully retrieved.
 */
bool SearchForWeaponBAIState::takeItem(BattleItem* item, BattleAction *action)
{
	bool placed = false;
	Ruleset* rules = _game->getBattleState()->getGame()->getRuleset();
	switch (item->getRules()->getBattleType())
	{
	case BT_AMMO:
		// find equipped weapons that can be loaded with this ammo
		if (action->actor->getItem("STR_RIGHT_HAND") && action->actor->getItem("STR_RIGHT_HAND")->getAmmoItem() == 0)
		{
			if (action->actor->getItem("STR_RIGHT_HAND")->setAmmoItem(item) == 0)
			{
				placed = true;
			}
		}
		else
		{
			for (int i = 0; i != 4; ++i)
			{
				if (!action->actor->getItem("STR_BELT", i))
				{
					item->moveToOwner(action->actor);
					item->setSlot(rules->getInventory("STR_BELT"));
					item->setSlotX(i);
					placed = true;
					break;
				}
			}
		}
		break;
	case BT_GRENADE:
	case BT_PROXIMITYGRENADE:
		for (int i = 0; i != 4; ++i)
		{
			if (!action->actor->getItem("STR_BELT", i))
			{
				item->moveToOwner(action->actor);
				item->setSlot(rules->getInventory("STR_BELT"));
				item->setSlotX(i);
				placed = true;
				break;
			}
		}
		break;
	case BT_FIREARM:
	case BT_MELEE:
		if (!action->actor->getItem("STR_RIGHT_HAND"))
		{
			item->moveToOwner(action->actor);
			item->setSlot(rules->getInventory("STR_RIGHT_HAND"));
			placed = true;
		}
		break;
	case BT_MEDIKIT:
	case BT_SCANNER:
		if (!action->actor->getItem("STR_BACK_PACK"))
		{
			item->moveToOwner(action->actor);
			item->setSlot(rules->getInventory("STR_BACK_PACK"));
			placed = true;
		}
		break;
	case BT_MINDPROBE:
		if (!action->actor->getItem("STR_LEFT_HAND"))
		{
			item->moveToOwner(action->actor);
			item->setSlot(rules->getInventory("STR_LEFT_HAND"));
			placed = true;
		}
		break;
	default: break;
	}
	return placed;
}

}