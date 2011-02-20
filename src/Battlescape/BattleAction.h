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
#ifndef OPENXCOM_BATTLEACTION_H
#define OPENXCOM_BATTLEACTION_H

#include <vector>
#include "Position.h"


namespace OpenXcom
{

class BattleItem;
class BattleUnit;
class SavedBattleGame;
class ResourcePack;
class Projectile;

enum BattleActionStatus { PENDING, INPROGRESS, FINISHED };
enum BattleActionType { THROW, AUTO_SHOT, SNAP_SHOT, AIMED_SHOT, STUN, HIT, EXPLODE, WALK, TURN };

/**
 * This class handles battlescape actions: stuff a unit or an item does that can affect the battlescape terrain or units.
 * These actions can be triggered by the player or the AI.
 */
class BattleAction
{
private:
	SavedBattleGame *_save;
	ResourcePack *_res;
	BattleItem *_item;
	BattleActionStatus _status;
	BattleActionType _type;
	std::string _result;
	Position _target;
	bool unitOpensDoor(BattleUnit *unit);
	Projectile *_projectile;
public:
	/// Creates a new BattleAction class
	BattleAction(SavedBattleGame *save, ResourcePack *res, BattleItem *item, BattleActionType type);
	/// Cleans up the BattleAction.
	~BattleAction();
	/// Start the BattleAction.
	void start();
	/// Set target of the BattleAction.
	void setTarget(const Position &target);
	/// The bullet action triggered by a specific timer.
	void moveBullet();
	/// The unit action triggered by a specific timer.
	void moveUnit();
	/// The animation action triggered by a sepecific timer.
	void animate();
	/// Cancels the action.
	void cancel();
	/// Get result of the action.
	std::string getResult() const;
	/// Get the battle action status.
	BattleActionStatus getStatus() const;
	/// Get the battle action type.
	BattleActionType getType() const;
};

}

#endif
