/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "BattlescapeState.h"
#include "../Engine/Options.h"
#include "../Ruleset/AlienDeployment.h"
#include "../Ruleset/Ruleset.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Abort Mission window.
 * @param game Pointer to the core game.
 * @param battleGame Pointer to the saved game.
 * @param state Pointer to the Battlescape state.
 */
AbortMissionState::AbortMissionState(SavedBattleGame *battleGame, BattlescapeState *state) : _battleGame(battleGame), _state(state), _inExitArea(0), _outExitArea(0)
{
	// Create objects
	_screen = false;
	_window = new Window(this, 320, 144, 0, 0);
	_txtInExit = new Text(304, 17, 16, 25);
	_txtOutsideExit = new Text(304, 17, 16, 50);
	_txtAbort = new Text(320, 17, 0, 75);
	_btnOk = new TextButton(120, 16, 16, 110);
	_btnCancel = new TextButton(120, 16, 184, 110);

	// Set palette
	_battleGame->setPaletteByDepth(this);

	add(_window);
	add(_txtInExit, "messageWindows", "battlescape");
	add(_txtOutsideExit, "messageWindows", "battlescape");
	add(_txtAbort, "messageWindows", "battlescape");
	add(_btnOk, "messageWindows", "battlescape");
	add(_btnCancel, "messageWindows", "battlescape");

	centerAllSurfaces();

	std::string nextStage;
	if (_battleGame->getMissionType() != "STR_UFO_GROUND_ASSAULT" && _battleGame->getMissionType() != "STR_UFO_CRASH_RECOVERY")
	{
		nextStage = _game->getRuleset()->getDeployment(_battleGame->getMissionType())->getNextStage();
	}

	// Calculate values
	for (std::vector<BattleUnit*>::iterator i = _battleGame->getUnits()->begin(); i != _battleGame->getUnits()->end(); ++i)
	{
		if ((*i)->getOriginalFaction() == FACTION_PLAYER && !(*i)->isOut())
		{
			if ((!nextStage.empty() && (*i)->isInExitArea(END_POINT)) || ((*i)->isInExitArea() && nextStage.empty()))
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
	_window->setColor(Palette::blockOffset(0)-1);
	_window->setHighContrast(true);
	_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));

	_txtInExit->setBig();
	_txtInExit->setAlign(ALIGN_CENTER);
	_txtInExit->setHighContrast(true);
	_txtInExit->setText(tr("STR_UNITS_IN_EXIT_AREA", _inExitArea));

	_txtOutsideExit->setBig();
	_txtOutsideExit->setAlign(ALIGN_CENTER);
	_txtOutsideExit->setHighContrast(true);
	_txtOutsideExit->setText(tr("STR_UNITS_OUTSIDE_EXIT_AREA", _outExitArea));
	if (_battleGame->getMissionType() == "STR_BASE_DEFENSE")
	{
		_txtInExit->setVisible(false);
		_txtOutsideExit->setVisible(false);
	}

	_txtAbort->setBig();
	_txtAbort->setAlign(ALIGN_CENTER);
	_txtAbort->setHighContrast(true);
	_txtAbort->setText(tr("STR_ABORT_MISSION_QUESTION"));


	_btnOk->setText(tr("STR_OK"));
	_btnOk->setHighContrast(true);
	_btnOk->onMouseClick((ActionHandler)&AbortMissionState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&AbortMissionState::btnOkClick, Options::keyOk);


	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->setHighContrast(true);
	_btnCancel->onMouseClick((ActionHandler)&AbortMissionState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&AbortMissionState::btnCancelClick, Options::keyCancel);
	_btnCancel->onKeyboardPress((ActionHandler)&AbortMissionState::btnCancelClick, Options::keyBattleAbort);
}

/**
 *
 */
AbortMissionState::~AbortMissionState()
{

}

/**
 * Confirms mission abort.
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
