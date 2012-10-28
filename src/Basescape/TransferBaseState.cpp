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
#include "TransferBaseState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Region.h"
#include "../Ruleset/RuleRegion.h"
#include "TransferItemsState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Select Destination Base window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
TransferBaseState::TransferBaseState(Game *game, Base *base) : State(game), _base(base), _bases()
{
	// Create objects
	_window = new Window(this, 280, 140, 20, 30);
	_btnCancel = new TextButton(264, 16, 28, 146);
	_txtTitle = new Text(270, 16, 25, 38);
	_txtFunds = new Text(250, 9, 30, 54);
	_txtName = new Text(130, 16, 28, 64);
	_txtArea = new Text(130, 16, 160, 64);
	_lstBases = new TextList(248, 64, 28, 80);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

	add(_window);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtFunds);
	add(_txtName);
	add(_txtArea);
	add(_lstBases);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnCancel->setColor(Palette::blockOffset(13)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&TransferBaseState::btnCancelClick);

	_txtTitle->setColor(Palette::blockOffset(13)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_SELECT_DESTINATION_BASE"));

	_txtFunds->setColor(Palette::blockOffset(13)+5);
	_txtFunds->setSecondaryColor(Palette::blockOffset(13));
	std::wstring s = _game->getLanguage()->getString("STR_CURRENT_FUNDS");
	s += L'\x01' + Text::formatFunding(_game->getSavedGame()->getFunds());
	_txtFunds->setText(s);

	_txtName->setColor(Palette::blockOffset(13)+5);
	_txtName->setText(_game->getLanguage()->getString("STR_NAME"));
	_txtName->setBig();

	_txtArea->setColor(Palette::blockOffset(13)+5);
	_txtArea->setText(_game->getLanguage()->getString("STR_AREA"));
	_txtArea->setBig();

	_lstBases->setColor(Palette::blockOffset(15)+1);
	_lstBases->setArrowColor(Palette::blockOffset(13)+5);
	_lstBases->setColumns(2, 130, 116);
	_lstBases->setSelectable(true);
	_lstBases->setBackground(_window);
	_lstBases->setMargin(2);
	_lstBases->onMouseClick((ActionHandler)&TransferBaseState::lstBasesClick);

	int row = 0;
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		if ((*i) != _base)
		{
			// Get area
			std::wstring area = L"";
			for (std::vector<Region*>::iterator j = _game->getSavedGame()->getRegions()->begin(); j != _game->getSavedGame()->getRegions()->end(); ++j)
			{
				if ((*j)->getRules()->insideRegion((*i)->getLongitude(), (*i)->getLatitude()))
				{
					area = _game->getLanguage()->getString((*j)->getRules()->getType());
					break;
				}
			}

			_lstBases->addRow(2, (*i)->getName().c_str(), area.c_str());
			_lstBases->setCellColor(row, 1, Palette::blockOffset(13)+5);
			_bases.push_back(*i);
			row++;
		}
	}
}

/**
 *
 */
TransferBaseState::~TransferBaseState()
{
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void TransferBaseState::btnCancelClick(Action *action)
{
	_game->popState();
}

/**
 * Shows Transfer screen for the selected base.
 * @param action Pointer to an action.
 */
void TransferBaseState::lstBasesClick(Action *action)
{
	_game->pushState(new TransferItemsState(_game, _base, _bases[_lstBases->getSelectedRow()]));
}

}
