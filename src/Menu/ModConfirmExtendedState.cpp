/*
 * Copyright 2010-2019 OpenXcom Developers.
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
#include "ModConfirmExtendedState.h"
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Engine/ModInfo.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Mod/Mod.h"
#include "ModListState.h"

namespace OpenXcom
{

	/**
	 * Initializes all the elements in the Confirm OXCE screen.
	 * @param state Pointer to the Options|Mod state.
	 * @param modInfo What exactly mod caused this question?
	 */
	ModConfirmExtendedState::ModConfirmExtendedState(ModListState *state, const ModInfo *modInfo) : _state(state), _isMaster(modInfo->isMaster())
	{
		_screen = false;

		// Create objects
		_window = new Window(this, 256, 100, 32, 50, POPUP_BOTH);
		_btnYes = new TextButton(60, 18, 60, 122);
		_btnNo = new TextButton(60, 18, 200, 122);
		_txtTitle = new Text(246, 50, 37, 64);

		// Set palette
		setInterface("optionsMenu");

		add(_window, "confirmDefaults", "optionsMenu");
		add(_btnYes, "confirmDefaults", "optionsMenu");
		add(_btnNo, "confirmDefaults", "optionsMenu");
		add(_txtTitle, "confirmDefaults", "optionsMenu");

		centerAllSurfaces();

		// Set up objects
		_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

		_btnYes->setText(tr("STR_YES"));
		_btnYes->onMouseClick((ActionHandler)&ModConfirmExtendedState::btnYesClick);

		_btnNo->setText(tr("STR_NO"));
		_btnNo->onMouseClick((ActionHandler)&ModConfirmExtendedState::btnNoClick);

		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(tr("STR_OXCE_REQUIRED_QUESTION").arg(modInfo->getRequiredExtendedEngine()));
	}

	/**
	 *
	 */
	ModConfirmExtendedState::~ModConfirmExtendedState()
	{

	}

	/**
	 * Closes the window. Enables the mod.
	 * @param action Pointer to an action.
	 */
	void ModConfirmExtendedState::btnYesClick(Action *)
	{
		_game->popState();

		if (_isMaster)
		{
			_state->changeMasterMod();
		}
		else
		{
			_state->toggleMod();
		}
	}

	/**
	 * Closes the window. Does not enable the mod.
	 * @param action Pointer to an action.
	 */
	void ModConfirmExtendedState::btnNoClick(Action *)
	{
		_game->popState();

		if (_isMaster)
		{
			_state->revertMasterMod();
		}
	}

}
