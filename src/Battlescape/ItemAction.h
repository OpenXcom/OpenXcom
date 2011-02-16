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
#ifndef OPENXCOM_ITEMACTION_H
#define OPENXCOM_ITEMACTION_H

#include <vector>
#include "Position.h"


namespace OpenXcom
{

class BattleItem;

enum ItemActionStatus { PENDING, ERROR, LAUNCHED, IMPACT, FINISHED };
enum ItemActionType { THROW, AUTO_SHOT, SNAP_SHOT, AIMED_SHOT, OPEN, CLOSE, STUN, PRIME_GRENADE, USE_SCANNER, USE_MEDIKIT, LAUNCH_MISSILE };

/**
 * A class that handles firing weapons and throwing grenades.
 */
class ItemAction
{
private:
	BattleItem *_item;
	ItemActionStatus _status;
	ItemActionType _type;
public:
	/// Creates a new ItemAction class
	ItemAction(BattleItem *item, ItemActionType type);
	/// Cleans up the ItemAction.
	~ItemAction();
	/// Start the item action.
	void start();
	/// Get the item action status.
	ItemActionStatus getStatus() const;
};

}

#endif
