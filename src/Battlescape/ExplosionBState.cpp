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
#include "TileEngine.h"
#include "UnitDieBState.h"
#include "Map.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Ruleset/RuleItem.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Sets up an ExplosionBState.
 * @param parent Pointer to the BattleScape.
 * @param center Center position in voxelspace.
 * @param item Item involved in the explosion (eg grenade)
 * @param unit Unit involved in the explosion (eg unit throwing the grenade)
 * @param tile Tile the explosion is on.
 */
ExplosionBState::ExplosionBState(BattlescapeGame *parent, Position center, BattleItem *item, BattleUnit *unit, Tile *tile) : BattleState(parent), _unit(unit), _center(center), _item(item), _tile(tile)
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
	if (_item == 0 || _item->getRules()->getHitAnimation() == 0)
	{
		_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED);
		// create 9 explosions
		for (int i = -32; i < 48; i+=32)
			for (int j = -32; j < 48; j+=32)
			{
				Position p = _center;
				p.x += i; p.y += j;
				Explosion *explosion = new Explosion(p, RNG::generate(0,6), true);
				// add the explosion on the map
				_parent->getMap()->getExplosions()->insert(explosion);
			}
		// explosion sound
		_parent->getResourcePack()->getSoundSet("GEO.CAT")->getSound(10)->play();
	}
	else
	{
		_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED/2);
		// create a bulet hit
		Explosion *explosion = new Explosion(_center, _item->getRules()->getHitAnimation(), false);
		// add the explosion on the map
		_parent->getMap()->getExplosions()->insert(explosion);
		// bullet hit sound
		_parent->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(_item->getRules()->getHitSound())->play();
	}
}

/*
 * Animate explosion sprites. If their animation is finished remove them from the list.
 * If the list is empty, this states is finished.
 */
void ExplosionBState::think()
{
	for (std::set<Explosion*>::const_iterator i = _parent->getMap()->getExplosions()->begin(), inext = i; i != _parent->getMap()->getExplosions()->end(); i = inext)
	{
		++inext;
		if(!(*i)->animate())
		{
			_parent->getMap()->getExplosions()->erase((*i));
			if (_parent->getMap()->getExplosions()->empty())
			{
				bool terrainExplosion = false;
				SavedBattleGame *save = _parent->getSave();
				// after the animation is done, the real explosion/hit takes place
				if (_item)
				{
					// heavy explosions, incendiary, smoke or stun bombs create AOE explosions
					// all the rest hits one point:
					// AP, melee (stun or AP), laser, plasma, acid
					if (_item->getRules()->getBattleType() != BT_MELEE && 
						(_item->getRules()->getDamageType() == DT_HE 
						|| _item->getRules()->getDamageType() == DT_IN 
						|| _item->getRules()->getDamageType() == DT_SMOKE
						|| _item->getRules()->getDamageType() == DT_STUN))
					{
						save->getTileEngine()->explode(_center, _item->getRules()->getPower(), _item->getRules()->getDamageType(), _item->getRules()->getExplosionRadius());
					}
					else
					{
						save->getTileEngine()->hit(_center, _item->getRules()->getPower(), _item->getRules()->getDamageType(), _unit);
					}
				}
				if (_tile)
				{
					save->getTileEngine()->explode(_center, _tile->getExplosive(), DT_HE, 100);
				}
				if (!_tile && !_item)
				{
					// explosion not caused by terrain or an item, must be by a unit (cyberdisc)
					save->getTileEngine()->explode(_center, 120, DT_HE, 8);
					terrainExplosion = true;
				}

				// now check for new casualties
				_parent->checkForCasualties(_item, _unit, false, terrainExplosion);

				// if this explosion was caused by a unit shooting, now it's the time to put the gun down
				if (_unit && !_unit->isOut())
				{
					_unit->aim(false);
				}
				_parent->getMap()->cacheUnits();
				_parent->popState();

				// check for terrain explosions
				Tile *t = save->getTileEngine()->checkForTerrainExplosions();
				if (t)
				{
					Position p = Position(t->getPosition().x * 16, t->getPosition().y * 16, t->getPosition().z * 24);
					_parent->statePushNext(new ExplosionBState(_parent, p, 0, _unit, t));
				}

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

}
