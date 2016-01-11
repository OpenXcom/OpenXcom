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
#include "BaseDestroyedState.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Region.h"
#include "../Savegame/AlienMission.h"
#include "../Savegame/Ufo.h"
#include "../Mod/RuleRegion.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

BaseDestroyedState::BaseDestroyedState(Base *base) : _base(base)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 160, 32, 20);
	_btnOk = new TextButton(100, 20, 110, 142);
	_txtMessage = new Text(224, 48, 48, 76);

	// Set palette
	setInterface("UFOInfo");

	add(_window, "window", "UFOInfo");
	add(_btnOk, "button", "UFOInfo");
	add(_txtMessage, "text", "UFOInfo");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK15.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BaseDestroyedState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&BaseDestroyedState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&BaseDestroyedState::btnOkClick, Options::keyCancel);

	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);

	_txtMessage->setText(tr("STR_THE_ALIENS_HAVE_DESTROYED_THE_UNDEFENDED_BASE").arg(_base->getName()));

	std::vector<Region*>::iterator k = _game->getSavedGame()->getRegions()->begin();
	for (; k != _game->getSavedGame()->getRegions()->end(); ++k)
	{
		if ((*k)->getRules()->insideRegion((base)->getLongitude(), (base)->getLatitude()))
		{
			break;
		}
	}

	AlienMission* am = _game->getSavedGame()->findAlienMission((*k)->getRules()->getType(), OBJECTIVE_RETALIATION);
	for (std::vector<Ufo*>::iterator i = _game->getSavedGame()->getUfos()->begin(); i != _game->getSavedGame()->getUfos()->end();)
	{
		if ((*i)->getMission() == am)
		{
			delete *i;
			i = _game->getSavedGame()->getUfos()->erase(i);
		}
		else
		{
			++i;
		}
	}

	for (std::vector<AlienMission*>::iterator i = _game->getSavedGame()->getAlienMissions().begin();
		i != _game->getSavedGame()->getAlienMissions().end(); ++i)
	{
		if ((AlienMission*)(*i) == am)
		{
			delete (*i);
			_game->getSavedGame()->getAlienMissions().erase(i);
			break;
		}
	}
}

/**
 *
 */
BaseDestroyedState::~BaseDestroyedState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BaseDestroyedState::btnOkClick(Action *)
{
	_game->popState();
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		if ((*i) == _base)
		{
			delete (*i);
			_game->getSavedGame()->getBases()->erase(i);
			break;
		}
	}
}

}
