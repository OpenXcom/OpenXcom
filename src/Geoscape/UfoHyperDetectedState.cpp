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
#include "UfoHyperDetectedState.h"
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
#include "../Savegame/Region.h"
#include "../Ruleset/RuleRegion.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Ufo Detected window.
 * @param game Pointer to the core game.
 * @param ufo Pointer to the UFO to get info from.
 * @param state Pointer to the Geoscape.
 * @param detected Was the UFO detected?
 */
UfoHyperDetectedState::UfoHyperDetectedState(Game *game, Ufo *ufo, GeoscapeState *state, bool detected) : State(game), _ufo(ufo), _state(state), _detected(detected)
{
	// Generate UFO ID
	if (_ufo->getId() == 0)
	{
		_ufo->setId(_game->getSavedGame()->getId("STR_UFO"));
	}

	_screen = false;

	// Create objects
	_window = new Window(this, 224, 180, 16, 10, POPUP_BOTH);
	_txtUfo = new Text(160, 16, 28, 20);
	_txtDetected = new Text(80, 8, 28, 36);
	_txtDetected2 = new Text(224, 8, 14, 44);
	_lstInfo = new TextList(176, 32, 28, 60);
	_lstInfo2 = new TextList(176, 32, 28, 96);
	_btnCentre = new TextButton(160, 12, 48, 144);
	_btnCancel = new TextButton(160, 12, 48, 160);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_btnCentre);
	add(_btnCancel);
	add(_txtUfo);
	add(_txtDetected);
	add(_txtDetected2);
	add(_lstInfo);
	add(_lstInfo2);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK15.SCR"));

	_btnCentre->setColor(Palette::blockOffset(8)+5);
	_btnCentre->setText(_game->getLanguage()->getString("STR_CENTER_ON_UFO_TIME_5_SECS"));
	_btnCentre->onMouseClick((ActionHandler)&UfoHyperDetectedState::btnCentreClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&UfoHyperDetectedState::btnCancelClick);

	_txtDetected->setColor(Palette::blockOffset(8)+5);
	_txtDetected2->setColor(Palette::blockOffset(8)+5);
	_txtDetected2->setAlign(ALIGN_CENTER);
	_txtDetected2->setText(_game->getLanguage()->getString("STR_HYPER_WAVE_TRANSMISSIONS_ARE_DECODED"));
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
	_lstInfo2->setColor(Palette::blockOffset(8)+5);
	_lstInfo2->setColumns(2, 82, 94);
	_lstInfo2->setDot(true);
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
	_lstInfo2->addRow(2, _game->getLanguage()->getString("STR_CRAFT_TYPE").c_str(), _game->getLanguage()->getString(_ufo->getRules()->getType()).c_str());
	_lstInfo2->setCellColor(0, 1, Palette::blockOffset(8)+10);
	_lstInfo2->addRow(2, _game->getLanguage()->getString("STR_RACE").c_str(), _game->getLanguage()->getString(_ufo->getAlienRace()).c_str());
	_lstInfo2->setCellColor(1, 1, Palette::blockOffset(8)+10);
	_lstInfo2->addRow(2, _game->getLanguage()->getString("STR_MISSION").c_str(), _game->getLanguage()->getString(_ufo->getMission()).c_str());
	_lstInfo2->setCellColor(2, 1, Palette::blockOffset(8)+10);
	bool set = false;
	for (std::vector<Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); ++i)
	{
		if((*i)->getRules()->insideRegion(_ufo->getDestination()->getLongitude(), _ufo->getDestination()->getLatitude()) && !set)
		{
			_lstInfo2->addRow(2, _game->getLanguage()->getString("STR_ZONE").c_str(), _game->getLanguage()->getString((*i)->getRules()->getType()).c_str());
			set = true;
		}
	}
	_lstInfo2->setCellColor(3, 1, Palette::blockOffset(8)+10);
}

/**
 *
 */
UfoHyperDetectedState::~UfoHyperDetectedState()
{

}

/**
 * Resets the palette.
 */
void UfoHyperDetectedState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);
}

/**
 * Centers on the UFO and returns to the previous screen.
 * @param action Pointer to an action.
 */
void UfoHyperDetectedState::btnCentreClick(Action *action)
{
	_state->timerReset();
	_state->getGlobe()->center(_ufo->getLongitude(), _ufo->getLatitude());
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void UfoHyperDetectedState::btnCancelClick(Action *action)
{
	_game->popState();
}

}
