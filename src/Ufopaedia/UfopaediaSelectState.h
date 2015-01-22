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

#ifndef OPENXCOM_UFOPAEDIASELECTSTATE_H
#define OPENXCOM_UFOPAEDIASELECTSTATE_H

#include "../Engine/State.h"
#include "Ufopaedia.h"
#include <string>

namespace OpenXcom
{
	class Game;
	class Action;
	class Window;
	class Text;
	class TextButton;
	class TextList;

	/**
	 * UfopaediaSelectState is the screen that lists articles of a given type.
	 */

	class UfopaediaSelectState : public State
	{
	public:
		UfopaediaSelectState(const std::string &section);
		virtual ~UfopaediaSelectState();
		void init();
	protected:
		std::string _section;
		Window *_window;
		Text *_txtTitle;
		TextButton *_btnOk;
		TextList *_lstSelection;
		ArticleDefinitionList _article_list;

		/// Handler for clicking the OK button
		void btnOkClick(Action *action);
		/// Handler for clicking the selection list.
		void lstSelectionClick(Action *action);
		/// load available articles into the selection list
		void loadSelectionList();
	};
}

#endif
