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
#include "../Resource/ResourcePack.h"
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
SoldierDiaryMissionState::SoldierDiaryMissionState(Game *game, Base *base, size_t soldier, int rowEntry) : State(game), _base(base), _soldier(soldier), _rowEntry(rowEntry)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 232, 128, 44, 36, POPUP_BOTH);
	_btnOk = new TextButton(180, 16, 70, 140);
	_txtTitle = new Text(222, 9, 49, 44);
	_txtScore = new Text(222, 9, 49, 62);
	_txtMissionType= new Text(222, 9, 49, 71);
	_txtUFO = new Text(222, 9, 49, 80);
	_txtKills = new Text(130, 9, 187, 62);
	_txtRace = new Text(130, 9, 187, 71);
	_txtDaylight = new Text(130, 9, 187, 80);
	_txtDaysWounded = new Text(130, 9, 187, 89);
	_lstKills = new TextList(200, 32, 49, 102);

	// Set palette
	setPalette("PAL_BASESCAPE");

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtScore);
	add(_txtKills);
	add(_txtMissionType);
	add(_txtUFO);
	add(_txtRace);
	add(_txtDaylight);
	add(_txtDaysWounded);
	add(_lstKills);

	centerAllSurfaces();

	// Set up objects
	Soldier *s = _base->getSoldiers()->at(_soldier);
	std::vector<MissionStatistics*> *missionStatistics = _game->getSavedGame()->getMissionStatistics();
    int missionId = s->getDiary()->getMissionIdList().at(_rowEntry);
	if (missionId > missionStatistics->size())
	{
		missionId = 0;
	}

    int daysWounded = 0;
    for (std::vector<std::pair<int, int> >::const_iterator wound = s->getDiary()->getDaysWounded().begin() ; wound != s->getDiary()->getDaysWounded().end(); ++wound)
    {
        if (wound->first == missionId)
        {
            daysWounded = wound->second;
            break;
        }
    }
    
	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierDiaryMissionState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierDiaryMissionState::btnOkClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(13)+5);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_MISSION_DETAILS"));

	_txtScore->setColor(Palette::blockOffset(13)+5);
	_txtScore->setSecondaryColor(Palette::blockOffset(13));
	_txtScore->setAlign(ALIGN_LEFT);
	_txtScore->setText(tr("STR_SCORE_VALUE").arg(missionStatistics->at(missionId)->score));

	_txtMissionType->setColor(Palette::blockOffset(13)+5);
	_txtMissionType->setSecondaryColor(Palette::blockOffset(13));
	_txtMissionType->setText(tr("STR_MISSION_TYPE").arg(tr(missionStatistics->at(missionId)->getMissionTypeLowerCase())));

	_txtUFO->setColor(Palette::blockOffset(13)+5);
	_txtUFO->setSecondaryColor(Palette::blockOffset(13));
	_txtUFO->setText(tr("STR_UFO_TYPE").arg(tr(missionStatistics->at(missionId)->ufo)));
	_txtUFO->setVisible(true);
	if (missionStatistics->at(missionId)->ufo == "NO_UFO") _txtUFO->setVisible(false);

	_txtRace->setColor(Palette::blockOffset(13)+5);
	_txtRace->setSecondaryColor(Palette::blockOffset(13));
	_txtRace->setText(tr("STR_RACE_TYPE").arg(tr(missionStatistics->at(missionId)->alienRace)));
	_txtRace->setVisible(true);
	if (missionStatistics->at(missionId)->alienRace == "STR_UNKNOWN") _txtUFO->setVisible(false);

	_txtDaylight->setColor(Palette::blockOffset(13)+5);
	_txtDaylight->setSecondaryColor(Palette::blockOffset(13));
	if (missionStatistics->at(missionId)->daylight == 0)
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
	if (daysWounded == 0) _txtDaysWounded->setVisible(false);

	_lstKills->setColor(Palette::blockOffset(13));
	_lstKills->setArrowColor(Palette::blockOffset(13)+5);
	_lstKills->setColumns(3, 40, 90, 80);
	_lstKills->setSelectable(false);
	_lstKills->setBackground(_window);
	_lstKills->setMargin(8);

	int count = 0;
    bool stunOrKill = false;
	std::wstringstream wssKills;

	for (std::vector<BattleUnitKills*>::iterator j = s->getDiary()->getKills().begin() ; j != s->getDiary()->getKills().end() ; ++j)
	{
		if ((*j)->mission != missionId) continue;
		std::wstringstream wssRank, wssRace, wssWeapon, wssAmmo;

		wssRace << tr((*j)->race.c_str());
		wssRank << tr((*j)->rank.c_str());
		wssWeapon << tr((*j)->weapon.c_str());

		std::wstringstream wssUnit, wssStatus;
		wssUnit << wssRace.str().c_str() << " " << wssRank.str().c_str();

		if ((*j)->getUnitStatusString() == "STATUS_DEAD")
		{
			wssStatus << tr("STR_KILLED").c_str();
            count++;
            stunOrKill = true;
		}
		else
		{
			wssStatus << tr("STR_STUNNED").c_str();
            stunOrKill = true;
		}

		_lstKills->addRow(3, wssStatus.str().c_str(), wssUnit.str().c_str(), wssWeapon.str().c_str());
	}
	if (!stunOrKill)
	{
		wssKills << tr("STR_NO_KILLS");
		_lstKills->addRow(1, wssKills.str().c_str());
	}

	_txtKills->setColor(Palette::blockOffset(13)+5);
	_txtKills->setSecondaryColor(Palette::blockOffset(13));
	_txtKills->setAlign(ALIGN_LEFT);
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
