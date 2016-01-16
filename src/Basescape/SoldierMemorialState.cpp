/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "SoldierMemorialState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/SoldierDeath.h"
#include "../Savegame/GameTime.h"
#include "SoldierInfoState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Memorial screen.
 * @param game Pointer to the core game.
 */
SoldierMemorialState::SoldierMemorialState()
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(288, 16, 16, 176);
	_txtTitle = new Text(310, 17, 5, 8);
	_txtName = new Text(114, 9, 16, 36);
	_txtRank = new Text(102, 9, 130, 36);
	_txtDate = new Text(90, 9, 218, 36);
	_txtRecruited = new Text(150, 9, 16, 24);
	_txtLost = new Text(150, 9, 160, 24);
	_lstSoldiers = new TextList(288, 120, 8, 44);

	// Set palette
	setInterface("soldierMemorial");

	add(_window, "window", "soldierMemorial");
	add(_btnOk, "button", "soldierMemorial");
	add(_txtTitle, "text", "soldierMemorial");
	add(_txtName, "text", "soldierMemorial");
	add(_txtRank, "text", "soldierMemorial");
	add(_txtDate, "text", "soldierMemorial");
	add(_txtRecruited, "text", "soldierMemorial");
	add(_txtLost, "text", "soldierMemorial");
	add(_lstSoldiers, "list", "soldierMemorial");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK02.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierMemorialState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldierMemorialState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_MEMORIAL"));

	_txtName->setText(tr("STR_NAME_UC"));

	_txtRank->setText(tr("STR_RANK"));

	_txtDate->setText(tr("STR_DATE_UC"));

	size_t lost = _game->getSavedGame()->getDeadSoldiers()->size();
	size_t recruited = lost;
	for (std::vector<Base*>::iterator i = _game->getSavedGame()->getBases()->begin(); i != _game->getSavedGame()->getBases()->end(); ++i)
	{
		recruited += (*i)->getTotalSoldiers();
	}

	_txtRecruited->setText(tr("STR_SOLDIERS_RECRUITED").arg(recruited));

	_txtLost->setText(tr("STR_SOLDIERS_LOST").arg(lost));

	_lstSoldiers->setColumns(5, 114, 88, 30, 25, 35);
	_lstSoldiers->setSelectable(true);
	_lstSoldiers->setBackground(_window);
	_lstSoldiers->setMargin(8);
	_lstSoldiers->onMouseClick((ActionHandler)&SoldierMemorialState::lstSoldiersClick);

	for (std::vector<Soldier*>::reverse_iterator i = _game->getSavedGame()->getDeadSoldiers()->rbegin(); i != _game->getSavedGame()->getDeadSoldiers()->rend(); ++i)
	{
		SoldierDeath *death = (*i)->getDeath();

		std::wostringstream saveDay, saveMonth, saveYear;
		saveDay << death->getTime()->getDayString(_game->getLanguage());
		saveMonth << tr(death->getTime()->getMonthString());
		saveYear << death->getTime()->getYear();
		_lstSoldiers->addRow(5, (*i)->getName().c_str(), tr((*i)->getRankString()).c_str(), saveDay.str().c_str(), saveMonth.str().c_str(), saveYear.str().c_str());
	}
}

/**
 *
 */
SoldierMemorialState::~SoldierMemorialState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierMemorialState::btnOkClick(Action *)
{
	_game->popState();
	_game->getMod()->playMusic("GMGEO");
}

/**
 * Shows the selected soldier's info.
 * @param action Pointer to an action.
 */
void SoldierMemorialState::lstSoldiersClick(Action *)
{
	_game->pushState(new SoldierInfoState(0, _lstSoldiers->getSelectedRow()));
}

}
