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
#include "NewManufactureListState.h"
#include <algorithm>
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Interface/ComboBox.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleManufacture.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Production.h"
#include "../Savegame/Base.h"
#include "ManufactureStartState.h"
#include "../Menu/ErrorMessageState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the productions list screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
NewManufactureListState::NewManufactureListState(Base *base) : _base(base)
{
	_screen = false;

	_window = new Window(this, 320, 156, 0, 22, POPUP_BOTH);
	_btnOk = new TextButton(304, 16, 8, 154);
	_txtTitle = new Text(320, 17, 0, 30);
	_txtItem = new Text(156, 9, 10, 62);
	_txtCategory = new Text(130, 9, 166, 62);
	_lstManufacture = new TextList(288, 80, 8, 70);
	_cbxCategory = new ComboBox(this, 146, 16, 166, 46);

	// Set palette
	setInterface("selectNewManufacture");

	add(_window, "window", "selectNewManufacture");
	add(_btnOk, "button", "selectNewManufacture");
	add(_txtTitle, "text", "selectNewManufacture");
	add(_txtItem, "text", "selectNewManufacture");
	add(_txtCategory, "text", "selectNewManufacture");
	add(_lstManufacture, "list", "selectNewManufacture");
	add(_cbxCategory, "catBox", "selectNewManufacture");

	centerAllSurfaces();

	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));

	_txtTitle->setText(tr("STR_PRODUCTION_ITEMS"));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtItem->setText(tr("STR_ITEM"));

	_txtCategory->setText(tr("STR_CATEGORY"));

	_lstManufacture->setColumns(2, 156, 130);
	_lstManufacture->setSelectable(true);
	_lstManufacture->setBackground(_window);
	_lstManufacture->setMargin(2);
	_lstManufacture->onMouseClick((ActionHandler)&NewManufactureListState::lstProdClick);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewManufactureListState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NewManufactureListState::btnOkClick, Options::keyCancel);

	_possibleProductions.clear();
	_game->getSavedGame()->getAvailableProductions(_possibleProductions, _game->getRuleset(), _base);
	_catStrings.push_back("STR_ALL_ITEMS");

	for (std::vector<RuleManufacture *>::iterator it = _possibleProductions.begin(); it != _possibleProductions.end(); ++it)
	{
		bool addCategory = true;
		for (size_t x = 0; x < _catStrings.size(); ++x)
		{
			if ((*it)->getCategory().c_str() == _catStrings[x])
			{
				addCategory = false;
				break;
			}
		}
		if (addCategory)
		{
			_catStrings.push_back((*it)->getCategory().c_str());
		}
	}

	_cbxCategory->setOptions(_catStrings);
	_cbxCategory->onChange((ActionHandler)&NewManufactureListState::cbxCategoryChange);

}

/**
 * Initializes state (fills list of possible productions).
 */
void NewManufactureListState::init()
{
	State::init();
	fillProductionList();
}

/**
 * Returns to the previous screen.
 * @param action A pointer to an Action.
 */
void NewManufactureListState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Opens the Production settings screen.
 * @param action A pointer to an Action.
*/
void NewManufactureListState::lstProdClick(Action *)
{
	RuleManufacture *rule = 0;
	for (std::vector<RuleManufacture *>::iterator it = _possibleProductions.begin(); it != _possibleProductions.end(); ++it)
	{
		if ((*it)->getName().c_str() == _displayedStrings[_lstManufacture->getSelectedRow()])
		{
			rule = (*it);
			break;
		}
	}
	if (rule->getCategory() == "STR_CRAFT" && _base->getAvailableHangars() - _base->getUsedHangars() <= 0)
	{
		_game->pushState(new ErrorMessageState(tr("STR_NO_FREE_HANGARS_FOR_CRAFT_PRODUCTION"), _palette, _game->getRuleset()->getInterface("basescape")->getElement("errorMessage")->color, "BACK17.SCR", _game->getRuleset()->getInterface("basescape")->getElement("errorPalette")->color));
	}
	else if (rule->getRequiredSpace() > _base->getFreeWorkshops())
	{
		_game->pushState(new ErrorMessageState(tr("STR_NOT_ENOUGH_WORK_SPACE"), _palette, _game->getRuleset()->getInterface("basescape")->getElement("errorMessage")->color, "BACK17.SCR", _game->getRuleset()->getInterface("basescape")->getElement("errorPalette")->color));
	}
	else
	{
		_game->pushState(new ManufactureStartState(_base, rule));
	}
}

/**
 * Updates the production list to match the category filter
 */

void NewManufactureListState::cbxCategoryChange(Action *)
{
	fillProductionList();
}

/**
 * Fills the list of possible productions.
 */
void NewManufactureListState::fillProductionList()
{
	_lstManufacture->clearList();
	_possibleProductions.clear();
	_game->getSavedGame()->getAvailableProductions(_possibleProductions, _game->getRuleset(), _base);
	_displayedStrings.clear();

	for (std::vector<RuleManufacture *>::iterator it = _possibleProductions.begin(); it != _possibleProductions.end(); ++it)
	{
		if (((*it)->getCategory().c_str() == _catStrings[_cbxCategory->getSelected()]) || (_catStrings[_cbxCategory->getSelected()] == "STR_ALL_ITEMS"))
		{
			_lstManufacture->addRow(2, tr((*it)->getName()).c_str(), tr((*it)->getCategory()).c_str());
			_displayedStrings.push_back((*it)->getName().c_str());
		}
	}
}

}
