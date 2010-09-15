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
#include "ConfirmDestinationState.h"
#include <sstream>
#include "Game.h"
#include "ResourcePack.h"
#include "Language.h"
#include "LangString.h"
#include "Font.h"
#include "Palette.h"
#include "Surface.h"
#include "Window.h"
#include "Text.h"
#include "TextButton.h"
#include "SavedGame.h"
#include "Craft.h"
#include "Ufo.h"

using namespace std;

/**
 * Initializes all the elements in the Confirm Destination window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to place.
 * @param globe Pointer to the Geoscape globe.
 */
ConfirmDestinationState::ConfirmDestinationState(Game *game, Craft *craft, Target *target) : State(game), _craft(craft), _target(target)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 224, 72, 16, 64);
	_btnOk = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 50, 12, 68, 104);
	_btnCancel = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 50, 12, 138, 104);
	_txtTarget = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 214, 16, 21, 80);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);
	
	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTarget);
	
	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBackground(game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+8);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&ConfirmDestinationState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(8)+8);
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString(STR_CANCEL_UC));
	_btnCancel->onMouseClick((EventHandler)&ConfirmDestinationState::btnCancelClick);

	_txtTarget->setColor(Palette::blockOffset(15)-1);
	_txtTarget->setBig();
	_txtTarget->setAlign(ALIGN_CENTER);
	Ufo* u = dynamic_cast<Ufo*>(_target);
	if (u)
	{
		stringstream ss;
		ss << _game->getResourcePack()->getLanguage()->getString(STR_TARGET) << _game->getResourcePack()->getLanguage()->getString(STR_UFO_) << u->getId();
		_txtTarget->setText(ss.str());
	}
}

/**
 *
 */
ConfirmDestinationState::~ConfirmDestinationState()
{
	
}

/**
 * Confirms the selected target for the craft.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void ConfirmDestinationState::btnOkClick(SDL_Event *ev, int scale)
{
	_craft->setTarget(_target);
	_craft->setStatus(STR_OUT);
	_game->popState();
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void ConfirmDestinationState::btnCancelClick(SDL_Event *ev, int scale)
{
	_game->popState();
}
