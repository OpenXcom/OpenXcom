/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleInterface.h"
#include "../Engine/SurfaceSet.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Action.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Savegame/BattleUnit.h"
#include "../Interface/Text.h"
#include <iostream>

namespace OpenXcom
{
/**
 * User interface string identifier of body parts.
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
 * Initializes the Medikit view.
 * @param w The MinikitView width.
 * @param h The MinikitView height.
 * @param x The MinikitView x origin.
 * @param y The MinikitView y origin.
 * @param game Pointer to the core game.
 * @param unit The wounded unit.
 * @param partTxt A pointer to a Text. Will be updated with the selected body part.
 * @param woundTxt A pointer to a Text. Will be updated with the amount of fatal wound.
 */
MedikitView::MedikitView (int w, int h, int x, int y, Game * game, BattleUnit *unit, Text *partTxt, Text *woundTxt) : InteractiveSurface(w, h, x, y), _game(game), _selectedPart(0), _unit(unit), _partTxt(partTxt), _woundTxt(woundTxt)
{
	updateSelectedPart();
	_redraw = true;
}

/**
 * Draws the medikit view.
 */
void MedikitView::draw()
{
	SurfaceSet *set = _game->getResourcePack()->getSurfaceSet("MEDIBITS.DAT");
	int fatal_wound = _unit->getFatalWound(_selectedPart);
	std::wostringstream ss, ss1;
	int green = 0;
	int red = 3;
	if (_game->getRuleset()->getInterface("medikit") && _game->getRuleset()->getInterface("medikit")->getElement("body"))
	{
		green = _game->getRuleset()->getInterface("medikit")->getElement("body")->color;
		red = _game->getRuleset()->getInterface("medikit")->getElement("body")->color2;
	}
	this->lock();
	for (unsigned int i = 0; i < set->getTotalFrames(); i++)
	{
		int wound = _unit->getFatalWound(i);
		Surface * surface = set->getFrame (i);
		int baseColor = wound ? red : green;
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
 * Handles clicks on the medikit view.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void MedikitView::mouseClick (Action *action, State *)
{
	SurfaceSet *set = _game->getResourcePack()->getSurfaceSet("MEDIBITS.DAT");
	int x = action->getRelativeXMouse() / action->getXScale();
	int y = action->getRelativeYMouse() / action->getYScale();
	for (unsigned int i = 0; i < set->getTotalFrames(); i++)
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
 * Gets the selected body part.
 * @return The selected body part.
 */
int MedikitView::getSelectedPart() const
{
	return _selectedPart;
}

/**
 * Updates the selected body part.
 * If there is a wounded body part, selects that.
 * Otherwise does not change the selected part.
 */
void MedikitView::updateSelectedPart()
{
	for (int i = 0; i < 6; ++i)
	{
		if (_unit->getFatalWound(i))
		{
			_selectedPart = i;
			break;
		}
	}
}
}
