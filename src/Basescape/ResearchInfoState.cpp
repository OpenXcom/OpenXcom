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
#include "ResearchInfoState.h"
#include "../Engine/Action.h"
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
#include "../Ruleset/RuleResearch.h"
#include "../Savegame/ResearchProject.h"
#include "ResearchState.h"
#include "NewResearchListState.h"
#include "../Interface/ArrowButton.h"
#include "../Engine/Timer.h"
#include "../Engine/RNG.h"

#include <sstream>

namespace OpenXcom
{

/**
 * Initializes all the elements in the ResearchProject screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param rule A RuleResearch which will be used to create a new ResearchProject
 */
ResearchInfoState::ResearchInfoState(Game *game, Base *base, RuleResearch * rule) : State(game), _base(base), _project(new ResearchProject(rule, int(rule->getCost() * OpenXcom::RNG::generate(50, 150)/100))), _rule(rule)
{
	buildUi ();
}

/**
 * Initializes all the elements in the ResearchProject screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param project A ResearchProject to modify
 */
ResearchInfoState::ResearchInfoState(Game *game, Base *base, ResearchProject * project) : State(game), _base(base), _project(project), _rule(0)
{
	buildUi ();
}

/**
 * Build dialog
 */
void ResearchInfoState::buildUi ()
{
	int width = 230;
	int height = 140;
	int max_width = 320;
	int max_height = 200;
	int start_x = (max_width - width) / 2;
	int start_y = (max_height - height) / 2;

	int button_x_border = 16;
	int button_y_border = 10;
	int button_height = 16;

	_screen = false;
	_window = new Window(this, width, height, start_x, start_y);

	_txtTitle = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + button_y_border);

	_txtAvailableScientist = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + 3*button_y_border);
	_txtAvailableSpace = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + 4*button_y_border);
	_txtAllocatedScientist = new Text(width - 2 * button_x_border, button_height, start_x + button_x_border, start_y + 5*button_y_border);
	_txtMore = new Text(width - 3 * button_x_border, button_height, start_x + 2.5*button_x_border + 8, start_y + 7*button_y_border);
	_txtLess = new Text(width - 3 * button_x_border, button_height, start_x + 2.5*button_x_border + 8, start_y + 9*button_y_border);
	_btnOk = new TextButton(width - 2 * button_x_border , button_height, start_x + button_x_border, start_y + height - button_height - button_y_border);

	_btnMore = new ArrowButton(ARROW_BIG_UP, button_x_border - 3, button_height - 2, start_x + 10*button_x_border, start_y + 7*button_y_border);
	_btnLess = new ArrowButton(ARROW_BIG_DOWN, button_x_border - 3, button_height - 2, start_x + 10*button_x_border, start_y + 9*button_y_border);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtAvailableScientist);
	add(_txtAvailableSpace);
	add(_txtAllocatedScientist);
	add(_txtMore);
	add(_txtLess);
	add(_btnMore);
	add(_btnLess);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));
	_txtTitle->setColor(Palette::blockOffset(13)+5);
	_txtTitle->setBig();
	_txtTitle->setText(_rule ? _game->getLanguage()->getString(_rule->getName()) : _game->getLanguage()->getString(_project->getRules ()->getName()));
	_txtAvailableScientist->setColor(Palette::blockOffset(13)+5);
	_txtAvailableScientist->setSecondaryColor(Palette::blockOffset(13));

	_txtAvailableSpace->setColor(Palette::blockOffset(13)+5);
	_txtAvailableSpace->setSecondaryColor(Palette::blockOffset(13));

	_txtAllocatedScientist->setColor(Palette::blockOffset(13)+5);
	_txtAllocatedScientist->setSecondaryColor(Palette::blockOffset(13));
	_txtAllocatedScientist->setBig();

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
	_btnMore->setColor(Palette::blockOffset(13)+5);
	_btnLess->setColor(Palette::blockOffset(13)+5);
	_btnMore->onMousePress((ActionHandler)&ResearchInfoState::morePress);
	_btnMore->onMouseRelease((ActionHandler)&ResearchInfoState::moreRelease);
	_btnMore->onMouseClick((ActionHandler)&ResearchInfoState::moreClick, 0);
	_btnLess->onMousePress((ActionHandler)&ResearchInfoState::lessPress);
	_btnLess->onMouseRelease((ActionHandler)&ResearchInfoState::lessRelease);
	_btnLess->onMouseClick((ActionHandler)&ResearchInfoState::lessClick, 0);

	_timerMore = new Timer(50);
	_timerMore->onTimer((StateHandler)&ResearchInfoState::more);
	_timerLess = new Timer(50);
	_timerLess->onTimer((StateHandler)&ResearchInfoState::less);

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ResearchInfoState::btnOkClick);
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ResearchInfoState::btnOkClick(Action *action)
{
	_game->popState();
}

