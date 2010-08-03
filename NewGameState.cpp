/*
 * Copyright 2010 Daniel Albano
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
#include "Game.h"
#include "ResourcePack.h"
#include "Language.h"
#include "LangString.h"
#include "Font.h"
#include "Palette.h"
#include "TextButton.h"
#include "Window.h"
#include "Text.h"
#include "Ruleset.h"
#include "GeoscapeState.h"
#include "BuildNewBaseState.h"

/**
 * Initializes all the elements in the Difficulty window.
 * @param game Pointer to the core game.
 */
NewGameState::NewGameState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 192, 180, 64, 10, POPUP_VERTICAL);
	_btnBeginner = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 55);
	_btnExperienced = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 80);
	_btnVeteran = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 105);
	_btnGenius = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 130);
	_btnSuperhuman = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 160, 18, 80, 155);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 192, 10, 64, 30);
	
	add(_window);
	add(_btnBeginner);
	add(_btnExperienced);
	add(_btnVeteran);
	add(_btnGenius);
	add(_btnSuperhuman);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+8);
	_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnBeginner->setColor(Palette::blockOffset(8)+8);
	_btnBeginner->setText(_game->getResourcePack()->getLanguage()->getString(STR_BEGINNER));
	_btnBeginner->onMouseClick((EventHandler)&NewGameState::btnBeginnerClick);

	_btnExperienced->setColor(Palette::blockOffset(8)+8);
	_btnExperienced->setText(_game->getResourcePack()->getLanguage()->getString(STR_EXPERIENCED));
	_btnExperienced->onMouseClick((EventHandler)&NewGameState::btnExperiencedClick);

	_btnVeteran->setColor(Palette::blockOffset(8)+8);
	_btnVeteran->setText(_game->getResourcePack()->getLanguage()->getString(STR_VETERAN));
	_btnVeteran->onMouseClick((EventHandler)&NewGameState::btnVeteranClick);

	_btnGenius->setColor(Palette::blockOffset(8)+8);
	_btnGenius->setText(_game->getResourcePack()->getLanguage()->getString(STR_GENIUS));
	_btnGenius->onMouseClick((EventHandler)&NewGameState::btnGeniusClick);

	_btnSuperhuman->setColor(Palette::blockOffset(8)+8);
	_btnSuperhuman->setText(_game->getResourcePack()->getLanguage()->getString(STR_SUPERHUMAN));
	_btnSuperhuman->onMouseClick((EventHandler)&NewGameState::btnSuperhumanClick);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setSmall();
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_SELECT_DIFFICULTY_LEVEL));
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
	_game->setSavedGame(_game->getRuleset()->newSave(diff));
	GeoscapeState *gs = new GeoscapeState(_game);
	_game->setState(gs);
	_game->pushState(new BuildNewBaseState(_game, _game->getSavedGame()->getBases()->back(), gs->getGlobe(), true));
}

/**
 * Creates a new game in Beginner difficulty and
 * jumps to the Geoscape screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewGameState::btnBeginnerClick(SDL_Event *ev, int scale)
{
	newGame(DIFF_BEGINNER);
}

/**
 * Creates a new game in Experienced difficulty and
 * jumps to the Geoscape screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewGameState::btnExperiencedClick(SDL_Event *ev, int scale)
{
	newGame(DIFF_EXPERIENCED);
}

/**
 * Creates a new game in Veteran difficulty and
 * jumps to the Geoscape screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewGameState::btnVeteranClick(SDL_Event *ev, int scale)
{
	newGame(DIFF_VETERAN);
}

/**
 * Creates a new game in Genius difficulty and
 * jumps to the Geoscape screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewGameState::btnGeniusClick(SDL_Event *ev, int scale)
{
	newGame(DIFF_GENIUS);
}

/**
 * Creates a new game in Superhuman difficulty and
 * jumps to the Geoscape screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void NewGameState::btnSuperhumanClick(SDL_Event *ev, int scale)
{
	newGame(DIFF_SUPERHUMAN);
}
