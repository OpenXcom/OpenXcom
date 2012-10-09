/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "ArticleStateUfo.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleUfo.h"
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

	ArticleStateUfo::ArticleStateUfo(Game *game, ArticleDefinitionUfo *defs) : ArticleState(game, defs->id)
	{
		RuleUfo *ufo = _game->getRuleset()->getUfo(defs->id);

		// add screen elements
		_txtTitle = new Text(160, 32, 5, 24);

		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());

		ArticleState::initLayout();

		// add other elements
		add(_txtTitle);

		// Set up objects
		_game->getResourcePack()->getSurface("BACK11.SCR")->blit(_bg);
		_btnOk->setColor(Palette::blockOffset(8)+5);
		_btnPrev->setColor(Palette::blockOffset(8)+5);
		_btnNext->setColor(Palette::blockOffset(8)+5);

		_txtTitle->setColor(Palette::blockOffset(8)+5);
		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(Ufopaedia::buildText(_game, defs->title));

		_image = new Surface(160, 52, 160, 6);
		add(_image);

		Surface *graphic = _game->getResourcePack()->getSurface("INTERWIN.DAT");
		graphic->setX(0);
		graphic->setY(0);
		graphic->getCrop()->x = 0;
		graphic->getCrop()->y = 0;
		graphic->getCrop()->w = 160;
		graphic->getCrop()->h = 52;
		_image->drawRect(graphic->getCrop(), 15);
/*
		graphic->getCrop()->y = 96;
		graphic->getCrop()->h = 15;
		graphic->blit(_image);
		graphic->setY(67);
		graphic->getCrop()->y = 111;
		graphic->getCrop()->h = 29;
		graphic->blit(_image);
*/
		graphic->getCrop()->y = 140 + 52 * ufo->getSprite();
		graphic->getCrop()->h = 52;
		graphic->blit(_image);

		_txtInfo = new Text(300, 50, 10, 140);
		add(_txtInfo);

		_txtInfo->setColor(Palette::blockOffset(8)+5);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(Ufopaedia::buildText(_game, defs->text));

		_lstInfo = new TextList(300, 60, 10, 68);
		add(_lstInfo);

		_lstInfo->setColor(Palette::blockOffset(8)+5);
		_lstInfo->setColumns(2, 180, 120);
//		_lstInfo->setCondensed(true);
		_lstInfo->setBig();
		_lstInfo->setDot(true);

		std::wstringstream ss;
		ss.str(L"");ss.clear();
		ss << ufo->getMaxDamage();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_DAMAGE_CAPACITY").c_str(), ss.str().c_str());

		ss.str(L"");ss.clear();
		ss << ufo->getWeaponPower();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_WEAPON_POWER").c_str(), ss.str().c_str());

		ss.str(L"");ss.clear();
		ss << ufo->getWeaponRange();
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_WEAPON_RANGE").c_str(), ss.str().c_str());

		ss.str(L"");ss.clear();
		ss << ufo->getMaxSpeed();
		ss << " ";
		ss << _game->getLanguage()->getString("STR_KNOTS");
		_lstInfo->addRow(2, _game->getLanguage()->getString("STR_MAXIMUM_SPEED").c_str(), ss.str().c_str());

		_lstInfo->draw();
	}

	ArticleStateUfo::~ArticleStateUfo()
	{}

}
