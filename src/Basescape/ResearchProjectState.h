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
	Text *_txtTitle, *_txtAvailableScientist, *_txtAvailableSpace, *_txtAssigned, *_txtMore, *_txtLess;
	TextList *_lstResearch;
	ResearchState * _researchState;
	NewResearchListState * _newResearchListState;
	std::vector<Text*> texts_;
	void SetAssignedScientist();
	ResearchProject * _project;
	RuleResearchProject * _rule;
	void buildUi ();
	Timer *_timerMore, *_timerLess;
public:
	/// Creates the Research state.
	ResearchProjectState(Game *game, Base *base, RuleResearchProject * rule, ResearchState * researchState, NewResearchListState * newResearchListState);
	ResearchProjectState(Game *game, Base *base, ResearchProject * project, ResearchState * researchState, NewResearchListState * newResearchListState);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	void btnMoreClick(Action *action);
	void btnLessClick(Action *action);
	void more();
	void less();
	void morePress(Action *action);
	void moreRelease(Action *action);
	void lessPress(Action *action);
	void lessRelease(Action *action);
	void think();
};
}

#endif
