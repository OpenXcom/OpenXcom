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
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/Armor.h"
#include "../Engine/RNG.h"

namespace OpenXcom
{

/**
 * Sets up an ExplosionBState.
 * @param parent Pointer to the BattleScape.
 * @param center Center position in voxelspace.
 * @param item Item involved in the explosion (eg grenade).
 * @param unit Unit involved in the explosion (eg unit throwing the grenade).
 * @param tile Tile the explosion is on.
 * @param lowerWeapon Whether the unit causing this explosion should now lower their weapon.
 */
ExplosionBState::ExplosionBState(BattlescapeGame *parent, Position center, BattleItem *item, BattleUnit *unit, Tile *tile, bool lowerWeapon) : BattleState(parent), _unit(unit), _center(center), _item(item), _tile(tile), _power(0), _areaOfEffect(false), _lowerWeapon(lowerWeapon), _pistolWhip(false)
{

}

/**
 * Deletes the ExplosionBState.
 */
ExplosionBState::~ExplosionBState()
{

}

/**
 * Initializes the explosion.
 * The animation and sound starts here.
 * If the animation is finished, the actual effect takes place.
 */
void ExplosionBState::init()
{
	if (_item)
	{
		_power = _item->getRules()->getPower();
		_pistolWhip = (_item->getRules()->getBattleType() != BT_MELEE &&
			_parent->getCurrentAction()->type == BA_HIT);
		if (_pistolWhip)
		{
			_power = _item->getRules()->getMeleePower();
		}
		// since melee aliens don't use a conventional weapon type, we use their strength instead.
		if (_item->getRules()->isStrengthApplied())
		{
			_power += _unit->getStats()->strength;
		}

		_areaOfEffect = _item->getRules()->getBattleType() != BT_MELEE &&
						_item->getRules()->getExplosionRadius() != 0 &&
						!_pistolWhip;
	}
	else if (_tile)
	{
		_power = _tile->getExplosive();
		_areaOfEffect = true;
	}
	else if (_unit && _unit->getSpecialAbility() == SPECAB_EXPLODEONDEATH)
	{
		_power = _parent->getRuleset()->getItem(_unit->getArmor()->getCorpseGeoscape())->getPower();
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
		if (_power)
		{
			int frame = 0;
			int counter = std::max(1, (_power/5) / 5);
			for (int i = 0; i < _power/5; i++)
			{
				int X = RNG::generate(-_power/2,_power/2);
				int Y = RNG::generate(-_power/2,_power/2);
				Position p = _center;
				p.x += X; p.y += Y;
				Explosion *explosion = new Explosion(p, frame, true);
				// add the explosion on the map
				_parent->getMap()->getExplosions()->insert(explosion);
				if (i > 0 && i % counter == 0)
				{
					--frame;
				}
			}
			_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED/2);
			// explosion sound
			if (_power <= 80)
				_parent->getResourcePack()->getSound("BATTLE.CAT", 2)->play();
			else
				_parent->getResourcePack()->getSound("BATTLE.CAT", 5)->play();

			_parent->getMap()->getCamera()->centerOnPosition(t->getPosition(), false);
		}
		else
		{
			_parent->popState();
		}
	}
	else
	// create a bullet hit
	{
		_parent->setStateInterval(std::max(1, ((BattlescapeState::DEFAULT_ANIM_SPEED/2) - (10 * _item->getRules()->getExplosionSpeed()))));
		bool hit = _pistolWhip || _item->getRules()->getBattleType() == BT_MELEE || _item->getRules()->getBattleType() == BT_PSIAMP;
		int anim = _item->getRules()->getHitAnimation();
		int sound = _item->getRules()->getHitSound();
		if (hit)
		{
			anim = _item->getRules()->getMeleeAnimation();
		}
		if (sound != -1)
		{
			// bullet hit sound
			_parent->getResourcePack()->getSound("BATTLE.CAT", sound)->play();
		}
		Explosion *explosion = new Explosion(_center, anim, false, hit);
		_parent->getMap()->getExplosions()->insert(explosion);
		_parent->getMap()->getCamera()->setViewLevel(_center.z / 24);

		BattleUnit *target = t->getUnit();
		if (hit && _parent->getSave()->getSide() == FACTION_HOSTILE && target && target->getFaction() == FACTION_PLAYER)
		{
			_parent->getMap()->getCamera()->centerOnPosition(t->getPosition(), false);
		}
	}
}

