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
#include "SoldierDiaryInfoState.h"
#include "../Savegame/SoldierDiary.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/Armor.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Armor window.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param soldier ID of the selected soldier.
 */
SoldierDiaryInfoState::SoldierDiaryInfoState(Game *game, Base *base, size_t soldier, int rowEntry) : State(game), _base(base), _soldier(soldier), _rowEntry(rowEntry)
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
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(4)), Palette::backPos, 16);

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
	_window->setColor(Palette::blockOffset(13)+10);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierDiaryInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierDiaryInfoState::btnOkClick, Options::keyCancel);

	_txtTitle->setColor(Palette::blockOffset(13)+5);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_MISSION_DETAILS"));

	Soldier *s = _base->getSoldiers()->at(_soldier);
	std::vector<SoldierDiaryEntries*> _soldierDiaryEntries = s->getDiary()->getSoldierDiaryEntries();
	int _score = _soldierDiaryEntries[_rowEntry]->getMissionScore();
	int _kills = _soldierDiaryEntries[_rowEntry]->getMissionKillTotal();
	int _daysWounded = _soldierDiaryEntries[_rowEntry]->getDaysWounded();
	std::string _missionType = _soldierDiaryEntries[_rowEntry]->getMissionType();
	std::string _UFO = _soldierDiaryEntries[_rowEntry]->getMissionUFO();
	std::string _missionRace = _soldierDiaryEntries[_rowEntry]->getMissionRace();

	_txtScore->setColor(Palette::blockOffset(13)+5);
	_txtScore->setSecondaryColor(Palette::blockOffset(13));
	_txtScore->setAlign(ALIGN_LEFT);
	_txtScore->setText(tr("STR_SCORE_VALUE").arg(_score));

	_txtKills->setColor(Palette::blockOffset(13)+5);
	_txtKills->setSecondaryColor(Palette::blockOffset(13));
	_txtKills->setAlign(ALIGN_LEFT);
	_txtKills->setText(tr("STR_KILLS").arg(_kills));

	_txtMissionType->setColor(Palette::blockOffset(13)+5);
	_txtMissionType->setSecondaryColor(Palette::blockOffset(13));
	_txtMissionType->setText(tr("STR_MISSION_TYPE").arg(tr(_missionType)));

	_txtUFO->setColor(Palette::blockOffset(13)+5);
	_txtUFO->setSecondaryColor(Palette::blockOffset(13));
	_txtUFO->setText(tr("STR_UFO_TYPE").arg(tr(_UFO)));
	_txtUFO->setVisible(true);
	if (_UFO == "NO_UFO") _txtUFO->setVisible(false);

	_txtRace->setColor(Palette::blockOffset(13)+5);
	_txtRace->setSecondaryColor(Palette::blockOffset(13));
	_txtRace->setText(tr("STR_RACE_TYPE").arg(tr(_missionRace)));
	_txtRace->setVisible(true);
	if (_missionRace == "STR_UNKNOWN") _txtUFO->setVisible(false);

	int _time = _soldierDiaryEntries[_rowEntry]->getMissionDaylight();

	_txtDaylight->setColor(Palette::blockOffset(13)+5);
	_txtDaylight->setSecondaryColor(Palette::blockOffset(13));
	if (_time == 0)
	{
		_txtDaylight->setText(tr("STR_DAYLIGHT_TYPE").arg(tr("STR_DAY")));
	}
	else
	{
		_txtDaylight->setText(tr("STR_DAYLIGHT_TYPE").arg(tr("STR_NIGHT")));
	}

	_txtDaysWounded->setColor(Palette::blockOffset(13)+5);
	_txtDaysWounded->setSecondaryColor(Palette::blockOffset(13));
	_txtDaysWounded->setText(tr("STR_DAYS_WOUNDED").arg(_daysWounded));
	if (_daysWounded == 0) _txtDaysWounded->setVisible(false);

	_lstKills->setColor(Palette::blockOffset(13));
	_lstKills->setArrowColor(Palette::blockOffset(13)+5);
	_lstKills->setColumns(3, 40, 90, 80);
	_lstKills->setSelectable(false);
	_lstKills->setBackground(_window);
	_lstKills->setMargin(8);

	std::vector<SoldierDiaryKills*> _missionKills = _soldierDiaryEntries[_rowEntry]->getMissionKills();
	std::wstringstream Kills;

	if (_missionKills.empty())
	{
		Kills << tr("STR_NO_KILLS");
		_lstKills->addRow(1, Kills.str().c_str());
	}
	else
	{
		for (std::vector<SoldierDiaryKills*>::iterator j = _missionKills.begin() ; j != _missionKills.end() ; ++j)
		{
			SoldierDiaryKills *_kills = (*j);
			std::wstringstream Rank, Race, Weapon, Ammo;

			Race << tr(_kills->getAlienRace().c_str());
			Rank << tr(_kills->getAlienRank().c_str());
			Weapon << tr(_kills->getWeapon().c_str());

			std::wstringstream Unit, Status;
			Unit << Race.str().c_str() << " " << Rank.str().c_str();

			if (_kills->getAlienState() == "STATUS_DEAD")
			{
				Status << tr("STR_KILLED").c_str();
			}
			else
			{
				Status << tr("STR_STUNNED").c_str();
			}

			_lstKills->addRow(3, Status.str().c_str(), Unit.str().c_str(), Weapon.str().c_str());
		}
	}

	
}

/**
 *
 */
SoldierDiaryInfoState::~SoldierDiaryInfoState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryInfoState::btnOkClick(Action *)
{
	_game->popState();
}

}
