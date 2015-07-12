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
#include "../Ruleset/MapScript.h"
#include "../Savegame/Tile.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Abort Mission window.
 * @param game Pointer to the core game.
 * @param battleGame Pointer to the saved game.
 * @param state Pointer to the Battlescape state.
 */
AbortMissionState::AbortMissionState(SavedBattleGame *battleGame, BattlescapeState *state) : _battleGame(battleGame), _state(state), _inEntrance(0), _inExit(0), _outside(0)
{
	// Create objects
	_screen = false;
	_window = new Window(this, 320, 144, 0, 0);
	_txtInEntrance = new Text(304, 17, 16, 20);
	_txtInExit = new Text(304, 17, 16, 40);
	_txtOutside = new Text(304, 17, 16, 60);
	_txtAbort = new Text(320, 17, 0, 80);
	_btnOk = new TextButton(120, 16, 16, 110);
	_btnCancel = new TextButton(120, 16, 184, 110);

	// Set palette
	_battleGame->setPaletteByDepth(this);

	add(_window, "messageWindowBorder", "battlescape");
	add(_txtInEntrance, "messageWindows", "battlescape");
	add(_txtInExit, "messageWindows", "battlescape");
	add(_txtOutside, "messageWindows", "battlescape");
	add(_txtAbort, "messageWindows", "battlescape");
	add(_btnOk, "messageWindowButtons", "battlescape");
	add(_btnCancel, "messageWindowButtons", "battlescape");

	// Check available areas (maybe should be cached somewhere)
	bool exit = false, craft = true;
	AlienDeployment *deployment = _game->getRuleset()->getDeployment(_battleGame->getMissionType());
	if (deployment != 0)
	{
		exit = !deployment->getNextStage().empty();
		const std::vector<MapScript*> *scripts = _game->getRuleset()->getMapScript(deployment->getScript());
		if (scripts != 0)
		{
			craft = false;
			for (std::vector<MapScript*>::const_iterator i = scripts->begin(); i != scripts->end(); ++i)
			{
				if ((*i)->getType() == MSC_ADDCRAFT)
				{
					craft = true;
					break;
				}
			}
		}
	}
	if (exit)
	{
		exit = false;
		for (int i = 0; i < _battleGame->getMapSizeXYZ(); ++i)
		{
			Tile *tile = _battleGame->getTiles()[i];
			if (tile && tile->getMapData(O_FLOOR) && tile->getMapData(O_FLOOR)->getSpecialType() == END_POINT)
			{
				exit = true;
				break;
			}
		}
	}

	// Calculate values
	for (std::vector<BattleUnit*>::iterator i = _battleGame->getUnits()->begin(); i != _battleGame->getUnits()->end(); ++i)
	{
		if ((*i)->getOriginalFaction() == FACTION_PLAYER && !(*i)->isOut())
		{
			if ((*i)->isInExitArea(START_POINT))
			{
				_inEntrance++;
			}
			else if ((*i)->isInExitArea(END_POINT))
			{
				_inExit++;
			}
			else
			{
				_outside++;
			}
		}
	}

	// Set up objects
	_window->setHighContrast(true);
	_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));

	_txtInEntrance->setBig();
	_txtInEntrance->setHighContrast(true);
	if (craft)
	{
		_txtInEntrance->setText(tr("STR_UNITS_IN_CRAFT", _inEntrance));		
	}
	else
	{
		_txtInEntrance->setText(tr("STR_UNITS_IN_ENTRANCE", _inEntrance));		
	}

	_txtInExit->setBig();
	_txtInExit->setHighContrast(true);
	_txtInExit->setText(tr("STR_UNITS_IN_EXIT", _inExit));

	_txtOutside->setBig();
	_txtOutside->setHighContrast(true);
	_txtOutside->setText(tr("STR_UNITS_OUTSIDE", _outside));


	if (_battleGame->getMissionType() == "STR_BASE_DEFENSE")
	{
		_txtInEntrance->setVisible(false);
		_txtInExit->setVisible(false);
		_txtOutside->setVisible(false);
	}
	else if (!exit)
	{
		_txtInEntrance->setY(26);
		_txtOutside->setY(54);
		_txtInExit->setVisible(false);
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

	centerAllSurfaces();
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
	_state->finishBattle(true, _inExit);
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
