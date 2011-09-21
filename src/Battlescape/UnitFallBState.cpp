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

#include "UnitFallBState.h"
#include "TerrainModifier.h"
#include "BattlescapeState.h"
#include "Map.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Engine/RNG.h"
#include "../Ruleset/XcomRuleset.h"
#include "../Ruleset/RuleArmor.h"

namespace OpenXcom
{

/**
 * Sets up an UnitFallBState.
 */
UnitFallBState::UnitFallBState(BattlescapeState *parent, BattleUnit *unit, ItemDamageType damageType) : BattleState(parent), _unit(unit), _damageType(damageType)
{
	
}

/**
 * Deletes the UnitFallBState.
 */
UnitFallBState::~UnitFallBState()
{

}

void UnitFallBState::init()
{
	// don't show the "fall to death" animation when a unit is blasted with explosives or he is already unconscious
	if (_damageType == DT_HE || _unit->getStatus() == STATUS_UNCONSCIOUS)
	{
		_unit->startFalling();
		while (_unit->getStatus() == STATUS_FALLING)
		{
			_unit->keepFalling();
		}
	}
	else
	{
		_parent->getMap()->centerOnPosition(_unit->getPosition());
		_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED);
		_unit->lookAt(3);
	}
	if (_unit->getHealth() == 0)
	{
		// soldiers have 3 screams depending on gender
		Soldier *s = dynamic_cast<Soldier*>(_unit->getUnit());
		if (s)
		{
			if (s->getGender() == GENDER_MALE)
			{
				_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(RNG::generate(41,43))->play();
			}
			else
			{
				_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(RNG::generate(44,46))->play();
			}
		}
		else
		{
			// todo get death sound from rulealien
			_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(10)->play();
		}
	}

	think();
}

/*
 * Think!
 */
void UnitFallBState::think()
{
	if (_unit->getStatus() == STATUS_TURNING)
	{
		_unit->turn();
	}
	else if (_unit->getStatus() == STATUS_STANDING)
	{
		_unit->startFalling();
	}
	else if (_unit->getStatus() == STATUS_FALLING)
	{
		_unit->keepFalling();
	}
	
	if (_unit->isOut())
	{
		_unit->keepFalling();
		TerrainModifier *terrain = _parent->getGame()->getSavedGame()->getBattleGame()->getTerrainModifier();
		convertUnitToCorpse(_unit, terrain);
		terrain->calculateUnitLighting();
		_parent->popState();
	}

	_parent->getMap()->cacheUnit(_unit);
}

/*
 * Unit falling cannot be cancelled.
 */
void UnitFallBState::cancel()
{
}

/*
 * Get the action result. Returns error messages or an empty string when everything went fine.
 * @return returnmessage Empty when everything is fine.
 */
std::string UnitFallBState::getResult() const
{
	return _result;
}

/*
 * Convert unit to corpse.
 * @param unit
 * @param terrain
 */
void UnitFallBState::convertUnitToCorpse(BattleUnit *unit, TerrainModifier *terrain)
{
	_parent->dropItem(_unit->getPosition(), new BattleItem(_parent->getGame()->getRuleset()->getItem(_unit->getUnit()->getArmor()->getCorpseItem()),_parent->getGame()->getSavedGame()->getBattleGame()->getCurrentItemId()), true);
	// move inventory from unit to the ground
	for (std::vector<BattleItem*>::iterator i = _unit->getInventory()->begin(); i != _unit->getInventory()->end(); ++i)
	{
		_unit->getTile()->addItem(*i);
		(*i)->setOwner(0);
	}
	_unit->getInventory()->clear();
}

}
