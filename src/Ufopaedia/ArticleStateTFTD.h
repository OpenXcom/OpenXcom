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

#ifndef OPENXCOM_ARTICLESTATETFTD_H
#define OPENXCOM_ARTICLESTATETFTD_H

#include "ArticleState.h"

namespace OpenXcom
{
	class Text;
	class ArticleDefinitionTFTD;

	/**
	 * Every TFTD article has a title, text block and a background image, with little to no variation.
	 */

	class ArticleStateTFTD : public ArticleState
	{
	public:
		ArticleStateTFTD(ArticleDefinitionTFTD *defs);
		virtual ~ArticleStateTFTD();

	protected:
		Text *_txtTitle;
		Text *_txtInfo;
	};
}

#endif
