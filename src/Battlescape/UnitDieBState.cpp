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

#include "UnitDieBState.h"
#include "ExplosionBState.h"
#include "TileEngine.h"
#include "BattlescapeState.h"
#include "Map.h"
#include "Camera.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Ruleset.h"
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"
#include "../Engine/Options.h"
#include "../Engine/Language.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/Unit.h"
#include "InfoboxOKState.h"
#include "InfoboxState.h"
#include "../Savegame/Node.h"

namespace OpenXcom
{

/**
 * Sets up an UnitDieBState.
 * @param parent Pointer to the Battlescape.
 * @param unit Dying unit.
 * @param damageType Type of damage that caused the death.
 * @param noSound Whether to disable the death sound.
 */
UnitDieBState::UnitDieBState(BattlescapeGame *parent, BattleUnit *unit, const RuleDamageType* damageType, bool noSound) : BattleState(parent), _unit(unit), _damageType(damageType), _noSound(noSound)
{
	// don't show the "fall to death" animation when a unit is blasted with explosives or he is already unconscious
	if (!_damageType->isDirect() || _unit->getStatus() == STATUS_UNCONSCIOUS)
	{
		_unit->startFalling();

		while (_unit->getStatus() == STATUS_COLLAPSING)
		{
			_unit->keepFalling();
		}
	}
	else
	{
		if (_unit->getFaction() == FACTION_PLAYER)
		{
			_parent->getMap()->setUnitDying(true);
		}
		_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED);
		_originalDir = _unit->getDirection();
		if (_originalDir != 3)
		{
			_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED / 3);
		}
	}

	_unit->clearVisibleTiles();
	_unit->clearVisibleUnits();

    if (_unit->getFaction() == FACTION_HOSTILE)
    {
        std::vector<Node *> *nodes = _parent->getSave()->getNodes();
        if (!nodes) return; // this better not happen.

        for (std::vector<Node*>::iterator  n = nodes->begin(); n != nodes->end(); ++n)
        {
            if (_parent->getSave()->getTileEngine()->distanceSq((*n)->getPosition(), _unit->getPosition()) < 4)
            {
                (*n)->setType((*n)->getType() | Node::TYPE_DANGEROUS);
            }
        }
    }
}

/**
 * Deletes the UnitDieBState.
 */
UnitDieBState::~UnitDieBState()
{

}

void UnitDieBState::init()
{
}

/**
 * Runs state functionality every cycle.
 * Progresses the death, displays any messages, checks if the mission is over, ...
 */
void UnitDieBState::think()
{
	if (_unit->getDirection() != 3 && _damageType->isDirect())
	{
		int dir = _unit->getDirection() + 1;
		if (dir == 8)
		{
			dir = 0;
		}
		_unit->lookAt(dir);
		_unit->turn();
		if (dir == 3)
		{
			_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED);
		}
	}
	else if (_unit->getStatus() == STATUS_COLLAPSING)
	{
		_unit->keepFalling();
	}
	else if (!_unit->isOut())
	{
		_unit->startFalling();

		if (!_noSound)
		{
			playDeathSound();
		}
	}

	if (_unit->isOut())
	{
		if (!_noSound && !_damageType->isDirect() && _unit->getStatus() != STATUS_UNCONSCIOUS)
		{
			playDeathSound();
		}
		if (_unit->getStatus() == STATUS_UNCONSCIOUS && _unit->getSpecialAbility() == SPECAB_EXPLODEONDEATH)
		{
			_unit->instaKill();
		}
		_parent->getMap()->setUnitDying(false);
		if (_unit->getTurnsSinceSpotted() < 255)
		{
			_unit->setTurnsSinceSpotted(255);
		}
		if (!_unit->getSpawnUnit().empty())
		{
			// converts the dead zombie to a chryssalid
			BattleUnit *newUnit = _parent->convertUnit(_unit, _unit->getSpawnUnit());
			newUnit->lookAt(_originalDir);
		}
		else
		{
			convertUnitToCorpse();
		}
		_parent->getTileEngine()->calculateUnitLighting();
		_parent->popState();
		if (_unit->getOriginalFaction() == FACTION_PLAYER && _unit->getSpawnUnit().empty() && _unit->getArmor()->getSize() == 1)
		{
			Game *game = _parent->getSave()->getBattleState()->getGame();
			if (_unit->getStatus() == STATUS_DEAD)
			{
				if (_damageType->ResistType == DT_NONE)
				{
					game->pushState(new InfoboxOKState(game->getLanguage()->getString("STR_HAS_DIED_FROM_A_FATAL_WOUND", _unit->getGender()).arg(_unit->getName(game->getLanguage()))));
				}
				else if (Options::battleNotifyDeath)
				{
					game->pushState(new InfoboxState(game->getLanguage()->getString("STR_HAS_BEEN_KILLED", _unit->getGender()).arg(_unit->getName(game->getLanguage()))));
				}
			}
			else
			{
				game->pushState(new InfoboxOKState(game->getLanguage()->getString("STR_HAS_BECOME_UNCONSCIOUS", _unit->getGender()).arg(_unit->getName(game->getLanguage()))));
			}
		}
		// if all units from either faction are killed - auto-end the mission.
		if (_parent->getSave()->getSide() == FACTION_PLAYER && Options::battleAutoEnd)
		{
			int liveAliens = 0;
			int liveSoldiers = 0;
			_parent->tallyUnits(liveAliens, liveSoldiers, false);

			if (liveAliens == 0 || liveSoldiers == 0)
			{
				_parent->getSave()->setSelectedUnit(0);
				_parent->cancelCurrentAction(true);
				_parent->requestEndTurn();
			}
		}
	}
	_parent->getMap()->cacheUnit(_unit);
}

