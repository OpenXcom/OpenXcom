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

#ifndef OPENXCOM_ARTICLESTATE_H
#define OPENXCOM_ARTICLESTATE_H

#include "../Engine/State.h"
#include <string>

namespace OpenXcom
{
	class Game;
	class Action;
	class Surface;
	class TextButton;

	/**
	 * UfopaediaArticle is the base class for all articles of various types.
	 *
	 * It encapsulates the basic characteristics.
	 */

	class ArticleState : public State
	{
	protected:
		/// constructor (protected, so it can only be instantiated by derived classes)
		ArticleState(Game *game, std::string article_id, int palSwitch);
		/// destructor
		virtual ~ArticleState();

	public:
		/// return the article id
		std::string getId() const { return _id; }

	protected:

		/// screen layout helpers
		void initLayout();

		/// callback for OK button
		void btnOkClick(Action *action);

		/// callback for PREV button
		void btnPrevClick(Action *action);

		/// callback for NEXT button
		void btnNextClick(Action *action);
		/// the article id
		std::string _id;

		/// screen elements common to all articles!
		Surface *_bg;
		TextButton *_btnOk;
		TextButton *_btnPrev;
		TextButton *_btnNext;

		int _palSwitch;
	};
}

#endif
