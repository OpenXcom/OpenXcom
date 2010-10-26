/*
 * Copyright 2010 Daniel Albano
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
#include "CraftSoldiersState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Resource/LangString.h"
#include "../Engine/Font.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Savegame/Base.h"
#include "../Savegame/Soldier.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"

/**
 * Initializes all the elements in the Craft Soldiers screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param craft ID of the selected craft.
 */
CraftSoldiersState::CraftSoldiersState(Game *game, Base *base, unsigned int craft) : State(game), _base(base), _craft(craft)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 288, 16, 16, 176);
	_txtTitle = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 300, 16, 16, 7);
	_txtName = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 114, 9, 16, 32);
	_txtRank = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 102, 9, 130, 32);
	_txtCraft = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 82, 9, 232, 32);
	_txtAvailable = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 110, 9, 16, 24);
	_txtUsed = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 110, 9, 130, 24);
	_lstSoldiers = new TextList(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 288, 128, 8, 40);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtName);
	add(_txtRank);
	add(_txtCraft);
	add(_txtAvailable);
	add(_txtUsed);
	add(_lstSoldiers);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+9);
	_window->setBackground(game->getResourcePack()->getSurface("BACK02.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((ActionHandler)&CraftSoldiersState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)+6);
	_txtTitle->setBig();
	Craft *c = _base->getCrafts()->at(_craft);
	std::stringstream ss;
	ss << _game->getResourcePack()->getLanguage()->getString(STR_SELECT_SQUAD_FOR) << c->getName(_game->getResourcePack()->getLanguage());
	_txtTitle->setText(ss.str());

	_txtName->setColor(Palette::blockOffset(15)+6);
	_txtName->setText(_game->getResourcePack()->getLanguage()->getString(STR_NAME));

	_txtRank->setColor(Palette::blockOffset(15)+6);
	_txtRank->setText(_game->getResourcePack()->getLanguage()->getString(STR_RANK));

	_txtCraft->setColor(Palette::blockOffset(15)+6);
	_txtCraft->setText(_game->getResourcePack()->getLanguage()->getString(STR_CRAFT));

	_txtAvailable->setColor(Palette::blockOffset(15)+6);

	_txtUsed->setColor(Palette::blockOffset(15)+6);

	_lstSoldiers->setColor(Palette::blockOffset(13)+10);
	_lstSoldiers->setArrowColor(Palette::blockOffset(15)+9);
	_lstSoldiers->setColumns(3, 114, 102, 64);
	_lstSoldiers->setSelectable(true);
	_lstSoldiers->setBackground(_window);
	_lstSoldiers->setMargin(8);
	_lstSoldiers->onMouseClick((ActionHandler)&CraftSoldiersState::lstSoldiersClick);
}

/**
 *
 */
CraftSoldiersState::~CraftSoldiersState()
{
}

/**
 * The soldier status can change
 * after clicking on it.
 */
void CraftSoldiersState::init()
{
	Craft *c = _base->getCrafts()->at(_craft);

	std::stringstream ss;
	ss << _game->getResourcePack()->getLanguage()->getString(STR_SPACE_AVAILABLE) << c->getRules()->getSoldiers() - c->getNumSoldiers();
	_txtAvailable->setText(ss.str());

	std::stringstream ss2;
	ss2 << _game->getResourcePack()->getLanguage()->getString(STR_SPACE_USED) << c->getNumSoldiers();
	_txtUsed->setText(ss2.str());

	int row = 0;
	_lstSoldiers->clearList();
	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); i++)
	{
		std::stringstream ss3;
		if ((*i)->getCraft() == 0)
			ss3 << _game->getResourcePack()->getLanguage()->getString(STR_NONE);
		else
			ss3 << (*i)->getCraft()->getName(_game->getResourcePack()->getLanguage());
		_lstSoldiers->addRow(3, (*i)->getName().c_str(), _game->getResourcePack()->getLanguage()->getString((*i)->getRankString()).c_str(), ss3.str().c_str());
		if ((*i)->getCraft() == c)
		{
			_lstSoldiers->getCell(row, 0)->setColor(Palette::blockOffset(13));
			_lstSoldiers->getCell(row, 1)->setColor(Palette::blockOffset(13));
			_lstSoldiers->getCell(row, 2)->setColor(Palette::blockOffset(13));
		}
		else if ((*i)->getCraft() != 0)
		{
			_lstSoldiers->getCell(row, 0)->setColor(Palette::blockOffset(15)+6);
			_lstSoldiers->getCell(row, 1)->setColor(Palette::blockOffset(15)+6);
			_lstSoldiers->getCell(row, 2)->setColor(Palette::blockOffset(15)+6);
		}
		row++;
	}
	_lstSoldiers->draw();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CraftSoldiersState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Shows the selected soldier's info.
 * @param action Pointer to an action.
 */
void CraftSoldiersState::lstSoldiersClick(Action *action)
{
	Craft *c = _base->getCrafts()->at(_craft);
	Soldier *s = _base->getSoldiers()->at(_lstSoldiers->getSelectedRow());
	if (s->getCraft() == c)
	{
		s->setCraft(0);
	}
	else
	{
		s->setCraft(c);
	}
	init();
}
