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

#include "ExplosionBState.h"
#include "BattlescapeState.h"
#include "Explosion.h"
#include "TileEngine.h"
#include "UnitDieBState.h"
#include "Map.h"
#include "Camera.h"
#include "../Engine/Game.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Sound.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Armor.h"
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
ExplosionBState::ExplosionBState(BattlescapeGame *parent, Position center, BattleItem *item, BattleUnit *unit, Tile *tile) : BattleState(parent), _unit(unit), _center(center), _item(item), _tile(tile), _power(0), _areaOfEffect(false)
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
 * The animation and sound starts here.
 * If the animation is finished, the actually effect takes place.
 */
void ExplosionBState::init()
{
	if (_item)
	{
		_power = _item->getRules()->getPower();
		// heavy explosions, incendiary, smoke or stun bombs create AOE explosions
		// all the rest hits one point:
		// AP, melee (stun or AP), laser, plasma, acid
		_areaOfEffect = _item->getRules()->getBattleType() != BT_MELEE && 
						(_item->getRules()->getDamageType() == DT_HE 
						|| _item->getRules()->getDamageType() == DT_IN 
						|| _item->getRules()->getDamageType() == DT_SMOKE
						|| _item->getRules()->getDamageType() == DT_STUN);
	}
	else if (_tile && _tile->getExplosive())
	{
		_power = _tile->getExplosive();
		_areaOfEffect = true;
	}
	else
	{
		_power = 120;
		_areaOfEffect = true;
	}
	
	Tile *t = _parent->getSave()->getTile(Position(_center.x/16, _center.y/16, _center.z/24));
	if (_areaOfEffect)
	{
		for (int i = 0; i < _power/5; i++)
		{
			int X = RNG::generate(-_power/2,_power/2);
			int Y = RNG::generate(-_power/2,_power/2);
			Position p = _center;
			p.x += X; p.y += Y;
			Explosion *explosion = new Explosion(p, RNG::generate(0,6), true);
			// add the explosion on the map
			_parent->getMap()->getExplosions()->insert(explosion);
		}
		_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED);
		// explosion sound
		if (_power <= 80)
			_parent->getResourcePack()->getSound("BATTLE.CAT", 12)->play();
		else
			_parent->getResourcePack()->getSound("BATTLE.CAT", 5)->play();
		if (t->isDiscovered(0))
			_parent->getMap()->getCamera()->centerOnPosition(t->getPosition());
	}
	else
	// create a bullet hit
	{
		_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED/2);
		bool hit = (_item->getRules()->getBattleType() == BT_MELEE || _item->getRules()->getBattleType() == BT_PSIAMP);
		Explosion *explosion = new Explosion(_center, _item->getRules()->getHitAnimation(), false, hit);
		_parent->getMap()->getExplosions()->insert(explosion);
		// bullet hit sound
		_parent->getResourcePack()->getSound("BATTLE.CAT", _item->getRules()->getHitSound())->play();
// is it really necessary to center bullet hit? this already happen on screen, though not on center.
//		if (t->getVisible() || (t->getUnit() && t->getUnit()->getFaction() == FACTION_PLAYER) || _parent->getSave()->getSide() == FACTION_PLAYER)
//			_parent->getMap()->getCamera()->centerOnPosition(Position(_center.x/16, _center.y/16, _center.z/24));
	}
}

/*
 * Animate explosion sprites. If their animation is finished remove them from the list.
 * If the list is empty, this state is finished and the actual calculations take place.
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
				explode();
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

void ExplosionBState::explode()
{
	bool terrainExplosion = false;
	SavedBattleGame *save = _parent->getSave();
	// after the animation is done, the real explosion/hit takes place
	if (_item)
	{
		if (_areaOfEffect)
		{
			save->getTileEngine()->explode(_center, _power, _item->getRules()->getDamageType(), _item->getRules()->getExplosionRadius(), _unit);
		}
		else
		{
			BattleUnit *victim = save->getTileEngine()->hit(_center, _power, _item->getRules()->getDamageType(), _unit);
			// check if this unit turns others into zombies
			if (!_unit->getZombieUnit().empty() && victim && victim->getArmor()->getSize() == 1)
			{
				// converts the victim to a zombie
				_parent->convertUnit(victim, _unit->getZombieUnit());
			}
		}
	}
	if (_tile)
	{
		save->getTileEngine()->explode(_center, _power, DT_HE, _power/10);
	}
	if (!_tile && !_item)
	{
		// explosion not caused by terrain or an item, must be by a unit (cyberdisc)
		save->getTileEngine()->explode(_center, _power, DT_HE, 6);
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

}

}
