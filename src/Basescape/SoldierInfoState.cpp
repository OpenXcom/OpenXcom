/*
 * Copyright 2010 OpenXcom Developers.
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
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include <string>
#include "../Engine/Font.h"
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

namespace OpenXcom
{

/**
 * Initializes all the elements in the Soldier Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param soldier ID of the selected soldier.
 */
SoldierInfoState::SoldierInfoState(Game *game, Base *base, unsigned int soldier) : State(game), _base(base), _soldier(soldier)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_rank = new Surface(26, 23, 4, 4);
	_btnPrev = new TextButton(28, 14, 0, 33);
	_btnOk = new TextButton(48, 14, 30, 33);
	_btnNext = new TextButton(28, 14, 80, 33);
	_btnArmour = new TextButton(60, 14, 130, 33);
	_edtSoldier = new TextEdit(200, 16, 40, 9);
	_txtArmour = new Text(120, 9, 194, 38);
	_txtRank = new Text(130, 9, 0, 48);
	_txtMissions = new Text(100, 9, 130, 48);
	_txtKills = new Text(100, 9, 230, 48);
	_txtCraft = new Text(130, 9, 0, 56);

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
	add(_btnArmour);
	add(_edtSoldier);
	add(_txtArmour);
	add(_txtRank);
	add(_txtMissions);
	add(_txtKills);
	add(_txtCraft);

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

	_btnOk->setColor(Palette::blockOffset(15)+9);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&SoldierInfoState::btnOkClick);

	_btnPrev->setColor(Palette::blockOffset(15)+9);
	_btnPrev->setText("<<");
	_btnPrev->onMouseClick((ActionHandler)&SoldierInfoState::btnPrevClick);

	_btnNext->setColor(Palette::blockOffset(15)+9);
	_btnNext->setText(">>");
	_btnNext->onMouseClick((ActionHandler)&SoldierInfoState::btnNextClick);

	_btnArmour->setColor(Palette::blockOffset(15)+9);
	_btnArmour->setText(_game->getResourcePack()->getLanguage()->getString("STR_ARMOR"));

	_edtSoldier->setColor(Palette::blockOffset(13)+10);
	_edtSoldier->setBig();

	_txtArmour->setColor(Palette::blockOffset(13));

	_txtRank->setColor(Palette::blockOffset(13));

	_txtMissions->setColor(Palette::blockOffset(13));

	_txtKills->setColor(Palette::blockOffset(13));

	_txtCraft->setColor(Palette::blockOffset(13));


	_txtTimeUnits->setColor(Palette::blockOffset(15)+1);
	_txtTimeUnits->setText(_game->getResourcePack()->getLanguage()->getString("STR_TIME_UNITS"));

	_numTimeUnits->setColor(Palette::blockOffset(13));

	_barTimeUnits->setColor(Palette::blockOffset(3));
	_barTimeUnits->setScale(1.0);
	_barTimeUnits->setInvert(true);

	_txtStamina->setColor(Palette::blockOffset(15)+1);
	_txtStamina->setText(_game->getResourcePack()->getLanguage()->getString("STR_STAMINA"));

	_numStamina->setColor(Palette::blockOffset(13));

	_barStamina->setColor(Palette::blockOffset(9));
	_barStamina->setScale(1.0);
	_barStamina->setInvert(true);

	_txtHealth->setColor(Palette::blockOffset(15)+1);
	_txtHealth->setText(_game->getResourcePack()->getLanguage()->getString("STR_HEALTH"));

	_numHealth->setColor(Palette::blockOffset(13));

	_barHealth->setColor(Palette::blockOffset(2));
	_barHealth->setScale(1.0);
	_barHealth->setInvert(true);

	_txtBravery->setColor(Palette::blockOffset(15)+1);
	_txtBravery->setText(_game->getResourcePack()->getLanguage()->getString("STR_BRAVERY"));

	_numBravery->setColor(Palette::blockOffset(13));

	_barBravery->setColor(Palette::blockOffset(4));
	_barBravery->setScale(1.0);
	_barBravery->setInvert(true);

	_txtReactions->setColor(Palette::blockOffset(15)+1);
	_txtReactions->setText(_game->getResourcePack()->getLanguage()->getString("STR_REACTIONS"));

	_numReactions->setColor(Palette::blockOffset(13));

	_barReactions->setColor(Palette::blockOffset(1));
	_barReactions->setScale(1.0);
	_barReactions->setInvert(true);

	_txtFiring->setColor(Palette::blockOffset(15)+1);
	_txtFiring->setText(_game->getResourcePack()->getLanguage()->getString("STR_FIRING_ACCURACY"));

	_numFiring->setColor(Palette::blockOffset(13));

	_barFiring->setColor(Palette::blockOffset(8));
	_barFiring->setScale(1.0);
	_barFiring->setInvert(true);

	_txtThrowing->setColor(Palette::blockOffset(15)+1);
	_txtThrowing->setText(_game->getResourcePack()->getLanguage()->getString("STR_THROWING_ACCURACY"));

	_numThrowing->setColor(Palette::blockOffset(13));

	_barThrowing->setColor(Palette::blockOffset(10));
	_barThrowing->setScale(1.0);
	_barThrowing->setInvert(true);

	_txtStrength->setColor(Palette::blockOffset(15)+1);
	_txtStrength->setText(_game->getResourcePack()->getLanguage()->getString("STR_STRENGTH"));

	_numStrength->setColor(Palette::blockOffset(13));

	_barStrength->setColor(Palette::blockOffset(5));
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
 * The soldier names can change
 * after going into other screens.
 */
