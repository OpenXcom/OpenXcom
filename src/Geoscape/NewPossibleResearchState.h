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
#ifndef OPENXCOM_NEWPOSSIBLERESEARCHSTATE
#define OPENXCOM_NEWPOSSIBLERESEARCHSTATE

#include "../Engine/State.h"
namespace OpenXcom
{
class Game;
class Window;
class TextButton;
class Text;
class Base;
class TextList;
class RuleResearchProject;

/**
   Window which inform the player of new possible research projects.
   Also allow to go to the ResearchState to dispatch available scientist.
 */
class NewPossibleResearchState : public State
{
	Window *_window;
	Text *_txtTitle;
	TextList * _possibilities;
     	TextButton *_btnSelect, *_btnOk;
	Base * _base;
public:
	NewPossibleResearchState(Game * game, Base * base, const std::vector<RuleResearchProject *> & possibilities);
	void btnOkClick(Action *action);
	void btnSelectClick(Action *action);
};
}
#endif
