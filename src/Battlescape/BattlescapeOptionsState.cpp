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
#include "../Menu/LoadState.h"
#include "../Menu/SaveState.h"

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

	_txtScrollType = new Text(160, 9, 150, 32);
	_txtTriggerScroll = new Text(130, 9, 182, 45);
	_txtAutoScroll = new Text(130, 9, 182, 61);
	_btnScrollType1 = new TextButton(30, 14, 150, 42);
	_btnScrollType2 = new TextButton(30, 14, 150, 58);

	_txtFireSpeed = new Text(200, 9, 16, 74);
	_btnFireSpeed1 = new TextButton(22, 12, 16, 84);
	_btnFireSpeed2 = new TextButton(28, 12, 40, 84);
	_btnFireSpeed3 = new TextButton(36, 12, 70, 84);
	_btnFireSpeed4 = new TextButton(45, 12, 108, 84);
	_btnFireSpeed5 = new TextButton(56, 12, 155, 84);
	_btnFireSpeed6 = new TextButton(69, 12, 213, 84);

	_txtXcomSpeed = new Text(200, 9, 16, 106);
	_btnXcomSpeed1 = new TextButton(22, 12, 16, 116);
	_btnXcomSpeed2 = new TextButton(28, 12, 40, 116);
	_btnXcomSpeed3 = new TextButton(36, 12, 70, 116);
	_btnXcomSpeed4 = new TextButton(45, 12, 108, 116);
	_btnXcomSpeed5 = new TextButton(56, 12, 155, 116);
	_btnXcomSpeed6 = new TextButton(69, 12, 213, 116);

	_txtAlienSpeed = new Text(200, 9, 16, 138);
	_btnAlienSpeed1 = new TextButton(22, 12, 16, 148);
	_btnAlienSpeed2 = new TextButton(28, 12, 40, 148);
	_btnAlienSpeed3 = new TextButton(36, 12, 70, 148);
	_btnAlienSpeed4 = new TextButton(45, 12, 108, 148);
	_btnAlienSpeed5 = new TextButton(56, 12, 155, 148);
	_btnAlienSpeed6 = new TextButton(69, 12, 213, 148);

	_btnOk = new TextButton(90, 16, 16, 174);
	_btnLoad = new TextButton(90, 16, 117, 174);
	_btnSave = new TextButton(90, 16, 214, 174);

	switch (Options::getInt("battleScrollSpeed"))
	{
	case 8: _scrollSpeed = _btnScrollSpeed1; break;
	case 16: _scrollSpeed = _btnScrollSpeed2; break;
	case 24: _scrollSpeed = _btnScrollSpeed3; break;
	case 32: _scrollSpeed = _btnScrollSpeed4; break;
	case 40: _scrollSpeed = _btnScrollSpeed5; break;
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
	add(_btnLoad);
	add(_btnSave);

	// Set up objects
	_window->setColor(Palette::blockOffset(0));
	_window->setHighContrast(true);
	_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));

	_txtTitle->setColor(Palette::blockOffset(0));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setHighContrast(true);
	_txtTitle->setText(_game->getLanguage()->getString("STR_GAME_OPTIONS"));

	_txtScrollSpeed->setColor(Palette::blockOffset(0));
	_txtScrollSpeed->setHighContrast(true);
	_txtScrollSpeed->setText(_game->getLanguage()->getString("STR_SCROLL_SPEED"));

	_btnScrollSpeed1->setColor(Palette::blockOffset(0));
	_btnScrollSpeed1->setHighContrast(true);
	_btnScrollSpeed1->setText(L"1");
	_btnScrollSpeed1->setGroup(&_scrollSpeed);

	_btnScrollSpeed2->setColor(Palette::blockOffset(0));
	_btnScrollSpeed2->setHighContrast(true);
	_btnScrollSpeed2->setText(L"2");
	_btnScrollSpeed2->setGroup(&_scrollSpeed);

	_btnScrollSpeed3->setColor(Palette::blockOffset(0));
	_btnScrollSpeed3->setHighContrast(true);
	_btnScrollSpeed3->setText(L"3");
	_btnScrollSpeed3->setGroup(&_scrollSpeed);

	_btnScrollSpeed4->setColor(Palette::blockOffset(0));
	_btnScrollSpeed4->setHighContrast(true);
	_btnScrollSpeed4->setText(L"4");
	_btnScrollSpeed4->setGroup(&_scrollSpeed);

	_btnScrollSpeed5->setColor(Palette::blockOffset(0));
	_btnScrollSpeed5->setHighContrast(true);
	_btnScrollSpeed5->setText(L"5");
	_btnScrollSpeed5->setGroup(&_scrollSpeed);

	_txtScrollType->setColor(Palette::blockOffset(0));
	_txtScrollType->setHighContrast(true);
	_txtScrollType->setText(_game->getLanguage()->getString("STR_SCROLL_TYPE"));

	_txtTriggerScroll->setColor(Palette::blockOffset(0));
	_txtTriggerScroll->setHighContrast(true);
	_txtTriggerScroll->setText(_game->getLanguage()->getString("STR_TRIGGER_SCROLL"));

	_txtAutoScroll->setColor(Palette::blockOffset(0));
	_txtAutoScroll->setHighContrast(true);
	_txtAutoScroll->setText(_game->getLanguage()->getString("STR_AUTO_SCROLL"));

	_btnScrollType1->setColor(Palette::blockOffset(0));
	_btnScrollType1->setHighContrast(true);
	_btnScrollType1->setText(L"1");
	_btnScrollType1->setGroup(&_scrollType);

	_btnScrollType2->setColor(Palette::blockOffset(0));
	_btnScrollType2->setHighContrast(true);
	_btnScrollType2->setText(L"2");
	_btnScrollType2->setGroup(&_scrollType);

	_txtFireSpeed->setColor(Palette::blockOffset(0));
	_txtFireSpeed->setHighContrast(true);
	_txtFireSpeed->setText(_game->getLanguage()->getString("STR_FIRE_SPEED"));

	_btnFireSpeed1->setColor(Palette::blockOffset(0));
	_btnFireSpeed1->setHighContrast(true);
	_btnFireSpeed1->setText(L"1");
	_btnFireSpeed1->setGroup(&_fireSpeed);

	_btnFireSpeed2->setColor(Palette::blockOffset(0));
	_btnFireSpeed2->setHighContrast(true);
	_btnFireSpeed2->setText(L"2");
	_btnFireSpeed2->setGroup(&_fireSpeed);

	_btnFireSpeed3->setColor(Palette::blockOffset(0));
	_btnFireSpeed3->setHighContrast(true);
	_btnFireSpeed3->setText(L"3");
	_btnFireSpeed3->setGroup(&_fireSpeed);

	_btnFireSpeed4->setColor(Palette::blockOffset(0));
	_btnFireSpeed4->setHighContrast(true);
	_btnFireSpeed4->setText(L"4");
	_btnFireSpeed4->setGroup(&_fireSpeed);

	_btnFireSpeed5->setColor(Palette::blockOffset(0));
	_btnFireSpeed5->setHighContrast(true);
	_btnFireSpeed5->setText(L"5");
	_btnFireSpeed5->setGroup(&_fireSpeed);

	_btnFireSpeed6->setColor(Palette::blockOffset(0));
	_btnFireSpeed6->setHighContrast(true);
	_btnFireSpeed6->setText(L"6");
	_btnFireSpeed6->setGroup(&_fireSpeed);

	_txtXcomSpeed->setColor(Palette::blockOffset(0));
	_txtXcomSpeed->setHighContrast(true);
	_txtXcomSpeed->setText(_game->getLanguage()->getString("STR_XCOM_MOVEMENT_SPEED"));

	_btnXcomSpeed1->setColor(Palette::blockOffset(0));
	_btnXcomSpeed1->setHighContrast(true);
	_btnXcomSpeed1->setText(L"1");
	_btnXcomSpeed1->setGroup(&_xcomSpeed);

	_btnXcomSpeed2->setColor(Palette::blockOffset(0));
	_btnXcomSpeed2->setHighContrast(true);
	_btnXcomSpeed2->setText(L"2");
	_btnXcomSpeed2->setGroup(&_xcomSpeed);

	_btnXcomSpeed3->setColor(Palette::blockOffset(0));
	_btnXcomSpeed3->setHighContrast(true);
	_btnXcomSpeed3->setText(L"3");
	_btnXcomSpeed3->setGroup(&_xcomSpeed);

	_btnXcomSpeed4->setColor(Palette::blockOffset(0));
	_btnXcomSpeed4->setHighContrast(true);
	_btnXcomSpeed4->setText(L"4");
	_btnXcomSpeed4->setGroup(&_xcomSpeed);

	_btnXcomSpeed5->setColor(Palette::blockOffset(0));
	_btnXcomSpeed5->setHighContrast(true);
	_btnXcomSpeed5->setText(L"5");
	_btnXcomSpeed5->setGroup(&_xcomSpeed);

	_btnXcomSpeed6->setColor(Palette::blockOffset(0));
	_btnXcomSpeed6->setHighContrast(true);
	_btnXcomSpeed6->setText(L"6");
	_btnXcomSpeed6->setGroup(&_xcomSpeed);

	_txtAlienSpeed->setColor(Palette::blockOffset(0));
	_txtAlienSpeed->setHighContrast(true);
	_txtAlienSpeed->setText(_game->getLanguage()->getString("STR_ALIEN_MOVEMENT_SPEED"));

	_btnAlienSpeed1->setColor(Palette::blockOffset(0));
	_btnAlienSpeed1->setHighContrast(true);
	_btnAlienSpeed1->setText(L"1");
	_btnAlienSpeed1->setGroup(&_alienSpeed);

	_btnAlienSpeed2->setColor(Palette::blockOffset(0));
	_btnAlienSpeed2->setHighContrast(true);
	_btnAlienSpeed2->setText(L"2");
	_btnAlienSpeed2->setGroup(&_alienSpeed);

	_btnAlienSpeed3->setColor(Palette::blockOffset(0));
	_btnAlienSpeed3->setHighContrast(true);
	_btnAlienSpeed3->setText(L"3");
	_btnAlienSpeed3->setGroup(&_alienSpeed);

	_btnAlienSpeed4->setColor(Palette::blockOffset(0));
	_btnAlienSpeed4->setHighContrast(true);
	_btnAlienSpeed4->setText(L"4");
	_btnAlienSpeed4->setGroup(&_alienSpeed);

	_btnAlienSpeed5->setColor(Palette::blockOffset(0));
	_btnAlienSpeed5->setHighContrast(true);
	_btnAlienSpeed5->setText(L"5");
	_btnAlienSpeed5->setGroup(&_alienSpeed);

	_btnAlienSpeed6->setColor(Palette::blockOffset(0));
	_btnAlienSpeed6->setHighContrast(true);
	_btnAlienSpeed6->setText(L"6");
	_btnAlienSpeed6->setGroup(&_alienSpeed);

	_btnOk->setColor(Palette::blockOffset(0));
	_btnOk->setHighContrast(true);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BattlescapeOptionsState::btnOkClick);

	_btnLoad->setColor(Palette::blockOffset(0));
	_btnLoad->setHighContrast(true);
	_btnLoad->setText(_game->getLanguage()->getString("STR_LOAD_GAME"));
	_btnLoad->onMouseClick((ActionHandler)&BattlescapeOptionsState::btnLoadClick);

	_btnSave->setColor(Palette::blockOffset(0));
	_btnSave->setHighContrast(true);
	_btnSave->setText(_game->getLanguage()->getString("STR_SAVE_GAME"));
	_btnSave->onMouseClick((ActionHandler)&BattlescapeOptionsState::btnSaveClick);
}