void SoldierInfoState::init()
{
	Soldier *s = _base->getSoldiers()->at(_soldier);
	_edtSoldier->setText(s->getName());

	SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
	texture->getFrame(s->getRankSprite())->setX(0);
	texture->getFrame(s->getRankSprite())->setY(0);
	texture->getFrame(s->getRankSprite())->blit(_rank);

	std::stringstream ss;
	ss << s->getTimeUnits();
	_numTimeUnits->setText(ss.str());		
	_barTimeUnits->setMax(s->getTimeUnits());
	_barTimeUnits->setValue(s->getTimeUnits());

	std::stringstream ss2;
	ss2 << s->getStamina();
	_numStamina->setText(ss2.str());		
	_barStamina->setMax(s->getStamina());
	_barStamina->setValue(s->getStamina());

	std::stringstream ss3;
	ss3 << s->getHealth();
	_numHealth->setText(ss3.str());		
	_barHealth->setMax(s->getHealth());
	_barHealth->setValue(s->getHealth());

	std::stringstream ss4;
	ss4 << s->getBravery();
	_numBravery->setText(ss4.str());		
	_barBravery->setMax(s->getBravery());
	_barBravery->setValue(s->getBravery());

	std::stringstream ss5;
	ss5 << s->getReactions();
	_numReactions->setText(ss5.str());		
	_barReactions->setMax(s->getReactions());
	_barReactions->setValue(s->getReactions());

	std::stringstream ss6;
	ss6 << s->getFiringAccuracy();
	_numFiring->setText(ss6.str());		
	_barFiring->setMax(s->getFiringAccuracy());
	_barFiring->setValue(s->getFiringAccuracy());

	std::stringstream ss7;
	ss7 << s->getThrowingAccuracy();
	_numThrowing->setText(ss7.str());		
	_barThrowing->setMax(s->getThrowingAccuracy());
	_barThrowing->setValue(s->getThrowingAccuracy());

	std::stringstream ss8;
	ss8 << s->getStrength();
	_numStrength->setText(ss8.str());		
	_barStrength->setMax(s->getStrength());
	_barStrength->setValue(s->getStrength());

	_txtArmour->setText(_game->getResourcePack()->getLanguage()->getString("STR_NONE_UC"));

	std::stringstream ss9;
	ss9 << _game->getResourcePack()->getLanguage()->getString("STR_RANK_") << _game->getResourcePack()->getLanguage()->getString(s->getRankString());
	_txtRank->setText(ss9.str());

	std::stringstream ss10;
	ss10 << _game->getResourcePack()->getLanguage()->getString("STR_MISSIONS") << s->getMissions();
	_txtMissions->setText(ss10.str());

	std::stringstream ss11;
	ss11 << _game->getResourcePack()->getLanguage()->getString("STR_KILLS") << s->getKills();
	_txtKills->setText(ss11.str());

	std::stringstream ss12;
	ss12 << _game->getResourcePack()->getLanguage()->getString("STR_CRAFT_");
	if (s->getCraft() == 0)
		ss12 << _game->getResourcePack()->getLanguage()->getString("STR_NONE");
	else
		ss12 << s->getCraft()->getName(_game->getResourcePack()->getLanguage());
	_txtCraft->setText(ss12.str());
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
	_base->getSoldiers()->at(_soldier)->setName(_edtSoldier->getText());
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
	_base->getSoldiers()->at(_soldier)->setName(_edtSoldier->getText());
	_soldier++;
	if (_soldier >= _base->getSoldiers()->size())
		_soldier = 0;
	init();
}

}
