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
#include <sstream>
#include "PsiTrainingState.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
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
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Psi Training screen.
 * @param game Pointer to the core game.
 */
PsiTrainingState::PsiTrainingState(Game *game) : State(game)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(300, 17, 10, 16);
	_btnOk = new TextButton(160, 14, 80, 174);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(7)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+6);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&PsiTrainingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&PsiTrainingState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_PSIONIC_TRAINING"));
	int buttons = 0;
	TextButton *_btnBase;
	for(std::vector<Base*>::const_iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end(); ++b)
	{
		if((*b)->getAvailablePsiLabs())
		{
			_bases.push_back(*b);
			if (buttons < 8)
			{
				_btnBase = new TextButton(160, 14, 80, 40 + 16*buttons);
				_btnBase->setColor(Palette::blockOffset(15)+6);
				_btnBase->onMouseClick((ActionHandler)&PsiTrainingState::btnBaseXClick);
				_btnBase->setText((*b)->getName());
				add(_btnBase);
				++buttons;
			}
		}
	}

	centerAllSurfaces();
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
void PsiTrainingState::btnOkClick(Action *)
{
	_game->popState();
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
}

void PsiTrainingState::btnBaseXClick(Action *action)
{
	int buttonIndex = (action->getSender()->getY() - 40) / 16;
	_game->pushState (new AllocatePsiTrainingState(_game, _bases.at(buttonIndex)));
}

}
