/*
 * Copyright 2010 Daniel Albano
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
#include "DismantleFacilityState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Resource/LangString.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"

/**
 * Initializes all the elements in a Dismantle Facility window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param fac Pointer to the facility to dismantle.
 */
DismantleFacilityState::DismantleFacilityState(Game *game, Base *base, BaseFacility *fac) : State(game), _base(base), _fac(fac)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 152, 80, 20, 60);
	_btnOk = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 44, 16, 36, 115);
	_btnCancel = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 44, 16, 112, 115);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 142, 9, 25, 75);
	_txtFacility = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 142, 9, 25, 85);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtFacility);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+4);
	_window->setBackground(game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+9);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&DismantleFacilityState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(15)+9);
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString(STR_CANCEL));
	_btnCancel->onMouseClick((EventHandler)&DismantleFacilityState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_DISMANTLE));

	_txtFacility->setColor(Palette::blockOffset(13)+10);
	_txtFacility->setAlign(ALIGN_CENTER);
	_txtFacility->setText(_game->getResourcePack()->getLanguage()->getString(_fac->getRules()->getType()));
}

/**
 *
 */
DismantleFacilityState::~DismantleFacilityState()
{
	
}

/**
 * Dismantles the facility and returns
 * to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DismantleFacilityState::btnOkClick(SDL_Event *ev, int scale)
{
	for (vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); i++)
	{
		if (*i == _fac)
		{
			_base->getFacilities()->erase(i);
			delete _fac;
			break;
		}
	}
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void DismantleFacilityState::btnCancelClick(SDL_Event *ev, int scale)
{
	_game->popState();
}
