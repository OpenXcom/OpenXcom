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

#ifndef OPENXCOM_ARTICLESTATECRAFT_H
#define OPENXCOM_ARTICLESTATECRAFT_H

#include <string>
#include "ArticleState.h"

namespace OpenXcom
{
	class Game;
	class Text;
	class TextList;
	class ArticleDefinitionCraft;
	
	/**
	 * ArticleStateText has only a caption and a text.
	 */
	
	class ArticleStateCraft : public ArticleState
	{
	public:
		ArticleStateCraft(Game *game, ArticleDefinitionCraft *article_defs);
		virtual ~ArticleStateCraft();
		
	protected:
		Text *_txtTitle;
		Text *_txtInfo;
		TextList *_lstInfo;
	};
}

#endif
