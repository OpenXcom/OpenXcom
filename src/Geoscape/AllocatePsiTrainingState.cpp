/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include <sstream>
#include "PsiTrainingState.h"
#include "AllocatePsiTrainingState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Interface/TextList.h"
#include "GeoscapeState.h"
#include "../Savegame/Soldier.h"
#include "../Engine/Action.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Psi Training screen.
 * @param game Pointer to the core game.
 */
AllocatePsiTrainingState::AllocatePsiTrainingState(Game *game, Base *base) : State(game)
{
	_base = base;
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(300, 16, 76, 8);
	_txtName = new Text(64, 16, 18, 28);
	_txtCraft = new Text(64, 16, 186, 28);
	_txtTraining = new Text(48, 32, 266, 20);
	_btnOk = new TextButton(120, 16, 100, 176);
	_lstSoldiers = new TextList(287, 128, 16, 37);
	
	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtCraft);
	add(_txtName);
	add(_txtTitle);
	add(_txtTraining);
	add(_lstSoldiers);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&AllocatePsiTrainingState::btnOkClick);
	
	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setText(base->getName());

	_txtTraining->setColor(Palette::blockOffset(15)+1);
	_txtTraining->setText(_game->getLanguage()->getString("STR_IN_TRAINING"));
	
	_txtName->setColor(Palette::blockOffset(15)+1);
	_txtName->setText(_game->getLanguage()->getString("STR_NAME"));
	
	_txtCraft->setColor(Palette::blockOffset(15)+1);
	_txtCraft->setText(_game->getLanguage()->getString("STR_CRAFT"));

	_lstSoldiers->setColor(Palette::blockOffset(13));
	_lstSoldiers->setArrowColumn(-1, ARROW_VERTICAL);
	_lstSoldiers->setColumns(3, 168, 80, 40);
	_lstSoldiers->setSelectable(true);
	_lstSoldiers->setBackground(_window);
	_lstSoldiers->setMargin(2);
	_lstSoldiers->onMousePress((ActionHandler)&AllocatePsiTrainingState::lstSoldiersPress);
	_lstSoldiers->onMouseClick((ActionHandler)&AllocatePsiTrainingState::lstSoldiersClick);
	_lstSoldiers->onMouseRelease((ActionHandler)&AllocatePsiTrainingState::lstSoldiersRelease);
	for(std::vector<Soldier*>::const_iterator s = base->getSoldiers()->begin(); s != base->getSoldiers()->end(); ++s)
	{
		_soldiers.push_back(*s);
		if((*s)->isInPsiTraining())
		{
			_lstSoldiers->addRow(3, (*s)->getName().c_str(), (*s)->getCraftString(_game->getLanguage()).c_str(),  _game->getLanguage()->getString("STR_YES").c_str());
		}
		else
		{
			_lstSoldiers->addRow(3, (*s)->getName().c_str(), (*s)->getCraftString(_game->getLanguage()).c_str(), _game->getLanguage()->getString("STR_NO").c_str());
		}
	}
}
/**
 *
 */
AllocatePsiTrainingState::~AllocatePsiTrainingState()
{

}

/**
 * Resets the palette.
 */
void AllocatePsiTrainingState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void AllocatePsiTrainingState::btnOkClick(Action *action)
{
	_game->popState();
	_game->pushState (new PsiTrainingState(_game));
}

void AllocatePsiTrainingState::lstSoldiersPress(Action *action)
{
}
void AllocatePsiTrainingState::lstSoldiersRelease(Action *action)
{
}
void AllocatePsiTrainingState::lstSoldiersClick(Action *action)
{
	_sel = _lstSoldiers->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (_base->getSoldiers()->at(_sel)->getWoundRecovery() == 0)
		{
			_base->getSoldiers()->at(_sel)->setPsiTraining();
			if(_base->getSoldiers()->at(_sel)->isInPsiTraining())
			{
				if(_base->getUsedPsiLabs() < _base->getAvailablePsiLabs())
				{
					_lstSoldiers->setCellText(_sel, 2, _game->getLanguage()->getString("STR_YES").c_str());
				}
			}
			else
			{
			_lstSoldiers->setCellText(_sel, 2, _game->getLanguage()->getString("STR_NO").c_str());
			}
		}
	}
}
}
