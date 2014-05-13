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
#include "SoldierDiaryOverviewState.h"
#include "SoldierDiaryPerformanceState.h"
#include "SoldierDiaryMissionState.h"
#include "SoldierInfoState.h"
#include <string>
#include "../Resource/ResourcePack.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDiary.h"
#include "../Savegame/SavedGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldiers screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
SoldierDiaryOverviewState::SoldierDiaryOverviewState(Game *game, Base *base, size_t soldierId, SoldierInfoState *soldierInfoState) : State(game), _base(base), _soldierId(soldierId), _soldierInfoState(soldierInfoState)
{
	if (_base == 0)
	{
		_list = _game->getSavedGame()->getDeadSoldiers();
	}
	else
	{
		_list = _base->getSoldiers();
	}

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
    _btnKills = new TextButton(70, 16, 8, 176);
	_btnMissions = new TextButton(70, 16, 86, 176);
    _btnCommendations = new TextButton(70, 16, 164, 176);
    _btnOk = new TextButton(70, 16, 242, 176);
	_btnPrev = new TextButton(28, 14, 8, 8);
	_btnNext = new TextButton(28, 14, 284, 8);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtLocation = new Text(114, 9, 16, 36);
	_txtStatus = new Text(102, 9, 120, 36);
	_txtDate = new Text(90, 9, 218, 36);
	_lstDiary= new TextList(288, 120, 8, 44);

	// Set palette
	setPalette("PAL_BASESCAPE");

	add(_window);
	add(_btnOk);
	add(_btnKills);
	add(_btnMissions);
	add(_btnCommendations);
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
	_btnOk->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierDiaryOverviewState::btnOkClick, Options::keyCancel);

	_btnKills->setColor(Palette::blockOffset(13)+10);
	_btnKills->setText(tr("STR_KILLS_UC"));
	_btnKills->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnKillsClick);
	
	_btnMissions->setColor(Palette::blockOffset(13)+10);
	_btnMissions->setText(tr("STR_MISSIONS_UC"));
	_btnMissions->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnMissionsClick);

	_btnCommendations->setColor(Palette::blockOffset(13)+10);
	_btnCommendations->setText(tr("STR_AWARDS_UC"));
	_btnCommendations->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnCommendationsClick);

	_btnPrev->setColor(Palette::blockOffset(15)+6);
	_btnPrev->setText(L"<<");
	_btnPrev->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnPrevClick);

	_btnNext->setColor(Palette::blockOffset(15)+6);
	_btnNext->setText(L">>");
	_btnNext->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::btnNextClick);

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
	_lstDiary->onMouseClick((ActionHandler)&SoldierDiaryOverviewState::lstDiaryInfoClick);


	init(); // Populate the list
}

/**
 *
 */
SoldierDiaryOverviewState::~SoldierDiaryOverviewState()
{

}

/**
 *  Clears all the variables and reinitializes the list of medals for the soldier.
 *
 */
void SoldierDiaryOverviewState::init()
{
	State::init();
	if (_list->empty())
	{
		_game->popState();
		return;
	}
	if (_soldierId >= _list->size())
	{
		_soldierId = 0;
	}
	_soldier = _list->at(_soldierId);
	_txtTitle->setText(_soldier->getName());
	_lstDiary->clearList();
	_lstDiary->scrollTo(0);
	std::vector<MissionStatistics*> *missionStatistics = _game->getSavedGame()->getMissionStatistics();

	for (std::vector<MissionStatistics*>::iterator j = missionStatistics->begin() ; j != missionStatistics->end() ; ++j)
	{
		int missionId = (*j)->id;
		bool wasOnMission = false;

		for (std::vector<int>::const_iterator k = _soldier->getDiary()->getMissionIdList().begin(); k != _soldier->getDiary()->getMissionIdList().end(); ++k)
		{
			if (missionId == (*k))
			{
				wasOnMission = true;
				break;
			}
		}
		if (!wasOnMission)
		{
			continue;
		}

		// See if this mission is part of the soldier's vector of missions
		std::wstringstream wssSuccess, wssRating, wssScore;
		std::wstringstream wssRegion, wssCountry;
		std::wstringstream wssHour, wssMinute, wssDay, wssMonth, wssYear;

		if ((*j)->success)
		{
			wssSuccess << tr("STR_MISSION_WIN");
		}
		else
		{
			wssSuccess << tr("STR_MISSION_LOSS");
		}
		
		wssRating << tr((*j)->rating.c_str());
		wssScore << (*j)->score;

		wssRegion << tr((*j)->region.c_str());
		wssCountry << tr((*j)->country.c_str());

		wssMonth << tr((*j)->time.getMonthString().c_str());
		wssDay << (*j)->time.getDayString(_game->getLanguage());
		wssYear << (*j)->time.getYear();

		std::wstringstream wssStatus, wssLocation;

		if ((*j)->country == "STR_UNKNOWN")
		{
			wssLocation << wssRegion.str().c_str();
		}
		else
		{
			wssLocation << wssCountry.str().c_str();
		}
		wssStatus << wssSuccess.str().c_str() << " - " << wssRating.str().c_str();
		
		_lstDiary->addRow(5, wssLocation.str().c_str(), wssStatus.str().c_str(), wssDay.str().c_str(), wssMonth.str().c_str(), wssYear.str().c_str());
	}
}

/**
 * Set the soldier's Id.
 */
void SoldierDiaryOverviewState::setSoldierId(size_t soldier)
{
	_soldierId = soldier;
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnOkClick(Action *)
{
	_soldierInfoState->setSoldierId(_soldierId);
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnKillsClick(Action *)
{
	int _display = 0;
	_game->pushState(new SoldierDiaryPerformanceState(_game, _base, _soldierId, this, _display));
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnMissionsClick(Action *)
{
	int _display = 1;
	_game->pushState(new SoldierDiaryPerformanceState(_game, _base, _soldierId, this, _display));
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnCommendationsClick(Action *)
{
	int _display = 2;
	_game->pushState(new SoldierDiaryPerformanceState(_game, _base, _soldierId, this, _display));
}


/**
 * Goes to the previous soldier.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnPrevClick(Action *)
{
	if (_soldierId == 0)
		_soldierId = _list->size() - 1;
	else
		_soldierId--;
	init();
}

/**
 * Goes to the next soldier.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::btnNextClick(Action *)
{
	_soldierId++;
	if (_soldierId >= _list->size())
		_soldierId = 0;
	init();
}

/**
 * Shows the selected soldier's info.
 * @param action Pointer to an action.
 */
void SoldierDiaryOverviewState::lstDiaryInfoClick(Action *)
{
    int absoluteRowEntry = _lstDiary->getSelectedRow() + _lstDiary->getScroll();
	_game->pushState(new SoldierDiaryMissionState(_game, _base, _soldierId, absoluteRowEntry));
}

}