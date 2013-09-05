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
#include "ManufactureStartState.h"
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Resource/ResourcePack.h"
#include "../Ruleset/RuleManufacture.h"
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
ManufactureStartState::ManufactureStartState(Game * game, Base * base, RuleManufacture * item) : State(game), _base(base), _item(item)
{
	_screen = false;
	int width = 320;
	int height = 170;
	int max_width = 320;
	int max_height = 200;
	int start_x = (max_width - width) / 2;
	int start_y = (max_height - height) / 2;
	int button_x_border = 10;
	int button_y_border = 10;
	int button_height = 16;

	int button_width = (width - 5 * button_x_border) / 2;
	_window = new Window(this, width, height, start_x, start_y);
	_btnCancel = new TextButton (button_width, button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);
	_txtTitle = new Text (width - 4 * button_x_border, button_height * 2, start_x + button_x_border, start_y + button_y_border);
	_txtManHour = new Text (width - 4 * button_x_border, button_height, start_x + button_x_border * 2, start_y + button_y_border * 3);
	_txtCost = new Text (width - 4 * button_x_border, button_height, start_x + button_x_border * 2, start_y + button_y_border * 4);
	_txtWorkSpace = new Text (width - 4 * button_x_border, button_height, start_x + button_x_border * 2, start_y + button_y_border * 5);

	_txtRequiredItemsTitle = new Text (width - 4 * button_x_border, button_height, start_x + button_x_border * 2, start_y + button_y_border * 6);
	_txtItemNameColumn = new Text (6 * button_x_border, button_height, start_x + button_x_border * 3, start_y + button_y_border * 7);
	_txtUnitRequiredColumn = new Text (6 * button_x_border, button_height, start_x + button_x_border * 14, start_y + button_y_border * 7);
	_txtUnitAvailableColumn = new Text (6 * button_x_border, button_height, start_x + button_x_border * 22, start_y + button_y_border * 7);
	_lstRequiredItems = new TextList(width - 8 * button_x_border, height - (start_y + button_y_border * 11), start_x + button_x_border * 3, start_y + button_y_border * 9);

	_btnStart = new TextButton (button_width, button_height, width - button_width - button_x_border, start_y + height - button_height - button_y_border);

	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_txtTitle);
	add(_txtManHour);
	add(_txtCost);
	add(_txtWorkSpace);
	add(_btnCancel);

	add(_txtRequiredItemsTitle);
	add(_txtItemNameColumn);
	add(_txtUnitRequiredColumn);
	add(_txtUnitAvailableColumn);
	add(_lstRequiredItems);

	add(_btnStart);

	centerAllSurfaces();

	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setText(_game->getLanguage()->getString(_item->getName()));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtManHour->setColor(Palette::blockOffset(13)+10);
	std::wstringstream sstr;
	sstr << _item->getManufactureTime () << _game->getLanguage()->getString("STR_ENGINEER_HOURS_TO_PRODUCE_ONE_UNIT");
	_txtManHour->setText(sstr.str());

	std::wstringstream sstr1;
	sstr1 << _game->getLanguage()->getString("STR_COST_PER_UNIT_") << L'\x01' << _item->getManufactureCost ();
	_txtCost->setColor(Palette::blockOffset(13)+10);
	_txtCost->setSecondaryColor(Palette::blockOffset(13));
	_txtCost->setText(sstr1.str());

	std::wstringstream sstr2;
	sstr2 << _game->getLanguage()->getString("STR_WORK_SPACE_REQUIRED") << L'\x01' << _item->getRequiredSpace ();
	_txtWorkSpace->setColor(Palette::blockOffset(13)+10);
	_txtWorkSpace->setSecondaryColor(Palette::blockOffset(13));
	_txtWorkSpace->setText(sstr2.str());

	_btnCancel->setColor(Palette::blockOffset(13)+10);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL_UC"));
	_btnCancel->onMouseClick((ActionHandler)&ManufactureStartState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&ManufactureStartState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	const std::map<std::string, int> & requiredItems (_item->getRequiredItems());
	int availableWorkSpace = _base->getFreeWorkshops();
	bool productionPossible (game->getSavedGame()->getFunds() > _item->getManufactureCost ());
	productionPossible &= (availableWorkSpace > 0);

	_txtRequiredItemsTitle->setColor(Palette::blockOffset(13)+10);
	_txtRequiredItemsTitle->setText(_game->getLanguage()->getString("STR_SPECIAL_MATERIALS_REQUIRED"));
	_txtRequiredItemsTitle->setAlign(ALIGN_CENTER);
	_txtItemNameColumn->setColor(Palette::blockOffset(13)+10);
	_txtItemNameColumn->setText(_game->getLanguage()->getString("STR_ITEM_REQUIRED"));
	_txtUnitRequiredColumn->setColor(Palette::blockOffset(13)+10);
	_txtUnitRequiredColumn->setText(_game->getLanguage()->getString("STR_UNITS_REQUIRED"));
	_txtUnitAvailableColumn->setColor(Palette::blockOffset(13)+10);
	_txtUnitAvailableColumn->setText(_game->getLanguage()->getString("STR_UNITS_AVAILABLE"));

	_lstRequiredItems->setColumns(3, 12 * button_x_border, 8 * button_x_border, 8 * button_x_border);
	_lstRequiredItems->setBackground(_window);
	_lstRequiredItems->setColor(Palette::blockOffset(13));

	ItemContainer * itemContainer (base->getItems());
	int row = 0;
	for(std::map<std::string, int>::const_iterator iter = requiredItems.begin ();
		iter != requiredItems.end ();
		++iter)
	{
		std::wstringstream s1, s2;
		s1 << iter->second;
		s2 << itemContainer->getItem(iter->first);
		productionPossible &= (itemContainer->getItem(iter->first) >= iter->second);
		_lstRequiredItems->addRow(3, _game->getLanguage()->getString(iter->first).c_str(), s1.str().c_str(), s2.str().c_str());
		_lstRequiredItems->setCellColor(row, 0, Palette::blockOffset(13)+10);
		_lstRequiredItems->addRow(1, L"");
		row += 2;
	}
	_txtRequiredItemsTitle->setVisible(!requiredItems.empty());
	_txtItemNameColumn->setVisible(!requiredItems.empty());
	_txtUnitRequiredColumn->setVisible(!requiredItems.empty());
	_txtUnitAvailableColumn->setVisible(!requiredItems.empty());
	_lstRequiredItems->setVisible(!requiredItems.empty());

	_btnStart->setColor(Palette::blockOffset(13)+10);
	_btnStart->setText(_game->getLanguage()->getString("STR_START_PRODUCTION"));
	_btnStart->onMouseClick((ActionHandler)&ManufactureStartState::btnStartClick);
	_btnStart->onKeyboardPress((ActionHandler)&ManufactureStartState::btnStartClick, (SDLKey)Options::getInt("keyOk"));
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
	_game->pushState(new ManufactureInfoState(_game, _base, _item));
}
}
