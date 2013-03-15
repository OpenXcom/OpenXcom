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
#include "AbortMissionState.h"
#include <sstream>
#include <vector>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Engine/Action.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Interface/Cursor.h"
#include "../Interface/FpsCounter.h"
#include "BattlescapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Abort Mission window.
 * @param game Pointer to the core game.
 * @param battleGame Pointer to the saved game.
 * @param state Pointer to the Battlescape state.
 */
AbortMissionState::AbortMissionState(Game *game, SavedBattleGame *battleGame, BattlescapeState *state) : State(game), _battleGame(battleGame), _state(state), _inExitArea(0), _outExitArea(0)
{
	// Create objects
	_screen = false;
	_window = new Window(this, 320, 144, 0, 0);
	_txtInExit = new Text(304, 16, 16, 25);
	_txtOutsideExit = new Text(304, 16, 16, 50);
	_txtAbort = new Text(320, 16, 0, 75);
	_btnOk = new TextButton(120, 16, 16, 110);
	_btnCancel = new TextButton(120, 16, 184, 110);

	add(_window);
	add(_txtInExit);
	add(_txtOutsideExit);
	add(_txtAbort);
	add(_btnOk);
	add(_btnCancel);

	// Calculate values
	for (std::vector<BattleUnit*>::iterator i = _battleGame->getUnits()->begin(); i != _battleGame->getUnits()->end(); ++i)
	{
		if ((*i)->getFaction() == FACTION_PLAYER && !(*i)->isOut())
		{
			if ((_battleGame->getNextStage() != "" && (*i)->isInExitArea(END_POINT)) || ((*i)->isInExitArea() && _battleGame->getNextStage() == ""))
			{
				_inExitArea++;
			}
			else
			{
				_outExitArea++;
			}
		}
	}

	// Set up objects
	_window->setColor(Palette::blockOffset(0));
	_window->setHighContrast(true);
	_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));

	_txtInExit->setColor(Palette::blockOffset(0));
	_txtInExit->setBig();
	_txtInExit->setHighContrast(true);
	_txtInExit->setText(tr("STR_n_UNITS_IN_EXIT_AREA", _inExitArea));

	_txtOutsideExit->setColor(Palette::blockOffset(0));
	_txtOutsideExit->setBig();
	_txtOutsideExit->setHighContrast(true);
	_txtOutsideExit->setText(tr("STR_n_UNITS_OUTSIDE_EXIT_AREA", _outExitArea));
	if (_battleGame->getMissionType() == "STR_BASE_DEFENSE")
	{
		_txtInExit->setVisible(false);
		_txtOutsideExit->setVisible(false);
	}
	_txtAbort->setColor(Palette::blockOffset(0));
	_txtAbort->setBig();
	_txtAbort->setAlign(ALIGN_CENTER);
	_txtAbort->setHighContrast(true);
	_txtAbort->setText(_game->getLanguage()->getString("STR_ABORT_MISSION"));

	_btnOk->setColor(Palette::blockOffset(0));
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->setHighContrast(true);
	_btnOk->onMouseClick((ActionHandler)&AbortMissionState::btnOkClick);

	_btnCancel->setColor(Palette::blockOffset(0));
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->setHighContrast(true);
	_btnCancel->onMouseClick((ActionHandler)&AbortMissionState::btnCancelClick);

}

/**
 *
 */
AbortMissionState::~AbortMissionState()
{

}

/**
 * Confirms to abort the mission.
 * @param action Pointer to an action.
 */
void AbortMissionState::btnOkClick(Action *)
{
	_game->popState();
	_battleGame->setAborted(true);
	_state->finishBattle(true, _inExitArea);
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void AbortMissionState::btnCancelClick(Action *)
{
	_game->popState();
}


}
