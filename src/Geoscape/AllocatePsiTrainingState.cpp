/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Interface/TextList.h"
#include "../Savegame/Soldier.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Psi Training screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to handle.
 */
AllocatePsiTrainingState::AllocatePsiTrainingState(Base *base) : _sel(0)
{
	_base = base;
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(300, 17, 10, 8);
	_txtRemaining = new Text(300, 10, 10, 24);
	_txtName = new Text(64, 10, 10, 40);
	_txtPsiStrength = new Text(80, 20, 124, 32);
	_txtPsiSkill = new Text(80, 20, 188, 32);
	_txtTraining = new Text(48, 20, 270, 32);
	_btnOk = new TextButton(160, 14, 80, 174);
	_lstSoldiers = new TextList(290, 112, 8, 52);

	// Set palette
	setInterface("allocatePsi");

	add(_window, "window", "allocatePsi");
	add(_btnOk, "button", "allocatePsi");
	add(_txtName, "text", "allocatePsi");
	add(_txtTitle, "text", "allocatePsi");
	add(_txtRemaining, "text", "allocatePsi");
	add(_txtPsiStrength, "text", "allocatePsi");
	add(_txtPsiSkill, "text", "allocatePsi");
	add(_txtTraining, "text", "allocatePsi");
	add(_lstSoldiers, "list", "allocatePsi");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&AllocatePsiTrainingState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&AllocatePsiTrainingState::btnOkClick, Options::keyCancel);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_PSIONIC_TRAINING"));

	_labSpace = base->getAvailablePsiLabs() - base->getUsedPsiLabs();
	_txtRemaining->setText(tr("STR_REMAINING_PSI_LAB_CAPACITY").arg(_labSpace));

	_txtName->setText(tr("STR_NAME"));

	_txtPsiStrength->setText(tr("STR_PSIONIC__STRENGTH"));

	_txtPsiSkill->setText(tr("STR_PSIONIC_SKILL_IMPROVEMENT"));

	_txtTraining->setText(tr("STR_IN_TRAINING"));

	_lstSoldiers->setAlign(ALIGN_RIGHT, 3);
	_lstSoldiers->setColumns(4, 114, 80, 62, 30);
	_lstSoldiers->setSelectable(true);
	_lstSoldiers->setBackground(_window);
	_lstSoldiers->setMargin(2);
	_lstSoldiers->onMouseClick((ActionHandler)&AllocatePsiTrainingState::lstSoldiersClick);
	int row = 0;
	for (std::vector<Soldier*>::const_iterator s = base->getSoldiers()->begin(); s != base->getSoldiers()->end(); ++s)
	{
		std::wostringstream ssStr;
		std::wostringstream ssSkl;
		_soldiers.push_back(*s);
		if ((*s)->getCurrentStats()->psiSkill > 0 || (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getRuleset()->getPsiRequirements())))
		{
			ssStr << L"   " << (*s)->getCurrentStats()->psiStrength;
			if (Options::allowPsiStrengthImprovement) ssStr << "/+" << (*s)->getPsiStrImprovement();
		}
		else
		{
			ssStr << tr("STR_UNKNOWN").c_str();
		}
		if ((*s)->getCurrentStats()->psiSkill > 0)
		{
			ssSkl << (*s)->getCurrentStats()->psiSkill << "/+" << (*s)->getImprovement();
		}
		else
		{
			ssSkl << "0/+0";
		}
		if ((*s)->isInPsiTraining())
		{
			_lstSoldiers->addRow(4, (*s)->getName(true).c_str(), ssStr.str().c_str(), ssSkl.str().c_str(), tr("STR_YES").c_str());
			_lstSoldiers->setRowColor(row, _lstSoldiers->getSecondaryColor());
		}
		else
		{
			_lstSoldiers->addRow(4, (*s)->getName(true).c_str(), ssStr.str().c_str(), ssSkl.str().c_str(), tr("STR_NO").c_str());
			_lstSoldiers->setRowColor(row, _lstSoldiers->getColor());
		}
		row++;
	}
}
/**
 *
 */
AllocatePsiTrainingState::~AllocatePsiTrainingState()
{

}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void AllocatePsiTrainingState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Assigns / removes a soldier from Psi Training.
 * @param action Pointer to an action.
 */
void AllocatePsiTrainingState::lstSoldiersClick(Action *action)
{
	_sel = _lstSoldiers->getSelectedRow();
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		if (!_base->getSoldiers()->at(_sel)->isInPsiTraining())
		{
			if (_base->getUsedPsiLabs() < _base->getAvailablePsiLabs())
			{
				_lstSoldiers->setCellText(_sel, 3, tr("STR_YES").c_str());
				_lstSoldiers->setRowColor(_sel, _lstSoldiers->getSecondaryColor());
				_labSpace--;
				_txtRemaining->setText(tr("STR_REMAINING_PSI_LAB_CAPACITY").arg(_labSpace));
				_base->getSoldiers()->at(_sel)->setPsiTraining();
			}
		}
		else
		{
			_lstSoldiers->setCellText(_sel, 3, tr("STR_NO").c_str());
			_lstSoldiers->setRowColor(_sel, _lstSoldiers->getColor());
			_labSpace++;
			_txtRemaining->setText(tr("STR_REMAINING_PSI_LAB_CAPACITY").arg(_labSpace));
			_base->getSoldiers()->at(_sel)->setPsiTraining();
		}
	}
}

}
