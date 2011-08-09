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
#include "Inventory.h"
#include <cmath>
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleInventory.h"
#include "../Engine/Palette.h"
#include "../Engine/Game.h"
#include "../Interface/Text.h"
#include "../Engine/Font.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/SurfaceSet.h"
#include "../Savegame/BattleItem.h"
#include "../Ruleset/RuleItem.h"
#include "../Savegame/BattleUnit.h"
#include "../Engine/Action.h"
#include "../Engine/SoundSet.h"
#include "../Engine/Sound.h"

namespace OpenXcom
{

/**
 * Sets up an inventory with the specified size and position.
 * @param game Pointer to core game.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
Inventory::Inventory(Game *game, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _game(game), _selItem(0)
{
	_grid = new Surface(width, height, x, y);
	_items = new Surface(width, height, x, y);
	_selection = new Surface(RuleInventory::HAND_W * RuleInventory::SLOT_W, RuleInventory::HAND_H * RuleInventory::SLOT_H, x, y);

	_invs = _game->getRuleset()->getInventories();
}

/**
 * Delete inventory surfaces.
 */
Inventory::~Inventory()
{
	delete _grid;
	delete _items;
	delete _selection;
}

/**
 * Replaces a certain amount of colors in the inventory's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void Inventory::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_grid->setPalette(colors, firstcolor, ncolors);
	_items->setPalette(colors, firstcolor, ncolors);
	_selection->setPalette(colors, firstcolor, ncolors);
}

/**
 * Draws the inventory elements.
 */
void Inventory::draw()
{
	drawGrid();
	drawItems();
}

/**
 * Draws the inventory grid for item placement.
 */
void Inventory::drawGrid()
{
	_grid->clear();
	Text text = Text(100, 9, 0, 0);
	text.setPalette(_grid->getPalette());
	text.setFonts(_game->getResourcePack()->getFont("BIGLETS.DAT"), _game->getResourcePack()->getFont("SMALLSET.DAT"));
	text.setColor(Palette::blockOffset(4)-1);
	text.setHighContrast(true);

	Uint8 color = Palette::blockOffset(0)+8;
	for (std::map<std::string, RuleInventory*>::iterator i = _invs->begin(); i != _invs->end(); ++i)
	{
		// Draw grid
		if (i->second->getType() == INV_SLOT)
		{
			for (std::vector<RuleSlot>::iterator j = i->second->getSlots()->begin(); j != i->second->getSlots()->end(); ++j)
			{
				SDL_Rect r;
				r.x = i->second->getX() + RuleInventory::SLOT_W * j->x;
				r.y = i->second->getY() + RuleInventory::SLOT_H * j->y;
				r.w = RuleInventory::SLOT_W + 1;
				r.h = RuleInventory::SLOT_H + 1;
				_grid->drawRect(&r, color);
				r.x++;
				r.y++;
				r.w -= 2;
				r.h -= 2;
				_grid->drawRect(&r, 0);
			}
		}
		else if (i->second->getType() == INV_HAND)
		{
			SDL_Rect r;
			r.x = i->second->getX();
			r.y = i->second->getY();
			r.w = RuleInventory::HAND_W * RuleInventory::SLOT_W;
			r.h = RuleInventory::HAND_H * RuleInventory::SLOT_H;
			_grid->drawRect(&r, color);
			r.x++;
			r.y++;
			r.w -= 2;
			r.h -= 2;
			_grid->drawRect(&r, 0);
		}
		else if (i->second->getType() == INV_GROUND)
		{
			for (int x = i->second->getX(); x <= 320; x += RuleInventory::SLOT_W)
			{
				for (int y = i->second->getY(); y <= 200; y += RuleInventory::SLOT_H)
				{
					SDL_Rect r;
					r.x = x;
					r.y = y;
					r.w = RuleInventory::SLOT_W + 1;
					r.h = RuleInventory::SLOT_H + 1;
					_grid->drawRect(&r, color);
					r.x++;
					r.y++;
					r.w -= 2;
					r.h -= 2;
					_grid->drawRect(&r, 0);
				}
			}
		}

		// Draw label
		text.setX(i->second->getX());
		text.setY(i->second->getY() - text.getFont()->getHeight() - text.getFont()->getSpacing());
		text.setText(_game->getLanguage()->getString(i->second->getId()));
		text.blit(_grid);
	}
}

/**
 * Draws the items contained in the soldier's inventory.
 */
void Inventory::drawItems()
{
	_items->clear();
	BattleUnit *unit = _game->getSavedGame()->getBattleGame()->getSelectedUnit();
	if (unit != 0)
	{
		SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BIGOBS.PCK");
		for (std::vector<BattleItem*>::iterator i = unit->getInventoryItems()->begin(); i != unit->getInventoryItems()->end(); ++i)
		{
			if ((*i) == _selItem)
				break;

			RuleInventory *inv = _invs->find((*i)->getSlot())->second;
			Surface *frame = texture->getFrame((*i)->getRules()->getBigSprite());
			if (inv->getType() == INV_SLOT)
			{
				frame->setX(inv->getX() + (*i)->getSlotX() * RuleInventory::SLOT_W);
				frame->setY(inv->getY() + (*i)->getSlotY() * RuleInventory::SLOT_H);
			}
			else if (inv->getType() == INV_HAND)
			{
				frame->setX(inv->getX() + (RuleInventory::HAND_W - (*i)->getRules()->getInventoryWidth()) * RuleInventory::SLOT_W/2);
				frame->setY(inv->getY() + (RuleInventory::HAND_H - (*i)->getRules()->getInventoryHeight()) * RuleInventory::SLOT_H/2);
			}
			texture->getFrame((*i)->getRules()->getBigSprite())->blit(_items);
		}
	}
}

/**
 * Checks if there's an inventory item in
 * the specified inventory position.
 * @param slot Inventory slot.
 * @param x X position in slot.
 * @param y Y position in slot.
 * @return Item in the slot, or NULL if none.
 */
BattleItem *Inventory::getItemInSlot(std::string slot, int x, int y) const
{
	BattleUnit *unit = _game->getSavedGame()->getBattleGame()->getSelectedUnit();
	if (unit != 0)
	{
		for (std::vector<BattleItem*>::iterator i = unit->getInventoryItems()->begin(); i != unit->getInventoryItems()->end(); ++i)
		{
			if ((*i)->getSlot() == slot)
			{
				if (_invs->find(slot)->second->getType() == INV_HAND || (*i)->occupiesSlot(x, y))
				{
					return *i;
				}
			}
		}
	}
	return 0;
}

/**
 * Gets the inventory slot located in the specified mouse position.
 * @param x Mouse X position. Returns the slot's X position.
 * @param y Mouse Y position. Returns the slot's Y position.
 * @return Slot id, or an empty string if none.
 */
std::string Inventory::getSlotInPosition(int *x, int *y) const
{
	for (std::map<std::string, RuleInventory*>::iterator i = _invs->begin(); i != _invs->end(); ++i)
	{
		if (i->second->checkSlotInPosition(x, y))
		{
			return i->second->getId();
		}
	}
	return "";
}

/**
 * Returns the item currently grabbed by the player.
 * @return Pointer to selected item, or NULL if none.
 */
BattleItem *Inventory::getSelectedItem() const
{
	return _selItem;
}

/**
 * Blits the inventory elements.
 * @param surface Pointer to surface to blit onto.
 */
void Inventory::blit(Surface *surface)
{
	clear();
	_grid->blit(this);
	_items->blit(this);
	_selection->blit(this);
	Surface::blit(surface);
}

/**
 * Moves the selected item.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Inventory::mouseOver(Action *action, State *state)
{
	_selection->setX((int)floor(action->getAbsoluteXMouse()) - _selection->getWidth()/2);
	_selection->setY((int)floor(action->getAbsoluteYMouse()) - _selection->getHeight()/2);
	InteractiveSurface::mouseOver(action, state);
}

/**
 * Picks up / drops an item.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Inventory::mouseClick(Action *action, State *state)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		// Pickup item
		if (_selItem == 0)
		{
			int x = (int)floor(action->getAbsoluteXMouse()),
				y = (int)floor(action->getAbsoluteYMouse());
			std::string slot = getSlotInPosition(&x, &y);
			if (slot != "")
			{
				BattleItem *item = getItemInSlot(slot, x, y);
				if (item != 0)
				{
					_selItem = item;
					_selItem->getRules()->drawHandSprite(_game->getResourcePack()->getSurfaceSet("BIGOBS.PCK"), _selection);
					drawItems();
				}
			}
		}
		// Drop item
		else
		{
			int x = _selection->getX() + (RuleInventory::HAND_W - _selItem->getRules()->getInventoryWidth()) * RuleInventory::SLOT_W/2,
				y = _selection->getY() + (RuleInventory::HAND_H - _selItem->getRules()->getInventoryHeight()) * RuleInventory::SLOT_H/2;
			std::string slot = getSlotInPosition(&x, &y);
			if (slot != "")
			{
				BattleItem *item = getItemInSlot(slot, x, y);
				if ((item == 0 || item == _selItem) && _invs->find(slot)->second->fitItemInSlot(_selItem->getRules(), x, y))
				{
					_selItem->setSlot(slot);
					_selItem->setSlotX(x);
					_selItem->setSlotY(y);
					_selItem = 0;
					_selection->clear();
					drawItems();
					_game->getResourcePack()->getSoundSet("BATTLE.CAT")->getSound(38)->play();
				}
			}
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		// Return item to original position
		_selItem = 0;
		_selection->clear();
		drawItems();
	}
	InteractiveSurface::mouseClick(action, state);
}

}
