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
#include "AlienTerrorState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "GeoscapeState.h"
#include "Globe.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/City.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Aliens Terrorise window.
 * @param game Pointer to the core game.
 * @param city Pointer to the city to get info from.
 * @param state Pointer to the Geoscape.
 */
AlienTerrorState::AlienTerrorState(Game *game, City *city, GeoscapeState *state) : State(game), _city(city), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 200, 0, 0, POPUP_BOTH);
	_btnCentre = new TextButton(200, 16, 28, 140);
	_btnCancel = new TextButton(200, 16, 28, 160);
	_txtTitle = new Text(246, 16, 5, 56);
	_txtCity = new Text(246, 16, 5, 80);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);

	add(_window);
	add(_btnCentre);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtCity);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK03.SCR"));

	_btnCentre->setColor(Palette::blockOffset(8)+5);
	_btnCentre->setText(_game->getLanguage()->getString("STR_CENTER_ON_SITE_TIME_5_SECS"));
	_btnCentre->onMouseClick((ActionHandler)&AlienTerrorState::btnCentreClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&AlienTerrorState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_ALIENS_TERRORISE"));

	_txtCity->setColor(Palette::blockOffset(8)+5);
	_txtCity->setBig();
	_txtCity->setAlign(ALIGN_CENTER);
	_txtCity->setText(_game->getLanguage()->getString(_city->getName()));
}

/**
 *
 */
AlienTerrorState::~AlienTerrorState()
{

}

/**
 * Resets the palette.
 */
void AlienTerrorState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);
}

/**
 * Centers on the UFO and returns to the previous screen.
 * @param action Pointer to an action.
 */
void AlienTerrorState::btnCentreClick(Action *action)
{
	_state->timerReset();
	_state->getGlobe()->center(_city->getLongitude(), _city->getLatitude());
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void AlienTerrorState::btnCancelClick(Action *action)
{
	_game->popState();
}

}
