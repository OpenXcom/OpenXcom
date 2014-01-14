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
#include "MedalsInfoState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/Armor.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Armor window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param soldier ID of the selected soldier.
 */
MedalsInfoState::MedalsInfoState(Base *base, Game *game, int rowEntry, size_t soldier, std::vector<int> medalListEntry) : _base(base), State(game), _rowEntry(rowEntry), _soldier(soldier), _medalListEntry(medalListEntry)
{
	// Soldier *s = _base->getSoldiers()->at(_soldier);
	_screen = false;

	// Create objects
	_window = new Window(this, 192, 120, 64, 40, POPUP_BOTH);
	_btnOk = new TextButton(140, 16, 90, 136);
	_txtMedalName = new Text(182, 9, 69, 48);
	_txtMedalLevel = new Text(182, 9, 69, 56);
	_txtMedalInfoTitle = new Text(90, 9, 80, 72);
	// _txtQuantity = new Text(70, 9, 177, 72);
	_txtMedalInfo = new Text(160, 48, 73, 88);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtMedalName);
	add(_txtMedalLevel);
	add(_txtMedalInfoTitle);
	add(_txtMedalInfo);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR")); // Differnet backgorund?

	_btnOk->setColor(Palette::blockOffset(13)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&MedalsInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&MedalsInfoState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_txtMedalName->setColor(Palette::blockOffset(13)+5);
	_txtMedalName->setAlign(ALIGN_CENTER);
	_txtMedalName->setText(tr(Soldier::getMedalNameString(_medalListEntry[rowEntry]))); // fetch the name of the medal

	_txtMedalLevel->setColor(Palette::blockOffset(13)+5);
	_txtMedalLevel->setAlign(ALIGN_CENTER);
	// NO _txtMedalLevel->setText(tr(getMedalAwardLevelString(_medal->second))); // fetch the level of the medal

	_txtMedalInfoTitle->setColor(Palette::blockOffset(13)+5);
	_txtMedalInfoTitle->setText(tr("STR_MEDAL_DESCRIPTION"));

	_medalListEntry[rowEntry];

	_txtMedalInfo->setColor(Palette::blockOffset(13));
	_txtMedalInfo->setWordWrap(true);
	_txtMedalInfo->setText(tr(Soldier::getMedalDescriptionString(_medalListEntry[rowEntry]))); // fetch the medal description

}

/**
 *
 */
MedalsInfoState::~MedalsInfoState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void MedalsInfoState::btnOkClick(Action *)
{
	_game->popState();
}

}
