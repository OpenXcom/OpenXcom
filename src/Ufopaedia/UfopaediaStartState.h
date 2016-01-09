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
#include <string>
#include <vector>

namespace OpenXcom
{
	class Game;
	class Action;
	class Window;
	class Text;
	class TextButton;

	/**
	 * UfopaediaStartState is the screen that opens when clicking Ufopaedia button in Geoscape.
	 * Presents buttons to all sections of Ufopaedia, opening a UfopaediaSelectState on click.
	 */

	class UfopaediaStartState : public State
	{
	public:
		UfopaediaStartState();
		virtual ~UfopaediaStartState();

	protected:
		Window *_window;
		Text *_txtTitle;
		TextButton *_btnOk;
		std::vector<TextButton*> _btnSections;

		// navigation callbacks
		void btnSectionClick(Action *action);
		void btnOkClick(Action *action);
	};
}
