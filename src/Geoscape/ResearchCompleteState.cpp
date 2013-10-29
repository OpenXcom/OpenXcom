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
#include "ResearchCompleteState.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Ruleset/RuleResearch.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ufopaedia/Ufopaedia.h"
#include <algorithm>
#include "../Engine/Options.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the EndResearch screen.
 * @param game Pointer to the core game.
 * @param research Pointer to the completed research.
 */
ResearchCompleteState::ResearchCompleteState(Game * game, const RuleResearch * research, const RuleResearch * bonus): State (game), _research(research), _bonus(bonus)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 224, 140, 48, 30, POPUP_BOTH);
	_btnOk = new TextButton(80, 16, 64, 146);
	_btnReport = new TextButton(80, 16, 176, 146);
	_txtTitle = new Text(224, 17, 48, 88);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnReport);
	add(_txtTitle);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(15)-1);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ResearchCompleteState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ResearchCompleteState::btnOkClick, (SDLKey)Options::getInt("keyCancel"));
	_btnReport->setColor(Palette::blockOffset(8)+5);
	_btnReport->setText(tr("STR_VIEW_REPORTS"));
	_btnReport->onMouseClick((ActionHandler)&ResearchCompleteState::btnReportClick);
	_btnReport->onKeyboardPress((ActionHandler)&ResearchCompleteState::btnReportClick, (SDLKey)Options::getInt("keyOk"));

	_txtTitle->setColor(Palette::blockOffset(15)-1);
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setText(tr("STR_RESEARCH_COMPLETED"));
}

/**
 * Resets the palette.
 */
void ResearchCompleteState::init()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(0)), Palette::backPos, 16);
}

/**
 * return to the previous screen
 * @param action Pointer to an action.
 */
void ResearchCompleteState::btnOkClick(Action *)
{
	_game->popState ();
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
	int palSwitch = 0;
	if(_bonus)
	{
		if (_bonus->getLookup() == "")
			bonusName = _bonus->getName();
		else
			bonusName = _bonus->getLookup();
		ArticleDefinition *art = _game->getRuleset()->getUfopaediaArticle(bonusName);
		switch(art->getType())
		{
		case UFOPAEDIA_TYPE_BASE_FACILITY:
			palSwitch = 1;
			break;
		case UFOPAEDIA_TYPE_CRAFT:
		case UFOPAEDIA_TYPE_TEXTIMAGE:
		case UFOPAEDIA_TYPE_TEXT:
		case UFOPAEDIA_TYPE_VEHICLE:
			palSwitch = 3;
			break;
		case UFOPAEDIA_TYPE_ITEM:
		case UFOPAEDIA_TYPE_CRAFT_WEAPON:
		case UFOPAEDIA_TYPE_ARMOR:
			palSwitch = 4;
			break;
		default:
			break;
		}
		Ufopaedia::openArticle(_game, bonusName, 0);
	}
	if(_research)
	{
		if (_research->getLookup() == "")
			name = _research->getName ();
		else
			name = _research->getLookup();
		Ufopaedia::openArticle(_game, name, palSwitch);
	}
}

}
