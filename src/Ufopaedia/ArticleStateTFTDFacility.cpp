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
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateTFTDFacility::ArticleStateTFTDFacility(ArticleDefinitionTFTD *defs) : ArticleStateTFTD(defs)
	{
		RuleBaseFacility *facility = _game->getMod()->getBaseFacility(defs->id, true);

		_lstInfo = new TextList(150, 50, 168, 150);
		add(_lstInfo);

		_lstInfo->setColor(Palette::blockOffset(0)+2);
		_lstInfo->setColumns(2, 104, 46);
		_lstInfo->setDot(true);

		std::wostringstream ss;
		int row = 0, offy=0;
		if (facility->getDefenseValue() > 0)
			offy += 16;
		if (facility->getRadarRange() > 0)
			offy += 16;
		if (facility->getStorage() > 0)
			offy += 8;
		if (facility->getPersonnel() > 0)
			offy += 8;
		if (facility->getAliens() > 0)
			offy += 8;
		if (facility->getCrafts() > 0)
			offy += 8;
		if (facility->getLaboratories() > 0)
			offy += 8;
		if (facility->getWorkshops() > 0)
			offy += 8;
		if (facility->getPsiLaboratories() > 0)
			offy += 8;
		if (offy)
			_lstInfo->setY(_lstInfo->getY() - offy);

		if (facility->getDefenseValue() > 0)
		{
			ss.str(L"");ss.clear();
			ss << facility->getDefenseValue();
			_lstInfo->addRow(2, tr("STR_DEFENSE_VALUE").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);

			ss.str(L"");ss.clear();
			ss << Text::formatPercentage(facility->getHitRatio());
			_lstInfo->addRow(2, tr("STR_HIT_RATIO").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}
		if (facility->getRadarRange() > 0)
		{
			ss.str(L""); ss.clear();
			ss << facility->getRadarRange();
			_lstInfo->addRow(2, tr("STR_RADAR_RANGE").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);

			ss.str(L""); ss.clear();
			ss << Text::formatPercentage(facility->getRadarChance());
			if (facility->isHyperwave())
				ss << tr("STR_HYPERWAVE_MARK");
			_lstInfo->addRow(2, tr("STR_RADAR_CHANCE").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}
		if (facility->getStorage() > 0)
		{
			ss.str(L""); ss.clear();
			ss << facility->getStorage();
			_lstInfo->addRow(2, tr("STR_STORAGE_CAP").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}
		if (facility->getPersonnel() > 0)
		{
			ss.str(L""); ss.clear();
			ss << facility->getPersonnel();
			_lstInfo->addRow(2, tr("STR_PERSONNEL_CAP").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}
		if (facility->getAliens() > 0)
		{
			ss.str(L""); ss.clear();
			ss << facility->getAliens();
			_lstInfo->addRow(2, tr("STR_ALIENS_CAP").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}
		if (facility->getCrafts() > 0)
		{
			ss.str(L""); ss.clear();
			ss << facility->getCrafts();
			_lstInfo->addRow(2, tr("STR_CRAFT_CAP").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}
		if (facility->getLaboratories() > 0)
		{
			ss.str(L""); ss.clear();
			ss << facility->getLaboratories();
			_lstInfo->addRow(2, tr("STR_LABORATORY_CAP").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}
		if (facility->getWorkshops() > 0)
		{
			ss.str(L""); ss.clear();
			ss << facility->getWorkshops();
			_lstInfo->addRow(2, tr("STR_WORKSHOP_CAP").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}
		if (facility->getPsiLaboratories() > 0)
		{
			ss.str(L""); ss.clear();
			ss << facility->getPsiLaboratories();
			_lstInfo->addRow(2, tr("STR_PSILAB_CAP").c_str(), ss.str().c_str());
			_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);
		}

		ss.str(L"");ss.clear();
		_lstInfo->addRow(2, tr("STR_CONSTRUCTION_TIME").c_str(), tr("STR_DAY", facility->getBuildTime()).c_str());
		_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);

		ss << Text::formatFunding(facility->getBuildCost());
		_lstInfo->addRow(2, tr("STR_CONSTRUCTION_COST").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);

		ss.str(L"");ss.clear();
		ss << Text::formatFunding(facility->getMonthlyCost());
		_lstInfo->addRow(2, tr("STR_MAINTENANCE_COST").c_str(), ss.str().c_str());
		_lstInfo->setCellColor(row++, 1, Palette::blockOffset(15)+4);

		centerAllSurfaces();
	}

	ArticleStateTFTDFacility::~ArticleStateTFTDFacility()
	{}

}
