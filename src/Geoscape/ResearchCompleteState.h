#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include "../Engine/State.h"

namespace OpenXcom
{

class Game;
class Window;
class TextButton;
class Text;
class Base;
class RuleResearch;

/**
 * Window which inform the player that a research project is finished.
 * Allow him to view information about the project (Ufopaedia).
 */
class ResearchCompleteState : public State
{
	Window *_window;
	Text *_txtTitle, *_txtResearch;
	TextButton *_btnReport, *_btnOk;
	const RuleResearch * _research, * _bonus;
public:
	/// Creates the EndResearch state.
	ResearchCompleteState(const RuleResearch *newResearch, const RuleResearch *bonus, const RuleResearch *research);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Report button.
	void btnReportClick(Action *action);
};

}
