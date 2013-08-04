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

#include "UnitDieBState.h"
#include "ExplosionBState.h"
#include "TileEngine.h"
#include "BattlescapeState.h"
#include "Map.h"
#include "Camera.h"
#include "../Engine/Game.h"
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
#include "PatrolBAIState.h"
#include "InfoboxOKState.h"
#include "InfoboxState.h"
#include "../Savegame/Node.h"

namespace OpenXcom
{

/**
 * Sets up an UnitDieBState.
 */
UnitDieBState::UnitDieBState(BattlescapeGame *parent, BattleUnit *unit, ItemDamageType damageType, bool noSound) : BattleState(parent), _unit(unit), _damageType(damageType), _noSound(noSound)
{
	// don't show the "fall to death" animation when a unit is blasted with explosives or he is already unconscious
	if (_damageType == DT_HE || _unit->getStatus() == STATUS_UNCONSCIOUS)
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
			_parent->getMap()->setUnitDying(true);
		_parent->getMap()->getCamera()->centerOnPosition(_unit->getPosition());
		_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED);
		_originalDir = _unit->getDirection();
		_unit->lookAt(3); // unit goes into status TURNING to prepare for a nice dead animation
	}
	
	_unit->clearVisibleTiles();
	_unit->clearVisibleUnits();
    _parent->resetSituationForAI();

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

/*
 * Think!
 */
void UnitDieBState::think()
{
	if (_unit->getStatus() == STATUS_TURNING)
	{
		_unit->turn();
	}
	else if (_unit->getStatus() == STATUS_STANDING)
	{
		_unit->startFalling();

		if (!_noSound)
		{
			playDeathSound();
		}
	}
	else if (_unit->getStatus() == STATUS_COLLAPSING)
	{
		_unit->keepFalling();
	}

	if (_unit->getStatus() == STATUS_DEAD || _unit->getStatus() == STATUS_UNCONSCIOUS)
	{

		if (!_noSound && _damageType == DT_HE && _unit->getStatus() != STATUS_UNCONSCIOUS)
		{
			playDeathSound();
		}
		if (_unit->getStatus() == STATUS_UNCONSCIOUS && _unit->getSpecialAbility() == SPECAB_EXPLODEONDEATH)
		{
			_unit->instaKill();
		}
		_parent->getMap()->setUnitDying(false);
		if (_unit->getTurnsExposed())
		{
			_unit->setTurnsExposed(255);
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
		if (_unit->getOriginalFaction() == FACTION_PLAYER && _unit->getSpawnUnit().empty())
		{
			Game *game = _parent->getSave()->getBattleState()->getGame();
			if (_unit->getStatus() == STATUS_DEAD)
			{
				if (_damageType == DT_NONE)
				{
					std::wstringstream ss;
					ss << _unit->getName(game->getLanguage()) << L'\n';
					ss << game->getLanguage()->getString("STR_HAS_DIED_FROM_A_FATAL_WOUND", _unit->getGender());
					game->pushState(new InfoboxOKState(game, ss.str()));
				}
				else if (Options::getBool("battleNotifyDeath"))
				{
					std::wstringstream ss;
					ss << _unit->getName(game->getLanguage()) << L'\n';
					ss << game->getLanguage()->getString("STR_HAS_BEEN_KILLED", _unit->getGender());
					game->pushState(new InfoboxOKState(game, ss.str()));
				}
			}
			else
			{
				std::wstringstream ss;
				ss << _unit->getName(game->getLanguage()) << L'\n';
				ss << game->getLanguage()->getString("STR_HAS_BECOME_UNCONSCIOUS", _unit->getGender());
				game->pushState(new InfoboxOKState(game, ss.str()));
			}
		}
		// if all units from either faction are killed - auto-end the mission.
		if (Options::getBool("battleAutoEnd"))
		{
			int liveAliens = 0;
			int liveSoldiers = 0;
			_parent->tallyUnits(liveAliens, liveSoldiers, false);

			if (liveAliens == 0 || liveSoldiers == 0)
			{
				_parent->statePushBack(0);
			}
		}
	}
	_parent->getMap()->cacheUnit(_unit);
}

/*
 * Unit falling cannot be cancelled.
 */
void UnitDieBState::cancel()
{
}

/*
 * Convert unit to corpse(item).
 */
void UnitDieBState::convertUnitToCorpse()
{
	_parent->getSave()->getBattleState()->showPsiButton(false);
	// in case the unit was unconscious
	_parent->getSave()->removeUnconsciousBodyItem(_unit);
	Position lastPosition = _unit->getPosition();
	int size = _unit->getArmor()->getSize() - 1;
	BattleItem *itemToKeep = 0;
	bool dropItems = !Options::getBool("weaponSelfDestruction") || (_unit->getOriginalFaction() != FACTION_HOSTILE || _unit->getStatus() == STATUS_UNCONSCIOUS);
	// move inventory from unit to the ground for non-large units
	if (size == 0 && dropItems)
	{
		for (std::vector<BattleItem*>::iterator i = _unit->getInventory()->begin(); i != _unit->getInventory()->end(); ++i)
		{
			_parent->dropItem(_unit->getPosition(), (*i));
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

	if (size == 0)
	{
		BattleItem *corpse = new BattleItem(_parent->getRuleset()->getItem(_unit->getArmor()->getCorpseItem()),_parent->getSave()->getCurrentItemId());
		corpse->setUnit(_unit);
		_parent->dropItem(_unit->getPosition(), corpse, true);
		_parent->getSave()->getTile(lastPosition)->setUnit(0);
	}
	else
	{
		int i = 1;
		for (int y = 0; y <= size; y++)
		{
			for (int x = 0; x <= size; x++)
			{
				std::stringstream ss;
				ss << _unit->getArmor()->getCorpseItem() << i;
				BattleItem *corpse = new BattleItem(_parent->getRuleset()->getItem(ss.str()),_parent->getSave()->getCurrentItemId());
				corpse->setUnit(_unit);
				_parent->getSave()->getTile(lastPosition + Position(x,y,0))->setUnit(0);
				_parent->dropItem(lastPosition + Position(x,y,0), corpse, true);
				i++;
			}
		}
	}

}

void UnitDieBState::playDeathSound()
{
	if ((_unit->getType() == "SOLDIER" && _unit->getGender() == GENDER_MALE) || _unit->getType() == "MALE_CIVILIAN")
	{
		_parent->getResourcePack()->getSound("BATTLE.CAT", RNG::generate(41,43))->play();
	}
	else if ((_unit->getType() == "SOLDIER" && _unit->getGender() == GENDER_FEMALE) || _unit->getType() == "FEMALE_CIVILIAN")
	{
		_parent->getResourcePack()->getSound("BATTLE.CAT", RNG::generate(44,46))->play();
	}
	else
	{
		_parent->getResourcePack()->getSound("BATTLE.CAT", _unit->getDeathSound())->play();
	}
}

}
