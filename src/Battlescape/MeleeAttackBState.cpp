/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "MeleeAttackBState.h"
#include "ExplosionBState.h"
#include "BattlescapeGame.h"
#include "BattlescapeState.h"
#include "TileEngine.h"
#include "Pathfinding.h"
#include "Map.h"
#include "InfoboxState.h"
#include "Camera.h"
#include "AlienBAIState.h"
#include "../Savegame/Tile.h"
#include "../Engine/RNG.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Engine/Sound.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleItem.h"
#include "../Ruleset/Armor.h"

namespace OpenXcom
{

/**
 * Sets up a MeleeAttackBState.
 */
MeleeAttackBState::MeleeAttackBState(BattlescapeGame *parent, BattleAction action) : BattleState(parent, action), _unit(0), _target(0), _weapon(0), _ammo(0), _initialized(false), _deathMessage(false)
{
}

/**
 * Deletes the MeleeAttackBState.
 */
MeleeAttackBState::~MeleeAttackBState()
{
}

/**
 * Initializes the sequence.
 * does a lot of validity checking.
 */
void MeleeAttackBState::init()
{
	if (_initialized) return;
	_initialized = true;

	_weapon = _action.weapon;
	_ammo = _weapon->getAmmoItem();

	if (!_weapon) // can't shoot without weapon
	{
		_parent->popState();
		return;
	}

	if (!_parent->getSave()->getTile(_action.target)) // invalid target position
	{
		_parent->popState();
		return;
	}

	_unit = _action.actor;
	
	if (_unit->isOut() || _unit->getHealth() == 0 || _unit->getHealth() < _unit->getStunlevel())
	{
		// something went wrong - we can't shoot when dead or unconscious, or if we're about to fall over.
		_parent->popState();
		return;
	}

	// reaction fire
	if (_unit->getFaction() != _parent->getSave()->getSide())
	{
		// no ammo or target is dead: give the time units back and cancel the shot.
		if (_ammo == 0
			|| !_parent->getSave()->getTile(_action.target)->getUnit()
			|| _parent->getSave()->getTile(_action.target)->getUnit()->isOut()
			|| _parent->getSave()->getTile(_action.target)->getUnit() != _parent->getSave()->getSelectedUnit())
		{
			_unit->setTimeUnits(_unit->getTimeUnits() + _unit->getActionTUs(_action.type, _action.weapon));
			_parent->popState();
			return;
		}
		_unit->lookAt(_action.target, _unit->getTurretType() != -1);
		while (_unit->getStatus() == STATUS_TURNING)
		{
			_unit->turn();
		}
	}

	
	AlienBAIState *ai = dynamic_cast<AlienBAIState*>(_unit->getCurrentAIState());

	if (ai && ai->getTarget())
	{
		_target = ai->getTarget();
	}
	else
	{
		_target = _parent->getSave()->getTile(_action.target)->getUnit();
	}

	int height = _target->getFloatHeight() + (_target->getHeight() / 2) - _parent->getSave()->getTile(_action.target)->getTerrainLevel();
	_voxel = _action.target * Position(16, 16, 24) + Position(8, 8, height);

	performMeleeAttack();
}
/**
 * Performs all the overall functions of the state, this code runs AFTER the explosion state pops.
 */
void MeleeAttackBState::think()
{
	if (_deathMessage)
	{
		Game *game = _parent->getSave()->getBattleState()->getGame();
		game->pushState(new InfoboxState(game->getLanguage()->getString("STR_HAS_BEEN_KILLED", _target->getGender()).arg(_target->getName(game->getLanguage()))));
		_parent->popState();
		return;
	}
	_parent->getSave()->getBattleState()->clearMouseScrollingState();

	// if the unit burns floortiles, burn floortiles
	if (_unit->getSpecialAbility() == SPECAB_BURNFLOOR || _unit->getSpecialAbility() == SPECAB_BURN_AND_EXPLODE)
	{
		_parent->getSave()->getTile(_action.target)->ignite(15);
	}
	// Determine if the attack was successful
	// we do this here instead of letting the explosionBState take care of damage and casualty checking
	// this is because unlike regular bullet hits or explosions, melee attacks can MISS.
	// we also do it at this point instead of in performMeleeAttack because we want the scream to come AFTER the hit sound
	resolveHit();
		// aliens
	if (_unit->getFaction() != FACTION_PLAYER &&
		// with enough TU for a second attack (*2 because they'll get charged for the initial attack when this state pops.)
		_unit->getTimeUnits() >= _unit->getActionTUs(BA_HIT, _action.weapon) * 2 &&
		// whose target is still alive or at least conscious
		_target && _target->getHealth() > 0 &&
		_target->getHealth() > _target->getStunlevel() &&
		// and we still have ammo to make the attack
		_weapon->getAmmoItem())
	{
		// spend the TUs immediately
		_unit->spendTimeUnits(_unit->getActionTUs(BA_HIT, _weapon));
		performMeleeAttack();
	}
	else
	{
		if (_action.cameraPosition.z != -1)
		{
			_parent->getMap()->getCamera()->setMapOffset(_action.cameraPosition);
			_parent->getMap()->invalidate();
		}
//		melee doesn't trigger a reaction, remove comments to enable.
//		if (!_parent->getSave()->getUnitsFalling())
//		{
//			_parent->getTileEngine()->checkReactionFire(_unit);
//		}
		
		if (_parent->getSave()->getSide() == FACTION_PLAYER || _parent->getSave()->getDebugMode())
		{
			_parent->setupCursor();
		}
		if (_parent->convertInfected() && Options::battleNotifyDeath && _target && _target->getFaction() == FACTION_PLAYER)
		{
			_deathMessage = true;
			_parent->setStateInterval(BattlescapeState::DEFAULT_ANIM_SPEED);
		}
		else
		{
			_parent->popState();
		}
	}
}

/**
 * Sets up a melee attack, inserts an explosion into the map and make noises.
 */
void MeleeAttackBState::performMeleeAttack()
{
	// set the soldier in an aiming position
	_unit->aim(true);
	_unit->setCache(0);
	_parent->getMap()->cacheUnit(_unit);
	// make some noise
	if (_ammo && _ammo->getRules()->getMeleeAttackSound() != -1)
	{
		_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), _ammo->getRules()->getMeleeAttackSound())->play(-1, _parent->getMap()->getSoundAngle(_action.target));
	}
	else if (_weapon->getRules()->getMeleeAttackSound() != -1)
	{
		_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), _weapon->getRules()->getMeleeAttackSound())->play(-1, _parent->getMap()->getSoundAngle(_action.target));
	}
	// use up ammo if applicable
	if (!_parent->getSave()->getDebugMode() && _weapon->getRules()->getBattleType() == BT_MELEE && _ammo && _ammo->spendBullet() == false)
	{
		_parent->getSave()->removeItem(_ammo);
		_action.weapon->setAmmoItem(0);
	}
	_parent->getMap()->setCursorType(CT_NONE);
	
	// make an explosion animation
	_parent->statePushFront(new ExplosionBState(_parent, _voxel, _action.weapon, _action.actor, 0, true, true));
}

