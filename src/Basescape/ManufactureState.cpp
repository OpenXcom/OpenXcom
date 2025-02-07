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
#include "ManufactureState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Unicode.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "../Savegame/SavedGame.h"
#include "../Mod/RuleManufacture.h"
#include "../Savegame/Production.h"
#include "NewManufactureListState.h"
#include "ManufactureInfoState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Manufacture screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
ManufactureState::ManufactureState(Base *base) : _base(base)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnNew = new TextButton(148, 16, 8, 176);
	_btnOk = new TextButton(148, 16, 164, 176);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtAvailable = new Text(150, 9, 8, 24);
	_txtAllocated = new Text(150, 9, 160, 24);
	_txtSpace = new Text(150, 9, 8, 34);
	_txtFunds = new Text(150, 9, 160, 34);
	_txtItem = new Text(80, 9, 10, 52);
	_txtEngineers = new Text(56, 18, 112, 44);
	_txtProduced = new Text(56, 18, 168, 44);
	_txtCost = new Text(44, 27, 222, 44);
	_txtTimeLeft = new Text(60, 27, 260, 44);
	_lstManufacture = new TextList(288, 88, 8, 80);

	// Set palette
	setInterface("manufactureMenu");

	add(_window, "window", "manufactureMenu");
	add(_btnNew, "button", "manufactureMenu");
	add(_btnOk, "button", "manufactureMenu");
	add(_txtTitle, "text1", "manufactureMenu");
	add(_txtAvailable, "text1", "manufactureMenu");
	add(_txtAllocated, "text1", "manufactureMenu");
	add(_txtSpace, "text1", "manufactureMenu");
	add(_txtFunds, "text1", "manufactureMenu");
	add(_txtItem, "text2", "manufactureMenu");
	add(_txtEngineers, "text2", "manufactureMenu");
	add(_txtProduced, "text2", "manufactureMenu");
	add(_txtCost, "text2", "manufactureMenu");
	add(_txtTimeLeft, "text2", "manufactureMenu");
	add(_lstManufacture, "list", "manufactureMenu");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK17.SCR"));

	_btnNew->setText(tr("STR_NEW_PRODUCTION"));
	_btnNew->onMouseClick((ActionHandler)&ManufactureState::btnNewProductionClick);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ManufactureState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ManufactureState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_CURRENT_PRODUCTION"));

	_txtItem->setText(tr("STR_ITEM"));

	_txtEngineers->setText(tr("STR_ENGINEERS__ALLOCATED"));
	_txtEngineers->setWordWrap(true);

	_txtProduced->setText(tr("STR_UNITS_PRODUCED"));
	_txtProduced->setWordWrap(true);

	_txtCost->setText(tr("STR_COST__PER__UNIT"));
	_txtCost->setWordWrap(true);

	_txtTimeLeft->setText(tr("STR_DAYS_HOURS_LEFT"));
	_txtTimeLeft->setWordWrap(true);

	_lstManufacture->setColumns(5, 115, 15, 52, 56, 48);
	_lstManufacture->setAlign(ALIGN_RIGHT);
	_lstManufacture->setAlign(ALIGN_LEFT, 0);
	_lstManufacture->setSelectable(true);
	_lstManufacture->setBackground(_window);
	_lstManufacture->setMargin(2);
	_lstManufacture->setWordWrap(true);
	_lstManufacture->onMouseClick((ActionHandler)&ManufactureState::lstManufactureClick);
	fillProductionList();
}

/**
 *
 */
ManufactureState::~ManufactureState()
{

}

/**
 * Updates the production list
 * after going to other screens.
 */
void ManufactureState::init()
{
	State::init();
	fillProductionList();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ManufactureState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Opens the screen with the list of possible productions.
 * @param action Pointer to an action.
 */
void ManufactureState::btnNewProductionClick(Action *)
{
	_game->pushState(new NewManufactureListState(_base));
}

/**
 * Fills the list of base productions.
 */
void ManufactureState::fillProductionList()
{
	const std::vector<Production *> productions(_base->getProductions());
	_lstManufacture->clearList();
	for (std::vector<Production *>::const_iterator iter = productions.begin(); iter != productions.end(); ++iter)
	{
		std::ostringstream s1;
		s1 << (*iter)->getAssignedEngineers();
		std::ostringstream s2;
		s2 << (*iter)->getAmountProduced() << "/";
		if ((*iter)->getInfiniteAmount()) s2 << "∞";
		else s2 << (*iter)->getAmountTotal();
		if ((*iter)->getSellItems()) s2 << " $";
		std::ostringstream s3;
		s3 << Unicode::formatFunding((*iter)->getRules()->getManufactureCost());
		std::ostringstream s4;
		if ((*iter)->getInfiniteAmount())
		{
			s4 << "∞";
		}
		else if ((*iter)->getAssignedEngineers() > 0)
		{
			int timeLeft = (*iter)->getAmountTotal() * (*iter)->getRules()->getManufactureTime() - (*iter)->getTimeSpent();
			int numEffectiveEngineers = (*iter)->getAssignedEngineers();
			// ensure we round up since it takes an entire hour to manufacture any part of that hour's capacity
			int hoursLeft = (timeLeft + numEffectiveEngineers - 1) / numEffectiveEngineers;
			int daysLeft = hoursLeft / 24;
			int hours = hoursLeft % 24;
			s4 << daysLeft << "/" << hours;
		}
		else
		{

			s4 << "-";
		}
		_lstManufacture->addRow(5, tr((*iter)->getRules()->getName()).c_str(), s1.str().c_str(), s2.str().c_str(), s3.str().c_str(), s4.str().c_str());
	}
	_txtAvailable->setText(tr("STR_ENGINEERS_AVAILABLE").arg(_base->getAvailableEngineers()));
	_txtAllocated->setText(tr("STR_ENGINEERS_ALLOCATED").arg(_base->getAllocatedEngineers()));
	_txtSpace->setText(tr("STR_WORKSHOP_SPACE_AVAILABLE").arg(_base->getFreeWorkshops()));
	_txtFunds->setText(tr("STR_CURRENT_FUNDS").arg(Unicode::formatFunding(_game->getSavedGame()->getFunds())));
}

/**
 * Opens the screen displaying production settings.
 * @param action Pointer to an action.
 */
void ManufactureState::lstManufactureClick(Action *)
{
	const std::vector<Production*> productions(_base->getProductions());
	_game->pushState(new ManufactureInfoState(_base, productions[_lstManufacture->getSelectedRow()]));
}

}
