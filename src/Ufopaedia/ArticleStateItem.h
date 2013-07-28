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

#ifndef OPENXCOM_ARTICLESTATEITEM_H
#define OPENXCOM_ARTICLESTATEITEM_H

#include "ArticleState.h"

namespace OpenXcom
{
	class Game;
	class Surface;
	class Text;
	class TextList;
	class ArticleDefinitionItem;
	class RuleItem;

	/**
	 * ArticleStateItem has a caption, text, preview image and a stats block.
	 * The facility image is found using the RuleBasefacility class.
	 */

	class ArticleStateItem : public ArticleState
	{
	public:
		ArticleStateItem(Game *game, ArticleDefinitionItem *article_defs, int palSwitch);
		virtual ~ArticleStateItem();

	protected:
		void setDamageTypeText(Text *text_field, RuleItem *ammo_rule);

		Surface *_image;
		Text *_txtTitle;
		Text *_txtInfo;
		TextList *_lstInfo;
		Text *_txtShotType;
		Text *_txtAccuracy;
		Text *_txtTuCost;
		Text *_txtDamage;
		Text *_txtAmmo;
		Text *_txtAmmoType[3];
		Text *_txtAmmoDamage[3];
		Surface *_imageAmmo[3];
	};
}

#endif
