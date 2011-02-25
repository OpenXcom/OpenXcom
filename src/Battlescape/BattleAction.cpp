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
#include "BattleAction.h"
#include "Pathfinding.h"
#include "TerrainModifier.h"
#include "Projectile.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "../Resource/ResourcePack.h"
#include "../Resource/XcomResourcePack.h"
#include "../Engine/RNG.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"
#include "../Ruleset/RuleItem.h"


namespace OpenXcom
{

/**
 * Sets up an BattleAction.
 * @param item pointer to the item object.
 */
BattleAction::BattleAction(SavedBattleGame *save, ResourcePack *res, BattleItem *item, BattleActionType type) : _save(save), _res(res), _item(item), _status(PENDING), _type(type), _result(""), _projectile(0), _animFrame(0)
{

}

/**
 * Deletes the BattleAction.
 */
BattleAction::~BattleAction()
{
	delete _projectile;
}

/**
 * Set the target.
 * todo: multiple targets are possible with blaster launchers, so we need list of targets here with add/remove
 * @param target Position in tile x/y/z.
 */
void BattleAction::setTarget(const Position &target)
{
	_target = target;
}

/**
 * Start the action.
 * Maybe there's a more elegant way to write this.
 */
void BattleAction::start()
{
	if (_type == THROW)
	{

	}
	else if (_type == AUTO_SHOT)
	{

	}
	else if (_type == SNAP_SHOT)
	{
		_save->getSelectedUnit()->aim(true);
		_projectile = new Projectile(_res, _save, _save->getSelectedUnit()->getPosition(), _target, _item->getRules()->getBulletSprite());
		if (_projectile->calculateTrajectory())
		{
			_status = INPROGRESS;
			_res->getSoundSet("BATTLE.CAT")->getSound(_item->getRules()->getFireSound())->play();
		}
		else
		{
			_status = FINISHED; // no line of fire
		}
	}
	else if (_type == AIMED_SHOT)
	{

	}
	else if (_type == STUN)
	{

	}
	else if (_type == HIT)
	{

	}
	else if (_type == EXPLODE)
	{

	}
	else if (_type == WALK)
	{
		Pathfinding *pf = _save->getPathfinding();
		pf->calculate(_save->getSelectedUnit(), _target);
		_status = INPROGRESS;
	}
	else if (_type == TURN)
	{
		BattleUnit *unit = _save->getSelectedUnit();
		unit->lookAt(_target);
		_status = INPROGRESS;
		if (unit->getStatus() != STATUS_TURNING)
		{
			unitOpensDoor(unit);
			_status = FINISHED;
		}
	}


}

/**
 * moveBullet is triggered by a timer. It advances the projectile, until the trajectory finished.
 */
void BattleAction::moveBullet()
{
	if (_projectile)
	{
		if(!_projectile->move())
		{
			// impact !
			_res->getSoundSet("BATTLE.CAT")->getSound(_item->getRules()->getHitSound())->play();
			_position = _projectile->getPosition(-1);
			delete _projectile;
			_projectile = 0;
			_save->getSelectedUnit()->aim(false);
			_type = HIT;
		}
	}
}

/**
 * moveUnit is triggered by a timer to advance unit movement.
 */
void BattleAction::moveUnit()
{
	int tu = 0;
	BattleUnit *unit = _save->getSelectedUnit();
	Pathfinding *pf = _save->getPathfinding();

	if (unit->getStatus() == STATUS_TURNING)
	{
		unit->turn(_type == TURN);
		_save->getTerrainModifier()->calculateFOV(unit);
		// if we finished turning and the action was to turn, the action is finished
		if (unit->getStatus() == STATUS_STANDING && _type == TURN)
		{
			_status = FINISHED;
		}
	}

	// during a walking cycle we make step sounds
	if (unit->getStatus() == STATUS_WALKING)
	{
		// play footstep sound 1
		if (unit->getWalkingPhase() == 3)
		{
			Tile *tile = _save->getTile(unit->getPosition());
			if (tile->getFootstepSound())
			{
				_res->getSoundSet("BATTLE.CAT")->getSound(22 + (tile->getFootstepSound()*2))->play();
			}
		}
		// play footstep sound 2
		if (unit->getWalkingPhase() == 7)
		{
			Tile *tile = _save->getTile(unit->getPosition());
			if (tile->getFootstepSound())
			{
				_res->getSoundSet("BATTLE.CAT")->getSound(23 + (tile->getFootstepSound()*2))->play();
			}
		}

		unit->keepWalking(); // advances the phase

		// unit moved from one tile to the other, update the tiles
		if (unit->getPosition() != unit->getLastPosition())
		{
			_save->getTile(unit->getLastPosition())->setUnit(0);
			_save->getTile(unit->getPosition())->setUnit(unit);
		}

		// is the walking cycle finished?
		if (unit->getStatus() == STATUS_STANDING)
		{
			_save->getTerrainModifier()->calculateFOV(unit);
			// if you want lighting to be calculated every step, uncomment next line
			//_save->getTerrainModifier()->calculateLighting();
		}
	}

	// we are just standing around, shouldn't we be walking?
	if (unit->getStatus() == STATUS_STANDING)
	{
		int dir = pf->getStartDirection();
		if (dir != -1)
		{
			// we are looking in the wrong way, turn first
			if (dir != unit->getDirection()) 
			{
				unit->lookAt(dir);
				return;
			}

			// now open doors (if any)
			if (unitOpensDoor(unit))
			{
				return; // don't start walking yet, wait for the ufo door to open
			}

			// now start moving
			dir = pf->dequeuePath();
			Position destination;
			tu = pf->getTUCost(unit->getPosition(), dir, &destination, unit);
			unit->startWalking(dir, destination);
		}
		else
		{
			// no more waypoints, this means we succesfully finished this action
			_save->getTerrainModifier()->calculateUnitLighting();
			_status = FINISHED;
		}
	}
}

/**
 * animate animates explosions and bullet impacts. Triggered by the animation timer.
 */
void BattleAction::animate()
{
	if (_type == HIT)
	{
		_animFrame++;
		if (_animFrame == 10)
			_status = FINISHED;
	}
}

/**
 * Cancel the current action (if it was still pending) or try to abort the walking sequence.
 */
void BattleAction::cancel()
{
	if (_status == INPROGRESS && _type == WALK)
	{
		_save->getPathfinding()->abortPath();
	}
	else if (_status == PENDING)
	{
		_status = FINISHED;
	}
}

/*
 * Get the action result. Returns error messages or an empty string when everything went fine.
 * @return returnmessage Empty when everything is fine.
 */
std::string BattleAction::getResult() const
{
	return _result;
}

/**
 * Returns the action's status.
 * @return actionstatus PENDING/INPROGRESS/FINISHED
 */
BattleActionStatus BattleAction::getStatus() const
{
	return _status;
}

/**
 * Returns the action's type.
 * @return actiontype
 */
BattleActionType BattleAction::getType() const
{
	return _type;
}

/**
 * Soldier opens a door (if any) by rightclick, or by walking through it.
 * Normal door changes the tile objects. We need to make a sound 3 here.
 * Ufo door updates the ufodooropened flag of the tile. We need to make a sound 20 or 21 and start the animation.
 * An ufo door takes a few animation frames: while this animation is running this function returns true.
 * @param unit
 * @return false if there is no door
 */
bool BattleAction::unitOpensDoor(BattleUnit *unit)
{
	int door = -1;
	Tile *tile = _save->getTile(unit->getPosition());
	switch(unit->getDirection())
	{
	case 0:	// north
		door = tile->openDoor(O_NORTHWALL);
		break;
	case 2: // east
		tile = _save->getTile(tile->getPosition() + Position(1, 0, 0));
		if (tile) door = tile->openDoor(O_WESTWALL);
		break;
	case 4: // south
		tile = _save->getTile(tile->getPosition() + Position(0, -1, 0));
		if (tile) door = tile->openDoor(O_NORTHWALL);
		break;
	case 6: // west
		door = tile->openDoor(O_WESTWALL);
		break;
	}

	if (door == 0) //normal door
	{
		_res->getSoundSet("BATTLE.CAT")->getSound(3)->play();
		_save->getTerrainModifier()->calculateFOV(tile->getPosition());
		return true;
	}
	if (door == 1) // ufo door
	{
		_res->getSoundSet("BATTLE.CAT")->getSound(RNG::generate(20,21))->play();
		_save->getTerrainModifier()->calculateFOV(tile->getPosition());
		return true;
	}
	if (door == 3) // ufo door opening
	{
		return true;
	}

	return false;
}

/**
 * Get the projectile type.
 * Depending on the weapon a different projectile sprite is drawn, there are 11 different types.
 * @return type 0-10
 */
int BattleAction::getProjectileType() const
{
	if (_projectile)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * Get position (of the projectile) in voxel space.
 * @param offset Used for trailing particles that need to be drawn at previous positions.
 * @return Position in voxel space.
 */
Position BattleAction::getPosition(int offset) const
{
	if (_projectile)
		return _projectile->getPosition(offset);
	else
		return _position;
}

/**
 * Get the particle ID. This is needed for drawing the projectile + trailing particles.
 * @param offset
 * @return particle ID
 */
int BattleAction::getProjectileParticle(int offset) const
{
	return _projectile->getParticle(offset);
}

/**
 * Get the hit visual effect animation frame.
 * @return frame ID
 */
int BattleAction::getAnimFrame() const
{
	return _animFrame;
}

}
