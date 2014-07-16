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

#include "Ufopaedia.h"
#include "../Ruleset/ArticleDefinition.h"
#include "ArticleStateTFTD.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"

namespace OpenXcom
{

	ArticleStateTFTD::ArticleStateTFTD(ArticleDefinitionTFTD *defs) : ArticleState(defs->id)
	{
		// add screen elements
		_txtTitle = new Text(284, 16, 36, 14);
		_btnTitle = new TextButton(288, 27, 34, 6);

		// Set palette
		setPalette("PAL_BASESCAPE");

		_btnOk->setX(227);
		_btnOk->setY(179);
		_btnOk->setHeight(10);
		_btnOk->setWidth(23);
		_btnOk->setColor(Palette::blockOffset(0)+2);
		_btnPrev->setX(254);
		_btnPrev->setY(179);
		_btnPrev->setHeight(10);
		_btnPrev->setWidth(23);
		_btnPrev->setColor(Palette::blockOffset(0)+2);
		_btnNext->setX(281);
		_btnNext->setY(179);
		_btnNext->setHeight(10);
		_btnNext->setWidth(23);
		_btnNext->setColor(Palette::blockOffset(0)+2);
		
		ArticleState::initLayout();

		_btnTitle->setColor(Palette::blockOffset(9) + 4);
		_btnTitle->setHighContrast(true);

		// add other elements
		add(_btnTitle);
		add(_txtTitle);
		// Set up objects
		_game->getResourcePack()->getSurface("UP030.BDY")->blit(_bg);
		_game->getResourcePack()->getSurface(defs->image_id)->blit(_bg);

		_txtTitle->setColor(Palette::blockOffset(0)+2);
		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setAlign(ALIGN_CENTER);
		_txtTitle->setText(tr(defs->title));

		_txtInfo = new Text(152, 64, 168, 40);
		add(_txtInfo);

		_txtInfo->setColor(Palette::blockOffset(0)+2);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(tr(defs->text));

		centerAllSurfaces();
	}

	ArticleStateTFTD::~ArticleStateTFTD()
	{}

}
