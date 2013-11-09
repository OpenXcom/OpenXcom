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
SoldierDiaryKillsState::SoldierDiaryKillsState(Game *game, Base *base, size_t soldier, SoldierDiaryState *soldierDiaryState, bool displayKills) : State(game), _base(base), _soldier(soldier), _soldierDiaryState(soldierDiaryState), _displayKills(displayKills)
{
	if (_displayKills == true) _displayMissions = false;
	else _displayMissions = true;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(96, 16, 216, 176);
	_btnPrev = new TextButton(28, 14, 8, 8);
	_btnNext = new TextButton(28, 14, 284, 8);
	_btnKills = new TextButton(96, 16, 8, 176);
	_btnMissions = new TextButton(96, 16, 112, 176);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtRace = new Text(76, 9, 16, 36);
	_txtRank = new Text(76, 9, 114, 36);
	_txtWeapon = new Text(76, 9, 212, 36);
	_lstRace = new TextList(90, 140, 16, 44);
	_lstRank = new TextList(90, 140, 114, 44);
	_lstWeapon = new TextList(90, 140, 212, 44);
	_txtLocation = new Text(76, 18, 16, 36);
	_txtType = new Text(76, 18, 104, 36);
	_txtUFO = new Text(76, 18, 222, 36);
	_lstLocation = new TextList(90, 140, 16, 52);
	_lstType = new TextList(110, 140, 104, 52);
	_lstUFO = new TextList(90, 140, 222, 52);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnKills);
	add(_btnMissions);
	add(_btnPrev);
	add(_btnNext); 
	add(_txtTitle);
	add(_txtRace);
	add(_txtRank);
	add(_txtWeapon);
	add(_lstRace);
	add(_lstRank);
	add(_lstWeapon);
	add(_txtLocation);
	add(_txtType);
	add(_txtUFO);
	add(_lstLocation);
	add(_lstType);
	add(_lstUFO);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK02.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierDiaryKillsState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierDiaryKillsState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_btnKills->setColor(Palette::blockOffset(13)+10);
	_btnKills->setText(tr("STR_TOTAL_KILLS"));
	_btnKills->onMouseClick((ActionHandler)&SoldierDiaryKillsState::btnKillsToggle);
	
	_btnMissions->setColor(Palette::blockOffset(13)+10);
	_btnMissions->setText(tr("STR_TOTAL_MISSIONS"));
	_btnMissions->onMouseClick((ActionHandler)&SoldierDiaryKillsState::btnMissionsToggle);

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

	_txtRank->setColor(Palette::blockOffset(15)+1);
	_txtRank->setText(tr("STR_KILLS_BY_RANK"));
	
	_txtWeapon->setColor(Palette::blockOffset(15)+1);
	_txtWeapon->setText(tr("STR_KILLS_BY_WEAPON"));

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
	_lstLocation->setColumns(2, 70, 10);
	_lstLocation->setBackground(_window);
	_lstLocation->setDot(true);
	
	_lstType->setColor(Palette::blockOffset(13)+10);
	_lstType->setArrowColor(Palette::blockOffset(15)+1);
	_lstType->setColumns(2, 100, 10);
	_lstType->setBackground(_window);
	_lstType->setDot(true);
	
	_lstUFO->setColor(Palette::blockOffset(13)+10);
	_lstUFO->setArrowColor(Palette::blockOffset(15)+1);
	_lstUFO->setColumns(2, 70, 10);
	_lstUFO->setBackground(_window);
	_lstUFO->setDot(true);

	init(); // Populate the list
}

/**
 *
 */
SoldierDiaryKillsState::~SoldierDiaryKillsState()
{

}

/**
 *  Clears all the variables and reinitializes the list of medals for the soldier.
 *
 */
void SoldierDiaryKillsState::init()
{
	// Set visibility
	_txtRace->setVisible(_displayKills);
	_txtRank->setVisible(_displayKills);
	_txtWeapon->setVisible(_displayKills);
	_lstRace->setVisible(_displayKills);
	_lstRank->setVisible(_displayKills);
	_lstWeapon->setVisible(_displayKills);

	_txtLocation->setVisible(_displayMissions);
	_txtType->setVisible(_displayMissions);
	_txtUFO->setVisible(_displayMissions);
	_lstLocation->setVisible(_displayMissions);
	_lstType->setVisible(_displayMissions);
	_lstUFO->setVisible(_displayMissions);

	Soldier *s = _base->getSoldiers()->at(_soldier);
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
	init();
}

/**
 * Display Missions totals.
 */
void SoldierDiaryKillsState::btnMissionsToggle(Action *)
{
	_displayKills = false;
	_displayMissions = true;
	init();
}
}