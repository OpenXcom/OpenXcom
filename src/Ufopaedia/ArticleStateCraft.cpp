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
#include "ArticleStateCraft.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleCraft.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"

namespace OpenXcom
{

	ArticleStateCraft::ArticleStateCraft(Game *game, ArticleDefinitionCraft *defs) : ArticleState(game, defs->id)
	{
		RuleCraft *craft = _game->getRuleset()->getCraft(defs->id);

		// add screen elements
		_txtTitle = new Text(140, 32, 5, 24);

		// Set palette
		_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_3")->getColors());

		ArticleState::initLayout();

		// add other elements
		add(_txtTitle);

		// Set up objects
		_game->getResourcePack()->getSurface(defs->image_id)->blit(_bg);
		_btnOk->setColor(Palette::blockOffset(15)-1);
		_btnPrev->setColor(Palette::blockOffset(15)-1);
		_btnNext->setColor(Palette::blockOffset(15)-1);

		_txtTitle->setColor(Palette::blockOffset(14)+15);
		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(Ufopaedia::buildText(_game, defs->title));

		_txtInfo = new Text(defs->rect_text.width, defs->rect_text.height, defs->rect_text.x, defs->rect_text.y);
		add(_txtInfo);

		_txtInfo->setColor(Palette::blockOffset(14)+15);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(Ufopaedia::buildText(_game, defs->text));

		_txtStats = new Text(defs->rect_stats.width, defs->rect_stats.height, defs->rect_stats.x, defs->rect_stats.y);
		add(_txtStats);

		_txtStats->setColor(Palette::blockOffset(14)+15);
		_txtStats->setSecondaryColor(Palette::blockOffset(15)+4);

		std::wstringstream ss;
		ss << _game->getLanguage()->getString("STR_MAXIMUM_SPEED_UC") << L'\x01' << craft->getMaxSpeed() << L'\x01' << L'\n';
		ss << _game->getLanguage()->getString("STR_ACCELERATION") << L'\x01' << craft->getAcceleration() << L'\x01' << L'\n';
		ss << _game->getLanguage()->getString("STR_FUEL_CAPACITY") << L'\x01' << craft->getMaxFuel() << L'\x01' << L'\n';
		ss << _game->getLanguage()->getString("STR_WEAPON_PODS") << L'\x01' << craft->getWeapons() << L'\x01' << L'\n';
		ss << _game->getLanguage()->getString("STR_DAMAGE_CAPACITY_UC") << L'\x01' << craft->getMaxDamage() << L'\x01' << L'\n';
		ss << _game->getLanguage()->getString("STR_CARGO_SPACE") << L'\x01' << craft->getSoldiers() << L'\x01' << L'\n';
		ss << _game->getLanguage()->getString("STR_HWP_CAPACITY") << L'\x01' << craft->getVehicles();
		_txtStats->setText(ss.str());
	}

	ArticleStateCraft::~ArticleStateCraft()
	{}

}
