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
#include "../Ruleset/Armor.h"
#include "../Ruleset/Unit.h"
#include "PatrolBAIState.h"
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

	if (!_noSound)
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
	
    parent->resetSituationForAI();

    if (_unit->getFaction() == FACTION_HOSTILE)
    {
        std::vector<Node *> *nodes = parent->getSave()->getNodes();
        if (!nodes) return; // this better not happen.

        for (std::vector<Node*>::iterator  n = nodes->begin(); n != nodes->end(); ++n)
        {
            if (parent->getSave()->getTileEngine()->distanceSq((*n)->getPosition(), unit->getPosition()) < 4)
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
	}
	else if (_unit->getStatus() == STATUS_COLLAPSING)
	{
		_unit->keepFalling();
	}

	if (_unit->getStatus() == STATUS_DEAD || _unit->getStatus() == STATUS_UNCONSCIOUS)
	{
		_parent->getMap()->setUnitDying(false);
		if (!_unit->getVisibleUnits()->empty())
		{
			_unit->clearVisibleUnits();
		}
		if (_unit->getTurnsExposed())
		{
			_unit->setTurnsExposed(0);
			_parent->getSave()->updateExposedUnits();
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
		if (_unit->getSpecialAbility() == SPECAB_EXPLODEONDEATH)
		{
			_unit->instaKill();
			if (_damageType != DT_STUN && _damageType != DT_HE)
			{
				Position p = Position(_unit->getPosition().x * 16, _unit->getPosition().y * 16, _unit->getPosition().z * 24);
				_parent->statePushNext(new ExplosionBState(_parent, p, 0, _unit, 0));
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
	// in case the unit was unconscious
	_parent->getSave()->removeUnconsciousBodyItem(_unit);

	int size = _unit->getArmor()->getSize() - 1;
	// move inventory from unit to the ground for non-large units
	if (size == 0)
	{
		for (std::vector<BattleItem*>::iterator i = _unit->getInventory()->begin(); i != _unit->getInventory()->end(); ++i)
		{
			_parent->dropItem(_unit->getPosition(), (*i));
		}
	}
	_unit->getInventory()->clear();

	// remove unit-tile link
	_unit->setTile(0);

	if (size == 0)
	{
		_parent->getSave()->getTile(_unit->getPosition())->setUnit(0);
		BattleItem *corpse = new BattleItem(_parent->getRuleset()->getItem(_unit->getArmor()->getCorpseItem()),_parent->getSave()->getCurrentItemId());
		corpse->setUnit(_unit);
		_parent->dropItem(_unit->getPosition(), corpse, true);
	}
	else
	{
		int i = 1;
		for (int y = 0; y <= size; y++)
		{
			for (int x = 0; x <= size; x++)
			{
				_parent->getSave()->getTile(_unit->getPosition() + Position(x,y,0))->setUnit(0);
				std::stringstream ss;
				ss << _unit->getArmor()->getCorpseItem() << i;
				BattleItem *corpse = new BattleItem(_parent->getRuleset()->getItem(ss.str()),_parent->getSave()->getCurrentItemId());
				corpse->setUnit(_unit); // no need for this, because large units never can be revived as they don't go unconscious
										// yes there freaking is because yes they freaking do, nerf their consciousness elswhere, 
										// because we need to recover live reapers and i need this kept track of for corpse recovery. also i hate reapers.
				_parent->dropItem(_unit->getPosition() + Position(x,y,0), corpse, true);
				i++;
			}
		}
	}
}

}
