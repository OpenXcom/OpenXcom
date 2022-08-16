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

#include "../Mod/ArticleDefinition.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleBaseFacility.h"
#include "ArticleStateTFTD.h"
#include "ArticleStateTFTDFacility.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Unicode.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateTFTDFacility::ArticleStateTFTDFacility(ArticleDefinitionTFTD *defs) : ArticleStateTFTD(defs)
	{
		_txtInfo->setHeight(112);

		RuleBaseFacility *facility = _game->getMod()->getBaseFacility(defs->id, true);

		_lstInfo = new TextList(150, 50, 168, 150);
		add(_lstInfo);

		_lstInfo->setColor(Palette::blockOffset(0)+2);
		_lstInfo->setColumns(2, 104, 46);
		_lstInfo->setDot(true);

		std::ostringstream ss;
		int row = 0;
		if (facility->getDefenseValue() > 0)
		{
			_lstInfo->setY(_lstInfo->getY() - 16);
			_txtInfo->setHeight(_txtInfo->getHeight() - 16);
			ss.str("");ss.clear();
			ss << facility->getDefenseValue();
			_lstInfo->addRow(2, tr("STR_DEFENSE_VALUE").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);

			ss.str("");ss.clear();
			ss << Unicode::formatPercentage(facility->getHitRatio());
			_lstInfo->addRow(2, tr("STR_HIT_RATIO").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}

		ss.str("");ss.clear();
		_lstInfo->addRow(2, tr("STR_CONSTRUCTION_TIME").c_str(), tr("STR_DAY", facility->getBuildTime()).c_str());
		_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);

		ss << Unicode::formatFunding(facility->getBuildCost());
		_lstInfo->addRow(2, tr("STR_CONSTRUCTION_COST").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);

		ss.str("");ss.clear();
		ss << Unicode::formatFunding(facility->getMonthlyCost());
		_lstInfo->addRow(2, tr("STR_MAINTENANCE_COST").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);

		centerAllSurfaces();
	}

	ArticleStateTFTDFacility::~ArticleStateTFTDFacility()
	{}

}
