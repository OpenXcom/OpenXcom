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

#include <sstream>

#include "Ufopaedia.h"
#include "ArticleStateCraft.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/RuleCraft.h"
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
	
	ArticleStateCraft::ArticleStateCraft(Game *game, ArticleDefinitionCraft *defs) : ArticleState(game, defs->id)
	{
		// add screen elements
		_txtTitle = new Text(140, 32, 5, 24);
		
		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_3")->getColors());
		
		ArticleState::initLayout();
		
		// add other elements
		add(_txtTitle);
		
		// Set up objects
		_game->getResourcePack()->getSurface(defs->image_id)->blit(_bg);
		_btnOk->setColor(Palette::blockOffset(15)+2);
		_btnPrev->setColor(Palette::blockOffset(15)+2);
		_btnNext->setColor(Palette::blockOffset(15)+2);
		
		_txtTitle->setColor(Palette::blockOffset(14)+15);
		_txtTitle->setBig();
		_txtTitle->setAlign(ALIGN_LEFT);
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(Ufopaedia::buildText(_game, defs->title));

		_txtInfo = new Text(defs->rect_text.width, defs->rect_text.height, defs->rect_text.x, defs->rect_text.y);
		add(_txtInfo);
		
		_txtInfo->setColor(Palette::blockOffset(14)+15);
		_txtInfo->setAlign(ALIGN_LEFT);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(Ufopaedia::buildText(_game, defs->text));

		_lstInfo = new TextList(defs->rect_stats.width, defs->rect_stats.height, defs->rect_stats.x, defs->rect_stats.y);
		add(_lstInfo);
		
		std::wstringstream ss;
		_lstInfo->setColor(Palette::blockOffset(14)+15);
		_lstInfo->setColumns(2, 82, 40);
//		_lstInfo->setDot(true);

		ss.str(L"");ss.clear();
		ss << defs->craft->getMaxSpeed();
		
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_MAXIMUM_SPEED_UC").c_str(), ss.str().c_str());
		_lstInfo->getCell(0, 1)->setColor(Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << defs->craft->getAcceleration();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_ACCELERATION").c_str(), ss.str().c_str());
		_lstInfo->getCell(1, 1)->setColor(Palette::blockOffset(15)+4);
		
		ss.str(L"");ss.clear();
		ss << defs->craft->getMaxFuel();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_FUEL_CAPACITY").c_str(), ss.str().c_str());
		_lstInfo->getCell(2, 1)->setColor(Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << defs->craft->getWeapons();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_WEAPON_PODS").c_str(), ss.str().c_str());
		_lstInfo->getCell(3, 1)->setColor(Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << defs->craft->getMaxDamage();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_DAMAGE_CAPACITY_UC").c_str(), ss.str().c_str());
		_lstInfo->getCell(4, 1)->setColor(Palette::blockOffset(15)+4);
		
		ss.str(L"");ss.clear();
		ss << defs->craft->getSoldiers();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_CARGO_SPACE").c_str(), ss.str().c_str());
		_lstInfo->getCell(5, 1)->setColor(Palette::blockOffset(15)+4);
		
		ss.str(L"");ss.clear();
		ss << defs->craft->getHWPs();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_HWP_CAPACITY").c_str(), ss.str().c_str());
		_lstInfo->getCell(6, 1)->setColor(Palette::blockOffset(15)+4);

		_lstInfo->draw();
	}
	
	ArticleStateCraft::~ArticleStateCraft()
	{}
	
}
