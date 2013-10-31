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
#include "OptionsAdvancedState.h"
#include <iostream>
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Options.h"
#include "../Engine/Action.h"
#include <algorithm>

namespace OpenXcom
{

/**
 * Initializes all the elements in the Advanced Options window.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsAdvancedState::OptionsAdvancedState(Game *game, OptionsOrigin origin) : OptionsBaseState(game, origin)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 17, 0, 8);
	_btnOk = new TextButton(148, 16, 8, 176);
	_btnCancel = new TextButton(148, 16, 164, 176);
	_lstOptions = new TextList(268, 104, 20, 30);
	_txtDescription = new Text(280, 32, 20, 142);

	add(_window);
	add(_txtTitle);
	add(_btnOk);
	add(_btnCancel);
	add(_lstOptions);
	add(_txtDescription);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_ADVANCED_OPTIONS"));
	
	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&OptionsAdvancedState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&OptionsAdvancedState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&OptionsAdvancedState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&OptionsAdvancedState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));
	
	_txtDescription->setColor(Palette::blockOffset(8)+10);
	_txtDescription->setWordWrap(true);

	_lstOptions->setAlign(ALIGN_RIGHT, 1);
	_lstOptions->setColumns(2, 220, 48);
	_lstOptions->setColor(Palette::blockOffset(8)+5);

	_settingBoolSet.push_back(std::pair<std::string, bool>("aggressiveRetaliation", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("alienContainmentLimitEnforced", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("canSellLiveAliens", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("allowAutoSellProduction", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("allowBuildingQueue", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("battleAutoEnd", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("battleInstantGrenade", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("battleNotifyDeath", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("battleRangeBasedAccuracy", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("canManufactureMoreItemsPerHour", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("canTransferCraftsWhileAirborne", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("craftLaunchAlways", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("customInitialBase", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("globeSeasons", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("playIntro", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("showFundsOnGeoscape", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("showMoreStatsInInventoryView", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("sneakyAI", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("strafe", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("weaponSelfDestruction", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("researchedItemsWillSpent", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("battleScrollDragInvert", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("allowPsionicCapture", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("psiStrengthEval", false));
	_settingBoolSet.push_back(std::pair<std::string, bool>("anytimePsiTraining", false));

	_boolQuantity = _settingBoolSet.size();
	int sel = 0;
	for (std::vector<std::pair<std::string, bool> >::iterator i = _settingBoolSet.begin(); i != _settingBoolSet.end(); ++i)
	{
		std::string settingName = (*i).first;
		(*i).second = Options::getBool(settingName);
		std::wstring setting =  (*i).second ? tr("STR_YES").c_str() : tr("STR_NO").c_str();
		transform(settingName.begin(), settingName.end(), settingName.begin(), toupper);
		_lstOptions->addRow(2, tr("STR_" + settingName).c_str(), setting.c_str());
		++sel;
	}
	
	_settingIntSet.push_back(std::pair<std::string, int>("battleNewPreviewPath", 0));
	_settingIntSet.push_back(std::pair<std::string, int>("battleExplosionHeight", 0));
	_settingIntSet.push_back(std::pair<std::string, int>("autosave", 0));
	_settingIntSet.push_back(std::pair<std::string, int>("maxFrameSkip", 0));

	for (std::vector<std::pair<std::string, int> >::iterator i = _settingIntSet.begin(); i != _settingIntSet.end(); ++i)
	{
		std::string settingName = (*i).first;
		(*i).second = Options::getInt(settingName);
		std::wstringstream ss;
		if (i->first == "battleNewPreviewPath")
		{
			ss << updatePathString(sel - _settingBoolSet.size()).c_str();
		}
		else
		{
			ss << i->second;
		}
		transform(settingName.begin(), settingName.end(), settingName.begin(), toupper);
		_lstOptions->addRow(2, tr("STR_" + settingName).c_str(), ss.str().c_str());
		++sel;
	}

	_lstOptions->setSelectable(true);
	_lstOptions->setBackground(_window);
	_lstOptions->onMousePress((ActionHandler)&OptionsAdvancedState::lstOptionsPress);
	_lstOptions->onMouseOver((ActionHandler)&OptionsAdvancedState::lstOptionsMouseOver);
	_lstOptions->onMouseOut((ActionHandler)&OptionsAdvancedState::lstOptionsMouseOut);
}

/**
 *
 */
OptionsAdvancedState::~OptionsAdvancedState()
{
	
}

/**
 * Saves the options.
 * @param action Pointer to an action.
 */
void OptionsAdvancedState::btnOkClick(Action *)
{
	for (std::vector<std::pair<std::string, bool> >::iterator i = _settingBoolSet.begin(); i != _settingBoolSet.end(); ++i)
	{
		Options::setBool((*i).first, (*i).second);
	}
	for (std::vector<std::pair<std::string, int> >::iterator i = _settingIntSet.begin(); i != _settingIntSet.end(); ++i)
	{
		Options::setInt((*i).first, (*i).second);
	}
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void OptionsAdvancedState::btnCancelClick(Action *)
{
	_game->popState();
}

void OptionsAdvancedState::lstOptionsPress(Action *action)
{
	if (action->getDetails()->button.button != SDL_BUTTON_LEFT && action->getDetails()->button.button != SDL_BUTTON_RIGHT)
	{
		return;
	}
	size_t sel = _lstOptions->getSelectedRow();
	std::wstring settingText = L"";
	if (sel < _boolQuantity)
	{
		_settingBoolSet.at(sel).second = !_settingBoolSet.at(sel).second;
		settingText = _settingBoolSet.at(sel).second ? tr("STR_YES").c_str() : tr("STR_NO").c_str();
	}
	else // integer variables will need special handling
	{
		size_t intSel = sel - _boolQuantity;
		int increment = 1;
		if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		{
			increment = -1;
		}
		std::wstringstream ss;
		switch (intSel)
		{
		case 0: // pathfinding setting
			_settingIntSet.at(intSel).second += increment;
			if (_settingIntSet.at(intSel).second == 4)
			{
				_settingIntSet.at(intSel).second = 0;
			}
			if (_settingIntSet.at(intSel).second == -1)
			{
				_settingIntSet.at(intSel).second = 3;
			}
			ss << updatePathString(intSel).c_str();
			break;
		case 1: // explosion height
			_settingIntSet.at(intSel).second += increment;
			if (_settingIntSet.at(intSel).second == 4)
			{
				_settingIntSet.at(intSel).second = 0;
			}
			if (_settingIntSet.at(intSel).second == -1)
			{
				_settingIntSet.at(intSel).second = 3;
			}
			ss << _settingIntSet.at(intSel).second;
			break;
		case 2: // autosave
			_settingIntSet.at(intSel).second = ++_settingIntSet.at(intSel).second % 4;
			ss << _settingIntSet.at(intSel).second;
			break;
		case 3: // frame skip
			_settingIntSet.at(intSel).second += increment;
			if (_settingIntSet.at(intSel).second > 10)
			{
				_settingIntSet.at(intSel).second = 0;
			}
			if (_settingIntSet.at(intSel).second < 0)
			{
				_settingIntSet.at(intSel).second = 10;
			}
			ss << _settingIntSet.at(intSel).second;
			break;
		default:
			_settingIntSet.at(intSel).second += increment;
			ss << _settingIntSet.at(intSel).second;
			break;
		}
		settingText = ss.str();
	}
	_lstOptions->setCellText(sel, 1, settingText.c_str());
}

void OptionsAdvancedState::lstOptionsMouseOver(Action *)
{
	size_t sel = _lstOptions->getSelectedRow();
	std::ostringstream ss;
	std::string settingName;
	if (sel < _boolQuantity)
	{
		settingName = _settingBoolSet.at(sel).first;
	}
	else
	{
		settingName = _settingIntSet.at(sel - _boolQuantity).first;
	}

	transform(settingName.begin(), settingName.end(), settingName.begin(), toupper);
	ss << "STR_" << settingName.c_str() << "_DESC";
	_txtDescription->setText(tr(ss.str()).c_str());
}

void OptionsAdvancedState::lstOptionsMouseOut(Action *)
{
	_txtDescription->setText(L"");
}

std::wstring OptionsAdvancedState::updatePathString(int sel)
{
	switch (_settingIntSet.at(sel).second)
	{
	case 0:
		return tr("STR_NONE_UC");
	case 1:
		return tr("STR_ARROWS");
	case 2:
		return tr("STR_TU_COST");
	case 3:
		return tr("STR_FULL");
	default:
		break;
	}
	return L"";
}
}
