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
#include "SoldierDiaryState.h"
#include "SoldierInfoState.h"
#include "SoldierDiaryInfoState.h"
#include "SoldierDiaryTotalsState.h"
#include <string>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Geoscape/AllocatePsiTrainingState.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Savegame/SoldierDiary.h"
#include "../Ruleset/RuleCraft.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldiers screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
SoldierDiaryState::SoldierDiaryState(Game *game, Base *base, size_t soldier, SoldierInfoState *soldierInfoState) : State(game), _base(base), _soldier(soldier), _soldierInfoState(soldierInfoState)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(96, 16, 216, 176);
	_btnTotals = new TextButton(96, 16, 8, 176);
	_btnPrev = new TextButton(28, 14, 8, 8);
	_btnNext = new TextButton(28, 14, 284, 8);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtLocation = new Text(114, 9, 16, 36);
	_txtStatus = new Text(102, 9, 130, 36);
	_txtDate = new Text(90, 9, 218, 36);
	_lstDiary= new TextList(288, 120, 8, 44);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnTotals);
	add(_btnPrev);
	add(_btnNext); 
	add(_txtTitle);
	add(_txtLocation);
	add(_txtStatus);
	add(_txtDate);
	add(_lstDiary);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK02.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierDiaryState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierDiaryState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_btnTotals->setColor(Palette::blockOffset(13)+10);
	_btnTotals->setText(tr("STR_TOTALS"));
	_btnTotals->onMouseClick((ActionHandler)&SoldierDiaryState::btnTotalsClick);

	_btnPrev->setColor(Palette::blockOffset(15)+6);
	_btnPrev->setText(L"<<");
	_btnPrev->onMouseClick((ActionHandler)&SoldierDiaryState::btnPrevClick);

	_btnNext->setColor(Palette::blockOffset(15)+6);
	_btnNext->setText(L">>");
	_btnNext->onMouseClick((ActionHandler)&SoldierDiaryState::btnNextClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	// Text is decided in init()

	_txtLocation->setColor(Palette::blockOffset(15)+1);
	_txtLocation->setText(tr("STR_LOCATION"));

	_txtStatus->setColor(Palette::blockOffset(15)+1);
	_txtStatus->setText(tr("STR_STATUS"));

	_txtDate->setColor(Palette::blockOffset(15)+1);
	_txtDate->setText(tr("STR_DATE_UC"));

	_lstDiary->setColor(Palette::blockOffset(13)+10);
	_lstDiary->setArrowColor(Palette::blockOffset(15)+1);
	_lstDiary->setColumns(5, 104, 98, 30, 25, 35);
	_lstDiary->setSelectable(true);
	_lstDiary->setBackground(_window);
	_lstDiary->setMargin(8);
	_lstDiary->onMouseClick((ActionHandler)&SoldierDiaryState::lstDiaryInfoClick);


	init(); // Populate the list
}

/**
 *
 */
SoldierDiaryState::~SoldierDiaryState()
{

}

/**
 *  Clears all the variables and reinitializes the list of medals for the soldier.
 *
 */
void SoldierDiaryState::init()
{
	Soldier *s = _base->getSoldiers()->at(_soldier);
	_txtTitle->setText(s->getName());
	_lstDiary->clearList();
	std::vector<SoldierDiaryEntries*> _diaryEntries = s->getDiary()->getSoldierDiaryEntries();

	for (std::vector<SoldierDiaryEntries*>::iterator j = _diaryEntries.begin() ; j != _diaryEntries.end() ; ++j)
	{
		std::wstringstream Success, Rating, Score;
		std::wstringstream Region, Country;
		std::wstringstream Hour, Minute, Day, Month, Year;

		if ((*j)->getMissionSuccess())
		{
			Success << tr("STR_MISSION_WIN");
		}
		else
		{
			Success << tr("STR_MISSION_LOSS");
		}
		
		Rating << tr((*j)->getMissionRating().c_str());
		Score << (*j)->getMissionScore();

		Region << tr((*j)->getMissionRegion().c_str());
		Country << tr((*j)->getMissionCountry().c_str());

		Month << tr((*j)->getMissionTime()->getMonthString().c_str());
		Day << (*j)->getMissionTime()->getDayString(_game->getLanguage());
		Year << (*j)->getMissionTime()->getYear();

		std::wstringstream Status, Location;

		if ((*j)->getMissionCountry() == "STR_COUNTRY_UNKNOWN")
		{
			Location << Region.str().c_str();
		}
		else
		{
			Location << Country.str().c_str();
		}
		Status << Success.str().c_str() << " - " << Rating.str().c_str();
		
		_lstDiary->addRow(5, Location.str().c_str(), Status.str().c_str(), Day.str().c_str(), Month.str().c_str(), Year.str().c_str());
	}
}

/**
 * Set the soldier's Id.
 */
void SoldierDiaryState::setSoldierId(size_t soldier)
{
	_soldier = soldier;
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryState::btnOkClick(Action *)
{
	_soldierInfoState->setSoldierId(_soldier);
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryState::btnTotalsClick(Action *)
{
	_game->pushState(new SoldierDiaryTotalsState(_game, _base, _soldier, this));
}

/**
 * Goes to the previous soldier.
 * @param action Pointer to an action.
 */
void SoldierDiaryState::btnPrevClick(Action *)
{
	if (_soldier == 0)
		_soldier = _base->getSoldiers()->size() - 1;
	else
		_soldier--;
	init();
}

/**
 * Goes to the next soldier.
 * @param action Pointer to an action.
 */
void SoldierDiaryState::btnNextClick(Action *)
{
	_soldier++;
	if (_soldier >= _base->getSoldiers()->size())
		_soldier = 0;
	init();
}

/**
 * Shows the selected soldier's info.
 * @param action Pointer to an action.
 */
void SoldierDiaryState::lstDiaryInfoClick(Action *)
{
	_game->pushState(new SoldierDiaryInfoState(_game, _base, _soldier,  _lstDiary->getSelectedRow()));
}

}