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
#ifndef OPENXCOM_ENDRESEARCHSTATE
#define OPENXCOM_ENDRESEARCHSTATE

#include "../Engine/State.h"
namespace OpenXcom
{
class Game;
class Window;
class TextButton;
class Text;
class Base;
class RuleResearchProject;

class EndResearchState : public State
{
	Window *_window;
	Text *_txtTitle;
     	TextButton *_btnReport, *_btnOk;
	Base * _base;
	const RuleResearchProject * _research;
public:
	EndResearchState(Game * game, Base * base, const RuleResearchProject * research);
	void btnOkClick(Action *action);
	void btnReportClick(Action *action);
};
}
#endif