/**
 * update count of assigned/free scientist, and available space lab
 */
void ResearchInfoState::SetAssignedScientist()
{
	std::wstringstream s1;
	s1 << _game->getLanguage()->getString("STR_SCIENTISTS_AVAILABLE_UC") << L'\x01' << _base->getAvailableScientists();
	std::wstringstream s2;
	s2 << _game->getLanguage()->getString("STR_LABORATORY_SPACE_AVAILABLE_UC") << L'\x01' << _base->getFreeLaboratories();
	std::wstringstream s3;
	s3 << _game->getLanguage()->getString("STR_SCIENTISTS_ALLOCATED") << L'\x01' << _project->getAssigned ();
	_txtAvailableScientist->setText(s1.str());
	_txtAvailableSpace->setText(s2.str());
	_txtAllocatedScientist->setText(s3.str());
}

/**
 * Start the timeMore timer
 * @param action a Pointer to an Action
 */
void ResearchInfoState::morePress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerMore->start();
}

/**
 * Stop the timeMore timer
 * @param action a Pointer to an Action
 */
void ResearchInfoState::moreRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerMore->stop();
}

/**
 * Allocate all scientists on right-click
 * @param action a Pointer to an Action
 */
void ResearchInfoState::moreClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		int assigned = _project->getAssigned ();
		int freeScientist = _base->getAvailableScientists();
		int freeSpaceLab = _base->getFreeLaboratories();
		if (freeScientist > 0 && freeSpaceLab > 0)
		{
			int change=std::min(freeScientist, freeSpaceLab);
			_project->setAssigned(assigned+=change);
			_base->setScientists(_base->getScientists()-change);
			SetAssignedScientist();
		}
	}
}

/**
 * Start the timeLess timer
 * @param action a Pointer to an Action
 */
void ResearchInfoState::lessPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLess->start ();
}

/**
 * Stop the timeLess timer
 * @param action a Pointer to an Action
 */
void ResearchInfoState::lessRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLess->stop ();
}

/**
 * Allocate 0 scientists on right-click
 * @param action a Pointer to an Action
 */
void ResearchInfoState::lessClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		int assigned = _project->getAssigned();
		if (assigned > 0)
		{
			_project->setAssigned(0);
			_base->setScientists(_base->getScientists()+assigned);
			SetAssignedScientist();
		}
	}
}

/**
 * Add one scientist to the project if possible
 */
void ResearchInfoState::more()
{
	int assigned = _project->getAssigned ();
	int freeScientist = _base->getAvailableScientists();
	int freeSpaceLab = _base->getFreeLaboratories();
	if(freeScientist > 0 && freeSpaceLab > 0)
	{
		_project->setAssigned(++assigned);
		_base->setScientists(_base->getScientists()-1);
		SetAssignedScientist();
	}
}

/**
 * Remove one scientist to the project if possible
 */
void ResearchInfoState::less()
{
	int assigned = _project->getAssigned ();
	if (assigned > 0)
	{
		_project->setAssigned(--assigned);
		_base->setScientists(_base->getScientists()+1);
		SetAssignedScientist();
	}
}

/**
 * Runs state functionality every cycle(used to update the timer).
 */
void ResearchInfoState::think()
{
	State::think();

	_timerLess->think (this, 0);
	_timerMore->think (this, 0);
}
}
