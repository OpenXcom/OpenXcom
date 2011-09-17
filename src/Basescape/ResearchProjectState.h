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
#ifndef OPENXCOM_RESEARCHPROJECTSTATE
#define OPENXCOM_RESEARCHPROJECTSTATE
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;
class RuleResearchProject;
class ResearchProject;
class ResearchState;
class NewResearchListState;
class ArrowButton;
class Timer;

/**
   Window which allow to change number of assigned scientist to a project.
*/
class ResearchProjectState : public State
{
private:
	Base *_base;
	TextButton *_btnOk;
	ArrowButton * _btnMore, *_btnLess;
	Window *_window;
	Text *_txtTitle, *_txtAvailableScientist, *_txtAvailableSpace, *_txtAllocatedScientist, *_txtMore, *_txtLess;
	TextList *_lstResearch;
	std::vector<Text*> texts_;
	void SetAssignedScientist();
	ResearchProject * _project;
	RuleResearchProject * _rule;
	void buildUi ();
	Timer *_timerMore, *_timerLess;
public:
	/// Creates the ResearchProject state.
	ResearchProjectState(Game *game, Base *base, RuleResearchProject * rule);
	ResearchProjectState(Game *game, Base *base, ResearchProject * project);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Function called every time the _timerMore timer is triggered.
	void more();
	/// Function called every time the _timerLess timer is triggered.
	void less();
	/// Handler for pressing the More button.
	void morePress(Action *action);
	/// Handler for releasing the More button.
	void moreRelease(Action *action);
	/// Handler for pressing the Less button.
	void lessPress(Action *action);
	/// Handler for releasing the Less button.
	void lessRelease(Action *action);
	/// Runs state functionality every cycle(used to update the timer).
	void think();
};
}

#endif
