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
	_sel = new Surface(HAND_W * SLOT_W, HAND_H * SLOT_H, x, y);

	_invs = _game->getRuleset()->getInventories();
}

/**
 * Delete inventory surfaces.
 */
Inventory::~Inventory()
{
	delete _grid;
	delete _items;
	delete _sel;
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
	_sel->setPalette(colors, firstcolor, ncolors);
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
				r.x = i->second->getX() + SLOT_W * j->x;
				r.y = i->second->getY() + SLOT_H * j->y;
				r.w = SLOT_W + 1;
				r.h = SLOT_H + 1;
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
			r.w = HAND_W * SLOT_W + 1;
			r.h = HAND_H * SLOT_H + 1;
			_grid->drawRect(&r, color);
			r.x++;
			r.y++;
			r.w -= 2;
			r.h -= 2;
			_grid->drawRect(&r, 0);
		}
		else if (i->second->getType() == INV_GROUND)
		{
			for (int x = i->second->getX(); x <= 320; x += SLOT_W)
			{
				for (int y = i->second->getY(); y <= 200; y += SLOT_H)
				{
					SDL_Rect r;
					r.x = x;
					r.y = y;
					r.w = SLOT_W + 1;
					r.h = SLOT_H + 1;
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
		for (std::vector<BattleItem*>::iterator i = unit->getInventoryItems()->begin(); i != unit->getInventoryItems()->end(); ++i)
		{
			RuleInventory *inv = _invs->find((*i)->getSlot())->second;
			SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BIGOBS.PCK");
			Surface *frame = texture->getFrame((*i)->getRules()->getBigSprite());
			if (inv->getType() == INV_SLOT)
			{
				frame->setX(inv->getX() + (*i)->getSlotX() * SLOT_W);
				frame->setY(inv->getY() + (*i)->getSlotY() * SLOT_H);
			}
			else if (inv->getType() == INV_HAND)
			{
				frame->setX(inv->getX() + (HAND_W - (*i)->getRules()->getInventoryWidth()) * SLOT_W/2);
				frame->setY(inv->getY() + (HAND_H - (*i)->getRules()->getInventoryHeight()) * SLOT_H/2);
			}
			texture->getFrame((*i)->getRules()->getBigSprite())->blit(_items);
		}
	}
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
	_sel->blit(this);
	Surface::blit(surface);
}

/**
 * Moves the selected item.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Inventory::mouseOver(Action *action, State *state)
{
	InteractiveSurface::mouseOver(action, state);
}

/**
 * Picks up / drops an item.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void Inventory::mouseClick(Action *action, State *state)
{
	InteractiveSurface::mouseClick(action, state);
}

}
