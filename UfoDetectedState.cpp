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
#include "UfoDetectedState.h"
#include <sstream>
#include "Game.h"
#include "ResourcePack.h"
#include "Language.h"
#include "LangString.h"
#include "Font.h"
#include "Palette.h"
#include "TextButton.h"
#include "Window.h"
#include "Text.h"
#include "TextList.h"
#include "Ufo.h"
#include "RuleUfo.h"
#include "GeoscapeState.h"
#include "Globe.h"
#include "SavedGame.h"

using namespace std;

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
		_ufo->setId(*_game->getSavedGame()->getUfoId());
		(*_game->getSavedGame()->getUfoId())++;
	}

	_screen = false;

	// Create objects
	_window = new Window(this, 208, 120, 24, 48, POPUP_BOTH);
	_btnCentre = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 12, 48, 128);
	_btnCancel = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 12, 48, 144);
	_txtUfo = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 16, 48, 56);
	_txtDetected = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 80, 8, 48, 72);
	_lstInfo = new TextList(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 32, 48, 82);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);

	add(_window);
	add(_btnCentre);
	add(_btnCancel);
	add(_txtUfo);
	add(_txtDetected);
	add(_lstInfo);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(game->getResourcePack()->getSurface("BACK15.SCR"));

	_btnCentre->setColor(Palette::blockOffset(8)+8);
	_btnCentre->setText(_game->getResourcePack()->getLanguage()->getString(STR_CENTRE_ON_UFO));
	_btnCentre->onMouseClick((EventHandler)&UfoDetectedState::btnCentreClick);

	_btnCancel->setColor(Palette::blockOffset(8)+8);
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString(STR_CANCEL_UC));
	_btnCancel->onMouseClick((EventHandler)&UfoDetectedState::btnCancelClick);

	_txtDetected->setColor(Palette::blockOffset(8)+5);
	if (_detected)
	{
		_txtDetected->setText(_game->getResourcePack()->getLanguage()->getString(STR_DETECTED));
	}
	else
	{
		_txtDetected->setText("");
	}

	_txtUfo->setColor(Palette::blockOffset(8)+5);
	_txtUfo->setBig();
	_txtUfo->setText(_ufo->getName(_game->getResourcePack()->getLanguage()));
	
	_lstInfo->setColor(Palette::blockOffset(8)+5);
	_lstInfo->setColumns(2, 82, 78);
	_lstInfo->setDot(true);
	_lstInfo->addRow(0, 2, _game->getResourcePack()->getLanguage()->getString(STR_SIZE).c_str(), _game->getResourcePack()->getLanguage()->getString(_ufo->getRules()->getSize()).c_str());
	_lstInfo->getCell(0, 1)->setColor(Palette::blockOffset(8)+10);
	_lstInfo->addRow(0, 2, _game->getResourcePack()->getLanguage()->getString(STR_ALTITUDE).c_str(), _game->getResourcePack()->getLanguage()->getString(_ufo->getAltitude()).c_str());
	_lstInfo->getCell(1, 1)->setColor(Palette::blockOffset(8)+10);
	_lstInfo->addRow(0, 2, _game->getResourcePack()->getLanguage()->getString(STR_HEADING).c_str(), _game->getResourcePack()->getLanguage()->getString(_ufo->getDirection()).c_str());
	_lstInfo->getCell(2, 1)->setColor(Palette::blockOffset(8)+10);
	stringstream ss;
	ss << _ufo->getSpeed();
	_lstInfo->addRow(0, 2, _game->getResourcePack()->getLanguage()->getString(STR_SPEED).c_str(), ss.str().c_str());
	_lstInfo->getCell(3, 1)->setColor(Palette::blockOffset(8)+10);
	_lstInfo->draw();
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
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void UfoDetectedState::btnCentreClick(SDL_Event *ev, int scale)
{
	_state->timerReset();
	_state->getGlobe()->center(_ufo->getLongitude(), _ufo->getLatitude());
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void UfoDetectedState::btnCancelClick(SDL_Event *ev, int scale)
{
	_game->popState();
}
