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

#include "../Mod/ArticleDefinition.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleCraft.h"
#include "ArticleStateTFTD.h"
#include "ArticleStateTFTDCraft.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Text.h"

namespace OpenXcom
{

	ArticleStateTFTDCraft::ArticleStateTFTDCraft(ArticleDefinitionTFTD *defs) : ArticleStateTFTD(defs)
	{
		RuleCraft *craft = _game->getMod()->getCraft(defs->id);
		_txtStats = new Text(131, 56, 187, 116);
		add(_txtStats);

		_txtStats->setColor(Palette::blockOffset(0)+2);
		_txtStats->setSecondaryColor(Palette::blockOffset(15)+4);

		std::wostringstream ss;
		ss << tr("STR_MAXIMUM_SPEED_UC").arg(Text::formatNumber(craft->getMaxSpeed())) << L'\n';
		ss << tr("STR_ACCELERATION").arg(craft->getAcceleration()) << L'\n';
		ss << tr("STR_FUEL_CAPACITY").arg(Text::formatNumber(craft->getMaxFuel())) << L'\n';
		ss << tr("STR_WEAPON_PODS").arg(craft->getWeapons()) << L'\n';
		ss << tr("STR_DAMAGE_CAPACITY_UC").arg(Text::formatNumber(craft->getMaxDamage())) << L'\n';
		ss << tr("STR_CARGO_SPACE").arg(craft->getSoldiers()) << L'\n';
		ss << tr("STR_HWP_CAPACITY").arg(craft->getVehicles());
		_txtStats->setText(ss.str());

		centerAllSurfaces();
	}

	ArticleStateTFTDCraft::~ArticleStateTFTDCraft()
	{}

}
