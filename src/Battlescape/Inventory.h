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
#include <map>
#include <string>

namespace OpenXcom
{

class RuleInventory;
class Game;
class BattleItem;

/**
 * Interactive view of an inventory.
 * Lets the player view and manage a soldier's equipment.
 */
class Inventory : public InteractiveSurface
{
private:
	Game *_game;
	std::map<std::string, RuleInventory*> *_invs;
	Surface *_grid, *_items, *_selection;
	BattleItem *_selItem;
public:
	/// Creates a new inventory view at the specified position and size.
	Inventory(Game *game, int width, int height, int x = 0, int y = 0);
	/// Cleans up the inventory.
	~Inventory();
	/// Sets the inventory's palette.
	void setPalette(SDL_Color *colors, int firstcolor = 0, int ncolors = 256);
	/// Draws the inventory.
	void draw();
	/// Draws the inventory grid.
	void drawGrid();
	/// Draws the inventory items.
	void drawItems();
	/// Gets the item in the specified slot.
	BattleItem *getItemInSlot(std::string slot, int x, int y) const;
	/// Gets the slot in the specified position.
	std::string getSlotInPosition(int *x, int *y) const;
	/// Blits the inventory onto another surface.
	void blit(Surface *surface);
	/// Special handling for mouse hovers.
	void mouseOver(Action *action, State *state);
	/// Special handling for mouse clicks.
	void mouseClick(Action *action, State *state);
};

}

#endif