/**
 * Unit falling cannot be cancelled.
 */
void UnitDieBState::cancel()
{
}

/**
 * Converts unit to a corpse (item).
 */
void UnitDieBState::convertUnitToCorpse()
{
	_parent->getSave()->getBattleState()->showPsiButton(false);
	Position lastPosition = _unit->getPosition();
	// remove the unconscious body item corresponding to this unit, and if it was being carried, keep track of what slot it was in
	if (lastPosition != Position(-1,-1,-1))
	{
		_parent->getSave()->removeUnconsciousBodyItem(_unit);
	}
	int size = _unit->getArmor()->getSize();
	BattleItem *itemToKeep = 0;
	bool dropItems = !Options::weaponSelfDestruction || (_unit->getOriginalFaction() != FACTION_HOSTILE || _unit->getStatus() == STATUS_UNCONSCIOUS);
	// move inventory from unit to the ground for non-large units
	if (size == 1 && dropItems)
	{
		for (std::vector<BattleItem*>::iterator i = _unit->getInventory()->begin(); i != _unit->getInventory()->end(); ++i)
		{
			_parent->dropItem(lastPosition, (*i));
			if (!(*i)->getRules()->isFixed())
			{
				(*i)->setOwner(0);
			}
			else
			{
				itemToKeep = *i;
			}
		}
	}
	_unit->getInventory()->clear();

	if (itemToKeep != 0)
	{
		_unit->getInventory()->push_back(itemToKeep);
	}

	// remove unit-tile link
	_unit->setTile(0);

	if (lastPosition == Position(-1,-1,-1)) // we're being carried
	{
		// replace the unconscious body item with a corpse in the carrying unit's inventory
		for (std::vector<BattleItem*>::iterator it = _parent->getSave()->getItems()->begin(); it != _parent->getSave()->getItems()->end(); )
		{
			if ((*it)->getUnit() == _unit)
			{
				RuleItem *corpseRules = _parent->getRuleset()->getItem(_unit->getArmor()->getCorpseBattlescape()[0]); // we're in an inventory, so we must be a 1x1 unit
				(*it)->convertToCorpse(corpseRules);
				break;
			}
			++it;
		}
	}
	else
	{
		int i = 0;
		for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
				BattleItem *corpse = new BattleItem(_parent->getRuleset()->getItem(_unit->getArmor()->getCorpseBattlescape()[i]), _parent->getSave()->getCurrentItemId());
				corpse->setUnit(_unit);
				if (_parent->getSave()->getTile(lastPosition + Position(x,y,0))->getUnit() == _unit) // check in case unit was displaced by another unit
				{
					_parent->getSave()->getTile(lastPosition + Position(x,y,0))->setUnit(0);
				}
				_parent->dropItem(lastPosition + Position(x,y,0), corpse, true);
				i++;
			}
		}
	}
}

/**
 * Plays the death sound.
 */
void UnitDieBState::playDeathSound()
{
	if ((_unit->getType() == "SOLDIER" && _unit->getGender() == GENDER_MALE) || _unit->getType() == "MALE_CIVILIAN")
	{
		_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), ResourcePack::MALE_SCREAM[RNG::generate(0, 2)])->play();
	}
	else if ((_unit->getType() == "SOLDIER" && _unit->getGender() == GENDER_FEMALE) || _unit->getType() == "FEMALE_CIVILIAN")
	{
		_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), ResourcePack::FEMALE_SCREAM[RNG::generate(0, 2)])->play();
	}
	else
	{
		_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), _unit->getDeathSound())->play();
	}
}

}
