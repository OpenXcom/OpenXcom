/*
 * Copyright 2010 OpenXcom Developers.
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
#include "CraftEquipmentState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
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

namespace OpenXcom
{

/**
 * Initializes all the elements in the Craft Equipment screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param craft ID of the selected craft.
 */
CraftEquipmentState::CraftEquipmentState(Game *game, Base *base, unsigned int craft) : State(game), _base(base), _craft(craft)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnOk = new TextButton(288, 16, 16, 176);
	_txtTitle = new Text(300, 16, 16, 7);
	_txtAvailable = new Text(110, 9, 16, 24);
	_txtUsed = new Text(110, 9, 130, 24);
	_lstEquipment = new TextList(288, 128, 8, 40);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(2)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtAvailable);
	add(_txtUsed);
	add(_lstEquipment);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+9);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK02.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CraftEquipmentState::btnOkClick);

	_txtTitle->setColor(Palette::blockOffset(15)+6);
	_txtTitle->setBig();
	Craft *c = _base->getCrafts()->at(_craft);
	std::wstringstream ss;
	ss << _game->getLanguage()->getString("STR_EQUIPMENT_FOR") << c->getName(_game->getLanguage());
	_txtTitle->setText(ss.str());

	_txtAvailable->setColor(Palette::blockOffset(15)+6);

	_txtUsed->setColor(Palette::blockOffset(15)+6);

	_lstEquipment->setColor(Palette::blockOffset(13)+10);
	_lstEquipment->setArrowColor(Palette::blockOffset(15)+9);
	_lstEquipment->setColumns(3, 114, 102, 64);
	_lstEquipment->setSelectable(true);
	_lstEquipment->setBackground(_window);
	_lstEquipment->setMargin(8);
	_lstEquipment->onMouseClick((ActionHandler)&CraftEquipmentState::lstEquipmentClick);
}

/**
 *
 */
CraftEquipmentState::~CraftEquipmentState()
{
}

/**
 * The soldier status can change
 * after clicking on it.
 */
void CraftEquipmentState::init()
{
	Craft *c = _base->getCrafts()->at(_craft);

	std::wstringstream ss;
	ss << _game->getLanguage()->getString("STR_SPACE_AVAILABLE") << c->getRules()->getSoldiers() - c->getNumSoldiers();
	_txtAvailable->setText(ss.str());

	std::wstringstream ss2;
	ss2 << _game->getLanguage()->getString("STR_SPACE_USED") << c->getNumSoldiers();
	_txtUsed->setText(ss2.str());

	int row = 0;
	_lstEquipment->clearList();
	for (std::vector<Soldier*>::iterator i = _base->getSoldiers()->begin(); i != _base->getSoldiers()->end(); i++)
	{
		std::wstringstream ss3;
		if ((*i)->getCraft() == 0)
			ss3 << _game->getLanguage()->getString("STR_NONE_UC");
		else
			ss3 << (*i)->getCraft()->getName(_game->getLanguage());
		_lstEquipment->addRow(3, (*i)->getName().c_str(), _game->getLanguage()->getString((*i)->getRankString()).c_str(), ss3.str().c_str());
		if ((*i)->getCraft() == c)
		{
			_lstEquipment->getCell(row, 0)->setColor(Palette::blockOffset(13));
			_lstEquipment->getCell(row, 1)->setColor(Palette::blockOffset(13));
			_lstEquipment->getCell(row, 2)->setColor(Palette::blockOffset(13));
		}
		else if ((*i)->getCraft() != 0)
		{
			_lstEquipment->getCell(row, 0)->setColor(Palette::blockOffset(15)+6);
			_lstEquipment->getCell(row, 1)->setColor(Palette::blockOffset(15)+6);
			_lstEquipment->getCell(row, 2)->setColor(Palette::blockOffset(15)+6);
		}
		row++;
	}
	_lstEquipment->draw();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CraftEquipmentState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * Shows the selected soldier's info.
 * @param action Pointer to an action.
 */
void CraftEquipmentState::lstEquipmentClick(Action *action)
{
	Craft *c = _base->getCrafts()->at(_craft);
	Soldier *s = _base->getSoldiers()->at(_lstEquipment->getSelectedRow());
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

}