/**
 *
 */
BattlescapeOptionsState::~BattlescapeOptionsState()
{

}

/**
 * Saves options and returns to the previous screen.
 * @param action Pointer to an action.
 */
void BattlescapeOptionsState::btnOkClick(Action *action)
{
	if (_scrollSpeed == _btnScrollSpeed1)
		Options::setInt("battleScrollSpeed", 8);
	else if (_scrollSpeed == _btnScrollSpeed2)
		Options::setInt("battleScrollSpeed", 16);
	else if (_scrollSpeed == _btnScrollSpeed3)
		Options::setInt("battleScrollSpeed", 24);
	else if (_scrollSpeed == _btnScrollSpeed4)
		Options::setInt("battleScrollSpeed", 32);
	else if (_scrollSpeed == _btnScrollSpeed5)
		Options::setInt("battleScrollSpeed", 40);

	if (_scrollType == _btnScrollType1)
		Options::setInt("battleScrollType", SCROLL_TRIGGER);
	else if (_scrollType == _btnScrollType2)
		Options::setInt("battleScrollType", SCROLL_AUTO);

	if (_fireSpeed == _btnFireSpeed1)
		Options::setInt("battleFireSpeed", 30);
	else if (_fireSpeed == _btnFireSpeed2)
		Options::setInt("battleFireSpeed", 25);
	else if (_fireSpeed == _btnFireSpeed3)
		Options::setInt("battleFireSpeed", 20);
	else if (_fireSpeed == _btnFireSpeed4)
		Options::setInt("battleFireSpeed", 15);
	else if (_fireSpeed == _btnFireSpeed5)
		Options::setInt("battleFireSpeed", 10);
	else if (_fireSpeed == _btnFireSpeed6)
		Options::setInt("battleFireSpeed", 5);

	if (_xcomSpeed == _btnXcomSpeed1)
		Options::setInt("battleXcomSpeed", 60);
	else if (_xcomSpeed == _btnXcomSpeed2)
		Options::setInt("battleXcomSpeed", 50);
	else if (_xcomSpeed == _btnXcomSpeed3)
		Options::setInt("battleXcomSpeed", 40);
	else if (_xcomSpeed == _btnXcomSpeed4)
		Options::setInt("battleXcomSpeed", 30);
	else if (_xcomSpeed == _btnXcomSpeed5)
		Options::setInt("battleXcomSpeed", 20);
	else if (_xcomSpeed == _btnXcomSpeed6)
		Options::setInt("battleXcomSpeed", 10);

	if (_alienSpeed == _btnAlienSpeed1)
		Options::setInt("battleAlienSpeed", 60);
	else if (_alienSpeed == _btnAlienSpeed2)
		Options::setInt("battleAlienSpeed", 50);
	else if (_alienSpeed == _btnAlienSpeed3)
		Options::setInt("battleAlienSpeed", 40);
	else if (_alienSpeed == _btnAlienSpeed4)
		Options::setInt("battleAlienSpeed", 30);
	else if (_alienSpeed == _btnAlienSpeed5)
		Options::setInt("battleAlienSpeed", 20);
	else if (_alienSpeed == _btnAlienSpeed6)
		Options::setInt("battleAlienSpeed", 10);

	Options::save();
	_game->popState();
}

/**
 * Opens the Load Game screen.
 * @param action Pointer to an action.
 */
void BattlescapeOptionsState::btnLoadClick(Action *action)
{
	_game->pushState(new LoadState(_game, false));
}

/**
 * Opens the Save Game screen.
 * @param action Pointer to an action.
 */
void BattlescapeOptionsState::btnSaveClick(Action *action)
{
	_game->pushState(new SaveState(_game, false));
}

}
