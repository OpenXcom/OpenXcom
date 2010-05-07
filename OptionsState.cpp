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
#include "OptionsState.h"

OptionsState::OptionsState(Game *game) : State(game)
{
	_screen = false;

	// Create objects
	_window = new Window(216, 160, 20, 20);
	_btnLoad = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 180, 20, 38, 60);
	_btnSave = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 180, 20, 38, 85);
	_btnAbandon = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 180, 20, 38, 110);
	_btnCancel = new Button(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 180, 20, 38, 135);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 206, 15, 25, 32);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnLoad);
	add(_btnSave);
	add(_btnAbandon);
	add(_btnCancel);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBg(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnLoad->setColor(Palette::blockOffset(15)+2);
	_btnLoad->setText(_game->getResourcePack()->getLanguage()->getString(STR_LOAD_GAME));
	_btnLoad->onMouseClick((EventHandler)&OptionsState::btnLoadClick);

	_btnSave->setColor(Palette::blockOffset(15)+2);
	_btnSave->setText(_game->getResourcePack()->getLanguage()->getString(STR_SAVE_GAME));
	_btnSave->onMouseClick((EventHandler)&OptionsState::btnSaveClick);

	_btnAbandon->setColor(Palette::blockOffset(15)+2);
	_btnAbandon->setText(_game->getResourcePack()->getLanguage()->getString(STR_ABANDON_GAME));
	_btnAbandon->onMouseClick((EventHandler)&OptionsState::btnAbandonClick);

	_btnCancel->setColor(Palette::blockOffset(15)+2);
	_btnCancel->setText(_game->getResourcePack()->getLanguage()->getString(STR_CANCEL_UC));
	_btnCancel->onMouseClick((EventHandler)&OptionsState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_GAME_OPTIONS));
}

OptionsState::~OptionsState()
{
	
}

void OptionsState::think()
{
}

void OptionsState::btnLoadClick(SDL_Event *ev, int scale)
{
	
}

void OptionsState::btnSaveClick(SDL_Event *ev, int scale)
{
	
}

void OptionsState::btnAbandonClick(SDL_Event *ev, int scale)
{
	_game->pushState(new AbandonGameState(_game));
}

void OptionsState::btnCancelClick(SDL_Event *ev, int scale)
{
	_game->popState();
}