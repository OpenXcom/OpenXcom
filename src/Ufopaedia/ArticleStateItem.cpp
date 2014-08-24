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

#include <sstream>
#include <algorithm>
#include "Ufopaedia.h"
#include "ArticleStateItem.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/RuleItem.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateItem::ArticleStateItem(ArticleDefinitionItem *defs) : ArticleState(defs->id)
	{
		RuleItem *item = _game->getRuleset()->getItem(defs->id);

		// add screen elements
		_txtTitle = new Text(148, 32, 5, 24);

		// Set palette
		setPalette("PAL_BATTLEPEDIA");

		ArticleState::initLayout();

		// add other elements
		add(_txtTitle);

		// Set up objects
		_game->getResourcePack()->getSurface("BACK08.SCR")->blit(_bg);
		_btnOk->setColor(Palette::blockOffset(9));
		_btnPrev->setColor(Palette::blockOffset(9));
		_btnNext->setColor(Palette::blockOffset(9));

		_txtTitle->setColor(Palette::blockOffset(14)+15);
		_txtTitle->setBig();
		_txtTitle->setWordWrap(true);
		_txtTitle->setText(tr(defs->title));

		// IMAGE
		_image = new Surface(32, 48, 157, 5);
		add(_image);

		item->drawHandSprite(_game->getResourcePack()->getSurfaceSet("BIGOBS.PCK"), _image);

		std::vector<std::string> *ammo_data = item->getCompatibleAmmo();

		// SHOT STATS TABLE (for firearms only)
		if (item->getBattleType() == BT_FIREARM)
		{
			_txtShotType = new Text(100, 17, 8, 66);
			add(_txtShotType);
			_txtShotType->setColor(Palette::blockOffset(14)+15);
			_txtShotType->setWordWrap(true);
			_txtShotType->setText(tr("STR_SHOT_TYPE"));

			_txtAccuracy = new Text(50, 17, 104, 66);
			add(_txtAccuracy);
			_txtAccuracy->setColor(Palette::blockOffset(14)+15);
			_txtAccuracy->setWordWrap(true);
			_txtAccuracy->setText(tr("STR_ACCURACY_UC"));

			_txtTuCost = new Text(60, 17, 158, 66);
			add(_txtTuCost);
			_txtTuCost->setColor(Palette::blockOffset(14)+15);
			_txtTuCost->setWordWrap(true);
			_txtTuCost->setText(tr("STR_TIME_UNIT_COST"));

			_lstInfo = new TextList(204, 55, 8, 82);
			add(_lstInfo);

			_lstInfo->setColor(Palette::blockOffset(15)+4); // color for %-data!
			_lstInfo->setColumns(3, 100, 52, 52);
			_lstInfo->setBig();

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
				_lstInfo->setCellColor(current_row, 0, Palette::blockOffset(14)+15);
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
				_lstInfo->setCellColor(current_row, 0, Palette::blockOffset(14)+15);
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
				_lstInfo->setCellColor(current_row, 0, Palette::blockOffset(14)+15);
				current_row++;
			}

			// text_info is BELOW the info table
			_txtInfo = new Text((ammo_data->size()<3 ? 300 : 180), 56, 8, 138);
		}
		else
		{
			// text_info is larger and starts on top
			_txtInfo = new Text(300, 125, 8, 67);
		}

		add(_txtInfo);

		_txtInfo->setColor(Palette::blockOffset(14)+15);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(tr(defs->text));


		// AMMO column
		std::wostringstream ss;

		for (int i = 0; i<3; ++i)
		{
			_txtAmmoType[i] = new Text(82, 16, 194, 20 + i*49);
			add(_txtAmmoType[i]);
			_txtAmmoType[i]->setColor(Palette::blockOffset(14)+15);
			_txtAmmoType[i]->setAlign(ALIGN_CENTER);
			_txtAmmoType[i]->setVerticalAlign(ALIGN_MIDDLE);
			_txtAmmoType[i]->setWordWrap(true);

			_txtAmmoDamage[i] = new Text(82, 17, 194, 40 + i*49);
			add(_txtAmmoDamage[i]);
			_txtAmmoDamage[i]->setColor(Palette::blockOffset(2));
			_txtAmmoDamage[i]->setAlign(ALIGN_CENTER);
			_txtAmmoDamage[i]->setBig();

			_imageAmmo[i] = new Surface(32, 48, 280, 16 + i*49);
			add(_imageAmmo[i]);
		}

		switch (item->getBattleType())
		{
			case BT_FIREARM:
				_txtDamage = new Text(82, 10, 194, 7);
				add(_txtDamage);
				_txtDamage->setColor(Palette::blockOffset(14)+15);
				_txtDamage->setAlign(ALIGN_CENTER);
				_txtDamage->setText(tr("STR_DAMAGE_UC"));

				_txtAmmo = new Text(50, 10, 268, 7);
				add(_txtAmmo);
				_txtAmmo->setColor(Palette::blockOffset(14)+15);
				_txtAmmo->setAlign(ALIGN_CENTER);
				_txtAmmo->setText(tr("STR_AMMO"));

				if (ammo_data->empty())
				{
					_txtAmmoType[0]->setText(tr(getDamageTypeText(item->getDamageType()->ResistType)));

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
							_txtAmmoType[i]->setText(tr(getDamageTypeText(ammo_rule->getDamageType()->ResistType)));

							ss.str(L"");ss.clear();
							ss << ammo_rule->getPower();
							if (ammo_rule->getShotgunPellets())
							{
								ss << L"x" << ammo_rule->getShotgunPellets();
							}
							_txtAmmoDamage[i]->setText(ss.str());

							ammo_rule->drawHandSprite(_game->getResourcePack()->getSurfaceSet("BIGOBS.PCK"), _imageAmmo[i]);
						}
					}
				}
				break;
			case BT_AMMO:
			case BT_GRENADE:
			case BT_PROXIMITYGRENADE:
			case BT_MELEE:
				_txtDamage = new Text(82, 10, 194, 7);
				add(_txtDamage);
				_txtDamage->setColor(Palette::blockOffset(14)+15);
				_txtDamage->setAlign(ALIGN_CENTER);
				_txtDamage->setText(tr("STR_DAMAGE_UC"));

				_txtAmmoType[0]->setText(tr(getDamageTypeText(item->getDamageType()->ResistType)));

				ss.str(L"");ss.clear();
				ss << item->getPower();
				_txtAmmoDamage[0]->setText(ss.str());
				break;
			default: break;
		}

		centerAllSurfaces();
	}

	ArticleStateItem::~ArticleStateItem()
	{}

}
