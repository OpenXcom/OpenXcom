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
#include "MonthlyCostsState.h"
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
#include "../Savegame/Base.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleCraft.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Monthly Costs screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
MonthlyCostsState::MonthlyCostsState(Base *base) : _base(base)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(300, 20, 10, 170);
	_txtTitle = new Text(310, 17, 5, 12);
	_txtCost = new Text(80, 9, 115, 32);
	_txtQuantity = new Text(55, 9, 195, 32);
	_txtTotal = new Text(60, 9, 249, 32);
	_txtRental = new Text(150, 9, 10, 48);
	_txtSalaries = new Text(150, 9, 10, 80);
	_txtIncome = new Text(150, 9, 10, 136);
	_lstCrafts = new TextList(288, 24, 10, 56);
	_lstSalaries = new TextList(300, 30, 10, 88);
	_lstMaintenance = new TextList(300, 9, 10, 120);
	_lstTotal = new TextList(100, 9, 205, 136);

	// Set palette
	setPalette("PAL_BASESCAPE", 6);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtCost);
	add(_txtQuantity);
	add(_txtTotal);
	add(_txtRental);
	add(_txtSalaries);
	add(_txtIncome);
	add(_lstCrafts);
	add(_lstSalaries);
	add(_lstMaintenance);
	add(_lstTotal);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+1);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&MonthlyCostsState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&MonthlyCostsState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&MonthlyCostsState::btnOkClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(15)+1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_MONTHLY_COSTS"));

	_txtCost->setColor(Palette::blockOffset(15)+1);
	_txtCost->setText(tr("STR_COST_PER_UNIT"));

	_txtQuantity->setColor(Palette::blockOffset(15)+1);
	_txtQuantity->setText(tr("STR_QUANTITY"));

	_txtTotal->setColor(Palette::blockOffset(15)+1);
	_txtTotal->setText(tr("STR_TOTAL"));

	_txtRental->setColor(Palette::blockOffset(15)+1);
	_txtRental->setText(tr("STR_CRAFT_RENTAL"));

	_txtSalaries->setColor(Palette::blockOffset(15)+1);
	_txtSalaries->setText(tr("STR_SALARIES"));

	_txtIncome->setColor(Palette::blockOffset(13)+10);
	std::wostringstream ss;
	ss << tr("STR_INCOME") << L"=" << Text::formatFunding(_game->getSavedGame()->getCountryFunding());
	_txtIncome->setText(ss.str());

	_lstCrafts->setColor(Palette::blockOffset(13)+10);
	_lstCrafts->setColumns(4, 125, 70, 44, 60);
	_lstCrafts->setDot(true);

	const std::vector<std::string> &crafts = _game->getRuleset()->getCraftsList();
	for (std::vector<std::string>::const_iterator i = crafts.begin(); i != crafts.end(); ++i)
	{
		RuleCraft *craft = _game->getRuleset()->getCraft(*i);
		if (craft->getRentCost() != 0 && _game->getSavedGame()->isResearched(craft->getRequirements()))
		{
			std::wostringstream ss2;
			ss2 << _base->getCraftCount((*i));
			_lstCrafts->addRow(4, tr(*i).c_str(), Text::formatFunding(craft->getRentCost()).c_str(), ss2.str().c_str(), Text::formatFunding(_base->getCraftCount(*i) * craft->getRentCost()).c_str());
		}
	}

	_lstSalaries->setColor(Palette::blockOffset(13)+10);
	_lstSalaries->setColumns(4, 125, 70, 44, 60);
	_lstSalaries->setDot(true);

	std::wostringstream ss4;
	ss4 << _base->getSoldiers()->size();
	_lstSalaries->addRow(4, tr("STR_SOLDIERS").c_str(), Text::formatFunding(_game->getRuleset()->getSoldierCost()).c_str(), ss4.str().c_str(), Text::formatFunding(_base->getSoldiers()->size() * _game->getRuleset()->getSoldierCost()).c_str());
	std::wostringstream ss5;
	ss5 << _base->getTotalEngineers();
	_lstSalaries->addRow(4, tr("STR_ENGINEERS").c_str(), Text::formatFunding(_game->getRuleset()->getEngineerCost()).c_str(), ss5.str().c_str(), Text::formatFunding(_base->getTotalEngineers() * _game->getRuleset()->getEngineerCost()).c_str());
	std::wostringstream ss6;
	ss6 << _base->getTotalScientists();
	_lstSalaries->addRow(4, tr("STR_SCIENTISTS").c_str(), Text::formatFunding(_game->getRuleset()->getScientistCost()).c_str(), ss6.str().c_str(), Text::formatFunding(_base->getTotalScientists() * _game->getRuleset()->getScientistCost()).c_str());

	_lstMaintenance->setColor(Palette::blockOffset(13)+10);
	_lstMaintenance->setColumns(2, 239, 60);
	_lstMaintenance->setDot(true);
	_lstMaintenance->addRow(2, tr("STR_BASE_MAINTENANCE").c_str(), Text::formatFunding(_base->getFacilityMaintenance()).c_str());
	_lstMaintenance->setCellColor(0, 0, Palette::blockOffset(15)+1);

	_lstTotal->setColor(Palette::blockOffset(13));
	_lstTotal->setColumns(2, 44, 55);
	_lstTotal->setDot(true);
	_lstTotal->addRow(2, tr("STR_TOTAL").c_str(), Text::formatFunding(_base->getMonthlyMaintenace()).c_str());
}

/**
 *
 */
MonthlyCostsState::~MonthlyCostsState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void MonthlyCostsState::btnOkClick(Action *)
{
	_game->popState();
}

}
