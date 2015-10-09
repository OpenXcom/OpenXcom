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
#include "ManufactureStartState.h"
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleManufacture.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"
#include "ManufactureInfoState.h"
#include "../Savegame/SavedGame.h"
#include <sstream>

namespace OpenXcom
{

/**
 * Initializes all the elements in the productions start screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param item The RuleManufacture to produce.
 */
ManufactureStartState::ManufactureStartState(Base * base, RuleManufacture * item) :  _base(base), _item(item)
{
	_screen = false;

	_window = new Window(this, 320, 160, 0, 20);
	_btnCancel = new TextButton(136, 16, 16, 155);
	_txtTitle = new Text(320, 17, 0, 30);
	_txtManHour = new Text(290, 9, 16, 50);
	_txtCost = new Text(290, 9, 16, 60);
	_txtWorkSpace = new Text(290, 9, 16, 70);

	_txtRequiredItemsTitle = new Text(290, 9, 16, 84);
	_txtItemNameColumn = new Text(60, 16, 30, 92);
	_txtUnitRequiredColumn = new Text(60, 16, 155, 92);
	_txtUnitAvailableColumn = new Text(60, 16, 230, 92);
	_lstRequiredItems = new TextList(270, 40, 30, 108);

	_btnStart = new TextButton(136, 16, 168, 155);

	// Set palette
	setInterface("allocateManufacture");

	add(_window, "window", "allocateManufacture");
	add(_txtTitle, "text", "allocateManufacture");
	add(_txtManHour, "text", "allocateManufacture");
	add(_txtCost, "text", "allocateManufacture");
	add(_txtWorkSpace, "text", "allocateManufacture");
	add(_btnCancel, "button", "allocateManufacture");

	add(_txtRequiredItemsTitle, "text", "allocateManufacture");
	add(_txtItemNameColumn, "text", "allocateManufacture");
	add(_txtUnitRequiredColumn, "text", "allocateManufacture");
	add(_txtUnitAvailableColumn, "text", "allocateManufacture");
	add(_lstRequiredItems, "list", "allocateManufacture");

	add(_btnStart, "button", "allocateManufacture");

	centerAllSurfaces();

	_window->setBackground(_game->getMod()->getSurface("BACK17.SCR"));

	_txtTitle->setText(tr(_item->getName()));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtManHour->setText(tr("STR_ENGINEER_HOURS_TO_PRODUCE_ONE_UNIT").arg(_item->getManufactureTime()));

	_txtCost->setText(tr("STR_COST_PER_UNIT_").arg(Text::formatFunding(_item->getManufactureCost())));

	_txtWorkSpace->setText(tr("STR_WORK_SPACE_REQUIRED").arg(_item->getRequiredSpace()));

	_btnCancel->setText(tr("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&ManufactureStartState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ManufactureStartState::btnCancelClick, Options::keyCancel);

	const std::map<std::string, int> & requiredItems (_item->getRequiredItems());
	int availableWorkSpace = _base->getFreeWorkshops();
	bool productionPossible = _game->getSavedGame()->getFunds() > _item->getManufactureCost();
	productionPossible &= (availableWorkSpace > 0);

	_txtRequiredItemsTitle->setText(tr("STR_SPECIAL_MATERIALS_REQUIRED"));
	_txtRequiredItemsTitle->setAlign(ALIGN_CENTER);

	_txtItemNameColumn->setText(tr("STR_ITEM_REQUIRED"));
	_txtItemNameColumn->setWordWrap(true);

	_txtUnitRequiredColumn->setText(tr("STR_UNITS_REQUIRED"));
	_txtUnitRequiredColumn->setWordWrap(true);

	_txtUnitAvailableColumn->setText(tr("STR_UNITS_AVAILABLE"));
	_txtUnitAvailableColumn->setWordWrap(true);

	_lstRequiredItems->setColumns(3, 140, 75, 55);
	_lstRequiredItems->setBackground(_window);

	ItemContainer * itemContainer (base->getStorageItems());
	int row = 0;
	for (std::map<std::string, int>::const_iterator iter = requiredItems.begin();
		iter != requiredItems.end();
		++iter)
	{
		std::wostringstream s1, s2;
		s1 << L'\x01' << iter->second;
		s2 << L'\x01' << itemContainer->getItem(iter->first);
		productionPossible &= (itemContainer->getItem(iter->first) >= iter->second);
		_lstRequiredItems->addRow(3, tr(iter->first).c_str(), s1.str().c_str(), s2.str().c_str());
		row++;
	}
	_txtRequiredItemsTitle->setVisible(!requiredItems.empty());
	_txtItemNameColumn->setVisible(!requiredItems.empty());
	_txtUnitRequiredColumn->setVisible(!requiredItems.empty());
	_txtUnitAvailableColumn->setVisible(!requiredItems.empty());
	_lstRequiredItems->setVisible(!requiredItems.empty());

	_btnStart->setText(tr("STR_START_PRODUCTION"));
	_btnStart->onMouseClick((ActionHandler)&ManufactureStartState::btnStartClick);
	_btnStart->onKeyboardPress((ActionHandler)&ManufactureStartState::btnStartClick, Options::keyOk);
	_btnStart->setVisible(productionPossible);
}

/**
 * Returns to previous screen.
 * @param action A pointer to an Action.
 */
void ManufactureStartState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Go to the Production settings screen.
 * @param action A pointer to an Action.
 */
void ManufactureStartState::btnStartClick(Action *)
{
	_game->pushState(new ManufactureInfoState(_base, _item));
}
}
