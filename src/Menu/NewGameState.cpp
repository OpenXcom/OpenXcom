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
#include "NewGameState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/Ruleset.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Geoscape/GeoscapeState.h"
#include "../Geoscape/BuildNewBaseState.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Difficulty window.
 * @param game Pointer to the core game.
 */
NewGameState::NewGameState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 192, 180, 64, 10, POPUP_VERTICAL);
	_btnBeginner = new TextButton(160, 18, 80, 42);
	_btnExperienced = new TextButton(160, 18, 80, 64);
	_btnVeteran = new TextButton(160, 18, 80, 86);
	_btnGenius = new TextButton(160, 18, 80, 108);
	_btnSuperhuman = new TextButton(160, 18, 80, 130);
	_btnCancel = new TextButton(160, 18, 80, 158);
	_txtTitle = new Text(192, 10, 64, 25);

	add(_window);
	add(_btnBeginner);
	add(_btnExperienced);
	add(_btnVeteran);
	add(_btnGenius);
	add(_btnSuperhuman);
	add(_btnCancel);
	add(_txtTitle);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnBeginner->setColor(Palette::blockOffset(8)+5);
	_btnBeginner->setText(_game->getLanguage()->getString("STR_1_BEGINNER"));
	_btnBeginner->onMouseClick((ActionHandler)&NewGameState::btnBeginnerClick);

	_btnExperienced->setColor(Palette::blockOffset(8)+5);
	_btnExperienced->setText(_game->getLanguage()->getString("STR_2_EXPERIENCED"));
	_btnExperienced->onMouseClick((ActionHandler)&NewGameState::btnExperiencedClick);

	_btnVeteran->setColor(Palette::blockOffset(8)+5);
	_btnVeteran->setText(_game->getLanguage()->getString("STR_3_VETERAN"));
	_btnVeteran->onMouseClick((ActionHandler)&NewGameState::btnVeteranClick);

	_btnGenius->setColor(Palette::blockOffset(8)+5);
	_btnGenius->setText(_game->getLanguage()->getString("STR_4_GENIUS"));
	_btnGenius->onMouseClick((ActionHandler)&NewGameState::btnGeniusClick);

	_btnSuperhuman->setColor(Palette::blockOffset(8)+5);
	_btnSuperhuman->setText(_game->getLanguage()->getString("STR_5_SUPERHUMAN"));
	_btnSuperhuman->onMouseClick((ActionHandler)&NewGameState::btnSuperhumanClick);

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&NewGameState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&NewGameState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setSmall();
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_DIFFICULTY_LEVEL"));
}

/**
 *
 */
NewGameState::~NewGameState()
{

}

/**
 * Sets up a new saved game and jumps to the Geoscape.
 * @param diff Difficulty for the saved game.
 */
void NewGameState::newGame(GameDifficulty diff)
{
	_game->setSavedGame(_game->getRuleset()->newSave());
	_game->getSavedGame()->setDifficulty(diff);
	GeoscapeState *gs = new GeoscapeState(_game);
	_game->setState(gs);
	gs->init();
	_game->pushState(new BuildNewBaseState(_game, _game->getSavedGame()->getBases()->back(), gs->getGlobe(), true));
}

/**
 * Creates a new game in Beginner difficulty and
 * jumps to the Geoscape screen.
 * @param action Pointer to an action.
 */
void NewGameState::btnBeginnerClick(Action *)
{
	newGame(DIFF_BEGINNER);
}

/**
 * Creates a new game in Experienced difficulty and
 * jumps to the Geoscape screen.
 * @param action Pointer to an action.
 */
void NewGameState::btnExperiencedClick(Action *)
{
	newGame(DIFF_EXPERIENCED);
}

/**
 * Creates a new game in Veteran difficulty and
 * jumps to the Geoscape screen.
 * @param action Pointer to an action.
 */
void NewGameState::btnVeteranClick(Action *)
{
	newGame(DIFF_VETERAN);
}

/**
 * Creates a new game in Genius difficulty and
 * jumps to the Geoscape screen.
 * @param action Pointer to an action.
 */
void NewGameState::btnGeniusClick(Action *)
{
	newGame(DIFF_GENIUS);
}

/**
 * Creates a new game in Superhuman difficulty and
 * jumps to the Geoscape screen.
 * @param action Pointer to an action.
 */
void NewGameState::btnSuperhumanClick(Action *)
{
	newGame(DIFF_SUPERHUMAN);
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
