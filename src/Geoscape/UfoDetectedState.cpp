/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "UfoDetectedState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Ufo.h"
#include "../Mod/RuleUfo.h"
#include "GeoscapeState.h"
#include "Globe.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Options.h"
#include "../Savegame/AlienMission.h"
#include "InterceptState.h"
#include "../Mod/RuleCraft.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Ufo Detected window.
 * @param game Pointer to the core game.
 * @param ufo Pointer to the UFO to get info from.
 * @param state Pointer to the Geoscape.
 * @param detected Was the UFO detected?
 * @param hyperwave Was it a hyperwave radar?
 */
UfoDetectedState::UfoDetectedState(Ufo *ufo, GeoscapeState *state, bool detected, bool hyperwave) : _ufo(ufo), _state(state)
{
	// Generate UFO ID
	if (_ufo->getId() == 0)
	{
		_ufo->setId(_game->getSavedGame()->getId("STR_UFO"));
	}
	if (_ufo->getAltitude() == "STR_GROUND" && _ufo->getLandId() == 0)
	{
		_ufo->setLandId(_game->getSavedGame()->getId("STR_LANDING_SITE"));
	}

	_screen = false;

	// Create objects
	if (hyperwave)
	{
		_window = new Window(this, 224, 180, 16, 10, POPUP_BOTH);
	}
	else
	{
		_window = new Window(this, 224, 128, 16, 44, POPUP_BOTH);
	}
	_btnIntercept = new TextButton(200, 12, 28, 118);
	_btnCentre = new TextButton(200, 12, 28, 134);
	_btnCancel = new TextButton(200, 12, 28, 150);
	_txtUfo = new Text(207, 17, 28, 53);
	_txtDetected = new Text(100, 9, 28, 69);
	_txtHyperwave = new Text(214, 17, 21, 44);
	_lstInfo = new TextList(217, 32, 28, 80);
	_lstInfo2 = new TextList(217, 32, 28, 96);

	if (hyperwave)
	{
		_btnIntercept->setY(136);
		_btnCentre->setY(152);
		_btnCancel->setY(168);
		_txtUfo->setY(20);
		_txtDetected->setY(36);
		_lstInfo->setY(60);
	}
	else
	{
		_txtHyperwave->setVisible(false);
		_lstInfo2->setVisible(false);
	}

	// Set palette
	setInterface("UFOInfo", hyperwave);

	add(_window, "window", "UFOInfo");
	add(_btnIntercept, "button", "UFOInfo");
	add(_btnCentre, "button", "UFOInfo");
	add(_btnCancel, "button", "UFOInfo");
	add(_txtUfo, "text", "UFOInfo");
	add(_txtDetected, "text", "UFOInfo");
	add(_txtHyperwave, "text", "UFOInfo");
	add(_lstInfo, "text", "UFOInfo");
	add(_lstInfo2, "text", "UFOInfo");

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK15.SCR"));

	centerAllSurfaces();

	_btnIntercept->setText(tr("STR_INTERCEPT"));
	_btnIntercept->onMouseClick((ActionHandler)&UfoDetectedState::btnInterceptClick);

	_btnCentre->setText(tr("STR_CENTER_ON_UFO_TIME_5_SECONDS"));
	_btnCentre->onMouseClick((ActionHandler)&UfoDetectedState::btnCentreClick);

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&UfoDetectedState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&UfoDetectedState::btnCancelClick, Options::keyCancel);

	if (detected)
	{
		_txtDetected->setText(tr("STR_DETECTED"));
	}
	else
	{
		_txtDetected->setText(L"");
	}

	_txtHyperwave->setAlign(ALIGN_CENTER);
	_txtHyperwave->setWordWrap(true);
	_txtHyperwave->setText(tr("STR_HYPER_WAVE_TRANSMISSIONS_ARE_DECODED"));

	_txtUfo->setBig();
	_txtUfo->setText(_ufo->getName(_game->getLanguage()));

	_lstInfo->setColumns(2, 77, 140);
	_lstInfo->setDot(true);

	std::wostringstream ss;
	ss << L'\x01' << tr(_ufo->getRules()->getSize());
	_lstInfo->addRow(2, tr("STR_SIZE_UC").c_str(), ss.str().c_str());
	ss.str(L"");

	std::string altitude = _ufo->getAltitude() == "STR_GROUND" ? "STR_GROUNDED" : _ufo->getAltitude();
	// Let's assume if there's any underwater craft, the UFO are underwater too
	bool underwater = false;
	const std::vector<std::string> &crafts = _game->getMod()->getCraftsList();
	for (std::vector<std::string>::const_iterator i = crafts.begin(); i != crafts.end() && !underwater; ++i)
	{
		underwater = _game->getMod()->getCraft(*i)->isWaterOnly();
	}
	if (underwater && !_state->getGlobe()->insideLand(_ufo->getLongitude(), _ufo->getLatitude()))
	{
		altitude = "STR_AIRBORNE";
	}
	ss << L'\x01' << tr(altitude);
	_lstInfo->addRow(2, tr("STR_ALTITUDE").c_str(), ss.str().c_str());

	std::string heading = _ufo->getDirection();
	if (_ufo->getStatus() != Ufo::FLYING)
	{
		heading = "STR_NONE_UC";
	}
	ss.str(L"");
	ss << L'\x01' << tr(heading);
	_lstInfo->addRow(2, tr("STR_HEADING").c_str(), ss.str().c_str());

	ss.str(L"");
	ss << L'\x01' << Text::formatNumber(_ufo->getSpeed());
	_lstInfo->addRow(2, tr("STR_SPEED").c_str(), ss.str().c_str());

	_lstInfo2->setColumns(2, 77, 140);
	_lstInfo2->setDot(true);

	ss.str(L"");
	ss << L'\x01' << tr(_ufo->getRules()->getType());
	_lstInfo2->addRow(2, tr("STR_CRAFT_TYPE").c_str(), ss.str().c_str());

	ss.str(L"");
	ss << L'\x01' << tr(_ufo->getAlienRace());
	_lstInfo2->addRow(2, tr("STR_RACE").c_str(), ss.str().c_str());

	ss.str(L"");
	ss << L'\x01' << tr(_ufo->getMissionType());
	_lstInfo2->addRow(2, tr("STR_MISSION").c_str(), ss.str().c_str());

	ss.str(L"");
	ss << L'\x01' << tr(_ufo->getMission()->getRegion());
	_lstInfo2->addRow(2, tr("STR_ZONE").c_str(), ss.str().c_str());
}

/**
 *
 */
UfoDetectedState::~UfoDetectedState()
{

}

/**
 * Picks a craft to intercept the UFO.
 * @param action Pointer to an action.
 */
void UfoDetectedState::btnInterceptClick(Action *)
{
	_state->timerReset();
	_state->getGlobe()->center(_ufo->getLongitude(), _ufo->getLatitude());
	_game->pushState(new InterceptState(_state->getGlobe(), 0, _ufo));
}

/**
 * Centers on the UFO and returns to the previous screen.
 * @param action Pointer to an action.
 */
void UfoDetectedState::btnCentreClick(Action *)
{
	_state->timerReset();
	_state->getGlobe()->center(_ufo->getLongitude(), _ufo->getLatitude());
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void UfoDetectedState::btnCancelClick(Action *)
{
	_game->popState();
}

}
