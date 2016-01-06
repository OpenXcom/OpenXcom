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
#include "NewGameState.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/ToggleTextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Geoscape/GeoscapeState.h"
#include "../Geoscape/BuildNewBaseState.h"
#include "../Engine/Options.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Difficulty window.
 * @param game Pointer to the core game.
 */
NewGameState::NewGameState()
{
	// Create objects
	_window = new Window(this, 192, 180, 64, 10, POPUP_VERTICAL);
	_btnBeginner = new TextButton(160, 18, 80, 32);
	_btnExperienced = new TextButton(160, 18, 80, 52);
	_btnVeteran = new TextButton(160, 18, 80, 72);
	_btnGenius = new TextButton(160, 18, 80, 92);
	_btnSuperhuman = new TextButton(160, 18, 80, 112);
	_btnIronman = new ToggleTextButton(78, 18, 80, 138);
	_btnOk = new TextButton(78, 16, 80, 164);
	_btnCancel = new TextButton(78, 16, 162, 164);
	_txtTitle = new Text(192, 9, 64, 20);
	_txtIronman = new Text(90, 24, 162, 135);

	_difficulty = _btnBeginner;

	// Set palette
	setInterface("newGameMenu");

	add(_window, "window", "newGameMenu");
	add(_btnBeginner, "button", "newGameMenu");
	add(_btnExperienced, "button", "newGameMenu");
	add(_btnVeteran, "button", "newGameMenu");
	add(_btnGenius, "button", "newGameMenu");
	add(_btnSuperhuman, "button", "newGameMenu");
	add(_btnIronman, "ironman", "newGameMenu");
	add(_btnOk, "button", "newGameMenu");
	add(_btnCancel, "button", "newGameMenu");
	add(_txtTitle, "text", "newGameMenu");
	add(_txtIronman, "ironman", "newGameMenu");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_btnBeginner->setText(tr("STR_1_BEGINNER"));
	_btnBeginner->setGroup(&_difficulty);

	_btnExperienced->setText(tr("STR_2_EXPERIENCED"));
	_btnExperienced->setGroup(&_difficulty);

	_btnVeteran->setText(tr("STR_3_VETERAN"));
	_btnVeteran->setGroup(&_difficulty);

	_btnGenius->setText(tr("STR_4_GENIUS"));
	_btnGenius->setGroup(&_difficulty);

	_btnSuperhuman->setText(tr("STR_5_SUPERHUMAN"));
	_btnSuperhuman->setGroup(&_difficulty);

	_btnIronman->setText(tr("STR_IRONMAN"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewGameState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NewGameState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&NewGameState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&NewGameState::btnCancelClick, Options::keyCancel);

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_SELECT_DIFFICULTY_LEVEL"));

	_txtIronman->setWordWrap(true);
	_txtIronman->setVerticalAlign(ALIGN_MIDDLE);
	_txtIronman->setText(tr("STR_IRONMAN_DESC"));
}

/**
 *
 */
NewGameState::~NewGameState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void NewGameState::btnOkClick(Action *)
{
	GameDifficulty diff = DIFF_BEGINNER;
	if (_difficulty == _btnBeginner)
	{
		diff = DIFF_BEGINNER;
	}
	else if (_difficulty == _btnExperienced)
	{
		diff = DIFF_EXPERIENCED;
	}
	else if (_difficulty == _btnVeteran)
	{
		diff = DIFF_VETERAN;
	}
	else if (_difficulty == _btnGenius)
	{
		diff = DIFF_GENIUS;
	}
	else if (_difficulty == _btnSuperhuman)
	{
		diff = DIFF_SUPERHUMAN;
	}
	SavedGame *save = _game->getMod()->newSave();
	save->setDifficulty(diff);
	save->setIronman(_btnIronman->getPressed());
	_game->setSavedGame(save);

	GeoscapeState *gs = new GeoscapeState;
	_game->setState(gs);
	gs->init();
	_game->pushState(new BuildNewBaseState(_game->getSavedGame()->getBases()->back(), gs->getGlobe(), true));
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void NewGameState::btnCancelClick(Action *)
{
	_game->setSavedGame(0);
	_game->popState();
}

}
