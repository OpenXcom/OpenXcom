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

#include <sstream>

#include "Ufopaedia.h"
#include "ArticleStateCraftWeapon.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateCraftWeapon::ArticleStateCraftWeapon(Game *game, ArticleDefinitionCraftWeapon *defs, int palSwitch) : ArticleState(game, defs->id, palSwitch)
	{
		RuleCraftWeapon *weapon = _game->getRuleset()->getCraftWeapon(defs->id);

		// add screen elements
		_txtTitle = new Text(200, 32, 5, 24);

		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_4")->getColors());

		ArticleState::initLayout();

		// add other elements
		add(_txtTitle);

		// Set up objects
		_game->getResourcePack()->getSurface(defs->image_id)->blit(_bg);
		_btnOk->setColor(Palette::blockOffset(1));
		_btnPrev->setColor(Palette::blockOffset(1));
		_btnNext->setColor(Palette::blockOffset(1));

		_txtTitle->setColor(Palette::blockOffset(14)+15);
		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(Ufopaedia::buildText(_game, defs->title));

		_txtInfo = new Text(310, 32, 5, 160);
		add(_txtInfo);

		_txtInfo->setColor(Palette::blockOffset(14)+15);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(Ufopaedia::buildText(_game, defs->text));

		_lstInfo = new TextList(250, 111, 5, 80);
		add(_lstInfo);

		std::wstringstream ss;
		_lstInfo->setColor(Palette::blockOffset(14)+15);
		_lstInfo->setColumns(2, 180, 70);
		_lstInfo->setDot(true);
		_lstInfo->setBig();

		ss.str(L"");ss.clear();
		ss << weapon->getDamage();

		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_DAMAGE").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(0, 1, Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << weapon->getRange() << _game->getLanguage()->getString("STR_KM").c_str();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_RANGE").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(1, 1, Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << weapon->getAccuracy() << "%";
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_ACCURACY").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(2, 1, Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << weapon->getStandardReload() << _game->getLanguage()->getString("STR_S").c_str();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_RE_LOAD_TIME").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(3, 1, Palette::blockOffset(15)+4);
	}

	ArticleStateCraftWeapon::~ArticleStateCraftWeapon()
	{}

}
