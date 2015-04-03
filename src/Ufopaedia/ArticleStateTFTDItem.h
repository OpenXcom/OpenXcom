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

#ifndef OPENXCOM_ARTICLESTATETFTDITEM_H
#define OPENXCOM_ARTICLESTATETFTDITEM_H

#include "ArticleStateTFTD.h"

namespace OpenXcom
{
	class TextList;
	class Text;
	class ArticleDefinitionTFTD;

	class ArticleStateTFTDItem : public ArticleStateTFTD
	{
	public:
		ArticleStateTFTDItem(ArticleDefinitionTFTD *defs);
		virtual ~ArticleStateTFTDItem();

	protected:
		TextList *_lstInfo;
		Text *_txtShotType;
		Text *_txtAccuracy;
		Text *_txtTuCost;
		Text *_txtAmmoType[3];
		Text *_txtAmmoDamage[3];
	};
}

#endif
