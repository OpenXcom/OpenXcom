/*
 * Copyright 2010 OpenXcom Developers.
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
#include "ConfirmNewBaseState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Resource/LangString.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Region.h"
#include "../Savegame/Base.h"
#include "BaseNameState.h"
#include "GeoscapeErrorState.h"

/**
 * Initializes all the elements in the Confirm New Base window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to place.
 * @param globe Pointer to the Geoscape globe.
 */
ConfirmNewBaseState::ConfirmNewBaseState(Game *game, Base *base, Globe *globe) : State(game), _base(base), _globe(globe), _cost(0)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 224, 72, 16, 64);
	_btnOk = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 54, 12, 68, 104);
	_btnCancel = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 54, 12, 138, 104);
	_txtCost = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 9, 68, 80);
	_txtArea = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 9, 68, 90);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
	
	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtCost);
	add(_txtArea);
	
	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+2);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((ActionHandler)&ConfirmNewBaseState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(15)+2);
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString(STR_CANCEL_UC));
	_btnCancel->onMouseClick((ActionHandler)&ConfirmNewBaseState::btnCancelClick);

	std::stringstream ss;
	for (std::map<LangString, Region*>::iterator i = _game->getSavedGame()->getRegions()->begin(); i != _game->getSavedGame()->getRegions()->end(); i++)
	{
		if (i->second->insideRegion(_base->getLongitude(), _base->getLatitude()))
		{
			_cost = i->second->getBaseCost();
			ss << _game->getResourcePack()->getLanguage()->getString(STR_AREA) << _game->getResourcePack()->getLanguage()->getString(i->first);
			break;
		}
	}
	
	std::string s = _game->getResourcePack()->getLanguage()->getString(STR_COST);
	s.erase(s.size()-1, 1);
	s += Text::formatFunding(_cost);
	_txtCost->setColor(Palette::blockOffset(15)-1);
	_txtCost->setText(s);
	
	_txtArea->setColor(Palette::blockOffset(15)-1);
	_txtArea->setText(ss.str());
}

/**
 *
 */
ConfirmNewBaseState::~ConfirmNewBaseState()
{
	
}

/**
 * Go to the Place Access Lift screen.
 * @param action Pointer to an action.
 */
void ConfirmNewBaseState::btnOkClick(Action *action)
{
	if (_game->getSavedGame()->getFunds() >= _cost)
	{
		_game->getSavedGame()->setFunds(_game->getSavedGame()->getFunds() - _cost);
		_game->getSavedGame()->getBases()->push_back(_base);
		_game->pushState(new BaseNameState(_game, _base, _globe, false));
	}
	else
	{
		_game->pushState(new GeoscapeErrorState(_game, STR_NOT_ENOUGH_MONEY));
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ConfirmNewBaseState::btnCancelClick(Action *action)
{
	_game->popState();
}
