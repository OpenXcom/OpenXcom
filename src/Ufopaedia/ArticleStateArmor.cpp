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
#include "ArticleStateArmor.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/RuleArmor.h"
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
	
	ArticleStateArmor::ArticleStateArmor(Game *game, ArticleDefinitionArmor *defs) : ArticleState(game, defs->id)
	{
		// add screen elements
		_txtTitle = new Text(300, 32, 5, 24);
		
		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
		
		ArticleState::initLayout();
		
		// add other elements
		add(_txtTitle);
		
		// Set up objects
//		_game->getResourcePack()->getSurface("BACK11.SCR")->blit(_bg);
		_btnOk->setColor(Palette::blockOffset(8)+8);
		_btnPrev->setColor(Palette::blockOffset(8)+8);
		_btnNext->setColor(Palette::blockOffset(8)+8);
		
		_txtTitle->setColor(Palette::blockOffset(8)+5);
		_txtTitle->setBig();
		_txtTitle->setAlign(ALIGN_LEFT);
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(Ufopaedia::buildText(_game, defs->title));
		
		_image = new Surface(60, 100, 50, 45);
		add(_image);
		
/*
		Surface *graphic = _game->getResourcePack()->getSurface("INTERWIN.DAT");
		graphic->setX(0);
		graphic->setY(0);
		graphic->getCrop()->x = 0;
		graphic->getCrop()->y = 0;
		graphic->getCrop()->w = 160;
		graphic->getCrop()->h = 52;
		_image->drawRect(graphic->getCrop(), 15);
*/		
		_lstInfo = new TextList(150, 60, 150, 69);
		add(_lstInfo);
		
		_lstInfo->setColor(Palette::blockOffset(8)+5);
		_lstInfo->setColumns(2, 125, 25);
		_lstInfo->setDot(true);
		
		std::wstringstream ss;
		ss.str(L"");ss.clear();
		ss << defs->armor->getFrontArmor();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_FRONT_ARMOR").c_str(), ss.str().c_str());
		
		ss.str(L"");ss.clear();
		ss << defs->armor->getSideArmor();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_LEFT_ARMOR").c_str(), ss.str().c_str());
		
		ss.str(L"");ss.clear();
		ss << defs->armor->getSideArmor();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_RIGHT_ARMOR").c_str(), ss.str().c_str());
		
		ss.str(L"");ss.clear();
		ss << defs->armor->getRearArmor();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_REAR_ARMOR").c_str(), ss.str().c_str());
		
		ss.str(L"");ss.clear();
		ss << defs->armor->getUnderArmor();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_UNDER_ARMOR").c_str(), ss.str().c_str());
		
		_lstInfo->draw();
	}
	
	ArticleStateArmor::~ArticleStateArmor()
	{}
	
}
