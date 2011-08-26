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
#include "BattlescapeOptionsState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Battlescape Options screen.
 * @param game Pointer to the core game.
 */
BattlescapeOptionsState::BattlescapeOptionsState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(320, 16, 0, 16);

	_txtScrollSpeed = new Text(120, 9, 16, 32);
	_btnScrollSpeed1 = new TextButton(22, 14, 16, 42);
	_btnScrollSpeed2 = new TextButton(22, 18, 40, 42);
	_btnScrollSpeed3 = new TextButton(22, 22, 64, 42);
	_btnScrollSpeed4 = new TextButton(22, 26, 88, 42);
	_btnScrollSpeed5 = new TextButton(22, 30, 112, 42);

	_txtScrollType = new Text(120, 9, 150, 32);
	_btnScrollType1 = new TextButton(30, 14, 150, 42);
	_btnScrollType2 = new TextButton(30, 14, 150, 58);

	_btnOk = new TextButton(120, 16, 16, 174);
	_btnSave = new TextButton(120, 16, 184, 174);

	switch (Options::getInt("battleScrollSpeed"))
	{
	case 40: _scrollSpeed = _btnScrollSpeed1; break;
	case 32: _scrollSpeed = _btnScrollSpeed2; break;
	case 24: _scrollSpeed = _btnScrollSpeed3; break;
	case 16: _scrollSpeed = _btnScrollSpeed4; break;
	case 8: _scrollSpeed = _btnScrollSpeed5; break;
	default: _scrollSpeed = 0; break;
	}
	switch (Options::getInt("battleScrollType"))
	{
	case SCROLL_TRIGGER: _scrollType = _btnScrollType1; break;
	case SCROLL_AUTO: _scrollType = _btnScrollType2; break;
	default: _scrollSpeed = 0; break;
	}
	switch (Options::getInt("battleFireSpeed"))
	{
	case 30: _fireSpeed = _btnFireSpeed1; break;
	case 25: _fireSpeed = _btnFireSpeed2; break;
	case 20: _fireSpeed = _btnFireSpeed3; break;
	case 15: _fireSpeed = _btnFireSpeed4; break;
	case 10: _fireSpeed = _btnFireSpeed5; break;
	case 5: _fireSpeed = _btnFireSpeed6; break;
	default: _fireSpeed = 0; break;
	}
	switch (Options::getInt("battleXcomSpeed"))
	{
	case 60: _xcomSpeed = _btnXcomSpeed1; break;
	case 50: _xcomSpeed = _btnXcomSpeed2; break;
	case 40: _xcomSpeed = _btnXcomSpeed3; break;
	case 30: _xcomSpeed = _btnXcomSpeed4; break;
	case 20: _xcomSpeed = _btnXcomSpeed5; break;
	case 10: _xcomSpeed = _btnXcomSpeed6; break;
	default: _xcomSpeed = 0; break;
	}
	switch (Options::getInt("battleAlienSpeed"))
	{
	case 60: _alienSpeed = _btnAlienSpeed1; break;
	case 50: _alienSpeed = _btnAlienSpeed2; break;
	case 40: _alienSpeed = _btnAlienSpeed3; break;
	case 30: _alienSpeed = _btnAlienSpeed4; break;
	case 20: _alienSpeed = _btnAlienSpeed5; break;
	case 10: _alienSpeed = _btnAlienSpeed6; break;
	default: _alienSpeed = 0; break;
	}

	add(_window);
	add(_txtTitle);

	add(_txtScrollSpeed);
	add(_btnScrollSpeed1);
	add(_btnScrollSpeed2);
	add(_btnScrollSpeed3);
	add(_btnScrollSpeed4);
	add(_btnScrollSpeed5);

	add(_txtScrollType);
	add(_txtTriggerScroll);
	add(_txtAutoScroll);
	add(_btnScrollType1);
	add(_btnScrollType2);

	add(_txtFireSpeed);
	add(_btnFireSpeed1);
	add(_btnFireSpeed2);
	add(_btnFireSpeed3);
	add(_btnFireSpeed4);
	add(_btnFireSpeed5);
	add(_btnFireSpeed6);

	add(_txtXcomSpeed);
	add(_btnXcomSpeed1);
	add(_btnXcomSpeed2);
	add(_btnXcomSpeed3);
	add(_btnXcomSpeed4);
	add(_btnXcomSpeed5);
	add(_btnXcomSpeed6);

	add(_txtAlienSpeed);
	add(_btnAlienSpeed1);
	add(_btnAlienSpeed2);
	add(_btnAlienSpeed3);
	add(_btnAlienSpeed4);
	add(_btnAlienSpeed5);
	add(_btnAlienSpeed6);

	add(_btnOk);
	add(_btnSave);

	// Set up objects
	_window->setColor(Palette::blockOffset(0)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));

	_txtTitle->setColor(Palette::blockOffset(0));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setHighContrast(true);
	_txtTitle->setText(_game->getLanguage()->getString("STR_GAME_OPTIONS"));

	_btnOk->setColor(Palette::blockOffset(0)+3);
	_btnOk->setHighContrast(true);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BattlescapeOptionsState::btnOkClick);

	_btnSave->setColor(Palette::blockOffset(0)+3);
	_btnSave->setHighContrast(true);
	_btnSave->setText(_game->getLanguage()->getString("STR_NOT_AVAILABLE"));
	_btnSave->onMouseClick((ActionHandler)&BattlescapeOptionsState::btnSaveClick);
}

/**
 *
 */
BattlescapeOptionsState::~BattlescapeOptionsState()
{
	
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BattlescapeOptionsState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Opens the Save Game screen.
 * @param action Pointer to an action.
 */
void BattlescapeOptionsState::btnSaveClick(Action *action)
{
	;
}

}
