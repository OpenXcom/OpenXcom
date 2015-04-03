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

#include <sstream>
#include "Ufopaedia.h"
#include "ArticleStateTFTDItem.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/RuleItem.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateTFTDItem::ArticleStateTFTDItem(ArticleDefinitionTFTD *defs) : ArticleStateTFTD(defs)
	{
		RuleItem *item = _game->getRuleset()->getItem(defs->id);

		std::vector<std::string> *ammo_data = item->getCompatibleAmmo();

		// SHOT STATS TABLE (for firearms only)
		if (item->getBattleType() == BT_FIREARM)
		{
			_txtShotType = new Text(100, 17, 28, 157);
			add(_txtShotType);
			_txtShotType->setColor(Palette::blockOffset(0)+2);
			_txtShotType->setWordWrap(true);
			_txtShotType->setText(tr("STR_SHOT_TYPE"));

			_txtAccuracy = new Text(50, 17, 76, 157);
			add(_txtAccuracy);
			_txtAccuracy->setColor(Palette::blockOffset(0)+2);
			_txtAccuracy->setWordWrap(true);
			_txtAccuracy->setText(tr("STR_ACCURACY_UC"));

			_txtTuCost = new Text(60, 17, 124, 157);
			add(_txtTuCost);
			_txtTuCost->setColor(Palette::blockOffset(0)+2);
			_txtTuCost->setWordWrap(true);
			_txtTuCost->setText(tr("STR_TIME_UNIT_COST"));

			_lstInfo = new TextList(204, 55, 28, 170);
			add(_lstInfo);

			_lstInfo->setColor(Palette::blockOffset(15)+4); // color for %-data!
			_lstInfo->setColumns(3, 48, 48, 48);

			int current_row = 0;
			if (item->getTUAuto()>0)
			{
				std::wstring tu = Text::formatPercentage(item->getTUAuto());
				if (item->getFlatRate())
				{
					tu.erase(tu.end() - 1);
				}
				_lstInfo->addRow(3,
								 tr("STR_SHOT_TYPE_AUTO").c_str(),
								 Text::formatPercentage(item->getAccuracyAuto()).c_str(),
								 tu.c_str());
				_lstInfo->setCellColor(current_row, 0, Palette::blockOffset(0)+2);
				current_row++;
			}

			if (item->getTUSnap()>0)
			{
				std::wstring tu = Text::formatPercentage(item->getTUSnap());
				if (item->getFlatRate())
				{
					tu.erase(tu.end() - 1);
				}
				_lstInfo->addRow(3,
								 tr("STR_SHOT_TYPE_SNAP").c_str(),
								 Text::formatPercentage(item->getAccuracySnap()).c_str(),
								 tu.c_str());
				_lstInfo->setCellColor(current_row, 0, Palette::blockOffset(0)+2);
				current_row++;
			}

			if (item->getTUAimed()>0)
			{
				std::wstring tu = Text::formatPercentage(item->getTUAimed());
				if (item->getFlatRate())
				{
					tu.erase(tu.end() - 1);
				}
				_lstInfo->addRow(3,
								 tr("STR_SHOT_TYPE_AIMED").c_str(),
								 Text::formatPercentage(item->getAccuracyAimed()).c_str(),
								 tu.c_str());
				_lstInfo->setCellColor(current_row, 0, Palette::blockOffset(0)+2);
				current_row++;
			}
		}

		// AMMO column
		std::wostringstream ss;

		for (int i = 0; i<3; ++i)
		{
			_txtAmmoType[i] = new Text(82, 16, 168, 144 + i*10);
			add(_txtAmmoType[i]);
			_txtAmmoType[i]->setColor(Palette::blockOffset(0)+2);
			_txtAmmoType[i]->setWordWrap(true);
			
			_txtAmmoDamage[i] = new Text(82, 16, 300, 144 + i*10);
			add(_txtAmmoDamage[i]);
			_txtAmmoDamage[i]->setColor(Palette::blockOffset(3)+6);
		}

		switch (item->getBattleType())
		{
			case BT_FIREARM:
				if (ammo_data->empty())
				{
					_txtAmmoType[0]->setText(tr(getDamageTypeText(item->getDamageType())));

					ss.str(L"");ss.clear();
					ss << item->getPower();
					if (item->getShotgunPellets())
					{
						ss << L"x" << item->getShotgunPellets();
					}
					_txtAmmoDamage[0]->setText(ss.str());
				}
				else
				{
					for (size_t i = 0; i < std::min(ammo_data->size(), (size_t)3); ++i)
					{
						ArticleDefinition *ammo_article = _game->getRuleset()->getUfopaediaArticle((*ammo_data)[i]);
						if (Ufopaedia::isArticleAvailable(_game->getSavedGame(), ammo_article))
						{
							RuleItem *ammo_rule = _game->getRuleset()->getItem((*ammo_data)[i]);
							_txtAmmoType[i]->setText(tr(getDamageTypeText(ammo_rule->getDamageType())));

							ss.str(L"");ss.clear();
							ss << ammo_rule->getPower();
							if (ammo_rule->getShotgunPellets())
							{
								ss << L"x" << ammo_rule->getShotgunPellets();
							}
							_txtAmmoDamage[i]->setText(ss.str());
						}
					}
				}
				break;
			case BT_AMMO:
			case BT_GRENADE:
			case BT_PROXIMITYGRENADE:
			case BT_MELEE:
				_txtAmmoType[0]->setText(tr(getDamageTypeText(item->getDamageType())));

				ss.str(L"");ss.clear();
				ss << item->getPower();
				_txtAmmoDamage[0]->setText(ss.str());
				break;
			default: break;
		}

		centerAllSurfaces();
	}

	ArticleStateTFTDItem::~ArticleStateTFTDItem()
	{}

}
