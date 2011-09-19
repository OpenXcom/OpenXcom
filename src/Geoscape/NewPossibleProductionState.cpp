/*
 * Copyright 2011 OpenXcom Developers.
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
#include "NewPossibleProductionState.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Basescape/ManufactureState.h"
#include "../Ruleset/RuleResearchProject.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the NewPossibleProductionState screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param project The completed ResearchProject
 */
NewPossibleProductionState::NewPossibleProductionState(Game * game, Base * base,RuleResearchProject * project) : NewPossibleResearchState(game, base)
{
	_btnAllocate->setText(_game->getLanguage()->getString("STR_ALLOCATE_MANUFACTURE"));
	_txtTitle->setText(_game->getLanguage()->getString("STR_WE_CAN_NOW_PRODUCE"));
	_lstPossibilities->addRow (1, _game->getLanguage()->getString(project->getName()).c_str());
}

/**
 * Open the ManufactureState so the player can dispatch available engineers.
 * @param action Pointer to an action.
 */
void NewPossibleProductionState::btnAllocateClick(Action *action)
{
	_game->popState ();
	_game->pushState (new ManufactureState(_game, _base));
}
}
