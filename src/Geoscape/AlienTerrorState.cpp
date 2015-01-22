/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "GeoscapeState.h"
#include "Globe.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/TerrorSite.h"
#include "../Engine/Options.h"
#include "InterceptState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Aliens Terrorise window.
 * @param game Pointer to the core game.
 * @param terror Pointer to the respective Terror Site.
 * @param city Terrorized city name.
 * @param state Pointer to the Geoscape.
 */
AlienTerrorState::AlienTerrorState(TerrorSite *terror, const std::string &city, GeoscapeState *state) : _terror(terror), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 200, 0, 0, POPUP_BOTH);
	_btnIntercept = new TextButton(200, 16, 28, 130);
	_btnCentre = new TextButton(200, 16, 28, 150);
	_btnCancel = new TextButton(200, 16, 28, 170);
	_txtTitle = new Text(246, 32, 5, 48);
	_txtCity = new Text(246, 17, 5, 80);

	// Set palette
	setPalette("PAL_GEOSCAPE", _game->getRuleset()->getInterface("terrorSite")->getElement("palette")->color);

	add(_window, "window", "terrorSite");
	add(_btnIntercept, "button", "terrorSite");
	add(_btnCentre, "button", "terrorSite");
	add(_btnCancel, "button", "terrorSite");
	add(_txtTitle, "text", "terrorSite");
	add(_txtCity, "text", "terrorSite");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK03.SCR"));

	_btnIntercept->setText(tr("STR_INTERCEPT"));
	_btnIntercept->onMouseClick((ActionHandler)&AlienTerrorState::btnInterceptClick);

	_btnCentre->setText(tr("STR_CENTER_ON_SITE_TIME_5_SECONDS"));
	_btnCentre->onMouseClick((ActionHandler)&AlienTerrorState::btnCentreClick);

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&AlienTerrorState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&AlienTerrorState::btnCancelClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setWordWrap(true);
	_txtTitle->setText(tr("STR_ALIENS_TERRORISE"));

	_txtCity->setBig();
	_txtCity->setAlign(ALIGN_CENTER);
	_txtCity->setText(tr(city));
}

/**
 *
 */
AlienTerrorState::~AlienTerrorState()
{

}

/**
 * Picks a craft to intercept the UFO.
 * @param action Pointer to an action.
 */
void AlienTerrorState::btnInterceptClick(Action *)
{
	_state->timerReset();
	_state->getGlobe()->center(_terror->getLongitude(), _terror->getLatitude());
	_game->pushState(new InterceptState(_state->getGlobe(), 0, _terror));
}

/**
 * Centers on the UFO and returns to the previous screen.
 * @param action Pointer to an action.
 */
void AlienTerrorState::btnCentreClick(Action *)
{
	_state->timerReset();
	_state->getGlobe()->center(_terror->getLongitude(), _terror->getLatitude());
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void AlienTerrorState::btnCancelClick(Action *)
{
	_game->popState();
}

}
