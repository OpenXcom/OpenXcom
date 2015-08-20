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
#include "ConfirmCydoniaState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Battlescape/BriefingState.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/AlienDeployment.h"
#include "../Engine/Options.h"
#include "../Ruleset/Ruleset.h"

namespace OpenXcom
{

ConfirmCydoniaState::ConfirmCydoniaState(Craft *craft) : _craft(craft)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 160, 32, 20);
	_btnYes = new TextButton(80, 20, 70, 142);
	_btnNo = new TextButton(80, 20, 170, 142);
	_txtMessage = new Text(224, 48, 48, 76);

	// Set palette
	setInterface("confirmCydonia");

	add(_window, "window", "confirmCydonia");
	add(_btnYes, "button", "confirmCydonia");
	add(_btnNo, "button", "confirmCydonia");
	add(_txtMessage, "text", "confirmCydonia");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnYes->setText(tr("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&ConfirmCydoniaState::btnYesClick);
	_btnYes->onKeyboardPress((ActionHandler)&ConfirmCydoniaState::btnYesClick, Options::keyOk);

	_btnNo->setText(tr("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&ConfirmCydoniaState::btnNoClick);
	_btnNo->onKeyboardPress((ActionHandler)&ConfirmCydoniaState::btnNoClick, Options::keyCancel);

	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);
	_txtMessage->setText(tr("STR_ARE_YOU_SURE_CYDONIA"));
}

/**
 *
 */
ConfirmCydoniaState::~ConfirmCydoniaState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ConfirmCydoniaState::btnYesClick(Action *)
{
	_game->popState();
	_game->popState();

	SavedBattleGame *bgame = new SavedBattleGame();
	_game->getSavedGame()->setBattleGame(bgame);
	BattlescapeGenerator bgen = BattlescapeGenerator(_game);
	for (std::vector<std::string>::const_iterator i = _game->getRuleset()->getDeploymentsList().begin(); i != _game->getRuleset()->getDeploymentsList().end(); ++i)
	{
		AlienDeployment *deployment = _game->getRuleset()->getDeployment(*i);
		if (deployment->isFinalDestination())
		{
			bgame->setMissionType(*i);
			bgen.setAlienRace(deployment->getRace());
			break;
		}
	}
	bgen.setCraft(_craft);
	bgen.run();

	_game->pushState(new BriefingState(_craft));

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ConfirmCydoniaState::btnNoClick(Action *)
{
	_game->popState();
}

}
