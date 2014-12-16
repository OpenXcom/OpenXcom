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
#include "ListGamesState.h"
#include <utility>
#include "../Engine/Logger.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Engine/CrossPlatform.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/ArrowButton.h"
#include "DeleteGameState.h"

namespace OpenXcom
{

struct compareSaveName : public std::binary_function<SaveInfo&, SaveInfo&, bool>
{
	bool _reverse;

	compareSaveName(bool reverse) : _reverse(reverse) {}

	bool operator()(const SaveInfo &a, const SaveInfo &b) const
	{
		if (a.reserved == b.reserved)
		{
			return CrossPlatform::naturalCompare(a.displayName, b.displayName);
		}
		else
		{
			return _reverse ? b.reserved : a.reserved;
		}
	}
};

struct compareSaveTimestamp : public std::binary_function<SaveInfo&, SaveInfo&, bool>
{
	bool _reverse;

	compareSaveTimestamp(bool reverse) : _reverse(reverse) {}

	bool operator()(const SaveInfo &a, const SaveInfo &b) const
	{
		if (a.reserved == b.reserved)
		{
			return a.timestamp < b.timestamp;
		}
		else
		{
			return _reverse ? b.reserved : a.reserved;
		}
	}
};

/**
 * Initializes all the elements in the Saved Game screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 * @param firstValidRow First row containing saves.
 * @param autoquick Show auto/quick saved games?
 */
ListGamesState::ListGamesState(OptionsOrigin origin, int firstValidRow, bool autoquick) : _origin(origin), _firstValidRow(firstValidRow), _autoquick(autoquick), _sortable(true)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnCancel = new TextButton(80, 16, 120, 172);
	_txtTitle = new Text(310, 17, 5, 7);
	_txtDelete = new Text(310, 9, 5, 23);
	_txtName = new Text(150, 9, 16, 32);
	_txtDate = new Text(110, 9, 204, 32);
	_lstSaves = new TextList(288, 112, 8, 42);
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
	add(_txtDetails);
	add(_sortName);
	add(_sortDate);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&ListGamesState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ListGamesState::btnCancelClick, Options::keyCancel);

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

	_lstSaves->setColor(Palette::blockOffset(8)+10);
	_lstSaves->setArrowColor(Palette::blockOffset(8)+5);
	_lstSaves->setColumns(3, 188, 60, 40);
	_lstSaves->setSelectable(true);
	_lstSaves->setBackground(_window);
	_lstSaves->setMargin(8);
	_lstSaves->onMouseOver((ActionHandler)&ListGamesState::lstSavesMouseOver);
	_lstSaves->onMouseOut((ActionHandler)&ListGamesState::lstSavesMouseOut);
	_lstSaves->onMousePress((ActionHandler)&ListGamesState::lstSavesPress);

	_txtDetails->setColor(Palette::blockOffset(15)-1);
	_txtDetails->setSecondaryColor(Palette::blockOffset(8)+10);
	_txtDetails->setWordWrap(true);
	_txtDetails->setText(tr("STR_DETAILS").arg(L""));

	_sortName->setX(_sortName->getX() + _txtName->getTextWidth() + 5);
	_sortName->setColor(Palette::blockOffset(15)-1);
	_sortName->onMouseClick((ActionHandler)&ListGamesState::sortNameClick);

	_sortDate->setX(_sortDate->getX() + _txtDate->getTextWidth() + 5);
	_sortDate->setColor(Palette::blockOffset(15)-1);
	_sortDate->onMouseClick((ActionHandler)&ListGamesState::sortDateClick);

	updateArrows();
}

/**
 *
 */
ListGamesState::~ListGamesState()
{

}

/**
 * Refreshes the saves list.
 */
void ListGamesState::init()
{
	State::init();

	if (_origin == OPT_BATTLESCAPE)
	{
		applyBattlescapeTheme();
	}

	try
	{
		_saves = SavedGame::getList(_game->getLanguage(), _autoquick);
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
void ListGamesState::updateArrows()
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
 * Sorts the save game list.
 * @param sort Order to sort the games in.
 */
void ListGamesState::sortList(SaveSort sort)
{
	switch (sort)
	{
	case SORT_NAME_ASC:
		std::sort(_saves.begin(), _saves.end(), compareSaveName(false));
		break;
	case SORT_NAME_DESC:
		std::sort(_saves.rbegin(), _saves.rend(), compareSaveName(true));
		break;
	case SORT_DATE_ASC:
		std::sort(_saves.begin(), _saves.end(), compareSaveTimestamp(false));
		break;
	case SORT_DATE_DESC:
		std::sort(_saves.rbegin(), _saves.rend(), compareSaveTimestamp(true));
		break;
	}
	updateList();
}

/**
 * Updates the save game list with the current list
 * of available savegames.
 */
void ListGamesState::updateList()
{
	int row = 0;
	for (std::vector<SaveInfo>::const_iterator i = _saves.begin(); i != _saves.end(); ++i)
	{
		_lstSaves->addRow(3, i->displayName.c_str(), i->isoDate.c_str(), i->isoTime.c_str());
		if (i->reserved && _origin != OPT_BATTLESCAPE)
		{
			_lstSaves->setRowColor(row, Palette::blockOffset(8)+5);
		}
		row++;
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ListGamesState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Shows the details of the currently hovered save.
 * @param action Pointer to an action.
 */
void ListGamesState::lstSavesMouseOver(Action *)
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
void ListGamesState::lstSavesMouseOut(Action *)
{
	_txtDetails->setText(tr("STR_DETAILS").arg(L""));
}

/**
 * Deletes the selected save.
 * @param action Pointer to an action.
 */
void ListGamesState::lstSavesPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT && _lstSaves->getSelectedRow() >= _firstValidRow)
	{
		_game->pushState(new DeleteGameState(_origin, _saves[_lstSaves->getSelectedRow() - _firstValidRow].fileName));
	}
}

/**
 * Sorts the saves by name.
 * @param action Pointer to an action.
 */
void ListGamesState::sortNameClick(Action *)
{
	if (_sortable)
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
}

/**
 * Sorts the saves by date.
 * @param action Pointer to an action.
 */
void ListGamesState::sortDateClick(Action *)
{
	if (_sortable)
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

void ListGamesState::disableSort()
{
	_sortable = false;
}

}
