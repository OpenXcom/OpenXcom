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
#include "UnitFallBState.h"
#include "Map.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
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
ExplosionBState::ExplosionBState(BattlescapeState *parent, Position center, BattleItem *item) : BattleState(parent), _center(center), _item(item)
{
	
}

/**
 * Deletes the ExplosionBState.
 */
ExplosionBState::~ExplosionBState()
{

}

/**
 * init explosion :
 * - create an explosion sprite
 * - add it to the list of explosion sprites(on map)
 * - explosion sound
 */
void ExplosionBState::init()
{	
	_parent->setStateInterval(DEFAULT_ANIM_SPEED);
	_unit = _parent->getGame()->getSavedGame()->getBattleGame()->getSelectedUnit();
	if (_parent->getSelectedItem()->getAmmoItem()->getRules()->getHitAnimation() == 0)
	{
		// create a new kaboom
		Explosion *explosion = new Explosion(_center, 0, true);
		// add the explosion on the map
		_parent->getMap()->getExplosions()->insert(explosion);
		// KABOOM
		_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(_parent->getSelectedItem()->getAmmoItem()->getRules()->getHitSound())->play();
	}
	else
	{
		// create a new kaboom
		Explosion *explosion = new Explosion(_center,_parent->getSelectedItem()->getAmmoItem()->getRules()->getHitAnimation(), false);
		// add the explosion on the map
		_parent->getMap()->getExplosions()->insert(explosion);
		// KABOOM
		_parent->getGame()->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(_parent->getSelectedItem()->getAmmoItem()->getRules()->getHitSound())->play();
	}
}

/*
 * Animate explosion sprites. If their animation is finished remove them from the list.
 * If the list is empty, this states is finished.
 */
void ExplosionBState::think()
{
	for (std::set<Explosion*>::const_iterator i = _parent->getMap()->getExplosions()->begin(); i != _parent->getMap()->getExplosions()->end(); i++)
	{
		if(!(*i)->animate())
		{
			_parent->getMap()->getExplosions()->erase((*i));
			if (_parent->getMap()->getExplosions()->empty())
			{
				SavedBattleGame *save = _parent->getGame()->getSavedGame()->getBattleGame();
				// after the animation is done, the real explosion takes place
				save->getTerrainModifier()->explode(_center, _item->getAmmoItem()->getRules()->getPower(), _item->getAmmoItem()->getRules()->getDamageType(), 100);

				// now check for new casualties
				for (std::vector<BattleUnit*>::iterator j = save->getUnits()->begin(); j != save->getUnits()->end(); j++)
				{
					if ((*j)->getHealth() == 0 && (*j)->getStatus() != STATUS_DEAD)
					{
						_parent->statePushFront(new UnitFallBState(_parent, (*j), _item->getAmmoItem()->getRules()->getDamageType() == DT_HE));
					}
				}
				_unit->aim(false);
				_parent->getMap()->cacheUnits();
				_parent->popState();
				return;
			}
		}
	}
}

/*
 * Explosion cannot be cancelled.
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
