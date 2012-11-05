/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "MedikitView.h"
#include "../Engine/Game.h"
#include "../Engine/SurfaceSet.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Action.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Savegame/BattleUnit.h"
#include "../Interface/Text.h"
#include "../Resource/ResourcePack.h"
#include <iostream>

namespace OpenXcom
{
/**
 * User interface string identifier of body parts
 */
const std::string PARTS_STRING[6] =
{
	"STR_HEAD",
	"STR_TORSO",
	"STR_RIGHT_ARM",
	"STR_LEFT_ARM",
	"STR_RIGHT_LEG",
	"STR_LEFT_LEG"
};

/**
 * Initialize the Medikit view
 * @param w the MinikitView width
 * @param h the MinikitView height
 * @param x the MinikitView x origin
 * @param y the MinikitView y origin
 * @param game Pointer to the core game.
 * @param unit the wounded unit
 * @param partTxt a pointer to a Text. Will be updated with selected body part
 * @param woundTxt a pointer to a Text. Will be updated with the amount of fatal wound
 */
MedikitView::MedikitView (int w, int h, int x, int y, Game * game, BattleUnit *unit, Text *partTxt, Text *woundTxt) : InteractiveSurface(w, h, x, y), _game(game), _selectedPart(0), _unit(unit), _partTxt(partTxt), _woundTxt(woundTxt)
{
	_redraw = true;
}

/**
 * Draw the medikit view
 */
void MedikitView::draw()
{
	SurfaceSet *set = _game->getResourcePack()->getSurfaceSet("MEDIBITS.DAT");
	int fatal_wound = _unit->getFatalWound(_selectedPart);
	std::wstringstream ss, ss1;
	int red = 3;
	
	this->lock();
	for (int i = 0; i < set->getTotalFrames(); i++)
	{
		int wound = _unit->getFatalWound(i);
		Surface * surface = set->getFrame (i);
		int baseColor = wound ? red : 0;
		surface->blitNShade(this, Surface::getX(), Surface::getY(), 0, false, baseColor);
	}
	this->unlock();

	_redraw = false;
	if (_selectedPart == -1)
	{
		return;
	}
	ss << _game->getLanguage()->getString(PARTS_STRING[_selectedPart]);
	ss1 << fatal_wound;
	_partTxt->setText(ss.str());
	_woundTxt->setText(ss1.str());
}

/**
 * Handle click on the medikit view
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void MedikitView::mouseClick (Action *action, State *state)
{
	SurfaceSet *set = _game->getResourcePack()->getSurfaceSet("MEDIBITS.DAT");
	int x = action->getRelativeXMouse() / action->getXScale();
	int y = action->getRelativeYMouse() / action->getYScale();
	for (int i = 0; i < set->getTotalFrames(); i++)
	{
		Surface * surface = set->getFrame (i);
		if (surface->getPixel(x, y))
		{
			_selectedPart = i;
			_redraw = true;
			break;
		}
	}
}

/**
 * Get the selected body part
 * @return the selected body part
 */
int MedikitView::getSelectedPart() const
{
	return _selectedPart;
}
}
