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
#include "../fmath.h"
#include "Ufopaedia.h"
#include "ArticleStateArmor.h"
#include "../Ruleset/ArticleDefinition.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/Armor.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Engine/CrossPlatform.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{

	ArticleStateArmor::ArticleStateArmor(ArticleDefinitionArmor *defs) : ArticleState(defs->id), _row(0)
	{
		Armor *armor = _game->getRuleset()->getArmor(defs->id);

		// add screen elements
		_txtTitle = new Text(300, 17, 5, 24);

		// Set palette
		setPalette("PAL_BATTLEPEDIA");

		ArticleState::initLayout();

		// add other elements
		add(_txtTitle);

		// Set up objects
		_btnOk->setColor(Palette::blockOffset(0)+15);
		_btnPrev->setColor(Palette::blockOffset(0)+15);
		_btnNext->setColor(Palette::blockOffset(0)+15);

		_txtTitle->setColor(Palette::blockOffset(14)+15);
		_txtTitle->setBig();
		_txtTitle->setText(tr(defs->title));

		_image = new Surface(320, 200, 0, 0);
		add(_image);

		std::string look = armor->getSpriteInventory();
		look += "M0.SPK";
		if (!CrossPlatform::fileExists(CrossPlatform::getDataFile("UFOGRAPH/" + look)) && !_game->getResourcePack()->getSurface(look))
		{
			look = armor->getSpriteInventory() + ".SPK";
		}
		_game->getResourcePack()->getSurface(look)->blit(_image);


		_lstInfo = new TextList(150, 96, 150, 46);
		add(_lstInfo);

		_lstInfo->setColor(Palette::blockOffset(14)+15);
		_lstInfo->setColumns(2, 125, 25);
		_lstInfo->setDot(true);

		_txtInfo = new Text(300, 56, 8, 150);
		add(_txtInfo);

		_txtInfo->setColor(Palette::blockOffset(14)+15);
		_txtInfo->setWordWrap(true);
		_txtInfo->setText(tr(defs->text));

		// Add armor values
		addStat("STR_FRONT_ARMOR", armor->getFrontArmor());
		addStat("STR_LEFT_ARMOR", armor->getSideArmor());
		addStat("STR_RIGHT_ARMOR", armor->getSideArmor());
		addStat("STR_REAR_ARMOR", armor->getRearArmor());
		addStat("STR_UNDER_ARMOR", armor->getUnderArmor());

		_lstInfo->addRow(0);
		++_row;

		// Add damage modifiers
		for (int i = 0; i < DAMAGE_TYPES; ++i)
		{
			ItemDamageType dt = (ItemDamageType)i;
			int percentage = (int)Round(armor->getDamageModifier(dt) * 100.0f);
			std::string damage = getDamageTypeText(dt);
			if (percentage != 100 && damage != "STR_UNKNOWN")
			{
				addStat(damage, Text::formatPercentage(percentage));
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

	ArticleStateArmor::~ArticleStateArmor()
	{}

	void ArticleStateArmor::addStat(std::string label, int stat, bool plus)
	{
		if (stat != 0)
		{
			std::wostringstream ss;
			if (plus && stat > 0)
				ss << L"+";
			ss << stat;
			_lstInfo->addRow(2, tr(label).c_str(), ss.str().c_str());
			_lstInfo->setCellColor(_row, 1, Palette::blockOffset(15)+4);
			++_row;
		}
	}

	void ArticleStateArmor::addStat(std::string label, std::wstring stat)
	{
		_lstInfo->addRow(2, tr(label).c_str(), stat.c_str());
		_lstInfo->setCellColor(_row, 1, Palette::blockOffset(15)+4);
		++_row;
	}
}
