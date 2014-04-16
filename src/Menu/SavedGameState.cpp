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
#include "SavedGameState.h"
#include <utility>
#include "../Engine/Logger.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Game.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Screen.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/ArrowButton.h"

namespace OpenXcom
{

struct compareSaveName : public std::binary_function<SaveInfo&, SaveInfo&, bool>
{
	bool operator()(const SaveInfo &a, const SaveInfo &b) const { return CrossPlatform::naturalCompare(a.displayName, b.displayName); }
};

struct compareSaveTimestamp : public std::binary_function<SaveInfo&, SaveInfo&, bool>
{
	bool operator()(const SaveInfo &a, const SaveInfo &b) const { return a.timestamp < b.timestamp; }
};

/**
 * Initializes all the elements in the Saved Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
SavedGameState::SavedGameState(Game *game, OptionsOrigin origin, int firstValidRow) : State(game), _origin(origin), _showMsg(true), _noUI(false), _firstValidRow(firstValidRow)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnCancel = new TextButton(80, 16, 120, 172);
	_txtTitle = new Text(310, 17, 5, 7);
	_txtDelete = new Text(310, 9, 5, 23);
	_txtName = new Text(150, 9, 16, 32);
	_txtDate = new Text(110, 9, 204, 32);
	_txtStatus = new Text(320, 17, 0, 92);
	_lstSaves = new TextList(288, 112, 8, 40);
	_txtDetails = new Text(288, 16, 16, 156);
	_sortName = new ArrowButton(ARROW_NONE, 11, 8, 16, 32);
	_sortDate = new ArrowButton(ARROW_NONE, 11, 8, 204, 32);

	// Set palette
	if (_origin == OPT_BATTLESCAPE)
	{
		setPalette("PAL_BATTLESCAPE");
	}
	else
	{
		setPalette("PAL_GEOSCAPE", 6);
	}

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtDelete);
	add(_txtName);
	add(_txtDate);
	add(_lstSaves);
	add(_txtStatus);
	add(_txtDetails);
	add(_sortName);
	add(_sortDate);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&SavedGameState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&SavedGameState::btnCancelClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtDelete->setColor(Palette::blockOffset(15)-1);
	_txtDelete->setAlign(ALIGN_CENTER);
	_txtDelete->setText(tr("STR_RIGHT_CLICK_TO_DELETE"));

	_txtName->setColor(Palette::blockOffset(15)-1);
	_txtName->setText(tr("STR_NAME"));

	_txtDate->setColor(Palette::blockOffset(15)-1);
	_txtDate->setText(tr("STR_DATE"));
	
	_txtStatus->setColor(Palette::blockOffset(8)+5);
	_txtStatus->setBig();
	_txtStatus->setAlign(ALIGN_CENTER);

	_lstSaves->setColor(Palette::blockOffset(8)+10);
	_lstSaves->setArrowColor(Palette::blockOffset(8)+5);
	_lstSaves->setColumns(3, 188, 60, 40);
	_lstSaves->setSelectable(true);
	_lstSaves->setBackground(_window);
	_lstSaves->setMargin(8);
	_lstSaves->onMouseOver((ActionHandler)&SavedGameState::lstSavesMouseOver);
	_lstSaves->onMouseOut((ActionHandler)&SavedGameState::lstSavesMouseOut);

	_txtDetails->setColor(Palette::blockOffset(15)-1);
	_txtDetails->setSecondaryColor(Palette::blockOffset(8)+10);
	_txtDetails->setWordWrap(true);
	_txtDetails->setText(tr("STR_DETAILS").arg(L""));

	_sortName->setX(_sortName->getX() + _txtName->getTextWidth() + 5);
	_sortName->setColor(Palette::blockOffset(15)-1);
	_sortName->onMouseClick((ActionHandler)&SavedGameState::sortNameClick);

	_sortDate->setX(_sortDate->getX() + _txtDate->getTextWidth() + 5);
	_sortDate->setColor(Palette::blockOffset(15)-1);
	_sortDate->onMouseClick((ActionHandler)&SavedGameState::sortDateClick);

	updateArrows();
}

/**
 * Initializes all the elements in the Saved Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param showMsg True if need to show messages like "Loading game" or "Saving game".
 */
SavedGameState::SavedGameState(Game *game, OptionsOrigin origin, int firstValidRow, bool showMsg) : State(game), _origin(origin), _showMsg(showMsg), _noUI(true), _firstValidRow(firstValidRow)
{
	if (_showMsg)
	{
		_txtStatus = new Text(320, 16, _game->getScreen()->getDX(), 92 + _game->getScreen()->getDY());
		add(_txtStatus);

		_txtStatus->setBig();
		_txtStatus->setAlign(ALIGN_CENTER);
		if (origin == OPT_BATTLESCAPE)
		{
			_txtStatus->setColor(Palette::blockOffset(5));
			_txtStatus->setHighContrast(true);
		}
		else
			_txtStatus->setColor(Palette::blockOffset(8)+5);
	}
}

/**
 *
 */
SavedGameState::~SavedGameState()
{

}

/**
 * Resets the palette and refreshes saves.
 */
void SavedGameState::init()
{
	if (_noUI)
	{
		_game->popState();
		return;
	}
	State::init();

	_txtStatus->setText(L"");

	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}

