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
#include "ConfirmEventDestinationState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Battlescape/BattlescapeState.h"
#include "../Battlescape/BriefingState.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/RNG.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleMissionSequence.h"
#include "../Engine/Options.h"
#include "../Savegame/ScriptedEvent.h"

namespace OpenXcom
{

ConfirmEventDestinationState::ConfirmEventDestinationState(Game *game, Craft *craft, ScriptedEvent *scriptedEvent) : _game(game), _craft(craft), _scriptedEvent(scriptedEvent), _screenData(NULL)
{
	_screen = false;

	_screenData = &_scriptedEvent->getRules().getConfirmScreen();

	// Create objects
	_window = new Window(this, 256, 160, 32, 20);
	_btnYes = new TextButton(80, 20, 70, 142);
	_btnNo = new TextButton(80, 20, 170, 142);
	_txtMessage = new Text(224, 48, 48, 76);


	// Set palette
	setPalette("PAL_GEOSCAPE", _screenData->palette);

	add(_window);
	add(_btnYes);
	add(_btnNo);
	add(_txtMessage);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);

	_window->setBackground(_game->getResourcePack()->getSurface(_screenData->background_id));

	_btnYes->setColor(Palette::blockOffset(8)+5);
	_btnYes->setText(tr("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&ConfirmEventDestinationState::btnYesClick);
	_btnYes->onKeyboardPress((ActionHandler)&ConfirmEventDestinationState::btnYesClick, Options::keyOk);

	_btnNo->setColor(Palette::blockOffset(8)+5);
	_btnNo->setText(tr("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&ConfirmEventDestinationState::btnNoClick);
	_btnNo->onKeyboardPress((ActionHandler)&ConfirmEventDestinationState::btnNoClick, Options::keyCancel);

	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);
	_txtMessage->setColor(Palette::blockOffset(8)+10);
	_txtMessage->setText(tr(_screenData->mainText_id));
}

/**
 *
 */
ConfirmEventDestinationState::~ConfirmEventDestinationState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ConfirmEventDestinationState::btnYesClick(Action *)
{
	_game->popState();
	_game->popState();
	
	SavedBattleGame *bgame = new SavedBattleGame();

	RuleMissionSequence *missionSeq = _game->getRuleset()->getMissionSequence(_scriptedEvent->getType());
	MissionData mission = missionSeq->getMission(0);

	_game->getSavedGame()->setBattleGame(bgame);
	bgame->setMissionType(mission.deployment_id);
	bgame->setScriptedEventType(_scriptedEvent->getType());
	bgame->setMissionSequenceType(missionSeq->getType());

	BattlescapeGenerator bgen = BattlescapeGenerator(_game);
	bgen.setCraft(_craft);
	bgen.setAlienRace(mission.race_id);
	bgen.setWorldShade(mission.shade);
	bgen.run();

	_game->pushState(new BriefingState(_craft));
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ConfirmEventDestinationState::btnNoClick(Action *)
{
	_game->popState();
}

}
