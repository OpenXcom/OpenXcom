/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "SetWindowedRootState.h"
#include <string>
#include <sstream>
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Engine/Game.h"
#include "../Savegame/SavedGame.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Text.h"
#include "OptionsVideoState.h"
#include "../Interface/TextEdit.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Asks user new coordinates screen.
 * @param game Pointer to the core game.
 * @param OptionsVideoState Options screen that originated this state.
 */
SetWindowedRootState::SetWindowedRootState(OptionsOrigin origin, OptionsVideoState *optionsVideoState) : _origin(origin), _optionsVideoState(optionsVideoState)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 216, 100, 52, 50, POPUP_BOTH);
	_txtTitle = new Text(206, 20, 57, 70);
	_txtWindowedModePositionX = new Text(160, 10, 25, 90);
	_txtWindowedModePositionY = new Text(160, 10, 25, 100);
	_btnOk = new TextButton(50, 20, 70, 120);
	_btnCancel = new TextButton(50, 20, 200, 120);
	_edtWindowedModePositionX = new TextEdit(this, 40, 10, 190, 90);
	_edtWindowedModePositionY = new TextEdit(this, 40, 10, 190, 100);

	// Set palette
	setInterface("mainMenu", false, _game->getSavedGame() ? _game->getSavedGame()->getSavedBattle() : 0);

	add(_window, "confirmVideo", "mainMenu");
	add(_btnOk, "confirmVideo", "mainMenu");
	add(_btnCancel, "confirmVideo", "mainMenu");
	add(_txtTitle, "confirmVideo", "mainMenu");
	add(_txtWindowedModePositionX, "confirmVideo", "mainMenu");
	add(_txtWindowedModePositionY, "confirmVideo", "mainMenu");
	add(_edtWindowedModePositionX, "confirmVideo", "mainMenu");
	add(_edtWindowedModePositionY, "confirmVideo", "mainMenu");

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setWordWrap(true);
	_txtTitle->setText(tr("STR_DISPLAY_SET_WINDOW_POSITION"));

	_txtWindowedModePositionX->setAlign(ALIGN_RIGHT);
	_txtWindowedModePositionX->setWordWrap(true);
	_txtWindowedModePositionX->setText(tr("STR_DISPLAY_WINDOW_POSITION_NEW_X"));

	_txtWindowedModePositionY->setAlign(ALIGN_RIGHT);
	_txtWindowedModePositionY->setWordWrap(true);
	_txtWindowedModePositionY->setText(tr("STR_DISPLAY_WINDOW_POSITION_NEW_Y"));

	std::wostringstream wss, wss2;

	wss << Options::newWindowedModePositionX;
	wss2 << Options::newWindowedModePositionY;

	_edtWindowedModePositionX->setText(wss.str());
	_edtWindowedModePositionX->setConstraint(TEC_NUMERIC);

	_edtWindowedModePositionY->setText(wss2.str());
	_edtWindowedModePositionY->setConstraint(TEC_NUMERIC);

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SetWindowedRootState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SetWindowedRootState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&SetWindowedRootState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&SetWindowedRootState::btnCancelClick, Options::keyCancel);

	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}
}

/**
 *
 */
SetWindowedRootState::~SetWindowedRootState()
{
}

/**
 * Roots borderless window
 * @param action Pointer to an action.
 */
void SetWindowedRootState::btnOkClick(Action *)
{
	std::wstringstream convStreamX, convStreamY;
	int newWindowedModePositionX = 0, newWindowedModePositionY = 0;

	convStreamX << _edtWindowedModePositionX->getText();
	convStreamY << _edtWindowedModePositionY->getText();

	convStreamX >> newWindowedModePositionX;
	convStreamY >> newWindowedModePositionY;

	Options::newRootWindowedMode = true;
	Options::newWindowedModePositionX = newWindowedModePositionX;
	Options::newWindowedModePositionY = newWindowedModePositionY;

	_game->popState();
}

/**
 * Cancels borderless window rooting
 * @param action Pointer to an action.
 */
void SetWindowedRootState::btnCancelClick(Action *)
{
	_optionsVideoState->unpressRootWindowedMode();

	_game->popState();
}

}
