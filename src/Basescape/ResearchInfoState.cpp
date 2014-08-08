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
#include "ResearchInfoState.h"
#include "../Engine/Action.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Base.h"
#include "../Ruleset/RuleResearch.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/ResearchProject.h"
#include "../Interface/ArrowButton.h"
#include "../Engine/Timer.h"
#include "../Engine/RNG.h"
#include <sstream>
#include <limits>

namespace OpenXcom
{

/**
 * Initializes all the elements in the ResearchProject screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param rule A RuleResearch which will be used to create a new ResearchProject
 */
ResearchInfoState::ResearchInfoState(Base *base, RuleResearch * rule) : _base(base), _project(new ResearchProject(rule, int(rule->getCost() * OpenXcom::RNG::generate(50, 150)/100))), _rule(rule)
{
	buildUi();
}

/**
 * Initializes all the elements in the ResearchProject screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param project A ResearchProject to modify
 */
ResearchInfoState::ResearchInfoState(Base *base, ResearchProject * project) : _base(base), _project(project), _rule(0)
{
	buildUi();
}

/**
 * Builds dialog.
 */
void ResearchInfoState::buildUi()
{
	_screen = false;

	_window = new Window(this, 230, 140, 45, 30);
	_txtTitle = new Text(210, 17, 61, 40);

	_txtAvailableScientist = new Text(210, 9, 61, 60);
	_txtAvailableSpace = new Text(210, 9, 61, 70);
	_txtAllocatedScientist = new Text(210, 17, 61, 80);
	_txtMore = new Text(110, 17, 85, 100);
	_txtLess = new Text(110, 17, 85, 120);
	_btnCancel = new TextButton(90, 16, 61, 145);
	_btnOk = new TextButton(90, 16, 169, 145);

	_btnMore = new ArrowButton(ARROW_BIG_UP, 13, 14, 195, 100);
	_btnLess = new ArrowButton(ARROW_BIG_DOWN, 13, 14, 195, 120);

	_surfaceScientists = new InteractiveSurface(230, 140, 45, 30);
	_surfaceScientists->onMouseClick((ActionHandler)&ResearchInfoState::handleWheel, 0);

	// Set palette
	setPalette("PAL_BASESCAPE", 1);

	add(_surfaceScientists);
	add(_window);
	add(_btnOk);
	add(_btnCancel);
	add(_txtTitle);
	add(_txtAvailableScientist);
	add(_txtAvailableSpace);
	add(_txtAllocatedScientist);
	add(_txtMore);
	add(_txtLess);
	add(_btnMore);
	add(_btnLess);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK05.SCR"));
	_txtTitle->setColor(Palette::blockOffset(13)+5);
	_txtTitle->setBig();

	_txtTitle->setText(_rule ? tr(_rule->getName()) : tr(_project->getRules ()->getName()));

	_txtAvailableScientist->setColor(Palette::blockOffset(13)+5);
	_txtAvailableScientist->setSecondaryColor(Palette::blockOffset(13));

	_txtAvailableSpace->setColor(Palette::blockOffset(13)+5);
	_txtAvailableSpace->setSecondaryColor(Palette::blockOffset(13));

	_txtAllocatedScientist->setColor(Palette::blockOffset(13)+5);
	_txtAllocatedScientist->setSecondaryColor(Palette::blockOffset(13));
	_txtAllocatedScientist->setBig();

	_txtMore->setText(tr("STR_INCREASE"));
	_txtLess->setText(tr("STR_DECREASE"));

	_txtMore->setColor(Palette::blockOffset(13)+5);
	_txtLess->setColor(Palette::blockOffset(13)+5);

	_txtMore->setBig();
	_txtLess->setBig();

	if (_rule)
	{
		_base->addResearch(_project);
		if (_rule->needItem() &&
				(_game->getRuleset()->getUnit(_rule->getName()) ||
				 Options::spendResearchedItems))
		{
			_base->getItems()->removeItem(_rule->getName(), 1);
		}
	}
	setAssignedScientist();
	_btnMore->setColor(Palette::blockOffset(13)+5);
	_btnLess->setColor(Palette::blockOffset(13)+5);
	_btnMore->onMousePress((ActionHandler)&ResearchInfoState::morePress);
	_btnMore->onMouseRelease((ActionHandler)&ResearchInfoState::moreRelease);
	_btnMore->onMouseClick((ActionHandler)&ResearchInfoState::moreClick, 0);
	_btnLess->onMousePress((ActionHandler)&ResearchInfoState::lessPress);
	_btnLess->onMouseRelease((ActionHandler)&ResearchInfoState::lessRelease);
	_btnLess->onMouseClick((ActionHandler)&ResearchInfoState::lessClick, 0);

	_timerMore = new Timer(250);
	_timerMore->onTimer((StateHandler)&ResearchInfoState::more);
	_timerLess = new Timer(250);
	_timerLess->onTimer((StateHandler)&ResearchInfoState::less);

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->onMouseClick((ActionHandler)&ResearchInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ResearchInfoState::btnOkClick, Options::keyOk);
	_btnCancel->setColor(Palette::blockOffset(13)+10);
	if (_rule)
	{
		_btnOk->setText(tr("STR_START_PROJECT"));
		_btnCancel->setText(tr("STR_CANCEL_UC"));
		_btnCancel->onKeyboardPress((ActionHandler)&ResearchInfoState::btnCancelClick, Options::keyCancel);
	}
	else
	{
		_btnOk->setText(tr("STR_OK"));
		_btnCancel->setText(tr("STR_CANCEL_PROJECT"));
		_btnOk->onKeyboardPress((ActionHandler)&ResearchInfoState::btnOkClick, Options::keyCancel);
	}
	_btnCancel->onMouseClick((ActionHandler)&ResearchInfoState::btnCancelClick);
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void ResearchInfoState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Returns to the previous screen, removing the current project from the active
 * research list.
 * @param action Pointer to an action.
 */
void ResearchInfoState::btnCancelClick(Action *)
{
	const RuleResearch *ruleResearch = _rule ? _rule : _project->getRules();
	if (ruleResearch->needItem() &&
			(_game->getRuleset()->getUnit(ruleResearch->getName()) ||
			 Options::spendResearchedItems))
	{
		_base->getItems()->addItem(ruleResearch->getName(), 1);
	}
	_base->removeResearch(_project);
	_game->popState();
}

/**
 * Updates count of assigned/free scientists and available lab space.
 */
void ResearchInfoState::setAssignedScientist()
{
	_txtAvailableScientist->setText(tr("STR_SCIENTISTS_AVAILABLE_UC").arg(_base->getAvailableScientists()));
	_txtAvailableSpace->setText(tr("STR_LABORATORY_SPACE_AVAILABLE_UC").arg(_base->getFreeLaboratories()));
	_txtAllocatedScientist->setText(tr("STR_SCIENTISTS_ALLOCATED").arg(_project->getAssigned()));
}

/**
 * Increases or decreases the scientists according the mouse-wheel used.
 * @param action Pointer to an Action.
 */
void ResearchInfoState::handleWheel(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP) moreByValue(Options::changeValueByMouseWheel);
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN) lessByValue(Options::changeValueByMouseWheel);
}

