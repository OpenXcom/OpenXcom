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
#include "ManufactureState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Screen.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleManufacture.h"
#include "../Savegame/Production.h"
#include "NewManufactureListState.h"
#include "ManufactureInfoState.h"
#include <limits>

namespace OpenXcom
{

/**
 * Initializes all the elements in the Manufacture screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
ManufactureState::ManufactureState(Game *game, Base *base) : State(game), _base(base)
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
	_txtTimeLeft = new Text(55, 18, 260, 44);
	_lstManufacture = new TextList(307, 90, 8, 80);

	// back up palette in case we're being called from Geoscape!
	memcpy(_oldPalette, _game->getScreen()->getPalette(), 256*sizeof(SDL_Color));

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);

	add(_window);
	add(_btnNew);
	add(_btnOk);
	add(_txtTitle);
	add(_txtAvailable);
	add(_txtAllocated);
	add(_txtSpace);
	add(_txtFunds);
	add(_txtItem);
	add(_txtEngineers);
	add(_txtProduced);
	add(_txtCost);
	add(_txtTimeLeft);
	add(_lstManufacture);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+6);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK17.SCR"));

	_btnNew->setColor(Palette::blockOffset(13)+10);
	_btnNew->setText(tr("STR_NEW_PRODUCTION"));
	_btnNew->onMouseClick((ActionHandler)&ManufactureState::btnNewProductionClick);

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ManufactureState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ManufactureState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTitle->setColor(Palette::blockOffset(15)+6);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_CURRENT_PRODUCTION"));

	_txtAvailable->setColor(Palette::blockOffset(15)+6);
	_txtAvailable->setSecondaryColor(Palette::blockOffset(13));

	_txtAllocated->setColor(Palette::blockOffset(15)+6);
	_txtAllocated->setSecondaryColor(Palette::blockOffset(13));

	_txtSpace->setColor(Palette::blockOffset(15)+6);
	_txtSpace->setSecondaryColor(Palette::blockOffset(13));

	_txtFunds->setColor(Palette::blockOffset(15)+6);
	_txtFunds->setSecondaryColor(Palette::blockOffset(13));
	_txtFunds->setText(tr("STR_CURRENT_FUNDS").arg(Text::formatFunding(_game->getSavedGame()->getFunds())));

	_txtItem->setColor(Palette::blockOffset(15)+1);
	_txtItem->setText(tr("STR_ITEM"));

	_txtEngineers->setColor(Palette::blockOffset(15)+1);
	_txtEngineers->setText(tr("STR_ENGINEERS__ALLOCATED"));
	_txtEngineers->setWordWrap(true);
	_txtEngineers->setVerticalAlign(ALIGN_BOTTOM);

	_txtProduced->setColor(Palette::blockOffset(15)+1);
	_txtProduced->setText(tr("STR_UNITS_PRODUCED"));
	_txtProduced->setWordWrap(true);
	_txtProduced->setVerticalAlign(ALIGN_BOTTOM);
	
	_txtCost->setColor(Palette::blockOffset(15)+1);
	_txtCost->setText(tr("STR_COST__PER__UNIT"));
	_txtCost->setWordWrap(true);
	_txtCost->setVerticalAlign(ALIGN_BOTTOM);

	_txtTimeLeft->setColor(Palette::blockOffset(15)+1);
	_txtTimeLeft->setText(tr("STR_DAYS_HOURS_LEFT"));
	_txtTimeLeft->setWordWrap(true);
	_txtTimeLeft->setVerticalAlign(ALIGN_BOTTOM);

	_lstManufacture->setColor(Palette::blockOffset(13)+10);
	_lstManufacture->setArrowColor(Palette::blockOffset(15)+9);
	_lstManufacture->setColumns(5, 122, 17, 52, 56, 32);
	_lstManufacture->setAlign(ALIGN_RIGHT);
	_lstManufacture->setAlign(ALIGN_LEFT, 0);
	_lstManufacture->setSelectable(true);
	_lstManufacture->setBackground(_window);
	_lstManufacture->setMargin(1);
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
void ManufactureState::init ()
{
	fillProductionList();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ManufactureState::btnOkClick(Action *)
{
	// restore palette
	_game->setPalette(_oldPalette);
	
	_game->popState();
}

/**
 * Opens the screen with the list of possible productions.
 * @param action Pointer to an action.
 */
void ManufactureState::btnNewProductionClick(Action *)
{
	_game->pushState(new NewManufactureListState(_game, _base));
}

/**
 * Fills the list of base productions.
 */
void ManufactureState::fillProductionList()
{
	const std::vector<Production *> productions(_base->getProductions ());
	_lstManufacture->clearList();
	for(std::vector<Production *>::const_iterator iter = productions.begin (); iter != productions.end (); ++iter)
	{
		std::wstringstream s1;
		s1 << (*iter)->getAssignedEngineers();
		std::wstringstream s2;
		s2 << (*iter)->getAmountProduced() << "/";
		if (Options::getBool("allowAutoSellProduction") && (*iter)->getAmountTotal() == std::numeric_limits<int>::max())
			s2 << "$$$";
		else s2 << (*iter)->getAmountTotal();
		std::wstringstream s3;
		s3 << Text::formatFunding((*iter)->getRules()->getManufactureCost());
		std::wstringstream s4;
		if ((*iter)->getAssignedEngineers() > 0)
		{
			int timeLeft;
			if (Options::getBool("allowAutoSellProduction") && (*iter)->getAmountTotal() == std::numeric_limits<int>::max())
				timeLeft = ((*iter)->getAmountProduced()+1) * (*iter)->getRules()->getManufactureTime() - (*iter)->getTimeSpent ();
			else timeLeft = (*iter)->getAmountTotal () * (*iter)->getRules()->getManufactureTime() - (*iter)->getTimeSpent ();
			timeLeft /= (*iter)->getAssignedEngineers();
			float dayLeft = timeLeft / 24.0f;
			int hours = (dayLeft - static_cast<int>(dayLeft)) * 24;
			s4 << static_cast<int>(dayLeft) << "/" << hours;
		}
		else
		{

			s4 << L"-";
		}
		_lstManufacture->addRow (5, tr((*iter)->getRules()->getName()).c_str(), s1.str().c_str(), s2.str().c_str(), s3.str().c_str(), s4.str().c_str());
	}
	_txtAvailable->setText(tr("STR_ENGINEERS_AVAILABLE").arg(_base->getAvailableEngineers()));
	_txtAllocated->setText(tr("STR_ENGINEERS_ALLOCATED").arg(_base->getAllocatedEngineers()));
	_txtSpace->setText(tr("STR_WORKSHOP_SPACE_AVAILABLE").arg(_base->getFreeWorkshops()));
}

/**
 * Opens the screen displaying production settings.
 * @param action Pointer to an action.
 */
void ManufactureState::lstManufactureClick(Action *)
{
	const std::vector<Production *> productions(_base->getProductions ());
	_game->pushState(new ManufactureInfoState(_game, _base, productions[_lstManufacture->getSelectedRow()]));
}

}
