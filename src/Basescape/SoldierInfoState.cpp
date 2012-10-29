/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "SoldierInfoState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/Bar.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Engine/Surface.h"
#include "../Savegame/Base.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/Soldier.h"
#include "../Engine/SurfaceSet.h"
#include "../Ruleset/Armor.h"
#include "SoldierArmorState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param soldier ID of the selected soldier.
 */
SoldierInfoState::SoldierInfoState(Game *game, Base *base, size_t soldier) : State(game), _base(base), _soldier(soldier)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_rank = new Surface(26, 23, 4, 4);
	_btnPrev = new TextButton(28, 14, 0, 33);
	_btnOk = new TextButton(48, 14, 30, 33);
	_btnNext = new TextButton(28, 14, 80, 33);
	_btnArmor = new TextButton(60, 14, 130, 33);
	_edtSoldier = new TextEdit(200, 16, 40, 9);
	_txtArmor = new Text(120, 9, 194, 38);
	_txtRank = new Text(130, 9, 0, 48);
	_txtMissions = new Text(100, 9, 130, 48);
	_txtKills = new Text(100, 9, 230, 48);
	_txtCraft = new Text(130, 9, 0, 56);
	_txtRecovery = new Text(180, 9, 130, 56);

	_txtTimeUnits = new Text(120, 9, 6, 82);
	_numTimeUnits = new Text(18, 9, 131, 82);
	_barTimeUnits = new Bar(170, 7, 150, 82);

	_txtStamina = new Text(120, 9, 6, 94);
	_numStamina = new Text(18, 9, 131, 94);
	_barStamina = new Bar(170, 7, 150, 94);

	_txtHealth = new Text(120, 9, 6, 106);
	_numHealth = new Text(18, 9, 131, 106);
	_barHealth = new Bar(170, 7, 150, 106);

	_txtBravery = new Text(120, 9, 6, 118);
	_numBravery = new Text(18, 9, 131, 118);
	_barBravery = new Bar(170, 7, 150, 118);

	_txtReactions = new Text(120, 9, 6, 130);
	_numReactions = new Text(18, 9, 131, 130);
	_barReactions = new Bar(170, 7, 150, 130);

	_txtFiring = new Text(120, 9, 6, 142);
	_numFiring = new Text(18, 9, 131, 142);
	_barFiring = new Bar(170, 7, 150, 142);

	_txtThrowing = new Text(120, 9, 6, 154);
	_numThrowing = new Text(18, 9, 131, 154);
	_barThrowing = new Bar(170, 7, 150, 154);

	_txtStrength = new Text(120, 9, 6, 166);
	_numStrength = new Text(18, 9, 131, 166);
	_barStrength = new Bar(170, 7, 150, 166);

	add(_bg);
	add(_rank);
	add(_btnOk);
	add(_btnPrev);
	add(_btnNext);
	add(_btnArmor);
	add(_edtSoldier);
	add(_txtArmor);
	add(_txtRank);
	add(_txtMissions);
	add(_txtKills);
	add(_txtCraft);
	add(_txtRecovery);

	add(_txtTimeUnits);
	add(_numTimeUnits);
	add(_barTimeUnits);

	add(_txtStamina);
	add(_numStamina);
	add(_barStamina);

	add(_txtHealth);
	add(_numHealth);
	add(_barHealth);

	add(_txtBravery);
	add(_numBravery);
	add(_barBravery);

	add(_txtReactions);
	add(_numReactions);
	add(_barReactions);

	add(_txtFiring);
	add(_numFiring);
	add(_barFiring);

	add(_txtThrowing);
	add(_numThrowing);
	add(_barThrowing);

	add(_txtStrength);
	add(_numStrength);
	add(_barStrength);

	// Set up objects
	_game->getResourcePack()->getSurface("BACK06.SCR")->blit(_bg);

	_btnOk->setColor(Palette::blockOffset(15)+6);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierInfoState::btnOkClick);

	_btnPrev->setColor(Palette::blockOffset(15)+6);
	_btnPrev->setText(L"<<");
	_btnPrev->onMouseClick((ActionHandler)&SoldierInfoState::btnPrevClick);

	_btnNext->setColor(Palette::blockOffset(15)+6);
	_btnNext->setText(L">>");
	_btnNext->onMouseClick((ActionHandler)&SoldierInfoState::btnNextClick);

	_btnArmor->setColor(Palette::blockOffset(15)+6);
	_btnArmor->setText(_game->getLanguage()->getString("STR_ARMOR"));
	_btnArmor->onMouseClick((ActionHandler)&SoldierInfoState::btnArmorClick);

	_edtSoldier->setColor(Palette::blockOffset(13)+10);
	_edtSoldier->setBig();
	_edtSoldier->onKeyboardPress((ActionHandler)&SoldierInfoState::edtSoldierKeyPress);

	_txtArmor->setColor(Palette::blockOffset(13));

	_txtRank->setColor(Palette::blockOffset(13)+10);
	_txtRank->setSecondaryColor(Palette::blockOffset(13));

	_txtMissions->setColor(Palette::blockOffset(13)+10);
	_txtMissions->setSecondaryColor(Palette::blockOffset(13));

	_txtKills->setColor(Palette::blockOffset(13)+10);
	_txtKills->setSecondaryColor(Palette::blockOffset(13));

	_txtCraft->setColor(Palette::blockOffset(13)+10);
	_txtCraft->setSecondaryColor(Palette::blockOffset(13));

	_txtRecovery->setColor(Palette::blockOffset(13)+10);
	_txtRecovery->setSecondaryColor(Palette::blockOffset(13));


	_txtTimeUnits->setColor(Palette::blockOffset(15)+1);
	_txtTimeUnits->setText(_game->getLanguage()->getString("STR_TIME_UNITS"));

	_numTimeUnits->setColor(Palette::blockOffset(13));

	_barTimeUnits->setColor(Palette::blockOffset(3));
	_barTimeUnits->setColor2(Palette::blockOffset(3)+4);
	_barTimeUnits->setScale(1.0);
	_barTimeUnits->setInvert(true);

	_txtStamina->setColor(Palette::blockOffset(15)+1);
	_txtStamina->setText(_game->getLanguage()->getString("STR_STAMINA"));

	_numStamina->setColor(Palette::blockOffset(13));

	_barStamina->setColor(Palette::blockOffset(9));
	_barStamina->setColor2(Palette::blockOffset(9)+4);
	_barStamina->setScale(1.0);
	_barStamina->setInvert(true);

	_txtHealth->setColor(Palette::blockOffset(15)+1);
	_txtHealth->setText(_game->getLanguage()->getString("STR_HEALTH"));

	_numHealth->setColor(Palette::blockOffset(13));

	_barHealth->setColor(Palette::blockOffset(2));
	_barHealth->setColor2(Palette::blockOffset(2)+4);
	_barHealth->setScale(1.0);
	_barHealth->setInvert(true);

	_txtBravery->setColor(Palette::blockOffset(15)+1);
	_txtBravery->setText(_game->getLanguage()->getString("STR_BRAVERY"));

	_numBravery->setColor(Palette::blockOffset(13));

	_barBravery->setColor(Palette::blockOffset(4));
	_barBravery->setColor2(Palette::blockOffset(4)+4);
	_barBravery->setScale(1.0);
	_barBravery->setInvert(true);

	_txtReactions->setColor(Palette::blockOffset(15)+1);
	_txtReactions->setText(_game->getLanguage()->getString("STR_REACTIONS"));

	_numReactions->setColor(Palette::blockOffset(13));

	_barReactions->setColor(Palette::blockOffset(1));
	_barReactions->setColor2(Palette::blockOffset(1)+4);
	_barReactions->setScale(1.0);
	_barReactions->setInvert(true);

	_txtFiring->setColor(Palette::blockOffset(15)+1);
	_txtFiring->setText(_game->getLanguage()->getString("STR_FIRING_ACCURACY"));

	_numFiring->setColor(Palette::blockOffset(13));

	_barFiring->setColor(Palette::blockOffset(8));
	_barFiring->setColor2(Palette::blockOffset(8)+4);
	_barFiring->setScale(1.0);
	_barFiring->setInvert(true);

	_txtThrowing->setColor(Palette::blockOffset(15)+1);
	_txtThrowing->setText(_game->getLanguage()->getString("STR_THROWING_ACCURACY"));

	_numThrowing->setColor(Palette::blockOffset(13));

	_barThrowing->setColor(Palette::blockOffset(10));
	_barThrowing->setColor2(Palette::blockOffset(10)+4);
	_barThrowing->setScale(1.0);
	_barThrowing->setInvert(true);

	_txtStrength->setColor(Palette::blockOffset(15)+1);
	_txtStrength->setText(_game->getLanguage()->getString("STR_STRENGTH"));

	_numStrength->setColor(Palette::blockOffset(13));

	_barStrength->setColor(Palette::blockOffset(5));
	_barStrength->setColor2(Palette::blockOffset(5)+4);
	_barStrength->setScale(1.0);
	_barStrength->setInvert(true);
}

