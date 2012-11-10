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
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "GeoscapeState.h"
#include "AllocatePsiTrainingState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Psi Training screen.
 * @param game Pointer to the core game.
 */
PsiTrainingState::PsiTrainingState(Game *game) : State(game), _base1(0), _base2(0), _base3(0), _base4(0), _base5(0), _base6(0), _base7(0), _base8(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(300, 16, 16, 8);
	_btnOk = new TextButton(120, 16, 100, 176);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&PsiTrainingState::btnOkClick);
	
	_txtTitle->setColor(Palette::blockOffset(8)+5);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(_game->getLanguage()->getString("STR_PSIONIC_TRAINING"));
	int buttons = 0;
	for(std::vector<Base*>::const_iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end(); ++b)
	{
		if((*b)->getAvailablePsiLabs())
		{
			buttons++;
			switch (buttons)
			{
			case 1:
				_btnBase1 = new TextButton(120, 16, 100, 26);
				_btnBase1->setColor(Palette::blockOffset(15)+6);
				_btnBase1->onMouseClick((ActionHandler)&PsiTrainingState::btnBase1Click);
				_btnBase1->setText((*b)->getName());
				_base1 = (*b);
				add(_btnBase1);
				break;
			case 2:
				_btnBase2 = new TextButton(120, 16, 100, 44);
				_btnBase2->setColor(Palette::blockOffset(15)+6);
				_btnBase2->setText((*b)->getName());
				_btnBase2->onMouseClick((ActionHandler)&PsiTrainingState::btnBase2Click);
				_base2 = (*b);
				add(_btnBase2);
				break;
			case 3:
				_btnBase3 = new TextButton(120, 16, 100, 62);
				_btnBase3->setColor(Palette::blockOffset(15)+6);
				_btnBase3->setText((*b)->getName());
				_btnBase3->onMouseClick((ActionHandler)&PsiTrainingState::btnBase3Click);
				_base3 = (*b);
				add(_btnBase3);
				break;
			case 4:
				_btnBase4 = new TextButton(120, 16, 100, 80);
				_btnBase4->setColor(Palette::blockOffset(15)+6);
				_btnBase4->setText((*b)->getName());
				_btnBase4->onMouseClick((ActionHandler)&PsiTrainingState::btnBase4Click);
				_base4 = (*b);
				add(_btnBase4);
				break;
			case 5:
				_btnBase5 = new TextButton(120, 16, 100, 98);
				_btnBase5->setColor(Palette::blockOffset(15)+6);
				_btnBase5->setText((*b)->getName());
				_btnBase5->onMouseClick((ActionHandler)&PsiTrainingState::btnBase5Click);
				_base5 = (*b);
				add(_btnBase5);
				break;
			case 6:
				_btnBase6 = new TextButton(120, 16, 100, 116);
				_btnBase6->setColor(Palette::blockOffset(15)+6);
				_btnBase6->setText((*b)->getName());
				_btnBase6->onMouseClick((ActionHandler)&PsiTrainingState::btnBase6Click);
				_base6 = (*b);
				add(_btnBase6);
				break;
			case 7:
				_btnBase7 = new TextButton(120, 16, 100, 134);
				_btnBase7->setColor(Palette::blockOffset(15)+6);
				_btnBase7->setText((*b)->getName());
				_btnBase7->onMouseClick((ActionHandler)&PsiTrainingState::btnBase7Click);
				_base7 = (*b);
				add(_btnBase7);
				break;
			case 8:
				_btnBase8 = new TextButton(120, 16, 100, 152);
				_btnBase8->setColor(Palette::blockOffset(15)+6);
				_btnBase8->setText((*b)->getName());
				_btnBase8->onMouseClick((ActionHandler)&PsiTrainingState::btnBase8Click);
				_base8 = (*b);
				add(_btnBase8);
				break;
			default: break;
			}
		}
	}
}
/**
 *
 */
PsiTrainingState::~PsiTrainingState()
{

}

/**
 * Resets the palette.
 */
void PsiTrainingState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void PsiTrainingState::btnOkClick(Action *action)
{
	_game->popState();
}

void PsiTrainingState::btnBase1Click(Action *action)
{	
	_game->popState();
	_game->pushState (new AllocatePsiTrainingState(_game, _base1));
}
void PsiTrainingState::btnBase2Click(Action *action)
{	
	_game->popState();
	_game->pushState (new AllocatePsiTrainingState(_game, _base2));
}
void PsiTrainingState::btnBase3Click(Action *action)
{	
	_game->popState();
	_game->pushState (new AllocatePsiTrainingState(_game, _base3));
}
void PsiTrainingState::btnBase4Click(Action *action)
{	
	_game->popState();
	_game->pushState (new AllocatePsiTrainingState(_game, _base4));
}
void PsiTrainingState::btnBase5Click(Action *action)
{	
	_game->popState();
	_game->pushState (new AllocatePsiTrainingState(_game, _base5));
}
void PsiTrainingState::btnBase6Click(Action *action)
{	
	_game->popState();
	_game->pushState (new AllocatePsiTrainingState(_game, _base6));
}
void PsiTrainingState::btnBase7Click(Action *action)
{	
	_game->popState();
	_game->pushState (new AllocatePsiTrainingState(_game, _base7));
}
void PsiTrainingState::btnBase8Click(Action *action)
{	
	_game->popState();
	_game->pushState (new AllocatePsiTrainingState(_game, _base8));
}
}
