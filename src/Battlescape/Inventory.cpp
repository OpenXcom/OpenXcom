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
#include "../Ruleset/RuleInventory.h"
#include "../Engine/Palette.h"
#include "../Engine/Game.h"
#include "../Interface/Text.h"
#include "../Engine/Font.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"

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
Inventory::Inventory(Game *game, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _game(game)
{
}

Inventory::~Inventory()
{
}

void Inventory::setRuleInventory(std::vector<RuleInventory*> *invs)
{
	_invs = invs;
}

void Inventory::draw()
{
	Text text = Text(100, 9, 0, 0);
	text.setPalette(this->getPalette());
	text.setFonts(_game->getResourcePack()->getFont("BIGLETS.DAT"), _game->getResourcePack()->getFont("SMALLSET.DAT"));
	text.setColor(Palette::blockOffset(4)-1);
	text.setHighContrast(true);

	Uint8 color = Palette::blockOffset(0)+8;
	for (std::vector<RuleInventory*>::iterator i = _invs->begin(); i != _invs->end(); ++i)
	{
		// Draw grid
		if ((*i)->getType() == INV_SLOT)
		{
			for (std::vector<RuleSlot>::iterator j = (*i)->getSlots()->begin(); j != (*i)->getSlots()->end(); ++j)
			{
				SDL_Rect r;
				r.x = (*i)->getX() + SLOT_W * j->x;
				r.y = (*i)->getY() + SLOT_H * j->y;
				r.w = SLOT_W + 1;
				r.h = SLOT_H + 1;
				this->drawRect(&r, color);
				r.x++;
				r.y++;
				r.w -= 2;
				r.h -= 2;
				this->drawRect(&r, 0);
			}
		}
		else if ((*i)->getType() == INV_HAND)
		{
			SDL_Rect r;
			r.x = (*i)->getX();
			r.y = (*i)->getY();
			r.w = HAND_W + 1;
			r.h = HAND_H + 1;
			this->drawRect(&r, color);
			r.x++;
			r.y++;
			r.w -= 2;
			r.h -= 2;
			this->drawRect(&r, 0);
		}
		else if ((*i)->getType() == INV_GROUND)
		{
			for (int x = (*i)->getX(); x <= 320; x += SLOT_W)
			{
				for (int y = (*i)->getY(); y <= 200; y += SLOT_H)
				{
					SDL_Rect r;
					r.x = x;
					r.y = y;
					r.w = SLOT_W + 1;
					r.h = SLOT_H + 1;
					this->drawRect(&r, color);
					r.x++;
					r.y++;
					r.w -= 2;
					r.h -= 2;
					this->drawRect(&r, 0);
				}
			}
		}

		// Draw label
		text.setX((*i)->getX());
		text.setY((*i)->getY() - text.getFont()->getHeight() - text.getFont()->getSpacing());
		text.setText(_game->getLanguage()->getString((*i)->getId()));
		text.blit(this);
	}
}

}
