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

#include "ExplosionBState.h"
#include "BattlescapeState.h"
#include "Explosion.h"
#include "TerrainModifier.h"
#include "Map.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

/**
 * Sets up an ExplosionBState.
 */
ExplosionBState::ExplosionBState(BattlescapeState *parent, Position center) : BattleState(parent), _center(center)
{
	
}

/**
 * Deletes the ExplosionBState.
 */
ExplosionBState::~ExplosionBState()
{

}

bool ExplosionBState::init()
{	
	_parent->setStateInterval(DEFAULT_ANIM_SPEED);
	_unit = _parent->getGame()->getSavedGame()->getBattleGame()->getSelectedUnit();
	// create a new kaboom
	Explosion *explosion = new Explosion(_center,_parent->getSelectedItem()->getRules()->getHitAnimation(), false);
	// add the projectile on the map
	_parent->getMap()->getExplosions()->insert(explosion);
	// KABOOM
	_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(_parent->getSelectedItem()->getRules()->getHitSound())->play();

	return true;
}

bool ExplosionBState::think()
{
	for (std::set<Explosion*>::const_iterator i = _parent->getMap()->getExplosions()->begin(); i != _parent->getMap()->getExplosions()->end(); i++)
	{
		if(!(*i)->animate())
		{
			// game over
			_parent->getMap()->getExplosions()->erase((*i));
			if (_parent->getMap()->getExplosions()->empty())
			{
				return false;
			}
		}
	}
	return true;
}

/*
 * Unit turning cannot be cancelled.
 */
void ExplosionBState::cancel()
{
}

/*
 * Get the action result. Returns error messages or an empty string when everything went fine.
 * @return returnmessage Empty when everything is fine.
 */
std::string ExplosionBState::getResult() const
{
	return _result;
}

}