/**
 * Determines if the melee attack hit, and performs all the applicable duties.
 */
void MeleeAttackBState::resolveHit()
{
	if (RNG::percent(_unit->getFiringAccuracy(BA_HIT, _weapon)))
	{	
		// Give soldiers XP
		if (_unit->getGeoscapeSoldier() &&
			_target && _target->getOriginalFaction() == FACTION_HOSTILE)
		{
			_unit->addMeleeExp();
		}

		// check if this unit turns others into zombies
		if (!_ammo->getRules()->getZombieUnit().empty()
			&& _target
			&& (_target->getGeoscapeSoldier() || _target->getUnitRules()->getRace() == "STR_CIVILIAN")
			&& _target->getSpawnUnit().empty())
		{
			// converts the victim to a zombie on death
			_target->setRespawn(true);
			_target->setSpawnUnit(_ammo->getRules()->getZombieUnit());
		}

		ItemDamageType type = _ammo->getRules()->getDamageType();
		int power = _ammo->getRules()->getPower();

		// special code for attacking with a rifle butt.
		if (_weapon->getRules()->getBattleType() == BT_FIREARM)
		{
			type = DT_STUN;
			power = _weapon->getRules()->getMeleePower();
		}

		// since melee aliens don't use a conventional weapon type, we use their strength instead.
		if (_weapon->getRules()->isStrengthApplied())
		{
			power += _unit->getBaseStats()->strength;
		}
		// make some noise to signal the hit.
		if (_weapon->getRules()->getMeleeHitSound() != -1)
		{
			_parent->getResourcePack()->getSoundByDepth(_parent->getDepth(), _action.weapon->getRules()->getMeleeHitSound())->play(-1, _parent->getMap()->getSoundAngle(_action.target));
		}
		
		// offset the damage voxel ever so slightly so that the target knows which side the attack came from
		Position difference = _unit->getPosition() - _action.target;
		// large units may cause it to offset too much, so we'll clamp the values.
		difference.x = std::max(-1, std::min(1, difference.x));
		difference.y = std::max(-1, std::min(1, difference.y));

		Position damagePosition = _voxel + difference;
		// damage the unit.
		_parent->getSave()->getTileEngine()->hit(damagePosition, power, type, _unit);
		// now check for new casualties
		_parent->checkForCasualties(_ammo, _unit);
	}
}

}
