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
#include "ModListState.h"
#include "ModConfirmExtendedState.h"
#include <climits>
#include <algorithm>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Window.h"
#include "../Interface/TextList.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/ComboBox.h"
#include "../Engine/Options.h"
#include "../Engine/Action.h"
#include "StartState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Mod Options window.
 * @param game Pointer to the core game.
 */
ModListState::ModListState() : _curMasterIdx(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);

	_txtMaster = new Text(305, 9, 8, 8);
	_cbxMasters = new ComboBox(this, 305, 16, 8, 18);
	_lstMods = new TextList(288, 104, 8, 40);

	_btnOk = new TextButton(100, 16, 8, 176);
	_btnCancel = new TextButton(100, 16, 212, 176);

	_txtTooltip = new Text(305, 25, 8, 148);

	// Set palette
	setInterface("modsMenu");

	add(_window, "window", "modsMenu");

	add(_txtMaster, "text", "modsMenu");
	add(_lstMods, "optionLists", "modsMenu");
	add(_btnOk, "button2", "modsMenu");
	add(_btnCancel, "button2", "modsMenu");
	add(_txtTooltip, "tooltip", "modsMenu");

	add(_cbxMasters, "button1", "modsMenu");

	centerAllSurfaces();

	// how much room do we need for YES/NO
	Text text = Text(100, 9, 0, 0);
	text.initText(_game->getMod()->getFont("FONT_BIG"), _game->getMod()->getFont("FONT_SMALL"), _game->getLanguage());
	text.setText(tr("STR_YES"));
	int yes = text.getTextWidth();
	text.setText(tr("STR_NO"));
	int no = text.getTextWidth();

	int rightcol = std::max(yes, no) + 2;
	int arrowCol = 25;
	int leftcol = _lstMods->getWidth() - (rightcol + arrowCol);

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_txtMaster->setText(tr("STR_BASE_GAME"));

	// scan for masters
	Options::refreshMods();
	const std::map<std::string, ModInfo> &modInfos(Options::getModInfos());
	std::vector<std::string> masterNames;
	for (std::vector< std::pair<std::string, bool> >::const_iterator i = Options::mods.begin(); i != Options::mods.end(); ++i)
	{
		std::string modId = i->first;
		const ModInfo *modInfo = &modInfos.at(modId);
		if (!modInfo->isMaster())
		{
			continue;
		}

		if (i->second)
		{
			_curMasterId = modId;
		}
		else if (_curMasterId.empty())
		{
			++_curMasterIdx;
		}
		_masters.push_back(modInfo);
		masterNames.push_back(modInfo->getName());
	}

	_cbxMasters->setOptions(masterNames);
	_cbxMasters->setSelected(_curMasterIdx);
	_cbxMasters->onChange((ActionHandler)&ModListState::cbxMasterChange);
	_cbxMasters->onMouseIn((ActionHandler)&ModListState::txtTooltipIn);
	_cbxMasters->onMouseOut((ActionHandler)&ModListState::txtTooltipOut);
	_cbxMasters->onMouseOver((ActionHandler)&ModListState::cbxMasterHover);
	_cbxMasters->onListMouseIn((ActionHandler)&ModListState::txtTooltipIn);
	_cbxMasters->onListMouseOut((ActionHandler)&ModListState::txtTooltipOut);
	_cbxMasters->onListMouseOver((ActionHandler)&ModListState::cbxMasterHover);

	_lstMods->setArrowColumn(leftcol + 1, ARROW_VERTICAL);
	_lstMods->setColumns(3, leftcol, arrowCol, rightcol);
	_lstMods->setAlign(ALIGN_RIGHT, 1);
	_lstMods->setSelectable(true);
	_lstMods->setBackground(_window);
	_lstMods->setWordWrap(true);
	_lstMods->onMouseClick((ActionHandler)&ModListState::lstModsClick);
	_lstMods->onLeftArrowClick((ActionHandler)&ModListState::lstModsLeftArrowClick);
	_lstMods->onRightArrowClick((ActionHandler)&ModListState::lstModsRightArrowClick);
	_lstMods->onMousePress((ActionHandler)&ModListState::lstModsMousePress);
	_lstMods->onMouseIn((ActionHandler)&ModListState::txtTooltipIn);
	_lstMods->onMouseOut((ActionHandler)&ModListState::txtTooltipOut);
	_lstMods->onMouseOver((ActionHandler)&ModListState::lstModsHover);
	lstModsRefresh(0);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ModListState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ModListState::btnOkClick, Options::keyOk);

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&ModListState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ModListState::btnCancelClick, Options::keyCancel);

	_txtTooltip->setWordWrap(true);
}

