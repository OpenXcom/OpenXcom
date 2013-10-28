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
#include "SoldiersState.h"
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
#include "../Ruleset/RuleCraft.h"
#include "SoldierInfoState.h"
#include "SoldierMemorialState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldiers screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
SoldiersState::SoldiersState(Game *game, Base *base) : State(game), _base(base)
{
	bool isPsiBtnVisible = Options::getBool("anytimePsiTraining") && _base->getAvailablePsiLabs() > 0;

	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	if (isPsiBtnVisible)
	{
		_btnOk = new TextButton(96, 16, 216, 176);
		_btnPsiTraining = new TextButton(96, 16, 112, 176);
		_btnMemorial = new TextButton(96, 16, 8, 176);
	}
	else
	{
		_btnOk = new TextButton(148, 16, 164, 176);
		_btnPsiTraining = new TextButton(148, 16, 164, 176);
		_btnMemorial = new TextButton(148, 16, 8, 176);
	}
	_txtTitle = new Text(310, 17, 5, 8);
	_txtName = new Text(114, 9, 16, 32);
	_txtRank = new Text(102, 9, 130, 32);
	_txtCraft = new Text(82, 9, 222, 32);
	_lstSoldiers = new TextList(288, 128, 8, 40);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnPsiTraining);
	add(_btnMemorial);
	add(_txtTitle);
	add(_txtName);
	add(_txtRank);
	add(_txtCraft);
	add(_lstSoldiers);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK02.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldiersState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&SoldiersState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_btnPsiTraining->setColor(Palette::blockOffset(13)+10);
	_btnPsiTraining->setText(tr("STR_PSIONIC_TRAINING"));
	_btnPsiTraining->onMouseClick((ActionHandler)&SoldiersState::btnPsiTrainingClick);
	_btnPsiTraining->setVisible(isPsiBtnVisible);

	_btnMemorial->setColor(Palette::blockOffset(13)+10);
	_btnMemorial->setText(tr("STR_MEMORIAL"));
	_btnMemorial->onMouseClick((ActionHandler)&SoldiersState::btnMemorialClick);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_SOLDIER_LIST"));

	_txtName->setColor(Palette::blockOffset(15)+1);
	_txtName->setText(tr("STR_NAME_UC"));

	_txtRank->setColor(Palette::blockOffset(15)+1);
	_txtRank->setText(tr("STR_RANK"));

	_txtCraft->setColor(Palette::blockOffset(15)+1);
	_txtCraft->setText(tr("STR_CRAFT"));

	_lstSoldiers->setColor(Palette::blockOffset(13)+10);
	_lstSoldiers->setArrowColor(Palette::blockOffset(15)+1);
	_lstSoldiers->setColumns(3, 114, 92, 74);
	_lstSoldiers->setSelectable(true);
	_lstSoldiers->setBackground(_window);
	_lstSoldiers->setMargin(8);
	_lstSoldiers->onMouseClick((ActionHandler)&SoldiersState::lstSoldiersClick);
}

/**
 *
 */
SoldiersState::~SoldiersState()
{

}

/**
 * Updates the soldiers list
 * after going to other screens.
 */
void SoldiersState::init()
{
	int row = 0;
	_lstSoldiers->clearList();
	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); ++i)
	{
		_lstSoldiers->addRow(3, (*i)->getName().c_str(), tr((*i)->getRankString()).c_str(), (*i)->getCraftString(_game->getLanguage()).c_str());
		if ((*i)->getCraft() == 0)
		{
			_lstSoldiers->setRowColor(row, Palette::blockOffset(15)+6);
		}
		row++;
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldiersState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Opens the Psionic Training screen.
 * @param action Pointer to an action.
 */
void SoldiersState::btnPsiTrainingClick(Action *)
{
	_game->pushState(new AllocatePsiTrainingState(_game, _base));
}

/**
 * Opens the Memorial screen.
 * @param action Pointer to an action.
 */
void SoldiersState::btnMemorialClick(Action *)
{
	_game->pushState(new SoldierMemorialState(_game));
}

/**
 * Shows the selected soldier's info.
 * @param action Pointer to an action.
 */
void SoldiersState::lstSoldiersClick(Action *)
{
	_game->pushState(new SoldierInfoState(_game, _base, _lstSoldiers->getSelectedRow()));
}

}