/**
 * Animates explosion sprites. If their animation is finished remove them from the list.
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

/**
 * Explosions cannot be cancelled.
 */
void ExplosionBState::cancel()
{
}

/**
 * Calculates the effects of the explosion.
 */
void ExplosionBState::explode()
{
	bool terrainExplosion = false;
	SavedBattleGame *save = _parent->getSave();
	// last minute adjustment: determine if we actually 
	if (_parent->getCurrentAction()->type == BA_HIT || _parent->getCurrentAction()->type == BA_STUN)
	{
		save->getBattleGame()->getCurrentAction()->type = BA_NONE;
		BattleUnit *targetUnit = save->getTile(_center / Position(16, 16, 24))->getUnit();
		if (_unit && !_unit->isOut())
		{
			_unit->aim(false);
			_unit->setCache(0);
		}
		if (!RNG::percent(_unit->getFiringAccuracy(BA_HIT, _item)))
		{
			_parent->getMap()->cacheUnits();
			_parent->popState();
			return;
		}
		else if (targetUnit && targetUnit->getOriginalFaction() == FACTION_HOSTILE &&
				_unit->getOriginalFaction() == FACTION_PLAYER)
		{
			_unit->addMeleeExp();
		}
		if (_item->getRules()->getMeleeHitSound() != -1)
		{
			_parent->getResourcePack()->getSound("BATTLE.CAT", _item->getRules()->getMeleeHitSound())->play();
		}
	}
	// after the animation is done, the real explosion/hit takes place
	if (_item)
	{
		if (!_unit && _item->getPreviousOwner())
		{
			_unit = _item->getPreviousOwner();
		}

		if (_areaOfEffect)
		{
			save->getTileEngine()->explode(_center, _power, _item->getRules()->getDamageType(), _item->getRules()->getExplosionRadius(), _unit);
		}
		else
		{
			ItemDamageType type = _item->getRules()->getDamageType();
			if (_pistolWhip)
			{
				type = DT_STUN;
			}
			BattleUnit *victim = save->getTileEngine()->hit(_center, _power, type, _unit);
			// check if this unit turns others into zombies
			if (!_item->getRules()->getZombieUnit().empty()
				&& victim
				&& victim->getArmor()->getSize() == 1
				&& victim->getSpawnUnit().empty()
				&& victim->getOriginalFaction() != FACTION_HOSTILE)
			{
				// converts the victim to a zombie on death
				victim->setSpecialAbility(SPECAB_RESPAWN);
				victim->setSpawnUnit(_item->getRules()->getZombieUnit());
			}
		}
	}
	if (_tile)
	{
		save->getTileEngine()->explode(_center, _power, DT_HE, _power/10);
		terrainExplosion = true;
	}
	if (!_tile && !_item)
	{
		int radius = 6;
		// explosion not caused by terrain or an item, must be by a unit (cyberdisc)
		if (_unit && _unit->getSpecialAbility() == SPECAB_EXPLODEONDEATH)
		{
			radius = _parent->getRuleset()->getItem(_unit->getArmor()->getCorpseGeoscape())->getExplosionRadius();
		}
		save->getTileEngine()->explode(_center, _power, DT_HE, radius);
		terrainExplosion = true;
	}

	// now check for new casualties
	_parent->checkForCasualties(_item, _unit, false, terrainExplosion);

	// if this explosion was caused by a unit shooting, now it's the time to put the gun down
	if (_unit && !_unit->isOut() && _lowerWeapon)
	{
		_unit->aim(false);
		_unit->setCache(0);
	}
	_parent->getMap()->cacheUnits();
	_parent->popState();

	// check for terrain explosions
	Tile *t = save->getTileEngine()->checkForTerrainExplosions();
	if (t)
	{
		Position p = Position(t->getPosition().x * 16, t->getPosition().y * 16, t->getPosition().z * 24);
		p += Position(8,8,0);
		_parent->statePushFront(new ExplosionBState(_parent, p, 0, _unit, t));
	}

	if (_item && (_item->getRules()->getBattleType() == BT_GRENADE || _item->getRules()->getBattleType() == BT_PROXIMITYGRENADE))
	{
		for (std::vector<BattleItem*>::iterator j = _parent->getSave()->getItems()->begin(); j != _parent->getSave()->getItems()->end(); ++j)
		{
			if (_item->getId() == (*j)->getId())
			{
				delete *j;
				_parent->getSave()->getItems()->erase(j);
				break;
			}
		}
	}
}

}
