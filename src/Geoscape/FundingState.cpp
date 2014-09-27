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
#include "FundingState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Country.h"
#include "../Ruleset/RuleCountry.h"
#include "../Savegame/SavedGame.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Funding screen.
 * @param game Pointer to the core game.
 */
FundingState::FundingState()
{
	_screen = false;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_btnOk = new TextButton(50, 12, 135, 180);
	_txtTitle = new Text(320, 17, 0, 8);
	_txtCountry = new Text(100, 9, 32, 30);
	_txtFunding = new Text(100, 9, 140, 30);
	_txtChange = new Text(72, 9, 240, 30);
	_lstCountries = new TextList(260, 136, 32, 40);

	// Set palette
	setPalette("PAL_GEOSCAPE", 0);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtCountry);
	add(_txtFunding);
	add(_txtChange);
	add(_lstCountries);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(15)-1);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&FundingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&FundingState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&FundingState::btnOkClick, Options::keyCancel);
	_btnOk->onKeyboardPress((ActionHandler)&FundingState::btnOkClick, Options::keyGeoFunding);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_INTERNATIONAL_RELATIONS"));

	_txtCountry->setColor(Palette::blockOffset(8)+5);
	_txtCountry->setText(tr("STR_COUNTRY"));

	_txtFunding->setColor(Palette::blockOffset(8)+5);
	_txtFunding->setText(tr("STR_FUNDING"));

	_txtChange->setColor(Palette::blockOffset(8)+5);
	_txtChange->setText(tr("STR_CHANGE"));

	_lstCountries->setColor(Palette::blockOffset(15)-1);
	_lstCountries->setSecondaryColor(Palette::blockOffset(8)+10);
	_lstCountries->setColumns(3, 108, 100, 52);
	_lstCountries->setDot(true);
	for (std::vector<Country*>::iterator i = _game->getSavedGame()->getCountries()->begin(); i != _game->getSavedGame()->getCountries()->end(); ++i)
	{
		std::wostringstream ss, ss2;
		ss << L'\x01' << Text::formatFunding((*i)->getFunding().at((*i)->getFunding().size()-1)) << L'\x01';
		if ((*i)->getFunding().size() > 1)
		{
			ss2 << L'\x01';
			int change = (*i)->getFunding().back() - (*i)->getFunding().at((*i)->getFunding().size()-2);
			if (change > 0)
				ss2 << L'+';
			ss2 << Text::formatFunding(change);
			ss2 << L'\x01';
		}
		else
		{
			ss2 << Text::formatFunding(0);
		}
		_lstCountries->addRow(3, tr((*i)->getRules()->getType()).c_str(), ss.str().c_str(), ss2.str().c_str());
	}
	_lstCountries->addRow(2, tr("STR_TOTAL_UC").c_str(), Text::formatFunding(_game->getSavedGame()->getCountryFunding()).c_str());
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
void FundingState::btnOkClick(Action *)
{
	_game->popState();
}

}
