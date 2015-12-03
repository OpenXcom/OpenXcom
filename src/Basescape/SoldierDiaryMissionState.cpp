/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "../Mod/Mod.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDiary.h"
#include "../Savegame/MissionStatistics.h"
#include "../Savegame/BattleUnitStatistics.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Diary Mission window.
 * @param soldier Pointer to the selected soldier.
 * @param rowEntry number to get mission info from.
 */
SoldierDiaryMissionState::SoldierDiaryMissionState(Soldier *soldier, int rowEntry) : _soldier(soldier), _rowEntry(rowEntry)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 272, 128, 24, 36, POPUP_BOTH);
	_btnOk = new TextButton(220, 16, 50, 140);
	_txtTitle = new Text(262, 9, 29, 44);
	_txtUFO = new Text(262, 9, 29, 52);
	_txtScore = new Text(180, 9, 29, 68);
	_txtKills = new Text(120, 9, 169, 68);
	_txtLocation = new Text(180, 9, 29, 76);
	_txtRace = new Text(120, 9, 169, 76);
	_txtDaylight = new Text(120, 9, 169, 84);
	_txtDaysWounded = new Text(180, 9, 29, 84);
	_txtNoRecord = new Text(240, 9, 29, 100);
	_lstKills = new TextList(240, 32, 29, 100);

	// Set palette
	setInterface("soldierMission");

	add(_window, "window", "soldierMission");
	add(_btnOk, "button", "soldierMission");
	add(_txtTitle, "text", "soldierMission");
	add(_txtUFO, "text", "soldierMission");
	add(_txtScore, "text", "soldierMission");
	add(_txtKills, "text", "soldierMission");
	add(_txtLocation, "text", "soldierMission");
	add(_txtRace, "text", "soldierMission");
	add(_txtDaylight, "text", "soldierMission");
	add(_txtDaysWounded, "text", "soldierMission");
	add(_txtNoRecord, "text", "soldierMission");
	add(_lstKills, "list", "soldierMission");

	centerAllSurfaces();

	// Set up objects
	std::vector<MissionStatistics*> *missionStatistics = _game->getSavedGame()->getMissionStatistics();
	unsigned int missionId = _soldier->getDiary()->getMissionIdList().at(_rowEntry);
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
	_txtTitle->setText(tr(missionStatistics->at(missionId)->type));

	_txtUFO->setAlign(ALIGN_CENTER);
	_txtUFO->setText(tr(missionStatistics->at(missionId)->ufo));
	_txtUFO->setVisible(missionStatistics->at(missionId)->ufo != "NO_UFO");

	_txtScore->setText(tr("STR_SCORE_VALUE").arg(missionStatistics->at(missionId)->score));

	_txtLocation->setText(tr("STR_LOCATION").arg(tr(missionStatistics->at(missionId)->getLocationString())));

	_txtRace->setText(tr("STR_RACE_TYPE").arg(tr(missionStatistics->at(missionId)->alienRace)));
	_txtRace->setVisible(missionStatistics->at(missionId)->alienRace != "STR_UNKNOWN");

	_txtDaylight->setText(tr("STR_DAYLIGHT_TYPE").arg(tr(missionStatistics->at(missionId)->getDaylightString())));

	_txtDaysWounded->setText(tr("STR_DAYS_WOUNDED").arg(daysWounded));
	_txtDaysWounded->setVisible(daysWounded != 0);

	_lstKills->setColumns(3, 60, 95, 85);

	int kills = 0;
    bool stunOrKill = false;

	for (std::vector<BattleUnitKills*>::iterator i = _soldier->getDiary()->getKills().begin() ; i != _soldier->getDiary()->getKills().end() ; ++i)
	{
		if ((unsigned int)(*i)->mission != missionId) continue;

		switch ((*i)->status)
		{
		case STATUS_DEAD:
			kills++;
		case STATUS_UNCONSCIOUS:
		case STATUS_PANICKING:
		case STATUS_TURNING:
			stunOrKill = true;
		default:
			break;
		}

		_lstKills->addRow(3, tr((*i)->getKillStatusString()).c_str(),
							 (*i)->getUnitName(_game->getLanguage()).c_str(),
							 tr((*i)->weapon).c_str());
	}

	_txtNoRecord->setAlign(ALIGN_CENTER);
	_txtNoRecord->setText(tr("STR_NO_RECORD"));
	_txtNoRecord->setVisible(!stunOrKill);

	_txtKills->setText(tr("STR_KILLS").arg(kills));
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
