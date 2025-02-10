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
#include "OptionsDefaultsState.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Options.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Restore Defaults screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param state Pointer to the base Options state.
 */
OptionsDefaultsState::OptionsDefaultsState(OptionsOrigin origin, OptionsBaseState *state) : _origin(origin), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 256, 100, 32, 50, POPUP_BOTH);
	_btnYes = new TextButton(60, 18, 60, 122);
	_btnNo = new TextButton(60, 18, 200, 122);
	_txtTitle = new Text(246, 32, 37, 70);

	// Set palette
	setInterface("optionsMenu", false, _game->getSavedGame() ? _game->getSavedGame()->getSavedBattle() : 0);

	add(_window, "confirmDefaults", "optionsMenu");
	add(_btnYes, "confirmDefaults", "optionsMenu");
	add(_btnNo, "confirmDefaults", "optionsMenu");
	add(_txtTitle, "confirmDefaults", "optionsMenu");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_btnYes->setText(tr("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&OptionsDefaultsState::btnYesClick);
	_btnYes->onKeyboardPress((ActionHandler)&OptionsDefaultsState::btnYesClick, Options::keyOk);

	_btnNo->setText(tr("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&OptionsDefaultsState::btnNoClick);
	_btnNo->onKeyboardPress((ActionHandler)&OptionsDefaultsState::btnNoClick, Options::keyCancel);

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setWordWrap(true);
	_txtTitle->setText(tr("STR_RESTORE_DEFAULTS_QUESTION"));

	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}
}

/**
 *
 */
OptionsDefaultsState::~OptionsDefaultsState()
{

}

/**
 * Restores the default options.
 * @param action Pointer to an action.
 */
void OptionsDefaultsState::btnYesClick(Action *action)
{
	Options::resetDefault(false);
	Options::save(true);
	_game->popState();
	_state->btnOkClick(action);
}

/**
 * Closes the window.
 * @param action Pointer to an action.
 */
void OptionsDefaultsState::btnNoClick(Action *)
{
	_game->popState();
}

}
