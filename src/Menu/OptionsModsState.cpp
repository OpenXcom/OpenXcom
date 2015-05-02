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
#include "OptionsModsState.h"
#include <algorithm>
#include <limits>
#include "../Engine/Game.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/FileMap.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Interface/Window.h"
#include "../Interface/TextList.h"
#include "../Interface/Text.h"
#include "../Engine/Options.h"
#include "../Engine/Action.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Mod Options window.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsModsState::OptionsModsState(OptionsOrigin origin) : OptionsBaseState(origin)
{
	setCategory(_btnMods);

	// Create objects
	_txtMaster = new Text(114, 9, 94, 8);
	_cbxMasters = new ComboBox(this, 218, 16, 94, 18);
	_lstMods = new TextList(200, 104, 94, 40);

	add(_txtMaster, "text", "modsMenu");
	add(_lstMods, "optionLists", "modsMenu");
	add(_cbxMasters, "button", "modsMenu");

	centerAllSurfaces();

	// how much room do we need for YES/NO
	Text text = Text(100, 9, 0, 0);
	text.initText(_game->getResourcePack()->getFont("FONT_BIG"), _game->getResourcePack()->getFont("FONT_SMALL"), _game->getLanguage());
	text.setText(tr("STR_YES"));
	int yes = text.getTextWidth();
	text.setText(tr("STR_NO"));
	int no = text.getTextWidth();

	int rightcol = std::max(yes, no) + 2;
	int arrowCol = 25;
	int leftcol = _lstMods->getWidth() - (rightcol + arrowCol);

	// Set up objects
	_txtMaster->setText(tr("STR_GAME_TYPE"));

	// scan for masters
	const std::map<std::string, ModInfo> &modInfos(Options::getModInfos());
	size_t curMasterIdx = 0;
	std::vector<std::wstring> masterNames;
	for (std::vector< std::pair<std::string, bool> >::const_iterator i = Options::mods.begin(); i != Options::mods.end(); ++i)
	{
		std::string modId = i->first;
		ModInfo modInfo = modInfos.find(modId)->second;
		if (!modInfo.isMaster())
		{
			continue;
		}

		if (i->second)
		{
			_curMasterId = modId;
		}
		else if (_curMasterId.empty())
		{
			++curMasterIdx;
		}
		_masters.push_back(&modInfos.at(modId));
		masterNames.push_back(Language::utf8ToWstr(modInfo.getName()));
	}

	_cbxMasters->setOptions(masterNames);
	_cbxMasters->setSelected(curMasterIdx);
	_cbxMasters->onChange((ActionHandler)&OptionsModsState::cbxMasterChange);
	_cbxMasters->onMouseIn((ActionHandler)&OptionsModsState::txtTooltipIn);
	_cbxMasters->onMouseOut((ActionHandler)&OptionsModsState::txtTooltipOut);
	_cbxMasters->onMouseOver((ActionHandler)&OptionsModsState::cbxMasterHover);
	_cbxMasters->onListMouseIn((ActionHandler)&OptionsModsState::txtTooltipIn);
	_cbxMasters->onListMouseOut((ActionHandler)&OptionsModsState::txtTooltipOut);
	_cbxMasters->onListMouseOver((ActionHandler)&OptionsModsState::cbxMasterHover);

	_lstMods->setArrowColumn(leftcol + 1, ARROW_VERTICAL);
	_lstMods->setColumns(3, leftcol, arrowCol, rightcol);
	_lstMods->setAlign(ALIGN_RIGHT, 1);
	_lstMods->setSelectable(true);
	_lstMods->setBackground(_window);
	_lstMods->setWordWrap(true);
	_lstMods->onMouseClick((ActionHandler)&OptionsModsState::lstModsClick);
	_lstMods->onLeftArrowClick((ActionHandler)&OptionsModsState::lstModsLeftArrowClick);
	_lstMods->onRightArrowClick((ActionHandler)&OptionsModsState::lstModsRightArrowClick);
	_lstMods->onMousePress((ActionHandler)&OptionsModsState::lstModsMousePress);
	_lstMods->onMouseIn((ActionHandler)&OptionsModsState::txtTooltipIn);
	_lstMods->onMouseOut((ActionHandler)&OptionsModsState::txtTooltipOut);
	_lstMods->onMouseOver((ActionHandler)&OptionsModsState::lstModsHover);
	lstModsRefresh(0);
}

