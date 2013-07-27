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
#include "AdvancedOptionsState.h"
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
 */
AdvancedOptionsState::AdvancedOptionsState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 16, 0, 8);
	_btnOk = new TextButton(100, 16, 8, 176);
	_btnCancel = new TextButton(100, 16, 110, 176);
	_btnDefault = new TextButton(100, 16, 212, 176);
	_lstOptions = new TextList(268, 104, 20, 30);
	_txtDescription = new Text(280, 32, 20, 142);

	add(_window);
	add(_txtTitle);
	add(_btnOk);
	add(_btnCancel);
	add(_btnDefault);
	add(_lstOptions);
	add(_txtDescription);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString("STR_ADVANCED_OPTIONS"));
	
	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&AdvancedOptionsState::btnOkClick);
	//_btnOk->onKeyboardPress((ActionHandler)&AdvancedOptionsState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&AdvancedOptionsState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&AdvancedOptionsState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_btnDefault->setColor(Palette::blockOffset(8)+5);
	_btnDefault->setText(_game->getLanguage()->getString("STR_RESTORE_DEFAULTS"));
	_btnDefault->onMouseClick((ActionHandler)&AdvancedOptionsState::btnDefaultClick);
	
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
	_settingBoolSet.push_back(std::pair<std::string, bool>("anytimePsiTraining", false));

	_boolQuantity = _settingBoolSet.size();
	int sel = 0;
	for (std::vector<std::pair<std::string, bool> >::iterator i = _settingBoolSet.begin(); i != _settingBoolSet.end(); ++i)
	{
		std::string settingName = (*i).first;
		(*i).second = Options::getBool(settingName);
		std::wstring setting =  (*i).second ? _game->getLanguage()->getString("STR_YES").c_str() : _game->getLanguage()->getString("STR_NO").c_str();
		transform(settingName.begin(), settingName.end(), settingName.begin(), toupper);
		_lstOptions->addRow(2, _game->getLanguage()->getString("STR_" + settingName).c_str(), setting.c_str());
		++sel;
	}
	
	_settingIntSet.push_back(std::pair<std::string, int>("battleNewPreviewPath", 0));
	_settingIntSet.push_back(std::pair<std::string, int>("battleExplosionHeight", 0));
	_settingIntSet.push_back(std::pair<std::string, int>("autosave", 0));

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
			ss << (*i).second;
		}
		transform(settingName.begin(), settingName.end(), settingName.begin(), toupper);
		_lstOptions->addRow(2, _game->getLanguage()->getString("STR_" + settingName).c_str(), ss.str().c_str());
		++sel;
	}

	_lstOptions->setSelectable(true);
	_lstOptions->setBackground(_window);
	_lstOptions->onMouseClick((ActionHandler)&AdvancedOptionsState::lstOptionsClick);
	_lstOptions->onMouseOver((ActionHandler)&AdvancedOptionsState::lstOptionsMouseOver);
	_lstOptions->onMouseOut((ActionHandler)&AdvancedOptionsState::lstOptionsMouseOut);
}

/**
 *
 */
AdvancedOptionsState::~AdvancedOptionsState()
{
	
}

/**
 * Saves the options.
 * @param action Pointer to an action.
 */
void AdvancedOptionsState::btnOkClick(Action *)
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
void AdvancedOptionsState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Restores the Options to default settings.
 * @param action Pointer to an action.
 */
void AdvancedOptionsState::btnDefaultClick(Action *)
{
	int sel = 0;
	for (std::vector<std::pair<std::string, bool> >::iterator i = _settingBoolSet.begin(); i != _settingBoolSet.end(); ++i)
	{
		if (i->first == "playIntro")
		{
			i->second = true;
			_lstOptions->setCellText(sel, 1, _game->getLanguage()->getString("STR_YES").c_str());
		}
		else
		{
			i->second = false;
			_lstOptions->setCellText(sel, 1, _game->getLanguage()->getString("STR_NO").c_str());
		}
		++sel;
	}
	
	for (std::vector<std::pair<std::string, int> >::iterator i = _settingIntSet.begin(); i != _settingIntSet.end(); ++i)
	{
		i->second = 0;
		if (i->first == "battleNewPreviewPath")
		{
			_lstOptions->setCellText(sel, 1, updatePathString(sel - _settingBoolSet.size()).c_str());
		}
		else
		{
			_lstOptions->setCellText(sel, 1, L"0");
		}
		++sel;
	}
}

void AdvancedOptionsState::lstOptionsClick(Action *)
{
	size_t sel = _lstOptions->getSelectedRow();
	std::wstring settingText = L"";
	if (sel < _boolQuantity)
	{
		_settingBoolSet.at(sel).second = !_settingBoolSet.at(sel).second;
		settingText = _settingBoolSet.at(sel).second ? _game->getLanguage()->getString("STR_YES").c_str() : _game->getLanguage()->getString("STR_NO").c_str();
	}
	else // integer variables will need special handling
	{
		size_t intSel = sel - _boolQuantity;
		int increment = 1;
		std::wstringstream ss;
		// this is purely future-proofing.
		switch (intSel)
		{
		case 0: // pathfinding setting
			if (_settingIntSet.at(intSel).second == 3)
			{
				increment = -3;
			}
			_settingIntSet.at(intSel).second += increment;
			ss << updatePathString(intSel).c_str();
			break;
		case 1: // explosion height
			if (_settingIntSet.at(intSel).second == 3)
			{
				increment = -3;
			}
			_settingIntSet.at(intSel).second += increment;
			ss << _settingIntSet.at(intSel).second;
			break;
		case 2: // autosave
			_settingIntSet.at(intSel).second = ++_settingIntSet.at(intSel).second % 4;
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

void AdvancedOptionsState::lstOptionsMouseOver(Action *)
{
	size_t sel = _lstOptions->getSelectedRow();
	std::stringstream ss;
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
	_txtDescription->setText(_game->getLanguage()->getString(ss.str()).c_str());
}

void AdvancedOptionsState::lstOptionsMouseOut(Action *)
{
	_txtDescription->setText(L"");
}

std::wstring AdvancedOptionsState::updatePathString(int sel)
{
	switch (_settingIntSet.at(sel).second)
	{
	case 0:
		return _game->getLanguage()->getString("STR_NONE_UC");
	case 1:
		return _game->getLanguage()->getString("STR_ARROWS");
	case 2:
		return _game->getLanguage()->getString("STR_TU_COST");
	case 3:
		return _game->getLanguage()->getString("STR_FULL");
	default:
		break;
	}
	return L"";
}
}
