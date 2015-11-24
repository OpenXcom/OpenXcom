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
#include "SoldierDiaryMissionState.h"
#include <sstream>
#include "../Mod/Mod.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Base.h"
#include "../Savegame/SoldierDiary.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Diary Mission window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param soldier ID of the selected soldier.
 * @param row number to get mission info from.
 */
SoldierDiaryMissionState::SoldierDiaryMissionState(Base *base, size_t soldierId, int rowEntry) : _base(base), _soldierId(soldierId), _rowEntry(rowEntry)
{
	if (_base == 0)
	{
		_list = _game->getSavedGame()->getDeadSoldiers();
	}
	else
	{
		_list = _base->getSoldiers();
	}

	_screen = false;

	// Create objects
	_window = new Window(this, 272, 128, 24, 36, POPUP_BOTH);
	_btnOk = new TextButton(220, 16, 50, 140);
	_txtTitle = new Text(222, 9, 49, 44);
	_txtScore = new Text(222, 9, 29, 62);
	_txtMissionType= new Text(222, 9, 29, 71);
	_txtUFO = new Text(222, 9, 29, 80);
	_txtKills = new Text(130, 9, 207, 62);
	_txtRace = new Text(130, 9, 207, 71);
	_txtDaylight = new Text(130, 9, 207, 80);
	_txtDaysWounded = new Text(130, 9, 207, 89);
	_lstKills = new TextList(240, 32, 29, 102);

	// Set palette
	setInterface("soldierMission");

	add(_window, "window", "soldierMission");
	add(_btnOk, "button", "soldierMission");
	add(_txtTitle, "text", "soldierMission");
	add(_txtScore, "text", "soldierMission");
	add(_txtKills, "text", "soldierMission");
	add(_txtMissionType, "text", "soldierMission");
	add(_txtUFO, "text", "soldierMission");
	add(_txtRace, "text", "soldierMission");
	add(_txtDaylight, "text", "soldierMission");
	add(_txtDaysWounded, "text", "soldierMission");
	add(_lstKills, "list", "soldierMission");

	centerAllSurfaces();

	// Set up objects
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
	std::vector<MissionStatistics*> *missionStatistics = _game->getSavedGame()->getMissionStatistics();
    int missionId = _soldier->getDiary()->getMissionIdList().at(_rowEntry);
	if (missionId > missionStatistics->size())
	{
		missionId = 0;
	}
	int daysWounded = missionStatistics->at(missionId)->injuryList[_soldier->getId()];
    
	_window->setBackground(_game->getMod()->getSurface("BACK16.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierDiaryMissionState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierDiaryMissionState::btnOkClick, Options::keyCancel);

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_MISSION_DETAILS"));

	_txtScore->setText(tr("STR_SCORE_VALUE").arg(missionStatistics->at(missionId)->score));

	_txtMissionType->setText(tr("STR_MISSION_TYPE").arg(tr(missionStatistics->at(missionId)->type)));

	_txtUFO->setText(tr("STR_UFO_TYPE").arg(tr(missionStatistics->at(missionId)->ufo)));
	_txtUFO->setVisible(missionStatistics->at(missionId)->ufo != "NO_UFO");

	_txtRace->setText(tr("STR_RACE_TYPE").arg(tr(missionStatistics->at(missionId)->alienRace)));
	_txtRace->setVisible(missionStatistics->at(missionId)->alienRace != "STR_UNKNOWN");

	if (missionStatistics->at(missionId)->daylight <= 5)
	{
		_txtDaylight->setText(tr("STR_DAYLIGHT_TYPE").arg(tr("STR_DAY")));
	}
	else
	{
		_txtDaylight->setText(tr("STR_DAYLIGHT_TYPE").arg(tr("STR_NIGHT")));
	}

	_txtDaysWounded->setColor(Palette::blockOffset(13)+5);
	_txtDaysWounded->setSecondaryColor(Palette::blockOffset(13));
	_txtDaysWounded->setText(tr("STR_DAYS_WOUNDED").arg(daysWounded));
	_txtDaysWounded->setVisible(daysWounded != 0);

	_lstKills->setColumns(3, 60, 90, 90);
	_lstKills->setSelectable(false);
	_lstKills->setBackground(_window);
	_lstKills->setMargin(8);

	int count = 0;
    bool stunOrKill = false;
	std::wstringstream wssKills;

	for (std::vector<BattleUnitKills*>::iterator j = _soldier->getDiary()->getKills().begin() ; j != _soldier->getDiary()->getKills().end() ; ++j)
	{
		if ((*j)->mission != missionId) continue;
		std::wstringstream wssRank, wssRace, wssWeapon, wssAmmo;

		wssRace << tr((*j)->race);
		wssRank << tr((*j)->rank);
		wssWeapon << tr((*j)->weapon);

		std::wstringstream wssUnit, wssStatus;
		wssUnit << wssRace.str() << " " << wssRank.str();

		if ((*j)->getUnitStatusString() == "STATUS_DEAD")
		{
			wssStatus << tr("STR_KILLED");
            count++;
            stunOrKill = true;
		}
		else if ((*j)->getUnitStatusString() == "STATUS_UNCONSCIOUS")
		{
			wssStatus << tr("STR_STUNNED");
            stunOrKill = true;
		}
        else if ((*j)->getUnitStatusString() == "STATUS_PANICKED")
		{
            wssStatus << tr("STR_PANICKED");
            stunOrKill = true;
        }
        else if ((*j)->getUnitStatusString() == "STATUS_TURNING")
        {
            wssStatus << tr("STR_MINDCONTROLLED");
            stunOrKill = true;
        }

		_lstKills->addRow(3, wssStatus.str().c_str(), wssUnit.str().c_str(), wssWeapon.str().c_str());
	}
	if (!stunOrKill)
	{
		wssKills << tr("STR_NO_KILLS");
		_lstKills->addRow(1, wssKills.str().c_str());
	}

	_txtKills->setText(tr("STR_KILLS").arg(count));
}

/**
 *
 */
SoldierDiaryMissionState::~SoldierDiaryMissionState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryMissionState::btnOkClick(Action *)
{
	_game->popState();
}

}