	try
	{
		_saves = SavedGame::getList(_game->getLanguage());
		_lstSaves->clearList();
		sortList(Options::saveOrder);
	}
	catch (Exception &e)
	{
		Log(LOG_ERROR) << e.what();
	}
}

/**
 * Updates the sorting arrows based
 * on the current setting.
 */
void SavedGameState::updateArrows()
{
	_sortName->setShape(ARROW_NONE);
	_sortDate->setShape(ARROW_NONE);
	switch (Options::saveOrder)
	{
	case SORT_NAME_ASC:
		_sortName->setShape(ARROW_SMALL_UP);
		break;
	case SORT_NAME_DESC:
		_sortName->setShape(ARROW_SMALL_DOWN);
		break;
	case SORT_DATE_ASC:
		_sortDate->setShape(ARROW_SMALL_UP);
		break;
	case SORT_DATE_DESC:
		_sortDate->setShape(ARROW_SMALL_DOWN);
		break;
	}
}

/**
 * Updates the save game list with the current list
 * of available savegames.
 */
void SavedGameState::sortList(SaveSort sort)
{
	switch (sort)
	{
	case SORT_NAME_ASC:
		std::sort(_saves.begin(), _saves.end(), compareSaveName());
		break;
	case SORT_NAME_DESC:
		std::sort(_saves.rbegin(), _saves.rend(), compareSaveName());
		break;
	case SORT_DATE_ASC:
		std::sort(_saves.begin(), _saves.end(), compareSaveTimestamp());
		break;
	case SORT_DATE_DESC:
		std::sort(_saves.rbegin(), _saves.rend(), compareSaveTimestamp());
		break;
	}
	updateList();
}

/**
 * Updates the save game list with the current list
 * of available savegames.
 */
void SavedGameState::updateList()
{
	for (std::vector<SaveInfo>::const_iterator i = _saves.begin(); i != _saves.end(); ++i)
	{
		_lstSaves->addRow(3, i->displayName.c_str(), i->isoDate.c_str(), i->isoTime.c_str());
	}
}

/**
 * Updates the status message in the center of the screen.
 * @param msg New message ID.
 */
void SavedGameState::updateStatus(const std::string &msg)
{
	_txtStatus->setText(tr(msg));
	blit();
	_game->getScreen()->flip();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SavedGameState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Shows the details of the currently hovered save.
 * @param action Pointer to an action.
 */
void SavedGameState::lstSavesMouseOver(Action *)
{
	int sel = _lstSaves->getSelectedRow() - _firstValidRow;
	std::wstring wstr;
	if (sel >= 0 && sel < (int)_saves.size())
	{
		wstr = _saves[sel].details;
	}
	_txtDetails->setText(tr("STR_DETAILS").arg(wstr));
}

/**
 * Clears the details.
 * @param action Pointer to an action.
 */
void SavedGameState::lstSavesMouseOut(Action *)
{
	_txtDetails->setText(tr("STR_DETAILS").arg(L""));
}

/**
 * Sorts the saves by name.
 * @param action Pointer to an action.
 */
void SavedGameState::sortNameClick(Action *)
{
	if (Options::saveOrder == SORT_NAME_ASC)
	{
		Options::saveOrder = SORT_NAME_DESC;
	}
	else
	{
		Options::saveOrder = SORT_NAME_ASC;
	}
	updateArrows();
	_lstSaves->clearList();
	sortList(Options::saveOrder);
}

/**
 * Sorts the saves by date.
 * @param action Pointer to an action.
 */
void SavedGameState::sortDateClick(Action *)
{
	if (Options::saveOrder == SORT_DATE_ASC)
	{
		Options::saveOrder = SORT_DATE_DESC;
	}
	else
	{
		Options::saveOrder = SORT_DATE_ASC;
	}
	updateArrows();
	_lstSaves->clearList();
	sortList(Options::saveOrder);
}

}
