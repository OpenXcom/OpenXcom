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
#include "TargetInfoState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Target.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Target Info window.
 * @param game Pointer to the core game.
 * @param target Pointer to the target to show info from.
 */
TargetInfoState::TargetInfoState(Game *game, Target *target) : State(game), _target(target)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 192, 120, 32, 40, POPUP_BOTH);
	_btnOk = new TextButton(160, 16, 48, 135);
	_txtTitle = new Text(182, 16, 37, 54);
	_txtTargetted = new Text(182, 8, 37, 74);
	_txtFollowers = new Text(182, 48, 37, 84);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtTargetted);
	add(_txtFollowers);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&TargetInfoState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_target->getName(_game->getLanguage()));

	_txtTargetted->setColor(Palette::blockOffset(15)-1);
	_txtTargetted->setAlign(ALIGN_CENTER);
	_txtTargetted->setText(_game->getLanguage()->getString("STR_TARGETTED_BY"));

	_txtFollowers->setColor(Palette::blockOffset(15)-1);
	_txtFollowers->setAlign(ALIGN_CENTER);
	std::wstring s = L"";
	for (std::vector<Target*>::iterator i = _target->getFollowers()->begin(); i != _target->getFollowers()->end(); ++i)
	{
		s += (*i)->getName(_game->getLanguage());
		s += L'\n';
	}
	_txtFollowers->setText(s);
}

/**
 *
 */
TargetInfoState::~TargetInfoState()
{

}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void TargetInfoState::btnOkClick(Action *action)
{
	_game->popState();
}

}
