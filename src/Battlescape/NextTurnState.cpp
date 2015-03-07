/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "NextTurnState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/Options.h"
#include "../Engine/Timer.h"
#include "../Engine/Screen.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Action.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "DebriefingState.h"
#include "../Interface/Cursor.h"
#include "BattlescapeState.h"
#include "../Menu/SaveGameState.h"
#include "Map.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Next Turn screen.
 * @param game Pointer to the core game.
 * @param battleGame Pointer to the saved game.
 * @param state Pointer to the Battlescape state.
 */
NextTurnState::NextTurnState(SavedBattleGame *battleGame, BattlescapeState *state) : _battleGame(battleGame), _state(state), _timer(0)
{
	// Create objects
	int y = state->getMap()->getMessageY();

	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(320, 17, 0, 68);
	_txtTurn = new Text(320, 17, 0, 92);
	_txtSide = new Text(320, 17, 0, 108);
	_txtMessage = new Text(320, 17, 0, 132);
	_bg = new Surface(_game->getScreen()->getWidth(), _game->getScreen()->getWidth(), 0, 0);

	// Set palette
	battleGame->setPaletteByDepth(this);

	add(_bg);
	add(_window);
	add(_txtTitle, "messageWindows", "battlescape");
	add(_txtTurn, "messageWindows", "battlescape");
	add(_txtSide, "messageWindows", "battlescape");
	add(_txtMessage, "messageWindows", "battlescape");

	centerAllSurfaces();

	_bg->setX(0);
	_bg->setY(0);
	SDL_Rect rect;
	rect.h = _bg->getHeight();
	rect.w = _bg->getWidth();
	rect.x = rect.y = 0;

	_bg->drawRect(&rect, Palette::blockOffset(0) + 15);
	// make this screen line up with the hidden movement screen
	_window->setY(y);
	_txtTitle->setY(y + 68);
	_txtTurn->setY(y + 92);
	_txtSide->setY(y + 108);
	_txtMessage->setY(y + 132);

	// Set up objects
	_window->setColor(Palette::blockOffset(0)-1);
	_window->setHighContrast(true);
	_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));


	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setHighContrast(true);
	_txtTitle->setText(tr("STR_OPENXCOM"));


	_txtTurn->setBig();
	_txtTurn->setAlign(ALIGN_CENTER);
	_txtTurn->setHighContrast(true);
	_txtTurn->setText(tr("STR_TURN").arg(_battleGame->getTurn()));


	_txtSide->setBig();
	_txtSide->setAlign(ALIGN_CENTER);
	_txtSide->setHighContrast(true);
	_txtSide->setText(tr("STR_SIDE").arg(tr((_battleGame->getSide() == FACTION_PLAYER ? "STR_XCOM" : "STR_ALIENS"))));


	_txtMessage->setBig();
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setHighContrast(true);
	_txtMessage->setText(tr("STR_PRESS_BUTTON_TO_CONTINUE"));

	_state->clearMouseScrollingState();

	if (Options::skipNextTurnScreen)
	{
		_timer = new Timer(NEXT_TURN_DELAY);
		_timer->onTimer((StateHandler)&NextTurnState::close);
		_timer->start();
	}
}

/**
 *
 */
NextTurnState::~NextTurnState()
{
	delete _timer;
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void NextTurnState::handle(Action *action)
{
	State::handle(action);

	if (action->getDetails()->type == SDL_KEYDOWN || action->getDetails()->type == SDL_MOUSEBUTTONDOWN)
	{
		close();
	}
}

/**
 * Keeps the timer running.
 */
void NextTurnState::think()
{
	if (_timer)
	{
		_timer->think(this, 0);
	}
}

/**
 * Closes the window.
 */
void NextTurnState::close()
{
	_battleGame->getBattleGame()->cleanupDeleted();
	_game->popState();

	int liveAliens = 0;
	int liveSoldiers = 0;
	_state->getBattleGame()->tallyUnits(liveAliens, liveSoldiers);
	if (liveAliens == 0 || liveSoldiers == 0)
	{
		_state->finishBattle(false, liveSoldiers);
	}
	else
	{
		_state->btnCenterClick(0);

		// Autosave every set amount of turns
		if ((_battleGame->getTurn() == 1 || _battleGame->getTurn() % Options::autosaveFrequency == 0) && _battleGame->getSide() == FACTION_PLAYER)
		{
			if (_game->getSavedGame()->isIronman())
			{
				_game->pushState(new SaveGameState(OPT_BATTLESCAPE, SAVE_IRONMAN, _palette));
			}
			else if (Options::autosave)
			{
				_game->pushState(new SaveGameState(OPT_BATTLESCAPE, SAVE_AUTO_BATTLESCAPE, _palette));
			}
		}
	}
}

void NextTurnState::resize(int &dX, int &dY)
{
	State::resize(dX, dY);
	_bg->setX(0);
	_bg->setY(0);
}
}
