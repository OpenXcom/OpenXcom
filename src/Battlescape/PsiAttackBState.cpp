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
#include "PsiAttackBState.h"
#include "ExplosionBState.h"
#include "BattlescapeGame.h"
#include "BattlescapeState.h"
#include "TileEngine.h"
#include "InfoboxState.h"
#include "Map.h"
#include "Camera.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Engine/Game.h"
#include "../Engine/RNG.h"
#include "../Engine/Language.h"
#include "../Engine/Sound.h"
#include "../Mod/Mod.h"
#include "../Savegame/BattleUnitStatistics.h"

namespace OpenXcom
{

/**
 * Sets up a PsiAttackBState.
 */
PsiAttackBState::PsiAttackBState(BattlescapeGame *parent, BattleAction action) : BattleState(parent, action), _unit(0), _item(0), _initialized(false)
{
}

/**
 * Deletes the PsiAttackBState.
 */
PsiAttackBState::~PsiAttackBState()
{
}

/**
 * Initializes the sequence:
 * - checks if the action is valid,
 * - adds a psi attack animation to the world.
 * - from that point on, the explode state takes precedence.
 * - when that state pops, we'll do our first think()
 */
void PsiAttackBState::init()
{
	if (_initialized) return;
	_initialized = true;

	_item = _action.weapon;
	_unit = _action.actor;

	if (!_parent->getSave()->getTile(_action.target)) // invalid target position
	{
		_parent->popState();
		return;
	}

	if (_unit->getTimeUnits() < _action.TU) // not enough time units
	{
		_action.result = "STR_NOT_ENOUGH_TIME_UNITS";
		_parent->popState();
		return;
	}

	_target = _parent->getSave()->getTile(_action.target)->getUnit();

	if (!_target) // invalid target
	{
		_parent->popState();
		return;
	}

	if (!_item) // can't make a psi attack without a weapon
	{
		_parent->popState();
		return;
	}
	else if (_item->getRules()->getHitSound() != -1)
	{
		_parent->getMod()->getSoundByDepth(_parent->getDepth(), _item->getRules()->getHitSound())->play(-1, _parent->getMap()->getSoundAngle(_action.target));
	}

	// make a cosmetic explosion
	int height = _target->getFloatHeight() + (_target->getHeight() / 2) - _parent->getSave()->getTile(_action.target)->getTerrainLevel();
	Position voxel = _action.target * Position(16, 16, 24) + Position(8, 8, height);
	_parent->statePushFront(new ExplosionBState(_parent, voxel, _item, _unit, 0, false, true));
}


/**
 * After the explosion animation is done doing its thing,
 * make the actual psi attack, and restore the camera/cursor.
 */
void PsiAttackBState::think()
{
	//make the psi attack.
	psiAttack();

	if (_action.cameraPosition.z != -1)
	{
		_parent->getMap()->getCamera()->setMapOffset(_action.cameraPosition);
		_parent->getMap()->invalidate();
	}
	if (_parent->getSave()->getSide() == FACTION_PLAYER || _parent->getSave()->getDebugMode())
	{
		_parent->setupCursor();
	}
	_parent->popState();
}

/**
 * Attempts a panic or mind control action.
 */
void PsiAttackBState::psiAttack()
{
	double attackStrength = _unit->getBaseStats()->psiStrength * _unit->getBaseStats()->psiSkill / 50.0;
	double defenseStrength = _target->getBaseStats()->psiStrength
		+ ((_target->getBaseStats()->psiSkill > 0) ? 10.0 + _target->getBaseStats()->psiSkill / 5.0 : 10.0);
	double dist = _parent->getTileEngine()->distance(_unit->getPosition(), _action.target);
	attackStrength -= dist;
	attackStrength += RNG::generate(0,55);

	if (_action.type == BA_MINDCONTROL)
	{
		defenseStrength += 20;
	}
	
	_unit->addPsiSkillExp();
	if (Options::allowPsiStrengthImprovement) _target->addPsiStrengthExp();
	if (attackStrength > defenseStrength)
	{
		Game *game = _parent->getSave()->getBattleState()->getGame();
		_action.actor->addPsiSkillExp();
		_action.actor->addPsiSkillExp();

		BattleUnitKills *killStat = new BattleUnitKills();
		killStat->setUnitStats(_target);
		killStat->setTurn(_parent->getSave()->getTurn(), _parent->getSave()->getSide());
		killStat->weapon = _action.weapon->getRules()->getName();
		killStat->weaponAmmo = _action.weapon->getRules()->getName();
		killStat->faction = _target->getFaction();
		killStat->mission = _parent->getSave()->getGeoscapeSave()->getMissionStatistics()->size();
		killStat->id = _target->getId();

		if (_action.type == BA_PANIC)
		{
			int moraleLoss = (110-_target->getBaseStats()->bravery);
			if (moraleLoss > 0)
			_target->moraleChange(-moraleLoss);
			// Award Panic battle unit kill
			if (!_unit->getStatistics()->duplicateEntry(STATUS_PANICKING, _target->getId()))
			{
				killStat->status = STATUS_PANICKING;
				_unit->getStatistics()->kills.push_back(killStat);
				_target->setMurdererId(_unit->getId());
			}
			if (_parent->getSave()->getSide() == FACTION_PLAYER)
			{
				game->pushState(new InfoboxState(game->getLanguage()->getString("STR_MORALE_ATTACK_SUCCESSFUL")));
			}
		}
		else if (_action.type == BA_MINDCONTROL)
		{
			// Award MC battle unit kill
			if (!_unit->getStatistics()->duplicateEntry(STATUS_TURNING, _target->getId()))
			{
				killStat->status = STATUS_TURNING;
				_unit->getStatistics()->kills.push_back(killStat);
				_target->setMurdererId(_unit->getId());
			}
			_target->convertToFaction(_unit->getFaction());
			_parent->getTileEngine()->calculateFOV(_target->getPosition());
			_parent->getTileEngine()->calculateUnitLighting();
			_target->recoverTimeUnits();
			_target->allowReselect();
			_target->abortTurn(); // resets unit status to STANDING
			// if all units from either faction are mind controlled - auto-end the mission.
			if (_parent->getSave()->getSide() == FACTION_PLAYER)
			{
				if (Options::battleAutoEnd && Options::allowPsionicCapture)
				{
					int liveAliens = 0;
					int liveSoldiers = 0;
					_parent->tallyUnits(liveAliens, liveSoldiers);
					if (liveAliens == 0 || liveSoldiers == 0)
					{
						_parent->getSave()->setSelectedUnit(0);
						_parent->cancelCurrentAction(true);
						_parent->requestEndTurn();
					}
				}
				game->pushState(new InfoboxState(game->getLanguage()->getString("STR_MIND_CONTROL_SUCCESSFUL")));
				_parent->getSave()->getBattleState()->updateSoldierInfo();
			}
			else
			{
				// show a little infobox with the name of the unit and "... is under alien control"
				game->pushState(new InfoboxState(game->getLanguage()->getString("STR_IS_UNDER_ALIEN_CONTROL", _target->getGender()).arg(_target->getName(game->getLanguage()))));
			}
		}

		// Cleanup unused stats
		if (killStat->status == STATUS_IGNORE_ME)
		{
			delete killStat;
		}
	}
	else
	{
		if (Options::allowPsiStrengthImprovement)
		{
			_target->addPsiStrengthExp();
		}
	}
}

}
