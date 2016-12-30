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

#include <sstream>
#include "ArticleStateCraftWeapon.h"
#include "../Mod/ArticleDefinition.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleCraftWeapon.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateCraftWeapon::ArticleStateCraftWeapon(ArticleDefinitionCraftWeapon *defs) : ArticleState(defs->id)
	{
		RuleCraftWeapon *weapon = _game->getMod()->getCraftWeapon(defs->id, true);

		// add screen elements
		_txtTitle = new Text(200, 32, 5, 24);

		// Set palette
		setPalette("PAL_BATTLEPEDIA");

		ArticleState::initLayout();

		// add other elements
		add(_txtTitle);

		// Set up objects
		_game->getMod()->getSurface(defs->image_id)->blit(_bg);
		_btnOk->setColor(Palette::blockOffset(1));
		_btnPrev->setColor(Palette::blockOffset(1));
		_btnNext->setColor(Palette::blockOffset(1));

		_txtTitle->setColor(Palette::blockOffset(14)+15);
		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(tr(defs->title));

		_txtInfo = new Text(310, 32, 5, 160);
		add(_txtInfo);

		_txtInfo->setColor(Palette::blockOffset(14)+15);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(tr(defs->text));

		_lstInfo = new TextList(250, 111, 5, 80);
		add(_lstInfo);


		_lstInfo->setColor(Palette::blockOffset(14)+15);
		_lstInfo->setColumns(2, 180, 70);
		_lstInfo->setDot(true);
		_lstInfo->setBig();

		_lstInfo->addRow(2, tr("STR_DAMAGE").c_str(), Text::formatNumber(weapon->getDamage()).c_str());
		_lstInfo->setCellColor(0, 1, Palette::blockOffset(15)+4);

		_lstInfo->addRow(2, tr("STR_RANGE").c_str(), tr("STR_KILOMETERS").arg(weapon->getRange()).c_str());
		_lstInfo->setCellColor(1, 1, Palette::blockOffset(15)+4);

		_lstInfo->addRow(2, tr("STR_ACCURACY").c_str(), Text::formatPercentage(weapon->getAccuracy()).c_str());
		_lstInfo->setCellColor(2, 1, Palette::blockOffset(15)+4);

		_lstInfo->addRow(2, tr("STR_RE_LOAD_TIME").c_str(), tr("STR_SECONDS").arg(weapon->getStandardReload()).c_str());
		_lstInfo->setCellColor(3, 1, Palette::blockOffset(15)+4);

		_lstInfo->addRow(2, tr("STR_ROUNDS").c_str(), Text::formatNumber(weapon->getAmmoMax()).c_str());
		_lstInfo->setCellColor(4, 1, Palette::blockOffset(15)+4);

		centerAllSurfaces();
	}

	ArticleStateCraftWeapon::~ArticleStateCraftWeapon()
	{}

}
