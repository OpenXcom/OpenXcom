/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"
#include "../Engine/InteractiveSurface.h"
#include "MainMenuState.h"
#include "../Engine/Action.h"
#include <algorithm>

namespace OpenXcom
{

/**
 * Initializes all the elements in the Advanced Options window.
 * @param game Pointer to the core game.
 */
AdvancedOptionsState::AdvancedOptionsState(Game *game) : State(game), _sel(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 16, 0, 8);
	_txtSubTitle = new Text(320, 8, 0, 24);
	_btnOk = new TextButton(100, 16, 8, 176);
	_btnCancel = new TextButton(100, 16, 110, 176);
	_btnDefault = new TextButton(100, 16, 212, 176);
	_lstOptions = new TextList(180, 96, 75, 34);
	_txtDescription = new Text(280, 40, 20, 134);

	add(_window);
	add(_txtTitle);
	add(_btnOk);
	add(_btnCancel);
	add(_btnDefault);
	add(_lstOptions);
	add(_txtSubTitle);
	add(_txtDescription);

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
	_btnOk->onKeyboardPress((ActionHandler)&AdvancedOptionsState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&AdvancedOptionsState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&AdvancedOptionsState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_btnDefault->setColor(Palette::blockOffset(8)+5);
	_btnDefault->setText(_game->getLanguage()->getString("STR_RESTORE_DEFAULTS"));
	_btnDefault->onMouseClick((ActionHandler)&AdvancedOptionsState::btnDefaultClick);
	
	_txtSubTitle->setColor(Palette::blockOffset(8)+10);
	_txtSubTitle->setAlign(ALIGN_CENTER);
	_txtSubTitle->setText(_game->getLanguage()->getString("STR_DESC_TOGGLE"));

	_txtDescription->setColor(Palette::blockOffset(8)+10);
	_txtDescription->setAlign(ALIGN_CENTER);
	_txtDescription->setWordWrap(true);


	_lstOptions->setColumns(2, 145, 35);
	_lstOptions->setColor(Palette::blockOffset(8)+5);


	_settingSet.push_back("aggressiveRetaliation");
	_settingSet.push_back("alienContainmentHasUpperLimit");
	_settingSet.push_back("canSellLiveAliens");
	_settingSet.push_back("allowAutoSellProduction");
	_settingSet.push_back("allowBuildingQueue");
	_settingSet.push_back("battleAutoEnd");
	_settingSet.push_back("battleInstantGrenade");
	_settingSet.push_back("battleNotifyDeath");
	_settingSet.push_back("battlePreviewPath");
	_settingSet.push_back("battleRangeBasedAccuracy");
	_settingSet.push_back("canManufactureMoreItemsPerHour");
	_settingSet.push_back("canTransferCraftsInAirborne");
	_settingSet.push_back("craftLaunchAlways");
	_settingSet.push_back("customInitialBase");
	_settingSet.push_back("globeSeasons");
	_settingSet.push_back("playIntro");
	_settingSet.push_back("showFundsOnGeoscape");
	_settingSet.push_back("showMoreStatsInInventoryView");
	_settingSet.push_back("sneakyAI");
	_settingSet.push_back("strafe");
	_settingSet.push_back("battleScrollDragInvert");
	_settingSet.push_back("battleExplosionHeight");


	for (size_t i = 0; i != 21; ++i)
	{
		std::wstring setting = Options::getBool(_settingSet[i]) ? _game->getLanguage()->getString("STR_YES").c_str() : _game->getLanguage()->getString("STR_NO").c_str();
		std::string s = _settingSet[i];
		transform(s.begin(), s.end(), s.begin(), toupper);
		_lstOptions->addRow(2, _game->getLanguage()->getString("STR_" + s).c_str(), setting.c_str());
	}

	std::wstringstream ss;
	ss << Options::getInt("battleExplosionHeight");
	_lstOptions->addRow(2, _game->getLanguage()->getString("STR_BATTLEEXPLOSIONHEIGHT").c_str(), ss.str().c_str());

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
	Options::save();
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
	Options::createDefault();
	Options::setString("language", "English");
	Options::save();
	_game->popState();
}

void AdvancedOptionsState::lstOptionsClick(Action *action)
{
	_sel = _lstOptions->getSelectedRow();
	bool newSetting = false;
	std::wstring setting = L"";
	if (_sel < 21)
	{
		newSetting = !Options::getBool(_settingSet[_sel]);
		Options::setBool(_settingSet[_sel], newSetting);
		setting = newSetting ? _game->getLanguage()->getString("STR_YES").c_str() : _game->getLanguage()->getString("STR_NO").c_str();
	}
	else if (_sel == 21)
	{
		int radius = 1 + Options::getInt("battleExplosionHeight");
		if (radius == 4)
		{
			radius = 0;
		}
		Options::setInt("battleExplosionHeight", radius);
		std::wstringstream ss;
		ss << radius;
		setting = ss.str();
	}
	_lstOptions->setCellText(_sel, 1, setting.c_str());
}

void AdvancedOptionsState::lstOptionsMouseOver(Action *)
{
	_sel = _lstOptions->getSelectedRow();
	std::stringstream ss;
	std::string s = _settingSet[_sel];
	transform(s.begin(), s.end(), s.begin(), toupper);
	ss << "STR_" << s.c_str() << "_DESC";

	_txtDescription->setText(_game->getLanguage()->getString(ss.str()).c_str());
}

void AdvancedOptionsState::lstOptionsMouseOut(Action *)
{
	_txtDescription->setText(L"");
}

}
