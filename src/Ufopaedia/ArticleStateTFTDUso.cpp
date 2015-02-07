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

#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleUfo.h"
#include "ArticleStateTFTD.h"
#include "ArticleStateTFTDUso.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateTFTDUso::ArticleStateTFTDUso(ArticleDefinitionTFTD *defs) : ArticleStateTFTD(defs)
	{
		RuleUfo *ufo = _game->getRuleset()->getUfo(defs->id);
		
		_lstInfo = new TextList(150, 50, 168, 142);
		add(_lstInfo);

		_lstInfo->setColor(Palette::blockOffset(0)+2);
		_lstInfo->setColumns(2, 90, 78);
		_lstInfo->setDot(true);

		_lstInfo->addRow(2, tr("STR_DAMAGE_CAPACITY").c_str(), Text::formatNumber(ufo->getMaxDamage()).c_str());

		_lstInfo->addRow(2, tr("STR_WEAPON_POWER").c_str(), Text::formatNumber(ufo->getWeaponPower()).c_str());

		_lstInfo->addRow(2, tr("STR_WEAPON_RANGE").c_str(), tr("STR_KILOMETERS").arg(ufo->getWeaponRange()).c_str());

		_lstInfo->addRow(2, tr("STR_MAXIMUM_SPEED").c_str(), tr("STR_KNOTS").arg(Text::formatNumber(ufo->getMaxSpeed())).c_str());
		for (int i = 0; i != 4; ++i)
		{
			_lstInfo->setCellColor(i, 1, Palette::blockOffset(15)+4);
		}

		centerAllSurfaces();
	}

	ArticleStateTFTDUso::~ArticleStateTFTDUso()
	{}

}
