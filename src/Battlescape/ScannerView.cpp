/*
 * Copyright 2011 OpenXcom Developers.
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
#include "ScannerView.h"
#include "../Engine/Game.h"
#include "../Engine/SurfaceSet.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Action.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/Tile.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Interface/Text.h"
#include "../Resource/ResourcePack.h"
#include <iostream>

namespace OpenXcom
{

/**
 * Initialize the Scanner view
 * @param w the ScannerView width
 * @param h the ScannerView height
 * @param x the ScannerView x origin
 * @param y the ScannerView y origin
 * @param game Pointer to the core game.
 * @param unit the current unit
 */
ScannerView::ScannerView (int w, int h, int x, int y, Game * game, BattleUnit *unit) : InteractiveSurface(w, h, x, y), _game(game), _unit(unit), _frame(0)
{
	_redraw = true;
}

/**
 * Draw the ScannerView view
 */
void ScannerView::draw()
{
	SurfaceSet *set = _game->getResourcePack()->getSurfaceSet("DETBLOB.DAT");
	Surface *surface = 0;

	clear();

	for (int x = -9; x < 10; x++)
	{
		for (int y = -9; y < 10; y++)
		{
			Tile *t = _game->getSavedGame()->getBattleGame()->getTile(Position(x,y,0) + _unit->getPosition());
			if (t && t->getUnit() && t->getUnit()->getMotionPoints())
			{
				int frame = (t->getUnit()->getMotionPoints() / 5) - 1;
				if (frame >= 0)
				{
					if (frame > 5) frame = 5;
					surface = set->getFrame(frame + _frame);
					surface->blitNShade(this, ((9+x)*8)-4, ((9+y)*8)-4, 0);
				}
			}
		}
	}

	// the arrow of the direction the unit is pointed
	surface = set->getFrame(7 + _unit->getDirection());

	surface->blitNShade(this, (9*8)-4, (9*8)-4, 0);


}

/**
 * Handle click on the scanner view
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void ScannerView::mouseClick (Action *action, State *state)
{
}

/**
 * Update scanner animation
*/
void ScannerView::animate()
{
	_frame++;
	if(_frame > 1)
	{
		_frame = 0;
	}
	_redraw = true;
}

}
