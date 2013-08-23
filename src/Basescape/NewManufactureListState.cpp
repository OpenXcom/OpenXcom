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
#include "NewManufactureListState.h"
#include <algorithm>
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
#include "../Ruleset/Ruleset.h"
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
NewManufactureListState::NewManufactureListState(Game *game, Base *base) : State(game), _base(base)
{
	int width = 320;
	int height = 140;
	int max_width = 320;
	int max_height = 200;
	int start_x = (max_width - width) / 2;
	int start_y = (max_height - height) / 2;
	int button_x_border = 8;
	int button_y_border = 8;
	int button_height = 16;
	_screen = false;
	_window = new Window(this, width, height, start_x, start_y, POPUP_BOTH);
	_btnOk = new TextButton (width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);
	_txtTitle = new Text (width - 2 * button_x_border, button_height, start_x + button_x_border + 2, start_y + button_y_border);
	_txtItem = new Text (10 * button_x_border, button_height / 2, start_x + button_x_border + 2, start_y + 3 * button_y_border);
	_txtCategory = new Text (10 * button_x_border, button_height / 2, start_x + 20.75f * button_x_border, start_y + 3 * button_y_border);
	_lstManufacture = new TextList(width - 4 * button_x_border, height - 3.75f * button_height - 2 * button_y_border, start_x + button_x_border, start_y + 5 * button_y_border);
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtItem);
	add(_txtCategory);
	add(_lstManufacture);

	centerAllSurfaces();

	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));
	_txtTitle->setColor(Palette::blockOffset(15)+1);
	_txtTitle->setText(_game->getLanguage()->getString("STR_PRODUCTION_ITEMS"));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtItem->setColor(Palette::blockOffset(15)+1);
	_txtItem->setText(_game->getLanguage()->getString("STR_ITEM"));

	_txtCategory->setColor(Palette::blockOffset(15)+1);
	_txtCategory->setText(_game->getLanguage()->getString("STR_CATEGORY"));

	_lstManufacture->setColumns(2, int(19.5f * button_x_border), int(16.25f * button_x_border));
	_lstManufacture->setSelectable(true);
	_lstManufacture->setBackground(_window);
	_lstManufacture->setMargin(2);
	_lstManufacture->setColor(Palette::blockOffset(13));
	_lstManufacture->setArrowColor(Palette::blockOffset(15)+1);
	_lstManufacture->onMouseClick((ActionHandler)&NewManufactureListState::lstProdClick);

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&NewManufactureListState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&NewManufactureListState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));
}

/**
 * Initializes state (fills list of possible productions).
 */
void NewManufactureListState::init ()
{
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
void NewManufactureListState::lstProdClick (Action *)
{
	RuleManufacture *rule = _possibleProductions[_lstManufacture->getSelectedRow()];
	if (rule->getCategory() == "STR_CRAFT" && _base->getAvailableHangars() - _base->getUsedHangars() == 0)
	{
		_game->pushState(new ErrorMessageState(_game, "STR_NO_FREE_HANGARS_FOR_CRAFT_PRODUCTION", Palette::blockOffset(15)+1, "BACK17.SCR", 6));
	}
	else if (rule->getRequiredSpace() > _base->getFreeWorkshops())
	{
		_game->pushState(new ErrorMessageState(_game, "STR_NOT_ENOUGH_WORK_SPACE", Palette::blockOffset(15)+1, "BACK17.SCR", 6));
	}
	else
	{
		_game->pushState(new ManufactureStartState(_game, _base, rule));
	}
}


/**
 * Fills the list of possible productions.
 */
void NewManufactureListState::fillProductionList()
{
	_lstManufacture->clearList();
	_possibleProductions.clear();
	_game->getSavedGame()->getAvailableProductions(_possibleProductions, _game->getRuleset(), _base);

	for (std::vector<RuleManufacture *>::iterator it = _possibleProductions.begin (); it != _possibleProductions.end (); ++it)
	{
		_lstManufacture->addRow(2, _game->getLanguage()->getString((*it)->getName()).c_str(), _game->getLanguage()->getString((*it)->getCategory ()).c_str());
	}
}

}
