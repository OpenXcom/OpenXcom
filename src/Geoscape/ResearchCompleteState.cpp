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
#include "ResearchCompleteState.h"
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Mod/Mod.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Mod/RuleResearch.h"
#include "../Ufopaedia/Ufopaedia.h"
#include "../Engine/Options.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the EndResearch screen.
 * @param game Pointer to the core game.
 * @param newResearch Pointer to the completed research (or 0, if the ufopedia article shouldn't popup again).
 * @param bonus Pointer to bonus unlocked research.
 * @param research Pointer to the research project.
 */
ResearchCompleteState::ResearchCompleteState(const RuleResearch *newResearch, const RuleResearch *bonus, const RuleResearch *research) : _research(newResearch), _bonus(bonus)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 230, 140, 45, 30, POPUP_BOTH);
	_btnOk = new TextButton(80, 16, 64, 146);
	_btnReport = new TextButton(80, 16, 176, 146);
	_txtTitle = new Text(230, 17, 45, 70);
	_txtResearch = new Text(230, 32, 45, 96);

	// Set palette
	setInterface("geoResearchComplete");

	add(_window, "window", "geoResearchComplete");
	add(_btnOk, "button", "geoResearchComplete");
	add(_btnReport, "button", "geoResearchComplete");
	add(_txtTitle, "text1", "geoResearchComplete");
	add(_txtResearch, "text2", "geoResearchComplete");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK05.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ResearchCompleteState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ResearchCompleteState::btnOkClick, Options::keyCancel);

	_btnReport->setText(tr("STR_VIEW_REPORTS"));
	_btnReport->onMouseClick((ActionHandler)&ResearchCompleteState::btnReportClick);
	_btnReport->onKeyboardPress((ActionHandler)&ResearchCompleteState::btnReportClick, Options::keyOk);

	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_RESEARCH_COMPLETED"));

	_txtResearch->setAlign(ALIGN_CENTER);
	_txtResearch->setBig();
	_txtResearch->setWordWrap(true);
	if (research)
	{
		_txtResearch->setText(tr(research->getName()));
	}
}

/**
 * return to the previous screen
 * @param action Pointer to an action.
 */
void ResearchCompleteState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * open the Ufopaedia to the entry about the Research.
 * @param action Pointer to an action.
 */
void ResearchCompleteState::btnReportClick(Action *)
{
	_game->popState();
	std::string name;
	std::string bonusName;
	if (_bonus)
	{
		if (_bonus->getLookup().empty())
			bonusName = _bonus->getName();
		else
			bonusName = _bonus->getLookup();
		Ufopaedia::openArticle(_game, bonusName);
	}
	if (_research)
	{
		if (_research->getLookup().empty())
			name = _research->getName();
		else
			name = _research->getLookup();
		Ufopaedia::openArticle(_game, name);
	}
}

}
