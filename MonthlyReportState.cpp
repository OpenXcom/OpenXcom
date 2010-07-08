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
#include "MonthlyReportState.h"

/**
 * Initializes all the elements in the Monthly Report screen.
 * @param game Pointer to the core game.
 */
MonthlyReportState::MonthlyReportState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(320, 200, 0, 0);
	_btnOk = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 50, 12, 135, 180);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 280, 16, 16, 8);
	_txtMonth = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 110, 8, 16, 24);
	_txtRating = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 180, 8, 125, 24);
	_txtChange = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 300, 8, 16, 32);
	_txtDesc = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 300, 140, 16, 40);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtMonth);
	add(_txtRating);
	add(_txtChange);
	add(_txtDesc);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+2);
	_window->setBackground(game->getResourcePack()->getSurface("BACK13.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+13);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&MonthlyReportState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getResourcePack()->getLanguage()->getString(STR_XCOM_PROJECT_MONTHLY_REPORT));
	
	stringstream ss;
	ss << _game->getResourcePack()->getLanguage()->getString(STR_MONTH) << _game->getResourcePack()->getLanguage()->getString(_game->getSavedGame()->getTime()->getMonthString()) << " " << _game->getSavedGame()->getTime()->getYear();

	_txtMonth->setColor(Palette::blockOffset(15)-1);
	_txtMonth->setText(ss.str());

	_txtRating->setColor(Palette::blockOffset(15)-1);
	_txtRating->setText(_game->getResourcePack()->getLanguage()->getString(STR_MONTHLY_RATING));

	_txtChange->setColor(Palette::blockOffset(15)-1);
	_txtChange->setText(_game->getResourcePack()->getLanguage()->getString(STR_FUNDING_CHANGE));

	_txtDesc->setColor(Palette::blockOffset(8)+10);
	_txtDesc->setText(_game->getResourcePack()->getLanguage()->getString(STR_COUNCIL_SATISFIED));
	_txtDesc->setWordWrap(true);
}

/**
 *
 */
MonthlyReportState::~MonthlyReportState()
{
	
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void MonthlyReportState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->popState();
}
