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
#ifndef OPENXCOM_NEWRESEARCHLISTSTATE_H
#define OPENXCOM_NEWRESEARCHLISTSTATE_H

#include "../Engine/State.h"
#include <vector>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;
class RuleResearch;

/**
 * Window which displays possible research projects.
 */
class NewResearchListState : public State
{
private:
	Base *_base;
	TextButton *_btnOK;
	Window *_window;
	Text *_txtTitle;
	TextList *_lstResearch;
	void onSelectProject(Action *action);
	std::vector<RuleResearch *> _projects;
public:
	/// Creates the New research list state.
	NewResearchListState(Game *game, Base *base);
	/// Handler for clicking the OK button.
	void btnOKClick(Action *action);
	/// Fills the ResearchProject list with possible ResearchProjects.
	void fillProjectList ();
	/// Initializes the state.
	void init();
};
}

#endif
