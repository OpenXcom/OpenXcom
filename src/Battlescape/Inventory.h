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
#ifndef OPENXCOM_INVENTORY_H
#define OPENXCOM_INVENTORY_H

#include "../Engine/InteractiveSurface.h"
#include <vector>

namespace OpenXcom
{

class RuleInventory;
class Game;

/**
 * Interactive view of an inventory.
 * Lets the player view and manage a soldier's equipment.
 */
class Inventory : public InteractiveSurface
{
private:
	static const int SLOT_W = 16;
	static const int SLOT_H = 16;
	static const int HAND_W = 32;
	static const int HAND_H = 48;

	Game *_game;
	std::vector<RuleInventory*> *_invs;
public:
	/// Creates a new inventory view at the specified position and size.
	Inventory(Game *game, int width, int height, int x = 0, int y = 0);
	/// Cleans up the inventory.
	~Inventory();
	/// Sets the ruleset to use for the inventory.
	void setRuleInventory(std::vector<RuleInventory*> *invs);
	/// Draws the inventory.
	void draw();
};

}

#endif
