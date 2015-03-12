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
#ifndef OPENXCOM_RESEARCHSTATE_H
#define OPENXCOM_RESEARCHSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;

/**
 * Research screen that lets the player manage
 * all the researching operations of a base.
 */
class ResearchState : public State
{
private:
	Base *_base;
	TextButton *_btnNew, *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtAvailable, *_txtAllocated, *_txtSpace, *_txtProject, *_txtScientists, *_txtProgress;
	TextList *_lstResearch;
public:
	/// Creates the Research state.
	ResearchState(Base *base);
	/// Cleans up the Research state.
	~ResearchState();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the New Research button.
	void btnNewClick(Action *action);
	/// Handler for clicking the ResearchProject list.
	void onSelectProject(Action *action);
	/// Fills the ResearchProject list with Base ResearchProjects.
	void fillProjectList();
	/// Updates the research list.
	void init();
};

}

#endif
