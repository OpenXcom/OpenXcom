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
#include "AliensCrashState.h"
#include "DebriefingState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Aliens Crash screen.
 * @param game Pointer to the core game.
 */
AliensCrashState::AliensCrashState()
{
	// Create objects
	_window = new Window(this, 256, 160, 32, 20);
	_btnOk = new TextButton(120, 18, 100, 154);
	_txtTitle = new Text(246, 80, 37, 50);
	
	// Set palette
	setPalette("PAL_BATTLESCAPE");
	
	add(_window, "messageWindowBorder", "battlescape");
	add(_btnOk, "messageWindowButtons", "battlescape");
	add(_txtTitle, "messageWindows", "battlescape");

	centerAllSurfaces();

	// Set up objects
	_window->setHighContrast(true);
	_window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));

	_btnOk->setHighContrast(true);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&AliensCrashState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&AliensCrashState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&AliensCrashState::btnOkClick, Options::keyCancel);

	_txtTitle->setHighContrast(true);
	_txtTitle->setText(tr("STR_ALL_ALIENS_KILLED_IN_CRASH"));
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setVerticalAlign(ALIGN_MIDDLE);
	_txtTitle->setBig();
	_txtTitle->setWordWrap(true);
}

/**
 *
 */
AliensCrashState::~AliensCrashState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void AliensCrashState::btnOkClick(Action *)
{
	_game->popState();
	_game->pushState(new DebriefingState);
}

}
