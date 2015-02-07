/*
 * Copyright 2010-2014 OpenXcom Developers.
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
#include "../Engine/Screen.h"
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
PsiTrainingState::PsiTrainingState()
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(300, 17, 10, 16);
	_btnOk = new TextButton(160, 14, 80, 174);

	// Set palette
	setPalette("PAL_BASESCAPE", _game->getRuleset()->getInterface("psiTraining")->getElement("palette")->color);

	add(_window, "window", "psiTraining");
	add(_btnOk, "button2", "psiTraining");
	add(_txtTitle, "text", "psiTraining");

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&PsiTrainingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&PsiTrainingState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_PSIONIC_TRAINING"));

	int buttons = 0;
	for (std::vector<Base*>::const_iterator b = _game->getSavedGame()->getBases()->begin(); b != _game->getSavedGame()->getBases()->end(); ++b)
	{
		if ((*b)->getAvailablePsiLabs())
		{
			TextButton *btnBase = new TextButton(160, 14, 80, 40 + 16 * buttons);
			btnBase->onMouseClick((ActionHandler)&PsiTrainingState::btnBaseXClick);
			btnBase->setText((*b)->getName());
			add(btnBase, "button1", "psiTraining");
			_bases.push_back(*b);
			_btnBases.push_back(btnBase);
			++buttons;
			if (buttons >= 8)
			{
				break;
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
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void PsiTrainingState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Goes to the allocation screen for the corresponding base.
 * @param action Pointer to an action.
 */
void PsiTrainingState::btnBaseXClick(Action *action)
{
	for (size_t i = 0; i < _btnBases.size(); ++i)
	{
		if (action->getSender() == _btnBases[i])
		{
			_game->pushState(new AllocatePsiTrainingState(_bases.at(i)));
			break;
		}
	}
}

}
