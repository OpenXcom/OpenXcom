/*
 * Copyright 2010 OpenXcom Developers.
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
#include "ConfirmLandingState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include <string>
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Target.h"
#include "../Savegame/Ufo.h"
#include "../Ruleset/Ruleset.h"
#include "../Battlescape/BriefingCrashState.h"

/**
 * Initializes all the elements in the Confirm Landing window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to confirm.
 * @param texture Texture of the landing site.
 */
ConfirmLandingState::ConfirmLandingState(Game *game, Craft *craft, int texture) : State(game), _craft(craft), _texture(texture)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 216, 160, 20, 20, POPUP_BOTH);
	_btnYes = new TextButton(80, 20, 40, 150);
	_btnNo = new TextButton(80, 20, 136, 150);
	_txtCraft = new Text(206, 16, 25, 40);
	_txtTarget = new Text(206, 16, 25, 88);
	_txtReady = new Text(206, 32, 25, 56);
	_txtBegin = new Text(206, 16, 25, 130);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);
	
	add(_window);
	add(_btnYes);
	add(_btnNo);
	add(_txtCraft);
	add(_txtTarget);
	add(_txtReady);
	add(_txtBegin);
	
	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK15.SCR"));

	_btnYes->setColor(Palette::blockOffset(8)+8);
	_btnYes->setText(_game->getResourcePack()->getLanguage()->getString("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&ConfirmLandingState::btnYesClick);

	_btnNo->setColor(Palette::blockOffset(8)+8);
	_btnNo->setText(_game->getResourcePack()->getLanguage()->getString("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&ConfirmLandingState::btnNoClick);

	_txtCraft->setColor(Palette::blockOffset(8)+10);
	_txtCraft->setBig();
	_txtCraft->setAlign(ALIGN_CENTER);
	_txtCraft->setText(_craft->getName(_game->getResourcePack()->getLanguage()));

	_txtTarget->setColor(Palette::blockOffset(8)+10);
	_txtTarget->setBig();
	_txtTarget->setAlign(ALIGN_CENTER);
	_txtTarget->setText(_craft->getDestination()->getName(_game->getResourcePack()->getLanguage()));

	_txtReady->setColor(Palette::blockOffset(8)+5);
	_txtReady->setBig();
	_txtReady->setAlign(ALIGN_CENTER);
	_txtReady->setText(_game->getResourcePack()->getLanguage()->getString("STR_READY_TO_LAND_NEAR"));

	_txtBegin->setColor(Palette::blockOffset(8)+5);
	_txtBegin->setBig();
	_txtBegin->setAlign(ALIGN_CENTER);
	_txtBegin->setText(_game->getResourcePack()->getLanguage()->getString("STR_BEGIN_MISSION"));
}

/**
 *
 */
ConfirmLandingState::~ConfirmLandingState()
{
	
}

/**
 * Enters the mission.
 * @param action Pointer to an action.
 */
void ConfirmLandingState::btnYesClick(Action *action)
{
	_game->popState();
	Ufo* u = dynamic_cast<Ufo*>(_craft->getDestination());
	if (u != 0)
		{
		_game->getRuleset()->newBattleSave(_game->getResourcePack(), _game->getSavedGame(), _texture, _craft, u);
		_game->pushState(new BriefingCrashState(_game, _craft));
	}
}

/**
 * Returns the craft to base and closes the window.
 * @param action Pointer to an action.
 */
void ConfirmLandingState::btnNoClick(Action *action)
{
	_craft->returnToBase();
	_game->popState();
}
