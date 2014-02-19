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
#include "SoldierDiaryKillsState.h"
#include <string>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Engine/SurfaceSet.h"
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
SoldierDiaryKillsState::SoldierDiaryKillsState(Game *game, Base *base, size_t soldier, SoldierDiaryState *soldierDiaryState, int display) : State(game), _base(base), _soldier(soldier), _soldierDiaryState(soldierDiaryState), _display(display)
{
	if (_display == 0)
    {
        _displayKills = true;
        _displayMissions = false;
		_displayCommendations = false;
    }
	else if (_display == 1)
    {
        _displayKills = false;
        _displayMissions = true;
		_displayCommendations = false;
    }
    else
    {
        _displayKills = false;
        _displayMissions = false;
		_displayCommendations = true;
    }

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnPrev = new TextButton(28, 14, 8, 8);
	_btnNext = new TextButton(28, 14, 284, 8);
    _btnKills = new TextButton(70, 16, 8, 176);
	_btnMissions = new TextButton(70, 16, 86, 176);
    _btnCommendations = new TextButton(70, 16, 164, 176);
    _btnOk = new TextButton(70, 16, 242, 176);
	_txtTitle = new Text(310, 16, 5, 8);
	// Kill stats
	_txtRace = new Text(90, 18, 16, 36);
	_txtRank = new Text(90, 18, 114, 36);
	_txtWeapon = new Text(90, 18, 212, 36);
	_lstRace = new TextList(90, 140, 16, 52);
	_lstRank = new TextList(90, 140, 114, 52);
	_lstWeapon = new TextList(90, 140, 212, 52);
	_lstKillTotals = new TextList(288, 9, 8, 167);
	// Mission stats
	_txtLocation = new Text(90, 18, 16, 36);
	_txtType = new Text(110, 18, 108, 36);
	_txtUFO = new Text(90, 18, 222, 36);
	_lstLocation = new TextList(90, 140, 16, 52);
	_lstType = new TextList(110, 140, 108, 52);
	_lstUFO = new TextList(90, 140, 222, 52);
	_lstMissionTotals = new TextList(296, 9, 8, 167);
    // Commendation stats
    _txtMedalName = new Text(90, 18, 16, 36);
    _txtMedalLevel = new Text(90, 18, 206, 36);
	_txtMedalInfo = new Text(280, 32, 20, 150);
    _lstCommendations = new TextList(240, 100, 48, 52);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnKills);
	add(_btnMissions);
    add(_btnCommendations);
	add(_btnPrev);
	add(_btnNext); 
	add(_txtTitle);
	add(_txtRace);
	add(_txtRank);
	add(_txtWeapon);
	add(_lstRace);
	add(_lstRank);
	add(_lstWeapon);
	add(_lstKillTotals);
	add(_txtLocation);
	add(_txtType);
	add(_txtUFO);
	add(_lstLocation);
	add(_lstType);
	add(_lstUFO);
	add(_lstMissionTotals);
    add(_txtMedalName);
    add(_txtMedalLevel);
	add(_txtMedalInfo);
    add(_lstCommendations);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK02.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierDiaryKillsState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierDiaryKillsState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_btnKills->setColor(Palette::blockOffset(13)+10);
	_btnKills->setText(tr("STR_KILLS_UC"));
	_btnKills->onMouseClick((ActionHandler)&SoldierDiaryKillsState::btnKillsToggle);
	
	_btnMissions->setColor(Palette::blockOffset(13)+10);
	_btnMissions->setText(tr("STR_MISSIONS_UC"));
	_btnMissions->onMouseClick((ActionHandler)&SoldierDiaryKillsState::btnMissionsToggle);

    _btnCommendations->setColor(Palette::blockOffset(13)+10);
	_btnCommendations->setText(tr("STR_AWARDS_UC"));
	_btnCommendations->onMouseClick((ActionHandler)&SoldierDiaryKillsState::btnCommendationsToggle);

	_btnPrev->setColor(Palette::blockOffset(15)+6);
	_btnPrev->setText(L"<<");
	_btnPrev->onMouseClick((ActionHandler)&SoldierDiaryKillsState::btnPrevClick);

	_btnNext->setColor(Palette::blockOffset(15)+6);
	_btnNext->setText(L">>");
	_btnNext->onMouseClick((ActionHandler)&SoldierDiaryKillsState::btnNextClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	// Text is decided in init()

	_txtRace->setColor(Palette::blockOffset(15)+1);
	_txtRace->setText(tr("STR_KILLS_BY_RACE"));
	_txtRace->setWordWrap(true);

	_txtRank->setColor(Palette::blockOffset(15)+1);
	_txtRank->setText(tr("STR_KILLS_BY_RANK"));
	_txtRank->setWordWrap(true);

	_txtWeapon->setColor(Palette::blockOffset(15)+1);
	_txtWeapon->setText(tr("STR_KILLS_BY_WEAPON"));
	_txtWeapon->setWordWrap(true);

	_lstRace->setColor(Palette::blockOffset(13)+10);
	_lstRace->setArrowColor(Palette::blockOffset(15)+1);
	_lstRace->setColumns(2, 80, 10);
	_lstRace->setBackground(_window);
	_lstRace->setDot(true);

	_lstRank->setColor(Palette::blockOffset(13)+10);
	_lstRank->setArrowColor(Palette::blockOffset(15)+1);
	_lstRank->setColumns(2, 80, 10);
	_lstRank->setBackground(_window);
	_lstRank->setDot(true);

	_lstWeapon->setColor(Palette::blockOffset(13)+10);
	_lstWeapon->setArrowColor(Palette::blockOffset(15)+1);
	_lstWeapon->setColumns(2, 80, 10);
	_lstWeapon->setBackground(_window);
	_lstWeapon->setDot(true);

	_lstKillTotals->setColor(Palette::blockOffset(15)+1);
	_lstKillTotals->setSecondaryColor(Palette::blockOffset(13)+10);
	_lstKillTotals->setColumns(2, 98, 98);
	_lstKillTotals->setMargin(8);
	_lstKillTotals->setBackground(_window);
	
	_txtLocation->setColor(Palette::blockOffset(15)+1);
	_txtLocation->setText(tr("STR_MISSIONS_BY_LOCATION"));
	_txtLocation->setWordWrap(true);
	
	_txtType->setColor(Palette::blockOffset(15)+1);
	_txtType->setText(tr("STR_MISSIONS_BY_TYPE"));
	_txtType->setWordWrap(true);
	
	_txtUFO->setColor(Palette::blockOffset(15)+1);
	_txtUFO->setText(tr("STR_MISSIONS_BY_UFO"));
	_txtUFO->setWordWrap(true);
	
	_lstLocation->setColor(Palette::blockOffset(13)+10);
	_lstLocation->setArrowColor(Palette::blockOffset(15)+1);
	_lstLocation->setColumns(2, 80, 10);
	_lstLocation->setBackground(_window);
	_lstLocation->setDot(true);
	
	_lstType->setColor(Palette::blockOffset(13)+10);
	_lstType->setArrowColor(Palette::blockOffset(15)+1);
	_lstType->setColumns(2, 100, 10);
	_lstType->setBackground(_window);
	_lstType->setDot(true);
	
	_lstUFO->setColor(Palette::blockOffset(13)+10);
	_lstUFO->setArrowColor(Palette::blockOffset(15)+1);
	_lstUFO->setColumns(2, 80, 10);
	_lstUFO->setBackground(_window);
	_lstUFO->setDot(true);

	_lstMissionTotals->setColor(Palette::blockOffset(15)+1);
	_lstMissionTotals->setSecondaryColor(Palette::blockOffset(13)+10);
	_lstMissionTotals->setColumns(4, 68, 68, 68, 84);
	_lstMissionTotals->setMargin(8);
	_lstMissionTotals->setBackground(_window);
	
	_txtMedalName->setColor(Palette::blockOffset(15)+1);
	_txtMedalName->setText(tr("STR_MEDAL_NAME"));
	_txtMedalName->setWordWrap(true);

	_txtMedalLevel->setColor(Palette::blockOffset(15)+1);
	_txtMedalLevel->setText(tr("STR_MEDAL_DECOR_LEVEL"));
	_txtMedalLevel->setWordWrap(true);

	_txtMedalInfo->setColor(Palette::blockOffset(13)+10);
	_txtMedalInfo->setWordWrap(true);

	_lstCommendations->setColor(Palette::blockOffset(13)+10);
	_lstCommendations->setArrowColor(Palette::blockOffset(15)+1);
	_lstCommendations->setColumns(2, 158, 80);
	_lstCommendations->setSelectable(true);
	_lstCommendations->setBackground(_window);
	_lstCommendations->onMouseOver((ActionHandler)&SoldierDiaryKillsState::lstInfoMouseOver);
	_lstCommendations->onMouseOut((ActionHandler)&SoldierDiaryKillsState::lstInfoMouseOut);

	init(); // Populate the list
}

