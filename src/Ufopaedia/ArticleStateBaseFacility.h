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

#ifndef OPENXCOM_ARTICLESTATEBASEFACILITY_H
#define OPENXCOM_ARTICLESTATEBASEFACILITY_H

#include "ArticleState.h"

namespace OpenXcom
{
	class Game;
	class Surface;
	class Text;
	class TextList;
	class ArticleDefinitionBaseFacility;

	/**
	 * ArticleStateBaseFacility has a caption, text, preview image and a stats block.
	 * The facility image is found using the RuleBasefacility class.
	 */

	class ArticleStateBaseFacility : public ArticleState
	{
	public:
		ArticleStateBaseFacility(Game *game, ArticleDefinitionBaseFacility *article_defs, int palSwitch);
		virtual ~ArticleStateBaseFacility();

	protected:
		Surface *_image;
		Text *_txtTitle;
		Text *_txtInfo;
		TextList *_lstInfo;
	};
}

#endif
