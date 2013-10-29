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
#include "ArticleStateArmor.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Engine/CrossPlatform.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateArmor::ArticleStateArmor(Game *game, ArticleDefinitionArmor *defs, int palSwitch) : ArticleState(game, defs->id, palSwitch)
	{
		Armor *armor = _game->getRuleset()->getArmor(defs->id);

		// add screen elements
		_txtTitle = new Text(300, 17, 5, 24);

		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_4")->getColors());

		ArticleState::initLayout();

		// add other elements
		add(_txtTitle);

		// Set up objects
		_btnOk->setColor(Palette::blockOffset(0)+15);
		_btnPrev->setColor(Palette::blockOffset(0)+15);
		_btnNext->setColor(Palette::blockOffset(0)+15);

		_txtTitle->setColor(Palette::blockOffset(14)+15);
		_txtTitle->setBig();
		_txtTitle->setText(Ufopaedia::buildText(_game, defs->title));

		_image = new Surface(320, 200, 0, 0);
		add(_image);

		std::string look = armor->getSpriteInventory();
		look += "M0.SPK";
		if (!CrossPlatform::fileExists(CrossPlatform::getDataFile("UFOGRAPH/" + look)) && !_game->getResourcePack()->getSurface(look))
		{
			look = armor->getSpriteInventory() + ".SPK";
		}
		_game->getResourcePack()->getSurface(look)->blit(_image);


		_lstInfo = new TextList(150, 60, 150, 69);
		add(_lstInfo);

		_lstInfo->setColor(Palette::blockOffset(14)+15);
		_lstInfo->setColumns(2, 125, 25);
		_lstInfo->setDot(true);

		_txtInfo = new Text(300, 56, 8, 150);
		add(_txtInfo);

		_txtInfo->setColor(Palette::blockOffset(14)+15);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(Ufopaedia::buildText(_game, defs->text));

		std::wstringstream ss;
		ss.str(L"");ss.clear();
		ss << armor->getFrontArmor();
		_lstInfo->addRow(2, tr("STR_FRONT_ARMOR").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(0, 1, Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << armor->getSideArmor();
		_lstInfo->addRow(2, tr("STR_LEFT_ARMOR").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(1, 1, Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << armor->getSideArmor();
		_lstInfo->addRow(2, tr("STR_RIGHT_ARMOR").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(2, 1, Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << armor->getRearArmor();
		_lstInfo->addRow(2, tr("STR_REAR_ARMOR").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(3, 1, Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << armor->getUnderArmor();
		_lstInfo->addRow(2, tr("STR_UNDER_ARMOR").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(4, 1, Palette::blockOffset(15)+4);

		centerAllSurfaces();
	}

	ArticleStateArmor::~ArticleStateArmor()
	{}

}