ModListState::~ModListState()
{

}

std::string ModListState::makeTooltip(const ModInfo &modInfo)
{
	return tr("STR_MODS_TOOLTIP").arg(modInfo.getVersion()).arg(modInfo.getAuthor()).arg(modInfo.getDescription());
}

void ModListState::cbxMasterHover(Action *)
{
	_txtTooltip->setText(makeTooltip(*_masters[_cbxMasters->getHoveredListIdx()]));
}

void ModListState::cbxMasterChange(Action *)
{
	const ModInfo *masterModInfo = _masters[_cbxMasters->getSelected()];

	// when changing a master mod, check if it requires OXCE
	{
		if (!masterModInfo->isEngineOk())
		{
			_game->pushState(new ModConfirmExtendedState(this, masterModInfo));
			return;
		}
	}

	changeMasterMod();
}

void ModListState::changeMasterMod()
{
	std::string masterId = _masters[_cbxMasters->getSelected()]->getId();
	for (std::vector< std::pair<std::string, bool> >::iterator i = Options::mods.begin(); i != Options::mods.end(); ++i)
	{
		if (masterId == i->first)
		{
			i->second = true;
		}
		else if (_curMasterId == i->first)
		{
			i->second = false;
		}
	}
	Options::reload = true;

	_curMasterIdx = _cbxMasters->getSelected();
	_curMasterId = masterId;
	lstModsRefresh(0);
}

void ModListState::revertMasterMod()
{
	_cbxMasters->setSelected(_curMasterIdx);
}

void ModListState::lstModsRefresh(size_t scrollLoc)
{
	_lstMods->clearList();
	_mods.clear();

	// only show mods that work with the current master
	for (std::vector< std::pair<std::string, bool> >::const_iterator i = Options::mods.begin(); i != Options::mods.end(); ++i)
	{
		ModInfo modInfo = Options::getModInfo(i->first);
		if (modInfo.isMaster() || !modInfo.canActivate(_curMasterId))
		{
			continue;
		}

		std::string modName = modInfo.getName();
		_lstMods->addRow(3, modName.c_str(), "", (i->second ? tr("STR_YES").c_str() : tr("STR_NO").c_str()));
		_mods.push_back(*i);
	}

	_lstMods->scrollTo(scrollLoc);
}

void ModListState::lstModsHover(Action *)
{
	size_t selectedRow = _lstMods->getSelectedRow();
	if ((unsigned int)-1 != selectedRow)
	{
		_txtTooltip->setText(makeTooltip(Options::getModInfos().at(_mods[selectedRow].first)));

	}
}

void ModListState::lstModsClick(Action *action)
{
	if (action->getAbsoluteXMouse() >= _lstMods->getArrowsLeftEdge() &&
		action->getAbsoluteXMouse() <= _lstMods->getArrowsRightEdge())
	{
		// don't count an arrow click as a mod enable toggle
		return;
	}

	std::pair<std::string, bool> &mod(_mods.at(_lstMods->getSelectedRow()));

	// when activating a mod, check if it requires OXCE
	if (!mod.second)
	{
		const ModInfo *modInfo = &Options::getModInfos().at(mod.first);
		if (!modInfo->isEngineOk())
		{
			_game->pushState(new ModConfirmExtendedState(this, modInfo));
			return;
		}
	}

	// if deactivating, or if not OXCE mod
	toggleMod();
}