OptionsModsState::~OptionsModsState()
{

}

std::wstring OptionsModsState::makeTooltip(const ModInfo &modInfo)
{
	return tr("STR_MODS_TOOLTIP").arg(modInfo.getVersion()).arg(modInfo.getAuthor()).arg(modInfo.getDescription());
}

void OptionsModsState::cbxMasterHover(Action *)
{
	_txtTooltip->setText(makeTooltip(*_masters[_cbxMasters->getHoveredListIdx()]));
}

void OptionsModsState::cbxMasterChange(Action *)
{
	std::string masterId = _masters[_cbxMasters->getSelected()]->getId();
	for (size_t i = 0; i < Options::mods.size(); ++i)
	{
		if (masterId == Options::mods[i].first)
		{
			Options::mods[i].second = true;
		}
		else if (_curMasterId == Options::mods[i].first)
		{
			Options::mods[i].second = false;
		}
	}
	Options::reload = true;

	_curMasterId = masterId;
	lstModsRefresh(0);
}

void OptionsModsState::lstModsRefresh(size_t scrollLoc)
{
	_lstMods->clearList();
	_mods.clear();

	// only show mods that work with the current master
	for (std::vector< std::pair<std::string, bool> >::iterator i = Options::mods.begin(); i != Options::mods.end(); ++i)
	{
		ModInfo modInfo = Options::getModInfos().find(i->first)->second;
		if (modInfo.isMaster() || (!modInfo.getMaster().empty() && modInfo.getMaster() != _curMasterId))
		{
			continue;
		}

		std::string  modId   = modInfo.getId();
		std::wstring modName = Language::fsToWstr(modInfo.getName());
		_lstMods->addRow(3, modName.c_str(), L"", (i->second ? tr("STR_YES").c_str() : tr("STR_NO").c_str()));
		_mods.push_back(*i);
	}

	_lstMods->scrollTo(scrollLoc);
}

void OptionsModsState::lstModsHover(Action *)
{
	size_t selectedRow = _lstMods->getSelectedRow();
	if ((unsigned int)-1 != selectedRow)
	{
		_txtTooltip->setText(makeTooltip(Options::getModInfos().at(_mods[selectedRow].first)));

	}
}

void OptionsModsState::lstModsClick(Action *action)
{
	if (action->getAbsoluteXMouse() >= _lstMods->getArrowsLeftEdge() &&
		action->getAbsoluteXMouse() <= _lstMods->getArrowsRightEdge())
	{
		// don't count an arrow click as a mod enable toggle
		return;
	}

	std::pair<std::string, bool> &mod(_mods.at(_lstMods->getSelectedRow()));
	for (size_t i = 0; i < Options::mods.size(); ++i)
	{
		if (mod.first != Options::mods[i].first)
		{
			continue;
		}

		mod.second = ! mod.second;
		Options::mods[i].second = mod.second;
		_lstMods->setCellText(_lstMods->getSelectedRow(), 2, (mod.second ? tr("STR_YES").c_str() : tr("STR_NO").c_str()));

		break;
	}
	Options::reload = true;
}

void OptionsModsState::lstModsLeftArrowClick(Action *action)
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

void OptionsModsState::moveModUp(Action *action, unsigned int row, bool max)
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

void OptionsModsState::lstModsRightArrowClick(Action *action)
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

void OptionsModsState::moveModDown(Action *action, unsigned int row, bool max)
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
		if (curScrollPos + _lstMods->getVisibleRows() > targetScrollPos)
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

void OptionsModsState::lstModsMousePress(Action *action)
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

}
