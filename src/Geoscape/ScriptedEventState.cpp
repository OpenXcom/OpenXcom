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
#include "ScriptedEventState.h"
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
#include "../Engine/Options.h"
#include "InterceptState.h"
#include "../Ruleset/RuleScriptedEvent.h"
#include "../Savegame/ScriptedEvent.h"

namespace OpenXcom
{

/**
 * Popup screen for Scripted Events on the geoscape.
 * 
 * @param scriptedEventTarget The respective scripted event geoscape target
 * @param scriptedEvent The live scripted event entry
 * @param scriptedEventScreen The display rules for this screen 
 *                             SS_DETECTED and SS_FAILED_ENGAGE supported.
 * @param state Pointer to the Geoscape.
 */
ScriptedEventState::ScriptedEventState(Target *scriptedEventTarget, ScriptedEvent *scriptedEvent, SCRIPTED_EVENT_SCREEN scriptedEventScreen, GeoscapeState *state) : _scriptedEventTarget(scriptedEventTarget), _scriptedEvent(scriptedEvent), _scriptedEventScreen(scriptedEventScreen), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 200, 0, 0, POPUP_BOTH);
	_btnIntercept = new TextButton(200, 16, 28, 130);
	_btnCentre = new TextButton(200, 16, 28, 150);
	_btnCancel = new TextButton(200, 16, 28, 170);
	_txtTitle = new Text(246, 32, 5, 20);
	_txtMain = new Text(200, 100, 28, 60);

	// Extract screen data
	MissionScreenData msd = _scriptedEventScreen == SS_DETECTED ? _scriptedEvent->getRules().getDetectedScreen() : _scriptedEvent->getRules().getFailedToEngageScreen();

	// Set palette
	setPalette("PAL_GEOSCAPE", msd.palette);

	add(_window);
	if (_scriptedEventScreen == SS_DETECTED) add(_btnIntercept);
	add(_btnCentre);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtMain);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface(msd.background_id));

	_btnIntercept->setColor(Palette::blockOffset(8)+5);
	_btnIntercept->setText(tr("STR_INTERCEPT"));
	_btnIntercept->onMouseClick((ActionHandler)&ScriptedEventState::btnInterceptClick);

	_btnCentre->setColor(Palette::blockOffset(8)+5);
	_btnCentre->setText(tr("STR_CENTER_ON_SITE_TIME_5_SECONDS"));
	_btnCentre->onMouseClick((ActionHandler)&ScriptedEventState::btnCentreClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&ScriptedEventState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ScriptedEventState::btnCancelClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setWordWrap(true);
	_txtTitle->setText(tr(msd.title_id));

	_txtMain->setColor(Palette::blockOffset(8)+5);
	_txtMain->setAlign(ALIGN_LEFT);
	_txtMain->setWordWrap(true);
	_txtMain->setText(tr(msd.mainText_id));
}

/**
 * Destrutor (empty by design)
 */
ScriptedEventState::~ScriptedEventState()
{

}

/**
 * Picks a craft to intercept the event
 * @param action Pointer to an action.
 */
void ScriptedEventState::btnInterceptClick(Action *)
{
	_state->timerReset();
	_state->getGlobe()->center(_scriptedEventTarget->getLongitude(), _scriptedEventTarget->getLatitude());
	_game->pushState(new InterceptState(_state->getGlobe(), 0, _scriptedEventTarget));
}

/**
 * Centers on the event and returns to the previous screen.
 * @param action Pointer to an action.
 */
void ScriptedEventState::btnCentreClick(Action *)
{
	_state->timerReset();
	_state->getGlobe()->center(_scriptedEventTarget->getLongitude(), _scriptedEventTarget->getLatitude());
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ScriptedEventState::btnCancelClick(Action *)
{
	_game->popState();
}

}
