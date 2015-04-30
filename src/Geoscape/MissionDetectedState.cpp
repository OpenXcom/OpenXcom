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
#include "MissionDetectedState.h"
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
#include "../Savegame/MissionSite.h"
#include "../Engine/Options.h"
#include "InterceptState.h"
#include "../Ruleset/AlienDeployment.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Mission Detected window.
 * @param game Pointer to the core game.
 * @param mission Pointer to the respective Mission Site.
 * @param state Pointer to the Geoscape.
 */
MissionDetectedState::MissionDetectedState(MissionSite *mission, GeoscapeState *state) : _mission(mission), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 200, 0, 0, POPUP_BOTH);
	_btnIntercept = new TextButton(200, 16, 28, 130);
	_btnCenter = new TextButton(200, 16, 28, 150);
	_btnCancel = new TextButton(200, 16, 28, 170);
	_txtTitle = new Text(246, 32, 5, 48);
	_txtCity = new Text(246, 17, 5, 80);

	// Set palette
	setInterface("terrorSite");

	add(_window, "window", "terrorSite");
	add(_btnIntercept, "button", "terrorSite");
	add(_btnCenter, "button", "terrorSite");
	add(_btnCancel, "button", "terrorSite");
	add(_txtTitle, "text", "terrorSite");
	add(_txtCity, "text", "terrorSite");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK03.SCR"));

	_btnIntercept->setText(tr("STR_INTERCEPT"));
	_btnIntercept->onMouseClick((ActionHandler)&MissionDetectedState::btnInterceptClick);

	_btnCenter->setText(tr("STR_CENTER_ON_SITE_TIME_5_SECONDS"));
	_btnCenter->onMouseClick((ActionHandler)&MissionDetectedState::btnCenterClick);

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&MissionDetectedState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&MissionDetectedState::btnCancelClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setWordWrap(true);
	_txtTitle->setText(tr(mission->getDeployment()->getAlertMessage()));

	_txtCity->setBig();
	_txtCity->setAlign(ALIGN_CENTER);
	_txtCity->setText(tr(mission->getCity()));
}

/**
 *
 */
MissionDetectedState::~MissionDetectedState()
{

}

/**
 * Picks a craft to intercept the mission site.
 * @param action Pointer to an action.
 */
void MissionDetectedState::btnInterceptClick(Action *)
{
	_state->timerReset();
	_state->getGlobe()->center(_mission->getLongitude(), _mission->getLatitude());
	_game->pushState(new InterceptState(_state->getGlobe(), 0, _mission));
}

/**
 * Centers on the mission site and returns to the previous screen.
 * @param action Pointer to an action.
 */
void MissionDetectedState::btnCenterClick(Action *)
{
	_state->timerReset();
	_state->getGlobe()->center(_mission->getLongitude(), _mission->getLatitude());
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void MissionDetectedState::btnCancelClick(Action *)
{
	_game->popState();
}

}
