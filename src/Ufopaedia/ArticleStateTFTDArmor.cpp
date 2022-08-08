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

#include <sstream>
#include "../fmath.h"
#include "ArticleStateTFTD.h"
#include "ArticleStateTFTDArmor.h"
#include "../Mod/ArticleDefinition.h"
#include "../Mod/Mod.h"
#include "../Mod/Armor.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Unicode.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateTFTDArmor::ArticleStateTFTDArmor(ArticleDefinitionTFTD *defs) : ArticleStateTFTD(defs), _row(0)
	{
		_txtInfo->setHeight(72);

		Armor *armor = _game->getMod()->getArmor(defs->id, true);

		_lstInfo = new TextList(150, 64, 168, 110);
		add(_lstInfo);

		_lstInfo->setColor(Palette::blockOffset(0)+2);
		_lstInfo->setColumns(2, 125, 25);
		_lstInfo->setDot(true);

		// Add armor values
		addStat("STR_FRONT_ARMOR", armor->getFrontArmor());
		addStat("STR_LEFT_ARMOR", armor->getSideArmor());
		addStat("STR_RIGHT_ARMOR", armor->getSideArmor());
		addStat("STR_REAR_ARMOR", armor->getRearArmor());
		addStat("STR_UNDER_ARMOR", armor->getUnderArmor());

		_lstInfo->addRow(0);
		++_row;

		// Add damage modifiers
		for (int i = 0; i < Armor::DAMAGE_TYPES; ++i)
		{
			ItemDamageType dt = (ItemDamageType)i;
			int percentage = (int)Round(armor->getDamageModifier(dt) * 100.0f);
			std::string damage = getDamageTypeText(dt);
			if (percentage != 100 && damage != "STR_UNKNOWN")
			{
				addStat(damage, Unicode::formatPercentage(percentage));
			}
		}

		_lstInfo->addRow(0);
		++_row;

		// Add unit stats
		addStat("STR_TIME_UNITS", armor->getStats()->tu, true);
		addStat("STR_STAMINA", armor->getStats()->stamina, true);
		addStat("STR_HEALTH", armor->getStats()->health, true);
		addStat("STR_BRAVERY", armor->getStats()->bravery, true);
		addStat("STR_REACTIONS", armor->getStats()->reactions, true);
		addStat("STR_FIRING_ACCURACY", armor->getStats()->firing, true);
		addStat("STR_THROWING_ACCURACY", armor->getStats()->throwing, true);
		addStat("STR_MELEE_ACCURACY", armor->getStats()->melee, true);
		addStat("STR_STRENGTH", armor->getStats()->strength, true);
		addStat("STR_PSIONIC_STRENGTH", armor->getStats()->psiStrength, true);
		addStat("STR_PSIONIC_SKILL", armor->getStats()->psiSkill, true);

		centerAllSurfaces();
	}

	ArticleStateTFTDArmor::~ArticleStateTFTDArmor()
	{}

	void ArticleStateTFTDArmor::addStat(const std::string &label, int stat, bool plus)
	{
		if (stat != 0)
		{
			std::ostringstream ss;
			if (plus && stat > 0)
				ss << "+";
			ss << stat;
			_lstInfo->addRow(2, tr(label).c_str(), ss.str().c_str());
			_lstInfo->setCellColor(_row, 1, Palette::blockOffset(15)+4);
			++_row;
		}
	}

	void ArticleStateTFTDArmor::addStat(const std::string &label, const std::string &stat)
	{
		_lstInfo->addRow(2, tr(label).c_str(), stat.c_str());
		_lstInfo->setCellColor(_row, 1, Palette::blockOffset(15)+4);
		++_row;
	}
}
