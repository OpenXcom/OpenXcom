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

#include "ProjectileFlyBState.h"
#include "ExplosionBState.h"
#include "BattlescapeState.h"
#include "Projectile.h"
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
 * Sets up an ProjectileFlyBState.
 */
ProjectileFlyBState::ProjectileFlyBState(BattlescapeState *parent) : BattleState(parent)
{
	
}

/**
 * Deletes the ProjectileFlyBState.
 */
ProjectileFlyBState::~ProjectileFlyBState()
{

}

/**
 * init sequence:
 * - create a projectile sprite & add it to the map
 * - calculate it's trajectory
 */
void ProjectileFlyBState::init()
{
	_parent->setStateInterval(DEFAULT_BULLET_SPEED);
	_unit = _parent->getGame()->getSavedGame()->getBattleGame()->getSelectedUnit();
	// create a new projectile
	Projectile *projectile = new Projectile(_parent->getGame()->getResourcePack(),
									_parent->getGame()->getSavedGame()->getBattleGame(),
									_unit->getPosition(),
									_parent->getTarget(),
									_parent->getSelectedItem()->getRules()->getBulletSprite()
									);
	// add the projectile on the map
	_parent->getMap()->setProjectile(projectile);
	// let it calculate a trajectory
	if (projectile->calculateTrajectory())
	{
		// set the soldier in an aiming position
		_unit->aim(true);
		_parent->getMap()->cacheUnits();
		// and we have a lift-off
		_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(_parent->getSelectedItem()->getRules()->getFireSound())->play();
	}
	else
	{
		// no line of fire
		delete projectile;
		_parent->getMap()->setProjectile(0);
		_parent->popState();
	}
}

/**
 * Animates the projectile (move to the next point in it's trajectory).
 * If the animation is finished the projectile sprite is removed from the map.
 * And this state is finished.
 */
void ProjectileFlyBState::think()
{
	if(!_parent->getMap()->getProjectile()->move())
	{
		// impact !
		int offset = 0;
		// explosions impact not inside the voxel but one step back
		if (_parent->getSelectedItem()->getAmmoItem() && (
			_parent->getSelectedItem()->getAmmoItem()->getRules()->getDamageType() == DT_HE || 
			_parent->getSelectedItem()->getAmmoItem()->getRules()->getDamageType() == DT_IN))
		{
			offset = -1;
		}
		_parent->statePushNext(new ExplosionBState(_parent, _parent->getMap()->getProjectile()->getPosition(offset), _parent->getSelectedItem()));

		delete _parent->getMap()->getProjectile();
		_parent->getMap()->setProjectile(0);
		_parent->popState();
	}
}

/*
 * Flying projectiles cannot be cancelled.
 */
void ProjectileFlyBState::cancel()
{
}

/*
 * Get the action result. Returns error messages or an empty string when everything went fine.
 * @return returnmessage Empty when everything is fine.
 */
std::string ProjectileFlyBState::getResult() const
{
	return _result;
}

}