/**
 *
 */
SoldierInfoState::~SoldierInfoState()
{

}

/**
 * Update soldier stats when
 * the soldier changes.
 */
void SoldierInfoState::init()
{
	Soldier *s = _base->getSoldiers()->at(_soldier);
	_edtSoldier->setText(s->getName());
	UnitStats *initial = s->getInitStats();
	UnitStats *current = s->getCurrentStats();

	SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
	texture->getFrame(s->getRankSprite())->setX(0);
	texture->getFrame(s->getRankSprite())->setY(0);
	texture->getFrame(s->getRankSprite())->blit(_rank);

	std::wstringstream ss;
	ss << current->tu;
	_numTimeUnits->setText(ss.str());
	_barTimeUnits->setMax(current->tu);
	_barTimeUnits->setValue(current->tu);
	_barTimeUnits->setValue2(initial->tu);

	std::wstringstream ss2;
	ss2 << current->stamina;
	_numStamina->setText(ss2.str());
	_barStamina->setMax(current->stamina);
	_barStamina->setValue(current->stamina);
	_barStamina->setValue2(initial->stamina);

	std::wstringstream ss3;
	ss3 << current->health;
	_numHealth->setText(ss3.str());
	_barHealth->setMax(current->health);
	_barHealth->setValue(current->health);
	_barHealth->setValue2(initial->health);

	std::wstringstream ss4;
	ss4 << current->bravery;
	_numBravery->setText(ss4.str());
	_barBravery->setMax(current->bravery);
	_barBravery->setValue(current->bravery);
	_barBravery->setValue2(initial->bravery);

	std::wstringstream ss5;
	ss5 << current->reactions;
	_numReactions->setText(ss5.str());
	_barReactions->setMax(current->reactions);
	_barReactions->setValue(current->reactions);
	_barReactions->setValue2(initial->reactions);

	std::wstringstream ss6;
	ss6 << current->firing;
	_numFiring->setText(ss6.str());
	_barFiring->setMax(current->firing);
	_barFiring->setValue(current->firing);
	_barFiring->setValue2(initial->firing);

	std::wstringstream ss7;
	ss7 << current->throwing;
	_numThrowing->setText(ss7.str());
	_barThrowing->setMax(current->throwing);
	_barThrowing->setValue(current->throwing);
	_barThrowing->setValue2(initial->throwing);

	std::wstringstream ss8;
	ss8 << current->strength;
	_numStrength->setText(ss8.str());
	_barStrength->setMax(current->strength);
	_barStrength->setValue(current->strength);
	_barStrength->setValue2(initial->strength);

	_txtArmor->setText(_game->getLanguage()->getString(s->getArmor()->getType()));

	std::wstringstream ss9;
	ss9 << _game->getLanguage()->getString("STR_RANK_") << L'\x01' << _game->getLanguage()->getString(s->getRankString());
	_txtRank->setText(ss9.str());

	std::wstringstream ss10;
	ss10 << _game->getLanguage()->getString("STR_MISSIONS") << L'\x01' << s->getMissions();
	_txtMissions->setText(ss10.str());

	std::wstringstream ss11;
	ss11 << _game->getLanguage()->getString("STR_KILLS") << L'\x01' << s->getKills();
	_txtKills->setText(ss11.str());

	std::wstringstream ss12;
	ss12 << _game->getLanguage()->getString("STR_CRAFT_") << L'\x01';
	if (s->getCraft() == 0)
		ss12 << _game->getLanguage()->getString("STR_NONE_UC");
	else
		ss12 << s->getCraft()->getName(_game->getLanguage());
	_txtCraft->setText(ss12.str());

	if (s->getWoundRecovery() > 1)
	{
		std::wstringstream ss13;
		ss13 << _game->getLanguage()->getString("STR_WOUND_RECOVERY") << L'\x01' << s->getWoundRecovery();
		if (s->getWoundRecovery() > 1)
			ss13 << _game->getLanguage()->getString("STR_DAYS");
		else
			ss13 << _game->getLanguage()->getString("STR_DAY");
		_txtRecovery->setText(ss13.str());
	}
	else
	{
		_txtRecovery->setText(L"");
	}
}

/**
 * Changes the soldier name.
 * @param action Pointer to an action.
 */
void SoldierInfoState::edtSoldierKeyPress(Action *action)
{
	if (action->getDetails()->key.keysym.sym == SDLK_RETURN ||
		action->getDetails()->key.keysym.sym == SDLK_KP_ENTER)
	{
		_base->getSoldiers()->at(_soldier)->setName(_edtSoldier->getText());
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnOkClick(Action *action)
{
	_base->getSoldiers()->at(_soldier)->setName(_edtSoldier->getText());
	_game->popState();
}

/**
 * Goes to the previous soldier.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnPrevClick(Action *action)
{
	if (_soldier == 0)
		_soldier = _base->getSoldiers()->size() - 1;
	else
		_soldier--;
	init();
}

/**
 * Goes to the next soldier.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnNextClick(Action *action)
{
	_soldier++;
	if (_soldier >= _base->getSoldiers()->size())
		_soldier = 0;
	init();
}

/**
 * Shows the Select Armor window.
 * @param action Pointer to an action.
 */
void SoldierInfoState::btnArmorClick(Action *action)
{
	_game->pushState(new SoldierArmorState(_game, _base, _soldier));
}

}