/**
 * Starts the timeMore timer.
 * @param action Pointer to an Action.
 */
void ResearchInfoState::morePress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerMore->start();
}

/**
 * Stops the timeMore timer.
 * @param action Pointer to an Action.
 */
void ResearchInfoState::moreRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerMore->setInterval(250);
		_timerMore->stop();
	}
}

/**
 * Allocates scientists to the current project;
 * one scientist on left-click, all scientists on right-click.
 * @param action Pointer to an Action.
 */
void ResearchInfoState::moreClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		moreByValue(std::numeric_limits<int>::max());
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
		moreByValue(1);
}

/**
 * Starts the timeLess timer.
 * @param action Pointer to an Action.
 */
void ResearchInfoState::lessPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLess->start ();
}

/**
 * Stops the timeLess timer.
 * @param action Pointer to an Action.
 */
void ResearchInfoState::lessRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerLess->setInterval(250);
		_timerLess->stop ();
	}
}

/**
 * Removes scientists from the current project;
 * one scientist on left-click, all scientists on right-click.
 * @param action Pointer to an Action.
 */
void ResearchInfoState::lessClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		lessByValue(std::numeric_limits<int>::max());
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
		lessByValue(1);
}

/**
 * Adds one scientist to the project if possible.
 */
void ResearchInfoState::more()
{
	_timerMore->setInterval(50);
	moreByValue(1);
}

/**
 * Adds the given number of scientists to the project if possible.
 * @param change Number of scientists to add.
 */
void ResearchInfoState::moreByValue(int change)
{
	if (0 >= change) return;
	int freeScientist = _base->getAvailableScientists();
	int freeSpaceLab = _base->getFreeLaboratories();
	if (freeScientist > 0 && freeSpaceLab > 0)
	{
		change = std::min(std::min(freeScientist, freeSpaceLab), change);
		_project->setAssigned(_project->getAssigned()+change);
		_base->setScientists(_base->getScientists()-change);
		setAssignedScientist();
	}
}

/**
 * Removes one scientist from the project if possible.
 */
void ResearchInfoState::less()
{
	_timerLess->setInterval(50);
	lessByValue(1);
}

/**
 * Removes the given number of scientists from the project if possible.
 * @param change Number of scientists to subtract.
 */
void ResearchInfoState::lessByValue(int change)
{
	if (0 >= change) return;
	int assigned = _project->getAssigned();
	if (assigned > 0)
	{
		change = std::min(assigned, change);
		_project->setAssigned(assigned-change);
		_base->setScientists(_base->getScientists()+change);
		setAssignedScientist();
	}
}

/**
 * Runs state functionality every cycle (used to update the timer).
 */
void ResearchInfoState::think()
{
	State::think();

	_timerLess->think (this, 0);
	_timerMore->think (this, 0);
}
}
