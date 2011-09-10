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
#include "ResearchProjectState.h"
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
#include "../Ruleset/RuleResearchProject.h"
#include "../Savegame/ResearchProject.h"
#include "ResearchState.h"
#include "NewResearchListState.h"
#include "../Interface/ArrowButton.h"
#include "../Engine/Timer.h"
#include "../Engine/RNG.h"

#include <sstream>

namespace OpenXcom
{

ResearchProjectState::ResearchProjectState(Game *game, Base *base, RuleResearchProject * rule) : State(game), _base(base), _project(new ResearchProject(rule, rule->getCost() * OpenXcom::RNG::generate(0.5f, 1.5f))), _rule(rule)
{
	buildUi ();
}

ResearchProjectState::ResearchProjectState(Game *game, Base *base, ResearchProject * project) : State(game), _base(base), _project(project), _rule(0)
{
	buildUi ();
}

void ResearchProjectState::buildUi ()
{
	int width = 220;
	int height = 140;
	int max_width = 320;
	int max_height = 200;
	int start_x = (max_width - width) / 2;
	int start_y = (max_height - height) / 2;
	
	int button_x_border = 10;
	int button_y_border = 10;
	int button_height = 16;

	_screen = false;
	_window = new Window(this, width, height, start_x, start_y/*, POPUP_BOTH*/);

	_txtTitle = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + button_y_border);
	
	_txtAvailableScientist = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + 3*button_y_border);
	_txtAvailableSpace = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + 4*button_y_border);
	_txtAssigned = new Text(width - 2 * button_x_border, button_height, start_x + 2*button_x_border, start_y + 5*button_y_border);
	_txtAssignedValue = new Text(2 * button_x_border, button_height, start_x + 11*button_x_border, start_y + 5*button_y_border);
	_txtMore = new Text(width - 3 * button_x_border, button_height, start_x + 2*button_x_border, start_y + 7*button_y_border);
	_txtLess = new Text(width - 3 * button_x_border, button_height, start_x + 2*button_x_border, start_y + 9*button_y_border);
	_btnOk = new TextButton(width - 2 * button_x_border , button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);

	_btnMore = new ArrowButton(ARROW_BIG_UP, 1.4*button_x_border, button_height, start_x + 12*button_x_border, start_y + 7*button_y_border);
	_btnLess = new ArrowButton(ARROW_BIG_DOWN, 1.4*button_x_border, button_height, start_x + 12*button_x_border, start_y + 9*button_y_border);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(1)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtAvailableScientist);
	add(_txtAvailableSpace);
	add(_txtAssigned);
	add(_txtAssignedValue);
	add(_txtMore);
	add(_txtLess);
	add(_btnMore);
	add(_btnLess);
	
	// Set up objects
	_window->setColor(Palette::blockOffset(13)+8);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));
	_txtTitle->setColor(Palette::blockOffset(13)+5);
	_txtTitle->setText(_rule ? _game->getLanguage()->getString(_rule->getName()) : _game->getLanguage()->getString(_project->getRuleResearchProject ()->getName()));
	_txtAvailableScientist->setColor(Palette::blockOffset(13)+5);
	_txtAvailableScientist->setSecondaryColor(Palette::blockOffset(13));

	_txtAvailableSpace->setColor(Palette::blockOffset(13)+5);
	_txtAvailableSpace->setSecondaryColor(Palette::blockOffset(13));

	_txtAssigned->setColor(Palette::blockOffset(13)+5);
	_txtAssigned->setText(_game->getLanguage()->getString("STR_SCIENTISTS_ALLOCATED"));
	_txtAssignedValue->setColor(Palette::blockOffset(13));
	_txtAssignedValue->setBig();
	_txtAssignedValue->setX(_txtAssigned->getX() + _txtAssigned->getTextWidth() + _txtAssignedValue->getFont()->getSpacing());

	_txtMore->setText(_game->getLanguage()->getString("STR_INCREASE"));
	_txtLess->setText(_game->getLanguage()->getString("STR_DECREASE"));

	_txtMore->setColor(Palette::blockOffset(13)+5);
	_txtLess->setColor(Palette::blockOffset(13)+5);

	_txtMore->setBig();
	_txtLess->setBig();

	if (_rule)
	{
		_base->addResearch(_project);
	}
	SetAssignedScientist();
	_btnMore->setColor(Palette::blockOffset(13)+8);
	_btnLess->setColor(Palette::blockOffset(13)+8);
	_btnMore->onMousePress((ActionHandler)&ResearchProjectState::morePress);
	_btnMore->onMouseRelease((ActionHandler)&ResearchProjectState::moreRelease);
	_btnLess->onMousePress((ActionHandler)&ResearchProjectState::lessPress);
	_btnLess->onMouseRelease((ActionHandler)&ResearchProjectState::lessRelease);

	_timerMore = new Timer(50);
	_timerMore->onTimer((StateHandler)&ResearchProjectState::more);
	_timerLess = new Timer(50);
	_timerLess->onTimer((StateHandler)&ResearchProjectState::less);

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ResearchProjectState::btnOkClick);
}

void ResearchProjectState::btnOkClick(Action *action)
{
	_game->popState();
}

void ResearchProjectState::SetAssignedScientist()
{
	std::wstringstream s1;
	int freeScientist = _base->getFreeScientist();
	int freeSpaceLab = _base->getFreeLaboratories();
	s1 << _game->getLanguage()->getString("STR_SCIENTISTS_AVAILABLE_UC") << L'\x01' << freeScientist;
	std::wstringstream s2;
	s2 << _game->getLanguage()->getString("STR_LABORATORY_SPACE_AVAILABLE_UC") << L'\x01' << freeSpaceLab;
	std::wstringstream s3;
	s3 << _project->getAssigned ();
	_txtAvailableScientist->setText(s1.str());
	_txtAvailableSpace->setText(s2.str());
	_txtAssignedValue->setText(s3.str());
}

void ResearchProjectState::btnMoreClick(Action *action)
{
	int assigned = _project->getAssigned ();
	int freeScientist = _base->getFreeScientist();
	int freeSpaceLab = _base->getFreeLaboratories();
	if(freeScientist > 0 && freeSpaceLab > 0)
	{
		_project->setAssigned(++assigned);
		SetAssignedScientist();
	}
}


void ResearchProjectState::btnLessClick(Action *action)
{
	int assigned = _project->getAssigned ();
	if (assigned > 0)
	{
		_project->setAssigned(--assigned);
		SetAssignedScientist();
	}
}

void ResearchProjectState::morePress(Action *action)
{
	_timerMore->start ();
}

void ResearchProjectState::moreRelease(Action *action)
{
	_timerMore->stop ();
}

void ResearchProjectState::lessPress(Action *action)
{
	_timerLess->start ();
}

void ResearchProjectState::lessRelease(Action *action)
{
	_timerLess->stop ();
}

void ResearchProjectState::more()
{
	btnMoreClick(NULL);
}

void ResearchProjectState::less()
{
	btnLessClick(NULL);
}

void ResearchProjectState::think()
{
	State::think();
	
	_timerLess->think (this, 0);
	_timerMore->think (this, 0);
}
}
