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
#include "OptionsBattlescapeState.h"
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
 * @param origin Game section that originated this state.
 */
OptionsBattlescapeState::OptionsBattlescapeState(Game *game, OptionsOrigin origin) : OptionsBaseState(game, origin)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 17, 0, 10);

	_txtScrollSpeed = new Text(130, 9, 16, 32);
	_btnScrollSpeed1 = new TextButton(22, 14, 16, 42);
	_btnScrollSpeed2 = new TextButton(22, 18, 40, 42);
	_btnScrollSpeed3 = new TextButton(22, 22, 64, 42);
	_btnScrollSpeed4 = new TextButton(22, 26, 88, 42);
	_btnScrollSpeed5 = new TextButton(22, 30, 112, 42);

	_txtScrollType = new Text(160, 9, 150, 32);
	_txtTriggerScroll = new Text(130, 9, 182, 44);
	_txtAutoScroll = new Text(130, 9, 182, 58);
	_txtDragScroll = new Text(130, 9, 182, 72);
	_btnScrollType1 = new TextButton(30, 12, 150, 42);
	_btnScrollType2 = new TextButton(30, 12, 150, 56);
	_btnScrollType3 = new TextButton(30, 12, 150, 70);

	_txtFireSpeed = new Text(200, 9, 16, 84);
	_btnFireSpeed1 = new TextButton(22, 12, 16, 94);
	_btnFireSpeed2 = new TextButton(28, 12, 40, 94);
	_btnFireSpeed3 = new TextButton(36, 12, 70, 94);
	_btnFireSpeed4 = new TextButton(45, 12, 108, 94);
	_btnFireSpeed5 = new TextButton(56, 12, 155, 94);
	_btnFireSpeed6 = new TextButton(69, 12, 213, 94);

	_txtXcomSpeed = new Text(200, 9, 16, 114);
	_btnXcomSpeed1 = new TextButton(22, 12, 16, 124);
	_btnXcomSpeed2 = new TextButton(28, 12, 40, 124);
	_btnXcomSpeed3 = new TextButton(36, 12, 70, 124);
	_btnXcomSpeed4 = new TextButton(45, 12, 108, 124);
	_btnXcomSpeed5 = new TextButton(56, 12, 155, 124);
	_btnXcomSpeed6 = new TextButton(69, 12, 213, 124);

	_txtAlienSpeed = new Text(200, 9, 16, 144);
	_btnAlienSpeed1 = new TextButton(22, 12, 16, 154);
	_btnAlienSpeed2 = new TextButton(28, 12, 40, 154);
	_btnAlienSpeed3 = new TextButton(36, 12, 70, 154);
	_btnAlienSpeed4 = new TextButton(45, 12, 108, 154);
	_btnAlienSpeed5 = new TextButton(56, 12, 155, 154);
	_btnAlienSpeed6 = new TextButton(69, 12, 213, 154);

	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);

	switch (Options::getInt("battleScrollSpeed"))
	{
	case 4: _scrollSpeed = _btnScrollSpeed1; break;
	case 8: _scrollSpeed = _btnScrollSpeed2; break;
	case 12: _scrollSpeed = _btnScrollSpeed3; break;
	case 16: _scrollSpeed = _btnScrollSpeed4; break;
	case 20: _scrollSpeed = _btnScrollSpeed5; break;
	default: _scrollSpeed = 0; break;
	}
	switch (Options::getInt("battleScrollType"))
	{
	case SCROLL_TRIGGER: _scrollType = _btnScrollType1; break;
	case SCROLL_AUTO: _scrollType = _btnScrollType2; break;
	case SCROLL_DRAG: _scrollType = _btnScrollType3; break;
	default: _scrollSpeed = 0; break;
	}
	switch (Options::getInt("battleFireSpeed"))
	{
	case 2: _fireSpeed = _btnFireSpeed1; break;
	case 4: _fireSpeed = _btnFireSpeed2; break;
	case 6: _fireSpeed = _btnFireSpeed3; break;
	case 8: _fireSpeed = _btnFireSpeed4; break;
	case 10: _fireSpeed = _btnFireSpeed5; break;
	case 12: _fireSpeed = _btnFireSpeed6; break;
	default: _fireSpeed = 0; break;
	}
	switch (Options::getInt("battleXcomSpeed"))
	{
	case 40: _xcomSpeed = _btnXcomSpeed1; break;
	case 30: _xcomSpeed = _btnXcomSpeed2; break;
	case 20: _xcomSpeed = _btnXcomSpeed3; break;
	case 10: _xcomSpeed = _btnXcomSpeed4; break;
	case 5: _xcomSpeed = _btnXcomSpeed5; break;
	case 1: _xcomSpeed = _btnXcomSpeed6; break;
	default: _xcomSpeed = 0; break;
	}
	switch (Options::getInt("battleAlienSpeed"))
	{
	case 40: _alienSpeed = _btnAlienSpeed1; break;
	case 30: _alienSpeed = _btnAlienSpeed2; break;
	case 20: _alienSpeed = _btnAlienSpeed3; break;
	case 10: _alienSpeed = _btnAlienSpeed4; break;
	case 5: _alienSpeed = _btnAlienSpeed5; break;
	case 1: _alienSpeed = _btnAlienSpeed6; break;
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
	add(_txtDragScroll);
	add(_btnScrollType1);
	add(_btnScrollType2);
	add(_btnScrollType3);

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
	add(_btnCancel);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8) + 5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_BATTLESCAPE"));

	_txtScrollSpeed->setColor(Palette::blockOffset(8) + 10);
	_txtScrollSpeed->setText(tr("STR_SCROLL_SPEED"));

	_btnScrollSpeed1->setColor(Palette::blockOffset(15)-1);
	_btnScrollSpeed1->setText(L"1");
	_btnScrollSpeed1->setGroup(&_scrollSpeed);

	_btnScrollSpeed2->setColor(Palette::blockOffset(15)-1);
	_btnScrollSpeed2->setText(L"2");
	_btnScrollSpeed2->setGroup(&_scrollSpeed);

	_btnScrollSpeed3->setColor(Palette::blockOffset(15)-1);
	_btnScrollSpeed3->setText(L"3");
	_btnScrollSpeed3->setGroup(&_scrollSpeed);

	_btnScrollSpeed4->setColor(Palette::blockOffset(15)-1);
	_btnScrollSpeed4->setText(L"4");
	_btnScrollSpeed4->setGroup(&_scrollSpeed);

	_btnScrollSpeed5->setColor(Palette::blockOffset(15)-1);
	_btnScrollSpeed5->setText(L"5");
	_btnScrollSpeed5->setGroup(&_scrollSpeed);

	_txtScrollType->setColor(Palette::blockOffset(8) + 10);
	_txtScrollType->setText(tr("STR_SCROLL_TYPE"));

	_txtTriggerScroll->setColor(Palette::blockOffset(15) - 1);
	_txtTriggerScroll->setText(tr("STR_TRIGGER_SCROLL"));

	_txtAutoScroll->setColor(Palette::blockOffset(15) - 1);
	_txtAutoScroll->setText(tr("STR_AUTO_SCROLL"));

	_txtDragScroll->setColor(Palette::blockOffset(15) - 1);
	_txtDragScroll->setText(tr("STR_DRAG_SCROLL"));

	_btnScrollType1->setColor(Palette::blockOffset(15)-1);
	_btnScrollType1->setText(L"1");
	_btnScrollType1->setGroup(&_scrollType);

	_btnScrollType2->setColor(Palette::blockOffset(15)-1);
	_btnScrollType2->setText(L"2");
	_btnScrollType2->setGroup(&_scrollType);

	_btnScrollType3->setColor(Palette::blockOffset(15)-1);
	_btnScrollType3->setText(L"3");
	_btnScrollType3->setGroup(&_scrollType);

	_txtFireSpeed->setColor(Palette::blockOffset(8) + 10);
	_txtFireSpeed->setText(tr("STR_FIRE_SPEED"));

	_btnFireSpeed1->setColor(Palette::blockOffset(15)-1);
	_btnFireSpeed1->setText(L"1");
	_btnFireSpeed1->setGroup(&_fireSpeed);

	_btnFireSpeed2->setColor(Palette::blockOffset(15)-1);
	_btnFireSpeed2->setText(L"2");
	_btnFireSpeed2->setGroup(&_fireSpeed);

	_btnFireSpeed3->setColor(Palette::blockOffset(15)-1);
	_btnFireSpeed3->setText(L"3");
	_btnFireSpeed3->setGroup(&_fireSpeed);

	_btnFireSpeed4->setColor(Palette::blockOffset(15)-1);
	_btnFireSpeed4->setText(L"4");
	_btnFireSpeed4->setGroup(&_fireSpeed);

	_btnFireSpeed5->setColor(Palette::blockOffset(15)-1);
	_btnFireSpeed5->setText(L"5");
	_btnFireSpeed5->setGroup(&_fireSpeed);

	_btnFireSpeed6->setColor(Palette::blockOffset(15)-1);
	_btnFireSpeed6->setText(L"6");
	_btnFireSpeed6->setGroup(&_fireSpeed);

	_txtXcomSpeed->setColor(Palette::blockOffset(8) + 10);
	_txtXcomSpeed->setText(tr("STR_XCOM_MOVEMENT_SPEED"));

	_btnXcomSpeed1->setColor(Palette::blockOffset(15)-1);
	_btnXcomSpeed1->setText(L"1");
	_btnXcomSpeed1->setGroup(&_xcomSpeed);

	_btnXcomSpeed2->setColor(Palette::blockOffset(15)-1);
	_btnXcomSpeed2->setText(L"2");
	_btnXcomSpeed2->setGroup(&_xcomSpeed);

	_btnXcomSpeed3->setColor(Palette::blockOffset(15)-1);
	_btnXcomSpeed3->setText(L"3");
	_btnXcomSpeed3->setGroup(&_xcomSpeed);

	_btnXcomSpeed4->setColor(Palette::blockOffset(15)-1);
	_btnXcomSpeed4->setText(L"4");
	_btnXcomSpeed4->setGroup(&_xcomSpeed);

	_btnXcomSpeed5->setColor(Palette::blockOffset(15)-1);
	_btnXcomSpeed5->setText(L"5");
	_btnXcomSpeed5->setGroup(&_xcomSpeed);

	_btnXcomSpeed6->setColor(Palette::blockOffset(15)-1);
	_btnXcomSpeed6->setText(L"6");
	_btnXcomSpeed6->setGroup(&_xcomSpeed);

	_txtAlienSpeed->setColor(Palette::blockOffset(8) + 10);
	_txtAlienSpeed->setText(tr("STR_ALIEN_MOVEMENT_SPEED"));

	_btnAlienSpeed1->setColor(Palette::blockOffset(15)-1);
	_btnAlienSpeed1->setText(L"1");
	_btnAlienSpeed1->setGroup(&_alienSpeed);

	_btnAlienSpeed2->setColor(Palette::blockOffset(15)-1);
	_btnAlienSpeed2->setText(L"2");
	_btnAlienSpeed2->setGroup(&_alienSpeed);

	_btnAlienSpeed3->setColor(Palette::blockOffset(15)-1);
	_btnAlienSpeed3->setText(L"3");
	_btnAlienSpeed3->setGroup(&_alienSpeed);

	_btnAlienSpeed4->setColor(Palette::blockOffset(15)-1);
	_btnAlienSpeed4->setText(L"4");
	_btnAlienSpeed4->setGroup(&_alienSpeed);

	_btnAlienSpeed5->setColor(Palette::blockOffset(15)-1);
	_btnAlienSpeed5->setText(L"5");
	_btnAlienSpeed5->setGroup(&_alienSpeed);

	_btnAlienSpeed6->setColor(Palette::blockOffset(15)-1);
	_btnAlienSpeed6->setText(L"6");
	_btnAlienSpeed6->setGroup(&_alienSpeed);

	_btnOk->setColor(Palette::blockOffset(8) + 5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&OptionsBattlescapeState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&OptionsBattlescapeState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(8) + 5);
	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler) &OptionsBattlescapeState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler) &OptionsBattlescapeState::btnCancelClick, (SDLKey) Options::getInt("keyCancel"));
}

