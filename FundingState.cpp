/*
 * Copyright 2010 Daniel Albano
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
#include "Game.h"
#include "ResourcePack.h"
#include "Language.h"
#include "LangString.h"
#include "Font.h"
#include "Palette.h"
#include "TextButton.h"
#include "Window.h"
#include "Text.h"
#include "TextList.h"
#include "Country.h"
#include "SavedGame.h"

using namespace std;

/**
 * Initializes all the elements in the Funding screen.
 * @param game Pointer to the core game.
 */
FundingState::FundingState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 50, 12, 135, 180);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 280, 16, 32, 8);
	_txtCountry = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 100, 16, 32, 24);
	_txtFunding = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 60, 16, 140, 24);
	_txtChange = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 120, 16, 200, 24);
	_lstCountries = new TextList(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 280, 136, 32, 40);
	
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
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBackground(game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)+2);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&FundingState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_INTERNATIONAL_RELATIONS));

	_txtCountry->setColor(Palette::blockOffset(15)-1);
	_txtCountry->setBig();
	_txtCountry->setText(_game->getResourcePack()->getLanguage()->getString(STR_COUNTRY));

	_txtFunding->setColor(Palette::blockOffset(15)-1);
	_txtFunding->setBig();
	_txtFunding->setText(_game->getResourcePack()->getLanguage()->getString(STR_FUNDING));

	_txtChange->setColor(Palette::blockOffset(15)-1);
	_txtChange->setBig();
	_txtChange->setText(_game->getResourcePack()->getLanguage()->getString(STR_CHANGE));

	_lstCountries->setColor(Palette::blockOffset(15)-1);
	_lstCountries->setColumns(3, 108, 100, 72);
	_lstCountries->setDot(true);
	for (map<LangString, Country*>::iterator i = _game->getSavedGame()->getCountries()->begin(); i != _game->getSavedGame()->getCountries()->end(); i++)
	{
		stringstream ss;
		ss << i->second->getChange();
		_lstCountries->addRow(3, _game->getResourcePack()->getLanguage()->getString(i->first).c_str(), Text::formatFunding(i->second->getFunding()).c_str(), ss.str().c_str());
	}
	_lstCountries->addRow(2, _game->getResourcePack()->getLanguage()->getString(STR_TOTAL_UC).c_str(), Text::formatFunding(_game->getSavedGame()->getCountryFunding()).c_str());
	_lstCountries->getCell(_game->getSavedGame()->getCountries()->size(), 0)->setColor(Palette::blockOffset(8)+5);
	_lstCountries->getCell(_game->getSavedGame()->getCountries()->size(), 1)->setColor(Palette::blockOffset(8)+5);
	_lstCountries->draw();
}

/**
 *
 */
FundingState::~FundingState()
{
	
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void FundingState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->popState();
}
