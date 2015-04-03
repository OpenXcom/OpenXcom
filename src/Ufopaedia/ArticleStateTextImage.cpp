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

#include "Ufopaedia.h"
#include "../Ruleset/ArticleDefinition.h"
#include "ArticleStateTextImage.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"

namespace OpenXcom
{

	ArticleStateTextImage::ArticleStateTextImage(ArticleDefinitionTextImage *defs) : ArticleState(defs->id)
	{
		// add screen elements
		_txtTitle = new Text(defs->text_width, 48, 5, 22);

		// Set palette
		setPalette("PAL_UFOPAEDIA");

		ArticleState::initLayout();

		// add other elements
		add(_txtTitle);

		// Set up objects
		_game->getResourcePack()->getSurface(defs->image_id)->blit(_bg);
		_btnOk->setColor(Palette::blockOffset(5)+3);
		_btnPrev->setColor(Palette::blockOffset(5)+3);
		_btnNext->setColor(Palette::blockOffset(5)+3);

		_txtTitle->setColor(Palette::blockOffset(15)+4);
		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(tr(defs->title));

		int text_height = _txtTitle->getTextHeight();

		_txtInfo = new Text(defs->text_width, 162, 5, 23 + text_height);
		add(_txtInfo);

		_txtInfo->setColor(Palette::blockOffset(15)-1);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(tr(defs->text));

		centerAllSurfaces();
	}

	ArticleStateTextImage::~ArticleStateTextImage()
	{}

}
