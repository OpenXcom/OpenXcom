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

enum ItemActionStatus { FLYING, EXPLODING, IDLE };

namespace OpenXcom
{

class BattleItem;

/**
 * A utility class that handles firing weapons and throwing grenades.
 */
class ItemAction
{
private:
	BattleItem *_item;
	ItemActionStatus _status;
public:
	/// Creates a new ItemAction class
	ItemAction(BattleItem *item);
	/// Cleans up the ItemAction.
	~ItemAction();
	/// Start the item action.
	void start();
	/// Get the item action status.
	ItemActionStatus getStatus() const;
};

}

#endif
