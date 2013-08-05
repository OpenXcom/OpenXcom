/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Ufo.h"
#include "../Ruleset/RuleUfo.h"
#include "GeoscapeState.h"
#include "Globe.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Options.h"
#include "../Savegame/AlienMission.h"

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
UfoDetectedState::UfoDetectedState(Game *game, Ufo *ufo, GeoscapeState *state, bool detected, bool hyperwave) : State(game), _ufo(ufo), _state(state), _hyperwave(hyperwave)
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
		_window = new Window(this, 224, 120, 16, 48, POPUP_BOTH);
	}
	_btnCentre = new TextButton(200, 12, 28, 128);
	_btnCancel = new TextButton(200, 12, 28, 144);
	_txtUfo = new Text(207, 16, 28, 56);
	_txtDetected = new Text(100, 8, 28, 72);
	_txtHyperwave = new Text(214, 16, 21, 44);
	_lstInfo = new TextList(207, 32, 28, 82);
	_lstInfo2 = new TextList(207, 32, 28, 96);

	if (hyperwave)
	{
		_btnCentre->setY(144);
		_btnCancel->setY(160);
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
	if (hyperwave)
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);
	}
	else
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);
	}

	add(_window);
	add(_btnCentre);
	add(_btnCancel);
	add(_txtUfo);
	add(_txtDetected);
	add(_txtHyperwave);
	add(_lstInfo);
	add(_lstInfo2);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK15.SCR"));

	centerAllSurfaces();

	_btnCentre->setColor(Palette::blockOffset(8)+5);
	_btnCentre->setText(_game->getLanguage()->getString("STR_CENTER_ON_UFO_TIME_5_SECS"));
	_btnCentre->onMouseClick((ActionHandler)&UfoDetectedState::btnCentreClick);
	_btnCentre->onKeyboardPress((ActionHandler)&UfoDetectedState::btnCentreClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&UfoDetectedState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&UfoDetectedState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_txtDetected->setColor(Palette::blockOffset(8)+5);
	if (detected)
	{
		_txtDetected->setText(_game->getLanguage()->getString("STR_DETECTED"));
	}
	else
	{
		_txtDetected->setText(L"");
	}

	_txtHyperwave->setColor(Palette::blockOffset(8)+5);
	_txtHyperwave->setAlign(ALIGN_CENTER);
	_txtHyperwave->setWordWrap(true);
	_txtHyperwave->setText(_game->getLanguage()->getString("STR_HYPER_WAVE_TRANSMISSIONS_ARE_DECODED"));

	_txtUfo->setColor(Palette::blockOffset(8)+5);
	_txtUfo->setBig();
	_txtUfo->setText(_ufo->getName(_game->getLanguage()));

	_lstInfo->setColor(Palette::blockOffset(8)+5);
	_lstInfo->setColumns(2, 87, 120);
	_lstInfo->setDot(true);
	_lstInfo->addRow(2, _game->getLanguage()->getString("STR_SIZE_UC").c_str(), _game->getLanguage()->getString(_ufo->getRules()->getSize()).c_str());
	_lstInfo->setCellColor(0, 1, Palette::blockOffset(8)+10);
	std::string altitude = _ufo->getAltitude() == "STR_GROUND" ? "STR_GROUNDED" : _ufo->getAltitude();
	_lstInfo->addRow(2, _game->getLanguage()->getString("STR_ALTITUDE").c_str(), _game->getLanguage()->getString(altitude).c_str());
	_lstInfo->setCellColor(1, 1, Palette::blockOffset(8)+10);
	std::string heading = _ufo->getDirection();
	if (_ufo->getStatus() != Ufo::FLYING)
	{
		heading = "STR_NONE_UC";
	}
	_lstInfo->addRow(2, _game->getLanguage()->getString("STR_HEADING").c_str(), _game->getLanguage()->getString(heading).c_str());
	_lstInfo->setCellColor(2, 1, Palette::blockOffset(8)+10);
	std::wstringstream ss;
	ss << _ufo->getSpeed();
	_lstInfo->addRow(2, _game->getLanguage()->getString("STR_SPEED").c_str(), ss.str().c_str());
	_lstInfo->setCellColor(3, 1, Palette::blockOffset(8)+10);

	_lstInfo2->setColor(Palette::blockOffset(8)+5);
	_lstInfo2->setColumns(2, 87, 120);
	_lstInfo2->setDot(true);
	_lstInfo2->addRow(2, _game->getLanguage()->getString("STR_CRAFT_TYPE").c_str(), _game->getLanguage()->getString(_ufo->getRules()->getType()).c_str());
	_lstInfo2->setCellColor(0, 1, Palette::blockOffset(8)+10);
	_lstInfo2->addRow(2, _game->getLanguage()->getString("STR_RACE").c_str(), _game->getLanguage()->getString(_ufo->getAlienRace()).c_str());
	_lstInfo2->setCellColor(1, 1, Palette::blockOffset(8)+10);
	_lstInfo2->addRow(2, _game->getLanguage()->getString("STR_MISSION").c_str(), _game->getLanguage()->getString(_ufo->getMissionType()).c_str());
	_lstInfo2->setCellColor(2, 1, Palette::blockOffset(8)+10);
	_lstInfo2->addRow(2, _game->getLanguage()->getString("STR_ZONE").c_str(), _game->getLanguage()->getString(_ufo->getMission()->getRegion()).c_str());
	_lstInfo2->setCellColor(3, 1, Palette::blockOffset(8)+10);
}

/**
 *
 */
UfoDetectedState::~UfoDetectedState()
{

}

/**
 * Resets the palette.
 */
void UfoDetectedState::init()
{
	if (_hyperwave)
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);
	}
	else
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);
	}
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