void ModListState::toggleMod()
{
	std::pair<std::string, bool> &mod(_mods.at(_lstMods->getSelectedRow()));

	for (std::vector< std::pair<std::string, bool> >::iterator i = Options::mods.begin(); i != Options::mods.end(); ++i)
	{
		if (mod.first != i->first)
		{
			continue;
		}

		mod.second = ! mod.second;
		i->second = mod.second;
		_lstMods->setCellText(_lstMods->getSelectedRow(), 2, (mod.second ? tr("STR_YES") : tr("STR_NO")));

		break;
	}
	Options::reload = true;
}

void ModListState::lstModsLeftArrowClick(Action *action)
{
	unsigned int row = _lstMods->getSelectedRow();
	if (row <= 0)
	{
		return;
	}

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		moveModUp(action, row);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		moveModUp(action, row, true);
	}
}

static void _moveAbove(const std::pair<std::string, bool> &srcMod, const std::pair<std::string, bool> &destMod)
{
	// insert copy of srcMod above destMod
	for (std::vector< std::pair<std::string, bool> >::iterator i = Options::mods.begin(); i != Options::mods.end(); ++i)
	{
		if (destMod.first == i->first)
		{
			Options::mods.insert(i, srcMod);
			break;
		}
	}

	// remove old copy of srcMod in separate loop since the insert above invalidated the iterator
	for (std::vector< std::pair<std::string, bool> >::reverse_iterator i = Options::mods.rbegin(); i != Options::mods.rend(); ++i)
	{
		if (srcMod.first == i->first)
		{
			Options::mods.erase(i.base() - 1);
			break;
		}
	}
}

void ModListState::moveModUp(Action *action, unsigned int row, bool max)
{
	if (max)
	{
		_moveAbove(_mods.at(row), _mods.at(0));
		// don't change the scroll position
		lstModsRefresh(_lstMods->getScroll());
	}
	else
	{
		// calculate target scroll pos
		int curScrollPos = _lstMods->getScroll();
		int targetScrollPos = 0;
		for (size_t i = 0; i < row - 1; ++i)
		{
			targetScrollPos += _lstMods->getNumTextLines(i);
		}
		if (curScrollPos < targetScrollPos)
		{
			int ydiff = _lstMods->getTextHeight(row - 1);
			SDL_WarpMouse(action->getLeftBlackBand() + action->getXMouse(),
				 action->getTopBlackBand() + action->getYMouse() - static_cast<Uint16>(ydiff * action->getYScale()));
		}
		else
		{
			int ydiff = _lstMods->getRowY(row) - _lstMods->getY();
			SDL_WarpMouse(action->getLeftBlackBand() + action->getXMouse(),
				 action->getTopBlackBand() + action->getYMouse() - static_cast<Uint16>(ydiff * action->getYScale()));
			_lstMods->scrollTo(targetScrollPos);
		}

		_moveAbove(_mods.at(row), _mods.at(row - 1));
		lstModsRefresh(_lstMods->getScroll());
	}
	Options::reload = true;
}

void ModListState::lstModsRightArrowClick(Action *action)
{
	unsigned int row = _lstMods->getSelectedRow();
	size_t numMods = _mods.size();
	if (0 >= numMods || INT_MAX < numMods || row >= numMods - 1)
	{
		return;
	}

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		moveModDown(action, row);
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		moveModDown(action, row, true);
	}
}