/**
 *
 */
SoldierDiaryKillsState::~SoldierDiaryKillsState()
{

}

/**
 *  Clears all the variables and reinitializes the list of kills or missions for the soldier.
 *
 */
void SoldierDiaryKillsState::init()
{
	// Set visibility for kills
	_txtRace->setVisible(_displayKills);
	_txtRank->setVisible(_displayKills);
	_txtWeapon->setVisible(_displayKills);
	_lstRace->setVisible(_displayKills);
	_lstRank->setVisible(_displayKills);
	_lstWeapon->setVisible(_displayKills);
	_lstKillTotals->setVisible(_displayKills);
	// Set visibility for missions
	_txtLocation->setVisible(_displayMissions);
	_txtType->setVisible(_displayMissions);
	_txtUFO->setVisible(_displayMissions);
	_lstLocation->setVisible(_displayMissions);
	_lstType->setVisible(_displayMissions);
	_lstUFO->setVisible(_displayMissions);
	_lstMissionTotals->setVisible(_displayMissions);
	// Set visibility for commendations
    _txtMedalName->setVisible(_displayCommendations);
    _txtMedalLevel->setVisible(_displayCommendations);
	_txtMedalInfo->setVisible(_displayCommendations);
    _lstCommendations->setVisible(_displayCommendations);
	// Set visibility for buttons
	_btnKills->setVisible(!_displayKills);
	_btnMissions->setVisible(!_displayMissions);
	_btnCommendations->setVisible(!_displayCommendations);

	Soldier *s = _base->getSoldiers()->at(_soldier);
	int _missionTotal = s->getDiary()->getMissionTotal();
	int _winTotal = s->getDiary()->getWinTotal();
	int _scoreTotal = s->getDiary()->getScoreTotal();
	int _killTotal = s->getDiary()->getKillTotal();
	int _stunTotal = s->getDiary()->getStunTotal();
	int _daysWoundedTotal = s->getDiary()->getDaysWoundedTotal();
	_lstKillTotals->clearList();
	_lstMissionTotals->clearList();
	_commendationsListEntry.clear();
	_lstKillTotals->addRow(2, tr("STR_KILLS").arg(_killTotal).c_str(), tr("STR_STUNS").arg(_stunTotal).c_str());
	_lstMissionTotals->addRow(4, tr("STR_MISSIONS").arg(_missionTotal).c_str(), tr("STR_WINS").arg(_winTotal).c_str(), tr("STR_SCORE_VALUE").arg(_scoreTotal).c_str(), tr("STR_DAYS_WOUNDED").arg(_daysWoundedTotal).c_str());

	_txtTitle->setText(s->getName());
	_lstRace->clearList();
	_lstRank->clearList();
	_lstWeapon->clearList();
	std::map<std::string, int> _raceTotals = s->getDiary()->getAlienRaceTotal();
	std::map<std::string, int> _rankTotals = s->getDiary()->getAlienRankTotal();
	std::map<std::string, int> _weaponTotals = s->getDiary()->getWeaponTotal();
	_lstLocation->clearList();
	_lstType->clearList();
	_lstUFO->clearList();
	std::map<std::string, int> _locationTotals = s->getDiary()->getRegionTotal();
	std::map<std::string, int> _typeTotals = s->getDiary()->getTypeTotal();
	std::map<std::string, int> _UFOTotals = s->getDiary()->getUFOTotal();
    _lstCommendations->clearList();
	int row;

	for (std::map<std::string, int>::const_iterator i = _raceTotals.begin() ; i != _raceTotals.end() ; ++i)
	{
		std::wstringstream ss1, ss2;

		ss1 << tr((*i).first.c_str());
		ss2 << (*i).second;
		_lstRace->addRow(2, ss1.str().c_str(), ss2.str().c_str());
	}

	for (std::map<std::string, int>::const_iterator i = _rankTotals.begin() ; i != _rankTotals.end() ; ++i)
	{
		std::wstringstream ss1, ss2;

		ss1 << tr((*i).first.c_str());
		ss2 << (*i).second;
		_lstRank->addRow(2, ss1.str().c_str(), ss2.str().c_str());
	}

	for (std::map<std::string, int>::const_iterator i = _weaponTotals.begin() ; i != _weaponTotals.end() ; ++i)
	{
		std::wstringstream ss1, ss2;

		ss1 << tr((*i).first.c_str());
		ss2 << (*i).second;
		_lstWeapon->addRow(2, ss1.str().c_str(), ss2.str().c_str());
	}

	for (std::map<std::string, int>::const_iterator i = _locationTotals.begin() ; i != _locationTotals.end() ; ++i)
	{
		std::wstringstream ss1, ss2;

		ss1 << tr((*i).first.c_str());
		ss2 << (*i).second;
		_lstLocation->addRow(2, ss1.str().c_str(), ss2.str().c_str());
	}

	for (std::map<std::string, int>::const_iterator i = _typeTotals.begin() ; i != _typeTotals.end() ; ++i)
	{
		std::wstringstream ss1, ss2;

		ss1 << tr((*i).first.c_str());
		ss2 << (*i).second;
		_lstType->addRow(2, ss1.str().c_str(), ss2.str().c_str());
	}

	for (std::map<std::string, int>::const_iterator i = _UFOTotals.begin() ; i != _UFOTotals.end() ; ++i)
	{
		if ((*i).first == "NO_UFO") continue;
		std::wstringstream ss1, ss2;

		ss1 << tr((*i).first.c_str());
		ss2 << (*i).second;
		_lstUFO->addRow(2, ss1.str().c_str(), ss2.str().c_str());
	}
    
	row = 0;
    for (std::vector<SoldierCommendations*>::const_iterator i = s->getDiary()->getSoldierCommendations()->begin() ; i != s->getDiary()->getSoldierCommendations()->end() ; ++i)
	{
		std::wstringstream ss1, ss2, ss3;
		int _sprite = (*i)->getSprite();
		int _decorationSprite = (*i)->getDecorationSprite();
		Surface *_commendation = new Surface(31, 8, 16, 52 + 8*row);
		Surface *_commendationDecoration = new Surface(31, 8, 16, 52 + 8*row);
		

		if ((*i)->getNoun() != "")
		{
			ss1 << tr((*i)->getCommendationName().c_str()).arg(tr((*i)->getNoun()).c_str());
			ss3 << tr((*i)->getCommendationDescription().c_str()).arg(tr((*i)->getNoun()).c_str());
		}
		else
		{
			ss1 << tr((*i)->getCommendationName().c_str());
			ss3 << tr((*i)->getCommendationDescription().c_str());
		}
		ss2 << tr((*i)->getDecorationDescription().c_str());
		_lstCommendations->addRow(2, ss1.str().c_str(), ss2.str().c_str());

		_commendationsListEntry.push_back(ss3.str().c_str());

		// Handle commendation sprites
		add(_commendation);
		SurfaceSet *commendationSprite = _game->getResourcePack()->getSurfaceSet("Commendations");
		commendationSprite->getFrame(_sprite)->setX(0);
		commendationSprite->getFrame(_sprite)->setY(0);
		commendationSprite->getFrame(_sprite)->blit(_commendation);

		// Handle commendation decoration sprites
		if (_decorationSprite != 0)
		{
			add(_commendationDecoration);
			SurfaceSet *commendationDecoration = _game->getResourcePack()->getSurfaceSet("CommendationDecorations");
			commendationDecoration->getFrame(_decorationSprite)->setX(0);
			commendationDecoration->getFrame(_decorationSprite)->setY(0);
			commendationDecoration->getFrame(_decorationSprite)->blit(_commendationDecoration);
		}
		row++;
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierDiaryKillsState::btnOkClick(Action *)
{
	_soldierDiaryState->setSoldierId(_soldier);
	_game->popState();
}

/**
 * Goes to the previous soldier.
 * @param action Pointer to an action.
 */
void SoldierDiaryKillsState::btnPrevClick(Action *)
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
void SoldierDiaryKillsState::btnNextClick(Action *)
{
	_soldier++;
	if (_soldier >= _base->getSoldiers()->size())
		_soldier = 0;
	init();
}

/**
 * Display Kills totals.
 */
void SoldierDiaryKillsState::btnKillsToggle(Action *)
{
	_displayKills = true;
	_displayMissions = false;
    _displayCommendations = false;
	init();
}

/**
 * Display Missions totals.
 */
void SoldierDiaryKillsState::btnMissionsToggle(Action *)
{
	_displayKills = false;
	_displayMissions = true;
    _displayCommendations = false;
	
	init();
}

/**
 * Display Commendations.
 */
void SoldierDiaryKillsState::btnCommendationsToggle(Action *)
{
	_displayKills = false;
	_displayMissions = false;
    _displayCommendations = true;
	init();
}

/*
 *
 */
void SoldierDiaryKillsState::lstInfoMouseOver(Action *)
{
	size_t _sel;
	_sel = _lstCommendations->getSelectedRow();

	if ( _commendationsListEntry.empty() || _sel > _commendationsListEntry.size() - 1)
	{
		_txtMedalInfo->setText(L"");
	}
	else 
	{
		_txtMedalInfo->setText(_commendationsListEntry[_sel]);
	}
}

/*
 *  Clears the Medal information
 */
void SoldierDiaryKillsState::lstInfoMouseOut(Action *)
{
	_txtMedalInfo->setText(L"");
}

}