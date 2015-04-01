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

#include "Ufopaedia.h"
#include <sstream>
#include "ArticleState.h"
#include "../Engine/Game.h"
#include "../Engine/Options.h"
#include "../Engine/Surface.h"
#include "../Engine/Language.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Ruleset/RuleItem.h"

namespace OpenXcom
{

	/**
	 * Constructor
	 * @param game Pointer to current game.
	 * @param article_id The article id of this article state instance.
	 */
	ArticleState::ArticleState(const std::string &article_id) : _id(article_id)
	{
		// init background and navigation elements
		_bg = new Surface(320, 200, 0, 0);
		_btnOk = new TextButton(30, 14, 5, 5);
		_btnPrev = new TextButton(30, 14, 40, 5);
		_btnNext = new TextButton(30, 14, 75, 5);
	}

	/**
	 * Destructor
	 */
	ArticleState::~ArticleState()
	{}

	std::string ArticleState::getDamageTypeText(ItemDamageType dt) const
	{
		std::string type;
		switch (dt)
		{
		case DT_AP:
			type = "STR_DAMAGE_ARMOR_PIERCING";
			break;
		case DT_IN:
			type = "STR_DAMAGE_INCENDIARY";
			break;
		case DT_HE:
			type = "STR_DAMAGE_HIGH_EXPLOSIVE";
			break;
		case DT_LASER:
			type = "STR_DAMAGE_LASER_BEAM";
			break;
		case DT_PLASMA:
			type = "STR_DAMAGE_PLASMA_BEAM";
			break;
		case DT_STUN:
			type = "STR_DAMAGE_STUN";
			break;
		case DT_MELEE:
			type = "STR_DAMAGE_MELEE";
			break;
		case DT_ACID:
			type = "STR_DAMAGE_ACID";
			break;
		case DT_SMOKE:
			type = "STR_DAMAGE_SMOKE";
			break;
		default:
			type = "STR_UNKNOWN";
			break;
		}
		return type;
	}

	/**
	 * Set captions and click handlers for the common control elements.
	 */
	void ArticleState::initLayout()
	{
		add(_bg);
		add(_btnOk);
		add(_btnPrev);
		add(_btnNext);

		_btnOk->setText(tr("STR_OK"));
		_btnOk->onMouseClick((ActionHandler)&ArticleState::btnOkClick);
		_btnOk->onKeyboardPress((ActionHandler)&ArticleState::btnOkClick,Options::keyOk);
		_btnOk->onKeyboardPress((ActionHandler)&ArticleState::btnOkClick,Options::keyCancel);
		_btnPrev->setText(L"<<");
		_btnPrev->onMouseClick((ActionHandler)&ArticleState::btnPrevClick);
		_btnNext->setText(L">>");
		_btnNext->onMouseClick((ActionHandler)&ArticleState::btnNextClick);
	}

	/**
	 * Returns to the previous screen.
	 * @param action Pointer to an action.
	 */
	void ArticleState::btnOkClick(Action *)
	{
		_game->popState();
	}

	/**
	 * Shows the previous available article.
	 * @param action Pointer to an action.
	 */
	void ArticleState::btnPrevClick(Action *)
	{
		Ufopaedia::prev(_game);
	}

	/**
	 * Shows the next available article. Loops to the first.
	 * @param action Pointer to an action.
	 */
	void ArticleState::btnNextClick(Action *)
	{
		Ufopaedia::next(_game);
	}

}
