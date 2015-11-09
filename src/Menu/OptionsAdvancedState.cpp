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
#include "OptionsAdvancedState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
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
OptionsAdvancedState::OptionsAdvancedState(OptionsOrigin origin) : OptionsBaseState(origin)
{
	setCategory(_btnAdvanced);

	// Create objects
	_lstOptions = new TextList(200, 136, 94, 8);
	
	if (origin != OPT_BATTLESCAPE)
	{
		add(_lstOptions, "optionLists", "advancedMenu");
	}
	else
	{
		add(_lstOptions, "optionLists", "battlescape");
	}
	centerAllSurfaces();

	// how much room do we need for YES/NO
	Text text = Text(100, 9, 0, 0);
	text.initText(_game->getMod()->getFont("FONT_BIG"), _game->getMod()->getFont("FONT_SMALL"), _game->getLanguage());
	text.setText(tr("STR_YES"));
	int yes = text.getTextWidth();
	text.setText(tr("STR_NO"));
	int no = text.getTextWidth();

	int rightcol = std::max(yes, no) + 2;
	int leftcol = _lstOptions->getWidth() - rightcol;

	// Set up objects
	_lstOptions->setAlign(ALIGN_RIGHT, 1);
	_lstOptions->setColumns(2, leftcol, rightcol);
	_lstOptions->setWordWrap(true);
	_lstOptions->setSelectable(true);
	_lstOptions->setBackground(_window);
	_lstOptions->onMouseClick((ActionHandler)&OptionsAdvancedState::lstOptionsClick, 0);
	_lstOptions->onMouseOver((ActionHandler)&OptionsAdvancedState::lstOptionsMouseOver);
	_lstOptions->onMouseOut((ActionHandler)&OptionsAdvancedState::lstOptionsMouseOut);

	_colorGroup = _lstOptions->getSecondaryColor();

	const std::vector<OptionInfo> &options = Options::getOptionInfo();
	for (std::vector<OptionInfo>::const_iterator i = options.begin(); i != options.end(); ++i)
	{
		if (i->type() != OPTION_KEY && !i->description().empty())
		{
			if (i->category() == "STR_GENERAL")
			{
				_settingsGeneral.push_back(*i);
			}
			else if (i->category() == "STR_GEOSCAPE")
			{
				_settingsGeo.push_back(*i);
			}
			else if (i->category() == "STR_BATTLESCAPE")
			{
				_settingsBattle.push_back(*i);
			}
		}
	}
}

/**
 *
 */
OptionsAdvancedState::~OptionsAdvancedState()
{
	
}

/**
 * Fills the settings list based on category.
 */
void OptionsAdvancedState::init()
{
	OptionsBaseState::init();
	_lstOptions->clearList();
	_lstOptions->addRow(2, tr("STR_GENERAL").c_str(), L"");
	_lstOptions->setCellColor(0, 0, _colorGroup);
	addSettings(_settingsGeneral);
	_lstOptions->addRow(2, L"", L"");
	_lstOptions->addRow(2, tr("STR_GEOSCAPE").c_str(), L"");
	_lstOptions->setCellColor(_settingsGeneral.size() + 2, 0, _colorGroup);
	addSettings(_settingsGeo);
	_lstOptions->addRow(2, L"", L"");
	_lstOptions->addRow(2, tr("STR_BATTLESCAPE").c_str(), L"");
	_lstOptions->setCellColor(_settingsGeneral.size() + 2 + _settingsGeo.size() + 2, 0, _colorGroup);
	addSettings(_settingsBattle);
}

/**
 * Adds a bunch of settings to the list.
 * @param settings List of settings.
 */
void OptionsAdvancedState::addSettings(const std::vector<OptionInfo> &settings)
{
	for (std::vector<OptionInfo>::const_iterator i = settings.begin(); i != settings.end(); ++i)
	{
		std::wstring name = tr(i->description());
		std::wstring value;
		if (i->type() == OPTION_BOOL)
		{
			value = *i->asBool() ? tr("STR_YES") : tr("STR_NO");
		}
		else if (i->type() == OPTION_INT)
		{
			std::wostringstream ss;
			ss << *i->asInt();
			value = ss.str();
		}
		_lstOptions->addRow(2, name.c_str(), value.c_str());
	}
}

/**
 * Gets the currently selected setting.
 * @param sel Selected row.
 * @return Pointer to option, NULL if none selected.
 */
OptionInfo *OptionsAdvancedState::getSetting(size_t sel)
{
	if (sel > 0 &&
		sel <= _settingsGeneral.size())
	{
		return &_settingsGeneral[sel - 1];
	}
	else if (sel > _settingsGeneral.size() + 2 &&
			 sel <= _settingsGeneral.size() + 2 + _settingsGeo.size())
	{
		return &_settingsGeo[sel - 1 - _settingsGeneral.size() - 2];
	}
	else if (sel > _settingsGeneral.size() + 2 + _settingsGeo.size() + 2 &&
			 sel <= _settingsGeneral.size() + 2 + _settingsGeo.size() + 2 + _settingsBattle.size())
	{
		return &_settingsBattle[sel - 1 - _settingsGeneral.size() - 2 - _settingsGeo.size() - 2];
	}
	else
	{
		return 0;
	}
}

/**
 * Changes the clicked setting.
 * @param action Pointer to an action.
 */
void OptionsAdvancedState::lstOptionsClick(Action *action)
{
	Uint8 button = action->getDetails()->button.button;
	if (button != SDL_BUTTON_LEFT && button != SDL_BUTTON_RIGHT)
	{
		return;
	}
	size_t sel = _lstOptions->getSelectedRow();
	OptionInfo *setting = getSetting(sel);
	if (!setting) return;

	std::wstring settingText;
	if (setting->type() == OPTION_BOOL)
	{
		bool *b = setting->asBool();
		*b = !*b;
		settingText = *b ? tr("STR_YES") : tr("STR_NO");
	}
	else if (setting->type() == OPTION_INT) // integer variables will need special handling
	{
		int *i = setting->asInt();

		int increment = (button == SDL_BUTTON_LEFT) ? 1 : -1; // left-click increases, right-click decreases
		if (i == &Options::changeValueByMouseWheel || i == &Options::FPS || i == &Options::FPSInactive)
		{
			increment *= 10;
		}
		*i += increment;

		int min = 0, max = 0;
		if (i == &Options::battleExplosionHeight)
		{
			min = 0;
			max = 3;
		}
		else if (i == &Options::changeValueByMouseWheel)
		{
			min = 0;
			max = 50;
		}
		else if (i == &Options::FPS)
		{
			min = 0;
			max = 120;
		}
		else if (i == &Options::FPSInactive) {
			min = 10;
			max = 120;
		}
		else if (i == &Options::mousewheelSpeed)
		{
			min = 1;
			max = 7;
		}
		else if (i == &Options::autosaveFrequency)
		{
			min = 1;
			max = 5;
		}

		if (*i < min)
		{
			*i = max;
		}
		else if (*i > max)
		{
			*i = min;
		}

		std::wostringstream ss;
		ss << *i;
		settingText = ss.str();
	}
	_lstOptions->setCellText(sel, 1, settingText.c_str());
}

void OptionsAdvancedState::lstOptionsMouseOver(Action *)
{
	size_t sel = _lstOptions->getSelectedRow();
	OptionInfo *setting = getSetting(sel);
	std::wstring desc;
	if (setting)
	{
		desc = tr(setting->description() + "_DESC");
	}
	_txtTooltip->setText(desc.c_str());
}

void OptionsAdvancedState::lstOptionsMouseOut(Action *)
{
	_txtTooltip->setText(L"");
}

}