/**
 *
 */
OptionsBattlescapeState::~OptionsBattlescapeState()
{

}

/**
 * Saves options and returns to the previous screen.
 * @param action Pointer to an action.
 */
void OptionsBattlescapeState::btnOkClick(Action *)
{
	if (_scrollSpeed == _btnScrollSpeed1)
		Options::setInt("battleScrollSpeed", 4);
	else if (_scrollSpeed == _btnScrollSpeed2)
		Options::setInt("battleScrollSpeed", 8);
	else if (_scrollSpeed == _btnScrollSpeed3)
		Options::setInt("battleScrollSpeed", 12);
	else if (_scrollSpeed == _btnScrollSpeed4)
		Options::setInt("battleScrollSpeed", 16);
	else if (_scrollSpeed == _btnScrollSpeed5)
		Options::setInt("battleScrollSpeed", 20);

	if (_scrollType == _btnScrollType1)
		Options::setInt("battleScrollType", SCROLL_TRIGGER);
	else if (_scrollType == _btnScrollType2)
		Options::setInt("battleScrollType", SCROLL_AUTO);
	else if (_scrollType == _btnScrollType3)
		Options::setInt("battleScrollType", SCROLL_DRAG);

	if (_fireSpeed == _btnFireSpeed1)
		Options::setInt("battleFireSpeed", 2);
	else if (_fireSpeed == _btnFireSpeed2)
		Options::setInt("battleFireSpeed", 4);
	else if (_fireSpeed == _btnFireSpeed3)
		Options::setInt("battleFireSpeed", 6);
	else if (_fireSpeed == _btnFireSpeed4)
		Options::setInt("battleFireSpeed", 8);
	else if (_fireSpeed == _btnFireSpeed5)
		Options::setInt("battleFireSpeed", 10);
	else if (_fireSpeed == _btnFireSpeed6)
		Options::setInt("battleFireSpeed", 12);

	if (_xcomSpeed == _btnXcomSpeed1)
		Options::setInt("battleXcomSpeed", 40);
	else if (_xcomSpeed == _btnXcomSpeed2)
		Options::setInt("battleXcomSpeed", 30);
	else if (_xcomSpeed == _btnXcomSpeed3)
		Options::setInt("battleXcomSpeed", 20);
	else if (_xcomSpeed == _btnXcomSpeed4)
		Options::setInt("battleXcomSpeed", 10);
	else if (_xcomSpeed == _btnXcomSpeed5)
		Options::setInt("battleXcomSpeed", 5);
	else if (_xcomSpeed == _btnXcomSpeed6)
		Options::setInt("battleXcomSpeed", 1);

	if (_alienSpeed == _btnAlienSpeed1)
		Options::setInt("battleAlienSpeed", 40);
	else if (_alienSpeed == _btnAlienSpeed2)
		Options::setInt("battleAlienSpeed", 30);
	else if (_alienSpeed == _btnAlienSpeed3)
		Options::setInt("battleAlienSpeed", 20);
	else if (_alienSpeed == _btnAlienSpeed4)
		Options::setInt("battleAlienSpeed", 10);
	else if (_alienSpeed == _btnAlienSpeed5)
		Options::setInt("battleAlienSpeed", 5);
	else if (_alienSpeed == _btnAlienSpeed6)
		Options::setInt("battleAlienSpeed", 1);

	_game->popState();
}

/**
* Returns to the previous screen.
* @param action Pointer to an action.
*/
void OptionsBattlescapeState::btnCancelClick(Action *)
{
	_game->popState();
}

}
