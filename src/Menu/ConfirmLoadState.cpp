/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "ConfirmLoadState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Engine/Options.h"
#include "LoadGameState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Confirm Load screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param fileName Name of the save file without extension.
 */
ConfirmLoadState::ConfirmLoadState(OptionsOrigin origin, const std::string &fileName) : _origin(origin), _fileName(fileName)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 216, 100, 52, 50, POPUP_BOTH);
	_btnYes = new TextButton(50, 20, 70, 120);
	_btnNo = new TextButton(50, 20, 200, 120);
	_txtText = new Text(204, 58, 58, 60);

	// Set palette
	if (_origin == OPT_BATTLESCAPE)
	{
		setPalette("PAL_BATTLESCAPE");
	}
	else
	{
		setPalette("PAL_GEOSCAPE", _game->getRuleset()->getInterface("saveMenus")->getElement("palette")->color);
	}

	add(_window, "confirmLoad", "saveMenus");
	add(_btnYes, "confirmLoad", "saveMenus");
	add(_btnNo, "confirmLoad", "saveMenus");
	add(_txtText, "confirmLoad", "saveMenus");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnYes->setText(tr("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&ConfirmLoadState::btnYesClick);
	_btnYes->onKeyboardPress((ActionHandler)&ConfirmLoadState::btnYesClick, Options::keyOk);

	_btnNo->setText(tr("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&ConfirmLoadState::btnNoClick);
	_btnNo->onKeyboardPress((ActionHandler)&ConfirmLoadState::btnNoClick, Options::keyCancel);

	_txtText->setAlign(ALIGN_CENTER);
	_txtText->setBig();
	_txtText->setWordWrap(true);
	_txtText->setText(tr("STR_MISSING_CONTENT_PROMPT"));
	
	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}
}

/// Cleans up the confirmation state.
ConfirmLoadState::~ConfirmLoadState()
{
}

/**
 * Proceed to load the save.
 * @param action Pointer to an action.
 */
void ConfirmLoadState::btnYesClick(Action *)
{
	_game->popState();
	_game->pushState(new LoadGameState(_origin, _fileName, _palette));
}

/**
 * Abort loading and return to save list.
 * @param action Pointer to an action.
 */
void ConfirmLoadState::btnNoClick(Action *)
{
	_game->popState();
}

}
