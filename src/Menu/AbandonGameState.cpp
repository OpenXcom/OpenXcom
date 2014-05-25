/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "AbandonGameState.h"
#include "../Engine/Game.h"
#include "../Engine/Options.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "MainMenuState.h"
#include "../Engine/Screen.h"
#include "../Savegame/SavedGame.h"
#include "SaveGameState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Abandon Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
AbandonGameState::AbandonGameState(Game *game, OptionsOrigin origin) : State(game), _origin(origin)
{
	_screen = false;

	int x;
	if (_origin == OPT_GEOSCAPE)
	{
		x = 20;
	}
	else
	{
		x = 52;
	}

	// Create objects
	_window = new Window(this, 216, 160, x, 20, POPUP_BOTH);
	_btnYes = new TextButton(50, 20, x+18, 140);
	_btnNo = new TextButton(50, 20, x+148, 140);
	_txtTitle = new Text(206, 17, x+5, 70);

	// Set palette
	if (_origin == OPT_BATTLESCAPE)
	{
		setPalette("PAL_BATTLESCAPE");
	}
	else
	{
		setPalette("PAL_GEOSCAPE", 0);
	}

	add(_window);
	add(_btnYes);
	add(_btnNo);
	add(_txtTitle);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnYes->setColor(Palette::blockOffset(15)-1);
	_btnYes->setText(tr("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&AbandonGameState::btnYesClick);
	_btnYes->onKeyboardPress((ActionHandler)&AbandonGameState::btnYesClick, Options::keyOk);

	_btnNo->setColor(Palette::blockOffset(15)-1);
	_btnNo->setText(tr("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&AbandonGameState::btnNoClick);
	_btnNo->onKeyboardPress((ActionHandler)&AbandonGameState::btnNoClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_ABANDON_GAME_QUESTION"));

	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}
}

/**
 *
 */
AbandonGameState::~AbandonGameState()
{

}

/**
 * Goes back to the Main Menu.
 * @param action Pointer to an action.
 */
void AbandonGameState::btnYesClick(Action *)
{
	if (!_game->getSavedGame()->isIronman())
	{
		Screen::updateScale(Options::geoscapeScale, Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, true);
		_game->getScreen()->resetDisplay(false);

		_game->setState(new MainMenuState(_game));
		_game->setSavedGame(0);
	}
	else
	{
		_game->pushState(new SaveGameState(_game, OPT_GEOSCAPE, SAVE_IRONMAN_END));
	}
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void AbandonGameState::btnNoClick(Action *)
{
	_game->popState();
}

}
