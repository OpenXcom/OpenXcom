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
#include "ConfirmCydoniaState.h"
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
#include "../Savegame/SavedGame.h"
#include "../Engine/RNG.h"
#include "../Ruleset/Ruleset.h"

namespace OpenXcom
{

ConfirmCydoniaState::ConfirmCydoniaState(Game *game, Craft *craft) : State(game), _craft(craft)
{
	_screen = false;
	// Create objects
	_window = new Window(this, 256, 160, 32, 20);
	_btnConfirm = new TextButton(80, 20, 70, 142);
	_btnCancel = new TextButton(80, 20, 170, 142);
	_txtMessage = new Text(224, 48, 48, 76);
	
	add(_window);
	add(_btnConfirm);
	add(_btnCancel);
	add(_txtMessage);
	
	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK12.SCR"));

	_btnConfirm->setColor(Palette::blockOffset(8)+5);
	_btnConfirm->setText(_game->getLanguage()->getString("STR_YES"));
	_btnConfirm->onMouseClick((ActionHandler)&ConfirmCydoniaState::btnConfirmClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_NO"));
	_btnCancel->onMouseClick((ActionHandler)&ConfirmCydoniaState::btnCancelClick);
	
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);
	_txtMessage->setColor(Palette::blockOffset(8)+10);
	_txtMessage->setText(_game->getLanguage()->getString("STR_ARE_YOU_SURE_CYDONIA"));
}

/**
 *
 */
ConfirmCydoniaState::~ConfirmCydoniaState()
{
}

/**
 *
 */
void ConfirmCydoniaState::init()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ConfirmCydoniaState::btnConfirmClick(Action *)
{
	_game->popState();
	_game->popState();
	
	int month = _game->getSavedGame()->getMonthsPassed();
	SavedBattleGame *bgame = new SavedBattleGame();
	_game->getSavedGame()->setBattleGame(bgame);
	bgame->setMissionType("STR_MARS_CYDONIA_LANDING");
	BattlescapeGenerator bgen = BattlescapeGenerator(_game);
	bgen.setCraft(_craft);
	bgen.setAlienRace("STR_SECTOID");
	bgen.setAlienItemlevel(_game->getRuleset()->getAlienItemLevels().at(month).at(RNG::generate(0,9)));
	bgen.setWorldShade(15);
	bgen.run();

	_game->pushState(new BriefingState(_game, _craft));
	
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ConfirmCydoniaState::btnCancelClick(Action *)
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
	_game->popState();
}

}