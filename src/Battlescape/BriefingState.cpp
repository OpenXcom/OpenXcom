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
#include "BriefingState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Craft.h"
#include "../Engine/Music.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Ufo.h"
#include "BattlescapeState.h"
#include "NextTurnState.h"
#include "InventoryState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Briefing screen.
 * @param game Pointer to the core game.
 */
BriefingState::BriefingState(Game *game, Craft *craft) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(120, 18, 100, 164);
	_txtTitle = new Text(274, 16, 16, 24);
	_txtTarget = new Text(274, 16, 16, 40);
	_txtCraft = new Text(274, 16, 16, 56);
	_txtBriefing = new Text(274, 64, 16, 72);

	std::string mission = _game->getSavedGame()->getBattleGame()->getMissionType();

	// Set palette
	if (mission == "STR_TERROR_MISSION" || mission == "STR_BASE_DEFENSE")
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);
		_game->getResourcePack()->getMusic("GMENBASE")->play();
	}
	else
	{
		_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
		_game->getResourcePack()->getMusic("GMDEFEND")->play();
	}

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	if (mission == "STR_ALIEN_BASE_ASSAULT")
	{
		_txtCraft->setY(40);
		_txtBriefing->setY(56);
	}
	else
	{
		add(_txtTarget);
	}
	add(_txtCraft);
	add(_txtBriefing);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BriefingState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();

	_txtTarget->setColor(Palette::blockOffset(8)+5);
	_txtTarget->setBig();
	_txtTarget->setText(craft->getDestination()->getName(_game->getLanguage()));

	_txtCraft->setColor(Palette::blockOffset(8)+5);
	_txtCraft->setBig();
	std::wstringstream ss;
	ss << _game->getLanguage()->getString("STR_CRAFT_") << craft->getName(_game->getLanguage());
	_txtCraft->setText(ss.str());

	_txtBriefing->setColor(Palette::blockOffset(8)+5);
	_txtBriefing->setWordWrap(true);

	// Show respective mission briefing
	if (mission == "STR_ALIEN_BASE_ASSAULT")
	{
		_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));
	}
	else
	{
		_window->setBackground(_game->getResourcePack()->getSurface("BACK16.SCR"));
	}

	if (mission == "STR_BASE_DEFENSE")
	{
		_txtTitle->setText(_game->getLanguage()->getString("STR_BASE_DEFENSE"));
		_txtBriefing->setText(_game->getLanguage()->getString("STR_BASE_DEFENSE_BRIEFING"));
	}
	else if (mission == "STR_TERROR_MISSION")
	{
		_txtTitle->setText(_game->getLanguage()->getString("STR_TERROR_MISSION"));
		_txtBriefing->setText(_game->getLanguage()->getString("STR_TERROR_MISSION_BRIEFING"));
	}
	else if (mission == "STR_ALIEN_BASE_ASSAULT")
	{
		_txtTitle->setText(_game->getLanguage()->getString("STR_ALIEN_BASE_ASSAULT"));
		_txtBriefing->setText(_game->getLanguage()->getString("STR_ALIEN_BASE_ASSAULT_BRIEFING"));
	}
	else
	{
		Ufo* u = dynamic_cast<Ufo*>(craft->getDestination());
		if (u->isCrashed())
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_CRASH_RECOVERY"));
			_txtBriefing->setText(_game->getLanguage()->getString("STR_UFO_CRASH_RECOVERY_BRIEFING"));
		}
		else
		{
			_txtTitle->setText(_game->getLanguage()->getString("STR_UFO_GROUND_ASSAULT"));
			_txtBriefing->setText(_game->getLanguage()->getString("STR_UFO_GROUND_ASSAULT_BRIEFING"));
		}
	}
}

/**
 *
 */
BriefingState::~BriefingState()
{

}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void BriefingState::btnOkClick(Action *action)
{
	_game->popState();
	BattlescapeState *bs = new BattlescapeState(_game);
	_game->pushState(bs);
	_game->pushState(new NextTurnState(_game, _game->getSavedGame()->getBattleGame(), bs));
	_game->pushState(new InventoryState(_game, false));
}

}
