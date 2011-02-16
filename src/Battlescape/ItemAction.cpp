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
#include "ItemAction.h"
#include "../Savegame/BattleItem.h"

namespace OpenXcom
{

/**
 * Sets up an ItemAction.
 * @param item pointer to the item object.
 */
ItemAction::ItemAction(BattleItem *item, ItemActionType type) : _item(item), _status(PENDING), _type(type)
{

}

/**
 * Deletes the ItemAction.
 */
ItemAction::~ItemAction()
{
}

/**
 * Start the item action.
 */
void ItemAction::start()
{

}

/// Get the item action status.
ItemActionStatus ItemAction::getStatus() const
{
	return _status;
}


}
