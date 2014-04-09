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
	setCategory(_btnAdvanced);

	// Create objects
	_lstOptions = new TextList(200, 136, 94, 8);
	
	add(_lstOptions);

	centerAllSurfaces();

	// how much room do we need for YES/NO
	Text text = Text(100, 9, 0, 0);
	text.initText(_game->getResourcePack()->getFont("FONT_BIG"), _game->getResourcePack()->getFont("FONT_SMALL"), _game->getLanguage());
	text.setText(tr("STR_YES"));
	int yes = text.getTextWidth();
	text.setText(tr("STR_NO"));
	int no = text.getTextWidth();

	int rightcol = std::max(yes, no) + 2;
	int leftcol = _lstOptions->getWidth() - rightcol;

	// Set up objects
	_lstOptions->setAlign(ALIGN_RIGHT, 1);
	_lstOptions->setColumns(2, leftcol, rightcol);
	_lstOptions->setColor(Palette::blockOffset(8)+10);
	_lstOptions->setArrowColor(Palette::blockOffset(8)+5);
	_lstOptions->setWordWrap(true);

	_settingBoolSet.push_back(std::pair<std::string, bool*>("aggressiveRetaliation", &Options::aggressiveRetaliation));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("alienContainmentLimitEnforced", &Options::alienContainmentLimitEnforced));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("canSellLiveAliens", &Options::canSellLiveAliens));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("allowAutoSellProduction", &Options::allowAutoSellProduction));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("allowBuildingQueue", &Options::allowBuildingQueue));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("battleAutoEnd", &Options::battleAutoEnd));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("battleInstantGrenade", &Options::battleInstantGrenade));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("battleUFOExtenderAccuracy", &Options::battleUFOExtenderAccuracy));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("canManufactureMoreItemsPerHour", &Options::canManufactureMoreItemsPerHour));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("canTransferCraftsWhileAirborne", &Options::canTransferCraftsWhileAirborne));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("craftLaunchAlways", &Options::craftLaunchAlways));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("customInitialBase", &Options::customInitialBase));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("globeSeasons", &Options::globeSeasons));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("playIntro", &Options::playIntro));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("disableAutoEquip", &Options::disableAutoEquip));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("showMoreStatsInInventoryView", &Options::showMoreStatsInInventoryView));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("sneakyAI", &Options::sneakyAI));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("strafe", &Options::strafe));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("weaponSelfDestruction", &Options::weaponSelfDestruction));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("spendResearchedItems", &Options::spendResearchedItems));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("battleScrollDragInvert", &Options::battleScrollDragInvert));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("allowPsionicCapture", &Options::allowPsionicCapture));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("psiStrengthEval", &Options::psiStrengthEval));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("anytimePsiTraining", &Options::anytimePsiTraining));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("skipNextTurnScreen", &Options::skipNextTurnScreen));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("TFTDDamage", &Options::TFTDDamage));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("battleSmoothCamera", &Options::battleSmoothCamera));
	_settingBoolSet.push_back(std::pair<std::string, bool*>("battleConfirmFireMode", &Options::battleConfirmFireMode));

	_boolQuantity = _settingBoolSet.size();
	int sel = 0;
	for (std::vector< std::pair<std::string, bool*> >::iterator i = _settingBoolSet.begin(); i != _settingBoolSet.end(); ++i)
	{
		std::string settingName = i->first;
		std::wstring setting =  *i->second ? tr("STR_YES").c_str() : tr("STR_NO").c_str();
		transform(settingName.begin(), settingName.end(), settingName.begin(), toupper);
		_lstOptions->addRow(2, tr("STR_" + settingName).c_str(), setting.c_str());
		++sel;
	}
	
	_settingIntSet.push_back(std::pair<std::string, int*>("battleExplosionHeight", &Options::battleExplosionHeight));
	_settingIntSet.push_back(std::pair<std::string, int*>("autosave", &Options::autosave));
	_settingIntSet.push_back(std::pair<std::string, int*>("maxFrameSkip", &Options::maxFrameSkip));

	for (std::vector<std::pair<std::string, int*> >::iterator i = _settingIntSet.begin(); i != _settingIntSet.end(); ++i)
	{
		std::string settingName = i->first;
		std::wostringstream ss;
		ss << *i->second;
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
		*_settingBoolSet.at(sel).second = !*_settingBoolSet.at(sel).second;
		settingText = *_settingBoolSet.at(sel).second ? tr("STR_YES").c_str() : tr("STR_NO").c_str();
	}
	else // integer variables will need special handling
	{
		size_t intSel = sel - _boolQuantity;
		int increment = 1;
		if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		{
			increment = -1;
		}
		std::wostringstream ss;
		switch (intSel)
		{
		case 0: // explosion height
			*_settingIntSet.at(intSel).second += increment;
			if (*_settingIntSet.at(intSel).second == 4)
			{
				*_settingIntSet.at(intSel).second = 0;
			}
			if (*_settingIntSet.at(intSel).second == -1)
			{
				*_settingIntSet.at(intSel).second = 3;
			}
			ss << *_settingIntSet.at(intSel).second;
			break;
		case 1: // autosave
			*_settingIntSet.at(intSel).second = ++*_settingIntSet.at(intSel).second % 4;
			ss << *_settingIntSet.at(intSel).second;
			break;
		case 2: // frame skip
			*_settingIntSet.at(intSel).second += increment;
			if (*_settingIntSet.at(intSel).second > 10)
			{
				*_settingIntSet.at(intSel).second = 0;
			}
			if (*_settingIntSet.at(intSel).second < 0)
			{
				*_settingIntSet.at(intSel).second = 10;
			}
			ss << *_settingIntSet.at(intSel).second;
			break;
		default:
			*_settingIntSet.at(intSel).second += increment;
			ss << *_settingIntSet.at(intSel).second;
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
	_txtTooltip->setText(tr(ss.str()).c_str());
}

void OptionsAdvancedState::lstOptionsMouseOut(Action *)
{
	_txtTooltip->setText(L"");
}
}
