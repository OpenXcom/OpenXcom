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

namespace OpenXcom
{

/**
 * Initializes all the elements in the Ufo Detected window.
 * @param game Pointer to the core game.
 * @param ufo Pointer to the UFO to get info from.
 * @param state Pointer to the Geoscape.
 * @param detected Was the UFO detected?
 */
UfoDetectedState::UfoDetectedState(Game *game, Ufo *ufo, GeoscapeState *state, bool detected) : State(game), _ufo(ufo), _state(state), _detected(detected)
{
	// Generate UFO ID
	if (_ufo->getId() == 0)
	{
		_ufo->setId(_game->getSavedGame()->getId("STR_UFO"));
	}

	_screen = false;

	// Create objects
	_window = new Window(this, 208, 120, 24, 48, POPUP_BOTH);
	_btnCentre = new TextButton(160, 12, 48, 128);
	_btnCancel = new TextButton(160, 12, 48, 144);
	_txtUfo = new Text(160, 16, 48, 56);
	_txtDetected = new Text(80, 8, 48, 72);
	_lstInfo = new TextList(176, 32, 48, 82);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);

	add(_window);
	add(_btnCentre);
	add(_btnCancel);
	add(_txtUfo);
	add(_txtDetected);
	add(_lstInfo);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK15.SCR"));

	_btnCentre->setColor(Palette::blockOffset(8)+5);
	_btnCentre->setText(_game->getLanguage()->getString("STR_CENTER_ON_UFO_TIME_5_SECS"));
	_btnCentre->onMouseClick((ActionHandler)&UfoDetectedState::btnCentreClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&UfoDetectedState::btnCancelClick);

	_txtDetected->setColor(Palette::blockOffset(8)+5);
	if (_detected)
	{
		_txtDetected->setText(_game->getLanguage()->getString("STR_DETECTED"));
	}
	else
	{
		_txtDetected->setText(L"");
	}

	_txtUfo->setColor(Palette::blockOffset(8)+5);
	_txtUfo->setBig();
	_txtUfo->setText(_ufo->getName(_game->getLanguage()));

	_lstInfo->setColor(Palette::blockOffset(8)+5);
	_lstInfo->setColumns(2, 82, 94);
	_lstInfo->setDot(true);
	_lstInfo->addRow(2, _game->getLanguage()->getString("STR_SIZE_UC").c_str(), _game->getLanguage()->getString(_ufo->getRules()->getSize()).c_str());
	_lstInfo->setCellColor(0, 1, Palette::blockOffset(8)+10);
	_lstInfo->addRow(2, _game->getLanguage()->getString("STR_ALTITUDE").c_str(), _game->getLanguage()->getString(_ufo->getAltitude()).c_str());
	_lstInfo->setCellColor(1, 1, Palette::blockOffset(8)+10);
	_lstInfo->addRow(2, _game->getLanguage()->getString("STR_HEADING").c_str(), _game->getLanguage()->getString(_ufo->getDirection()).c_str());
	_lstInfo->setCellColor(2, 1, Palette::blockOffset(8)+10);
	std::wstringstream ss;
	ss << _ufo->getSpeed();
	_lstInfo->addRow(2, _game->getLanguage()->getString("STR_SPEED").c_str(), ss.str().c_str());
	_lstInfo->setCellColor(3, 1, Palette::blockOffset(8)+10);
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
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);
}

/**
 * Centers on the UFO and returns to the previous screen.
 * @param action Pointer to an action.
 */
void UfoDetectedState::btnCentreClick(Action *action)
{
	_state->timerReset();
	_state->getGlobe()->center(_ufo->getLongitude(), _ufo->getLatitude());
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void UfoDetectedState::btnCancelClick(Action *action)
{
	_game->popState();
}

}
