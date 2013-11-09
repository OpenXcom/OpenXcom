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
#include "SoldierDiaryMissionsState.h"
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
SoldierDiaryMissionsState::SoldierDiaryMissionsState(Game *game, Base *base, size_t soldier, SoldierDiaryState *soldierDiaryState) : State(game), _base(base), _soldier(soldier), _soldierDiaryState(soldierDiaryState)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(96, 16, 216, 176);
	_btnPrev = new TextButton(28, 14, 8, 8);
	_btnNext = new TextButton(28, 14, 284, 8);
	_txtTitle = new Text(310, 16, 5, 8);
	_txtLocation = new Text(76, 9, 16, 36);
	_txtType = new Text(76, 9, 104, 36);
	_txtUFO = new Text(76, 9, 222, 36);
	_lstLocation = new TextList(90, 140, 16, 44);
	_lstType = new TextList(110, 140, 104, 44);
	_lstUFO = new TextList(90, 140, 222, 44);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnPrev);
	add(_btnNext); 
	add(_txtTitle);
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
	_btnOk->onMouseClick((ActionHandler)&SoldierDiaryMissionsState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierDiaryMissionsState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_btnPrev->setColor(Palette::blockOffset(15)+6);
	_btnPrev->setText(L"<<");
	_btnPrev->onMouseClick((ActionHandler)&SoldierDiaryMissionsState::btnPrevClick);

	_btnNext->setColor(Palette::blockOffset(15)+6);
	_btnNext->setText(L">>");
	_btnNext->onMouseClick((ActionHandler)&SoldierDiaryMissionsState::btnNextClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	// Text is decided in init()

	_txtLocation->setColor(Palette::blockOffset(15)+1);
	_txtLocation->setText(tr("STR_MISSIONS_BY_LOCATION"));

	_txtType->setColor(Palette::blockOffset(15)+1);
	_txtType->setText(tr("STR_MISSIONS_BY_TYPE"));

	_txtUFO->setColor(Palette::blockOffset(15)+1);
	_txtUFO->setText(tr("STR_MISSIONS_BY_UFO"));

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
SoldierDiaryMissionsState::~SoldierDiaryMissionsState()
{

}

/**
 *  Clears all the variables and reinitializes the list of medals for the soldier.
 *
 */
void SoldierDiaryMissionsState::init()
{
	Soldier *s = _base->getSoldiers()->at(_soldier);
	_txtTitle->setText(s->getName());
	_lstLocation->clearList();
	_lstType->clearList();
	_lstUFO->clearList();
	std::map<std::string, int> _locationTotals = s->getDiary()->getRegionTotal();
	std::map<std::string, int> _typeTotals = s->getDiary()->getTypeTotal();
	std::map<std::string, int> _UFOTotals = s->getDiary()->getUFOTotal();

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
void SoldierDiaryMissionsState::btnOkClick(Action *)
{
	_soldierDiaryState->setSoldierId(_soldier);
	_game->popState();
}

/**
 * Goes to the previous soldier.
 * @param action Pointer to an action.
 */
void SoldierDiaryMissionsState::btnPrevClick(Action *)
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
void SoldierDiaryMissionsState::btnNextClick(Action *)
{
	_soldier++;
	if (_soldier >= _base->getSoldiers()->size())
		_soldier = 0;
	init();
}

}