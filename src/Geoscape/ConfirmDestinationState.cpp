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
#include "ConfirmDestinationState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Target.h"
#include "../Savegame/Waypoint.h"
#include "../Savegame/Base.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Confirm Destination window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to retarget.
 * @param target Pointer to the selected target (NULL if it's just a point on the globe).
 */
ConfirmDestinationState::ConfirmDestinationState(Game *game, Craft *craft, Target *target) : State(game), _craft(craft), _target(target)
{
	Waypoint *w = dynamic_cast<Waypoint*>(_target);
	_screen = false;

	// Create objects
	_window = new Window(this, 224, 72, 16, 64);
	_btnOk = new TextButton(50, 12, 68, 104);
	_btnCancel = new TextButton(50, 12, 138, 104);
	_txtTarget = new Text(214, 32, 21, 72);

	// Set palette
	if (w != 0 && w->getId() == 0)
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
	}
	else
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);
	}

	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTarget);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ConfirmDestinationState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ConfirmDestinationState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&ConfirmDestinationState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ConfirmDestinationState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTarget->setColor(Palette::blockOffset(15)-1);
	_txtTarget->setBig();
	_txtTarget->setAlign(ALIGN_CENTER);
	_txtTarget->setVerticalAlign(ALIGN_MIDDLE);
	_txtTarget->setWordWrap(true);
	std::wstringstream ss;
	if (w != 0 && w->getId() == 0)
	{
		ss << _game->getLanguage()->getString("STR_TARGET_WAY_POINT");
	}
	else
	{
		ss << _game->getLanguage()->getString("STR_TARGET") << _target->getName(_game->getLanguage());
	}
	_txtTarget->setText(ss.str());
}

/**
 *
 */
ConfirmDestinationState::~ConfirmDestinationState()
{

}

/**
 * Confirms the selected target for the craft.
 * @param action Pointer to an action.
 */
void ConfirmDestinationState::btnOkClick(Action *)
{
	Waypoint *w = dynamic_cast<Waypoint*>(_target);
	if (w != 0 && w->getId() == 0)
	{
		w->setId(_game->getSavedGame()->getId("STR_WAYPOINT"));
		_game->getSavedGame()->getWaypoints()->push_back(w);
	}
	_craft->setDestination(_target);
	_craft->setStatus("STR_OUT");
	if(_craft->getInterceptionOrder() == 0)
	{
		int maxInterceptionOrder = 0;
		for(std::vector<Base*>::iterator baseIt = _game->getSavedGame()->getBases()->begin(); baseIt != _game->getSavedGame()->getBases()->end(); ++baseIt)
		{
			for(std::vector<Craft*>::iterator craftIt = (*baseIt)->getCrafts()->begin(); craftIt != (*baseIt)->getCrafts()->end(); ++craftIt)
			{
				if((*craftIt)->getInterceptionOrder() > maxInterceptionOrder)
				{
					maxInterceptionOrder = (*craftIt)->getInterceptionOrder();
				}
			}
		}
		_craft->setInterceptionOrder(++maxInterceptionOrder);
	}
	_game->popState();
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ConfirmDestinationState::btnCancelClick(Action *)
{
	Waypoint *w = dynamic_cast<Waypoint*>(_target);
	if (w != 0 && w->getId() == 0)
	{
		delete w;
	}
	_game->popState();
}

}
