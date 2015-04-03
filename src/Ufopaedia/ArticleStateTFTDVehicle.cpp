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

#include "../Ruleset/ArticleDefinition.h"
#include "ArticleStateTFTDVehicle.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextList.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/Unit.h"
#include "../Ruleset/Armor.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

	ArticleStateTFTDVehicle::ArticleStateTFTDVehicle(ArticleDefinitionTFTD *defs) : ArticleStateTFTD(defs)
	{
		Unit *unit = _game->getRuleset()->getUnit(defs->id);
		Armor *armor = _game->getRuleset()->getArmor(unit->getArmor());
		RuleItem *item = _game->getRuleset()->getItem(defs->id);

		_lstStats = new TextList(150, 89, 168, 86);

		add(_lstStats);

		_lstStats->setColor(Palette::blockOffset(0)+2);
		_lstStats->setColumns(2, 65, 85);
		_lstStats->setDot(true);

		std::wostringstream ss;
		ss << unit->getStats()->tu;
		_lstStats->addRow(2, tr("STR_TIME_UNITS").c_str(), ss.str().c_str());
		
		std::wostringstream ss2;
		ss2 << unit->getStats()->health;
		_lstStats->addRow(2, tr("STR_HEALTH").c_str(), ss2.str().c_str());
		
		std::wostringstream ss3;
		ss3 << armor->getFrontArmor();
		_lstStats->addRow(2, tr("STR_FRONT_ARMOR").c_str(), ss3.str().c_str());
		
		std::wostringstream ss4;
		ss4 << armor->getSideArmor();
		_lstStats->addRow(2, tr("STR_LEFT_ARMOR").c_str(), ss4.str().c_str());
		
		std::wostringstream ss5;
		ss5 << armor->getSideArmor();
		_lstStats->addRow(2, tr("STR_RIGHT_ARMOR").c_str(), ss5.str().c_str());
		
		std::wostringstream ss6;
		ss6 << armor->getRearArmor();
		_lstStats->addRow(2, tr("STR_REAR_ARMOR").c_str(), ss6.str().c_str());
		
		std::wostringstream ss7;
		ss7 << armor->getUnderArmor();
		_lstStats->addRow(2, tr("STR_UNDER_ARMOR").c_str(), ss7.str().c_str());
		
		_lstStats->addRow(2, tr("STR_WEAPON").c_str(), tr(defs->weapon).c_str());
				
		if (!item->getCompatibleAmmo()->empty())
		{
			RuleItem *ammo = _game->getRuleset()->getItem(item->getCompatibleAmmo()->front());

			std::wostringstream ss8;
			ss8 << ammo->getPower();
			_lstStats->addRow(2, tr("STR_WEAPON_POWER").c_str(), ss8.str().c_str());

			_lstStats->addRow(2, tr("STR_AMMUNITION").c_str(), tr(ammo->getName()).c_str());
			
			std::wostringstream ss9;
			ss9 << ammo->getClipSize();
			_lstStats->addRow(2, tr("STR_ROUNDS").c_str(), ss9.str().c_str());
		}
		else
		{
			std::wostringstream ss8;
			ss8 << item->getPower();
			_lstStats->addRow(2, tr("STR_WEAPON_POWER").c_str(), ss8.str().c_str());
		}

		for (size_t i = 0; i != _lstStats->getRows(); ++i)
		{
			_lstStats->setCellColor(i, 1, Palette::blockOffset(15)+4);
		}

		centerAllSurfaces();
	}

	ArticleStateTFTDVehicle::~ArticleStateTFTDVehicle()
	{}

}
