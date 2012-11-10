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
#include "AlienBaseState.h"
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
#include "../Savegame/Region.h"
#include "../Ruleset/RuleRegion.h"
#include "../Savegame/AlienBase.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Aliens Terrorise window.
 * @param game Pointer to the core game.
 * @param city Pointer to the city to get info from.
 * @param state Pointer to the Geoscape.
 */
AlienBaseState::AlienBaseState(Game *game, AlienBase *base, GeoscapeState *state) : State(game), _base(base), _state(state)
{
	
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(50, 12, 135, 180);
	_txtTitle = new Text(320, 60, 0, 64);

	add(_window);
	add(_btnOk);
	add(_txtTitle);

	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));
	
	_btnOk->setColor(Palette::blockOffset(15)-1);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&AlienBaseState::btnOkClick);

	for (std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin(); k != _game->getSavedGame()->getRegions()->end(); ++k)
	{
		if((*k)->getRules()->insideRegion(_base->getLongitude(), _base->getLatitude())) 
		{
			_region = *k;
		}
	}
	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setWordWrap(true);
	_txtTitle->setText(_game->getLanguage()->getString("STR_XCOM_AGENTS_HAVE_LOCATED_AN_ALIEN_BASE_IN") + _game->getLanguage()->getString(_region->getRules()->getType()));
}

/**
 *
 */
AlienBaseState::~AlienBaseState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void AlienBaseState::btnOkClick(Action *action)
{
	_game->popState();
}

}
