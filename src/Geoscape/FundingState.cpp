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
#include "FundingState.h"
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
#include "../Savegame/Country.h"
#include "../Ruleset/RuleCountry.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Funding screen.
 * @param game Pointer to the core game.
 */
FundingState::FundingState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(50, 12, 135, 180);
	_txtTitle = new Text(320, 16, 0, 8);
	_txtCountry = new Text(100, 16, 32, 30);
	_txtFunding = new Text(120, 16, 140, 30);
	_txtChange = new Text(120, 16, 240, 30);
	_lstCountries = new TextList(280, 136, 32, 40);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtCountry);
	add(_txtFunding);
	add(_txtChange);
	add(_lstCountries);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)-1);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&FundingState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString("STR_INTERNATIONAL_RELATIONS"));

	_txtCountry->setColor(Palette::blockOffset(8)+5);
	_txtCountry->setText(_game->getLanguage()->getString("STR_COUNTRY"));

	_txtFunding->setColor(Palette::blockOffset(8)+5);
	_txtFunding->setText(_game->getLanguage()->getString("STR_FUNDING"));

	_txtChange->setColor(Palette::blockOffset(8)+5);
	_txtChange->setText(_game->getLanguage()->getString("STR_CHANGE"));

	_lstCountries->setColor(Palette::blockOffset(15)-1);
	_lstCountries->setSecondaryColor(Palette::blockOffset(8)+10);
	_lstCountries->setColumns(3, 108, 100, 72);
	_lstCountries->setDot(true);
	for (std::vector<Country*>::iterator i = _game->getSavedGame()->getCountries()->begin(); i != _game->getSavedGame()->getCountries()->end(); ++i)
	{
		std::wstringstream ss, ss2;
		ss << L'\x01' << Text::formatFunding((*i)->getFunding()) << L'\x01';
		ss2 << (*i)->getChange();
		_lstCountries->addRow(3, _game->getLanguage()->getString((*i)->getRules()->getType()).c_str(), ss.str().c_str(), ss2.str().c_str());
	}
	_lstCountries->addRow(2, _game->getLanguage()->getString("STR_TOTAL_UC").c_str(), Text::formatFunding(_game->getSavedGame()->getCountryFunding()).c_str());
	_lstCountries->setRowColor(_game->getSavedGame()->getCountries()->size(), Palette::blockOffset(8)+5);
}

/**
 *
 */
FundingState::~FundingState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void FundingState::btnOkClick(Action *action)
{
	_game->popState();
}

}