static void _moveBelow(const std::pair<std::string, bool> &srcMod, const std::pair<std::string, bool> &destMod)
{
	// insert copy of srcMod below destMod
	for (std::vector< std::pair<std::string, bool> >::reverse_iterator i = Options::mods.rbegin(); i != Options::mods.rend(); ++i)
	{
		if (destMod.first == i->first)
		{
			Options::mods.insert(i.base(), srcMod);
			break;
		}
	}

	// remove old copy of srcMod in separate loop since the insert above invalidated the iterator
	for (std::vector< std::pair<std::string, bool> >::iterator i = Options::mods.begin(); i != Options::mods.end(); ++i)
	{
		if (srcMod.first == i->first)
		{
			Options::mods.erase(i);
			break;
		}
	}
}

void ModListState::moveModDown(Action *action, unsigned int row, bool max)
{
	if (max)
	{
		_moveBelow(_mods.at(row), _mods.back());
		// don't change the scroll position
		lstModsRefresh(_lstMods->getScroll());
	}
	else
	{
		// calculate target scroll pos
		int curScrollPos = _lstMods->getScroll();
		int targetScrollPos = 0;
		for (size_t i = 0; i <= row + 1; ++i)
		{
			if (i == row)
			{
				// don't count the current row -- it will be moved down
				continue;
			}
			targetScrollPos += _lstMods->getNumTextLines(i);
		}
		if (curScrollPos + (int)_lstMods->getVisibleRows() > targetScrollPos)
		{
			int ydiff = _lstMods->getTextHeight(row + 1);
			SDL_WarpMouse(action->getLeftBlackBand() + action->getXMouse(),
				 action->getTopBlackBand() + action->getYMouse() + static_cast<Uint16>(ydiff * action->getYScale()));
		}
		else
		{
			int ydiff = _lstMods->getY() + _lstMods->getHeight() - (_lstMods->getRowY(row) + _lstMods->getTextHeight(row));
			SDL_WarpMouse(action->getLeftBlackBand() + action->getXMouse(),
				 action->getTopBlackBand() + action->getYMouse() + static_cast<Uint16>(ydiff * action->getYScale()));
			_lstMods->scrollTo(targetScrollPos - _lstMods->getVisibleRows() + 1);
		}

		_moveBelow(_mods.at(row), _mods.at(row + 1));
		lstModsRefresh(_lstMods->getScroll());
	}
	Options::reload = true;
}

void ModListState::lstModsMousePress(Action *action)
{
	if (Options::changeValueByMouseWheel == 0)
		return;
	unsigned int row = _lstMods->getSelectedRow();
	size_t numMods = _mods.size();
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP &&
		row > 0)
	{
		if (action->getAbsoluteXMouse() >= _lstMods->getArrowsLeftEdge() &&
			action->getAbsoluteXMouse() <= _lstMods->getArrowsRightEdge())
		{
			moveModUp(action, row);
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN &&
			 0 < numMods && INT_MAX >= numMods && row < numMods - 1)
	{
		if (action->getAbsoluteXMouse() >= _lstMods->getArrowsLeftEdge() &&
			action->getAbsoluteXMouse() <= _lstMods->getArrowsRightEdge())
		{
			moveModDown(action, row);
		}
	}
}

/**
 * Restarts game with new mods.
 * @param action Pointer to an action.
 */
void ModListState::btnOkClick(Action *)
{
	Options::save();
	if (Options::reload)
	{
		_game->setState(new StartState);
	}
	else
	{
		_game->popState();
	}
}

/**
 * Ignores mod changes and returns to the previous screen.
 * @param action Pointer to an action.
 */
void ModListState::btnCancelClick(Action *)
{
	Options::reload = false;
	Options::load();
	_game->popState();
}

/**
 * Shows a tooltip for the appropriate button.
 * @param action Pointer to an action.
 */
void ModListState::txtTooltipIn(Action *action)
{
	_currentTooltip = action->getSender()->getTooltip();
	_txtTooltip->setText(tr(_currentTooltip));
}

/**
 * Clears the tooltip text.
 * @param action Pointer to an action.
 */
void ModListState::txtTooltipOut(Action *action)
{
	if (_currentTooltip == action->getSender()->getTooltip())
	{
		_txtTooltip->setText("");
	}
}

}
