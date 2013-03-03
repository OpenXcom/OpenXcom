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
#include "UnitInfoState.h"
#include <sstream>
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
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
#include "../Ruleset/Armor.h"
#include "../Savegame/Soldier.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Options.h"
#include "BattlescapeGame.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Unit Info screen.
 * @param game Pointer to the core game.
 * @param unit Pointer to the selected unit.
 */
UnitInfoState::UnitInfoState(Game *game, BattleUnit *unit) : State(game), _unit(unit)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_txtName = new Text(312, 192, 4, 4);

	_txtTimeUnits = new Text(120, 9, 8, 31);
	_numTimeUnits = new Text(18, 9, 150, 31);
	_barTimeUnits = new Bar(170, 5, 170, 32);

	_txtEnergy = new Text(120, 9, 8, 41);
	_numEnergy = new Text(18, 9, 150, 41);
	_barEnergy = new Bar(170, 5, 170, 42);

	_txtHealth = new Text(120, 9, 8, 51);
	_numHealth = new Text(18, 9, 150, 51);
	_barHealth = new Bar(170, 5, 170, 52);

	_txtFatalWounds = new Text(120, 9, 8, 61);
	_numFatalWounds = new Text(18, 9, 150, 61);
	_barFatalWounds = new Bar(170, 5, 170, 62);

	_txtBravery = new Text(120, 9, 8, 71);
	_numBravery = new Text(18, 9, 150, 71);
	_barBravery = new Bar(170, 5, 170, 72);

	_txtMorale = new Text(120, 9, 8, 81);
	_numMorale = new Text(18, 9, 150, 81);
	_barMorale = new Bar(170, 5, 170, 82);

	_txtReactions = new Text(120, 9, 8, 91);
	_numReactions = new Text(18, 9, 150, 91);
	_barReactions = new Bar(170, 5, 170, 92);

	_txtFiring = new Text(120, 9, 8, 101);
	_numFiring = new Text(18, 9, 150, 101);
	_barFiring = new Bar(170, 5, 170, 102);

	_txtThrowing = new Text(120, 9, 8, 111);
	_numThrowing = new Text(18, 9, 150, 111);
	_barThrowing = new Bar(170, 5, 170, 112);

	_txtStrength = new Text(120, 9, 8, 121);
	_numStrength = new Text(18, 9, 150, 121);
	_barStrength = new Bar(170, 5, 170, 122);

	_txtPsiStrength = new Text(120, 9, 8, 131);
	_numPsiStrength = new Text(18, 9, 150, 131);
	_barPsiStrength = new Bar(170, 5, 170, 132);

	_txtPsiSkill = new Text(120, 9, 8, 141);
	_numPsiSkill = new Text(18, 9, 150, 141);
	_barPsiSkill = new Bar(170, 5, 170, 142);

	_txtFrontArmor = new Text(120, 9, 8, 151);
	_numFrontArmor= new Text(18, 9, 150, 151);
	_barFrontArmor = new Bar(170, 5, 170, 152);

	_txtLeftArmor = new Text(120, 9, 8, 161);
	_numLeftArmor = new Text(18, 9, 150, 161);
	_barLeftArmor = new Bar(170, 5, 170, 162);

	_txtRightArmor = new Text(120, 9, 8, 171);
	_numRightArmor = new Text(18, 9, 150, 171);
	_barRightArmor = new Bar(170, 5, 170, 172);

	_txtRearArmor = new Text(120, 9, 8, 181);
	_numRearArmor = new Text(18, 9, 150, 181);
	_barRearArmor = new Bar(170, 5, 170, 182);

	_txtUnderArmor = new Text(120, 9, 8, 191);
	_numUnderArmor = new Text(18, 9, 150, 191);
	_barUnderArmor = new Bar(170, 5, 170, 192);

	add(_bg);
	add(_txtName);

	add(_txtTimeUnits);
	add(_numTimeUnits);
	add(_barTimeUnits);

	add(_txtEnergy);
	add(_numEnergy);
	add(_barEnergy);

	add(_txtHealth);
	add(_numHealth);
	add(_barHealth);

	add(_txtFatalWounds);
	add(_numFatalWounds);
	add(_barFatalWounds);

	add(_txtBravery);
	add(_numBravery);
	add(_barBravery);

	add(_txtMorale);
	add(_numMorale);
	add(_barMorale);

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

	add(_txtPsiStrength);
	add(_numPsiStrength);
	add(_barPsiStrength);

	add(_txtPsiSkill);
	add(_numPsiSkill);
	add(_barPsiSkill);

	add(_txtFrontArmor);
	add(_numFrontArmor);
	add(_barFrontArmor);

	add(_txtLeftArmor);
	add(_numLeftArmor);
	add(_barLeftArmor);

	add(_txtRightArmor);
	add(_numRightArmor);
	add(_barRightArmor);

	add(_txtRearArmor);
	add(_numRearArmor);
	add(_barRearArmor);

	add(_txtUnderArmor);
	add(_numUnderArmor);
	add(_barUnderArmor);

	// Set up objects
	_game->getResourcePack()->getSurface("UNIBORD.PCK")->blit(_bg);

	_txtName->setAlign(ALIGN_CENTER);
	_txtName->setBig();
	_txtName->setColor(Palette::blockOffset(4));
	_txtName->setHighContrast(true);

	_txtTimeUnits->setColor(Palette::blockOffset(3));
	_txtTimeUnits->setHighContrast(true);
	_txtTimeUnits->setText(_game->getLanguage()->getString("STR_TIME_UNITS"));

	_numTimeUnits->setColor(Palette::blockOffset(9));
	_numTimeUnits->setHighContrast(true);

	_barTimeUnits->setColor(Palette::blockOffset(4));
	_barTimeUnits->setScale(1.0);

	_txtEnergy->setColor(Palette::blockOffset(3));
	_txtEnergy->setHighContrast(true);
	_txtEnergy->setText(_game->getLanguage()->getString("STR_ENERGY"));

	_numEnergy->setColor(Palette::blockOffset(9));
	_numEnergy->setHighContrast(true);

	_barEnergy->setColor(Palette::blockOffset(9));
	_barEnergy->setScale(1.0);

	_txtHealth->setColor(Palette::blockOffset(3));
	_txtHealth->setHighContrast(true);
	_txtHealth->setText(_game->getLanguage()->getString("STR_HEALTH"));

	_numHealth->setColor(Palette::blockOffset(9));
	_numHealth->setHighContrast(true);

	_barHealth->setColor(Palette::blockOffset(2));
	_barHealth->setColor2(Palette::blockOffset(5)+2);
	_barHealth->setScale(1.0);

	_txtFatalWounds->setColor(Palette::blockOffset(3));
	_txtFatalWounds->setHighContrast(true);
	_txtFatalWounds->setText(_game->getLanguage()->getString("STR_FATAL_WOUNDS"));

	_numFatalWounds->setColor(Palette::blockOffset(9));
	_numFatalWounds->setHighContrast(true);

	_barFatalWounds->setColor(Palette::blockOffset(2));
	_barFatalWounds->setScale(1.0);

	_txtBravery->setColor(Palette::blockOffset(3));
	_txtBravery->setHighContrast(true);
	_txtBravery->setText(_game->getLanguage()->getString("STR_BRAVERY"));

	_numBravery->setColor(Palette::blockOffset(9));
	_numBravery->setHighContrast(true);

	_barBravery->setColor(Palette::blockOffset(12));
	_barBravery->setScale(1.0);

	_txtMorale->setColor(Palette::blockOffset(3));
	_txtMorale->setHighContrast(true);
	_txtMorale->setText(_game->getLanguage()->getString("STR_MORALE"));

	_numMorale->setColor(Palette::blockOffset(9));
	_numMorale->setHighContrast(true);

	_barMorale->setColor(Palette::blockOffset(12));
	_barMorale->setScale(1.0);

	_txtReactions->setColor(Palette::blockOffset(3));
	_txtReactions->setHighContrast(true);
	_txtReactions->setText(_game->getLanguage()->getString("STR_REACTIONS"));

	_numReactions->setColor(Palette::blockOffset(9));
	_numReactions->setHighContrast(true);

	_barReactions->setColor(Palette::blockOffset(9));
	_barReactions->setScale(1.0);

	_txtFiring->setColor(Palette::blockOffset(3));
	_txtFiring->setHighContrast(true);
	_txtFiring->setText(_game->getLanguage()->getString("STR_FIRING_ACCURACY"));

	_numFiring->setColor(Palette::blockOffset(9));
	_numFiring->setHighContrast(true);

	_barFiring->setColor(Palette::blockOffset(8));
	_barFiring->setScale(1.0);

	_txtThrowing->setColor(Palette::blockOffset(3));
	_txtThrowing->setHighContrast(true);
	_txtThrowing->setText(_game->getLanguage()->getString("STR_THROWING_ACCURACY"));

	_numThrowing->setColor(Palette::blockOffset(9));
	_numThrowing->setHighContrast(true);

	_barThrowing->setColor(Palette::blockOffset(6));
	_barThrowing->setScale(1.0);

	_txtStrength->setColor(Palette::blockOffset(3));
	_txtStrength->setHighContrast(true);
	_txtStrength->setText(_game->getLanguage()->getString("STR_STRENGTH"));

	_numStrength->setColor(Palette::blockOffset(9));
	_numStrength->setHighContrast(true);

	_barStrength->setColor(Palette::blockOffset(3));
	_barStrength->setScale(1.0);

	_txtPsiStrength->setColor(Palette::blockOffset(3));
	_txtPsiStrength->setHighContrast(true);
	_txtPsiStrength->setText(_game->getLanguage()->getString("STR_PSIONIC_STRENGTH"));

	_numPsiStrength->setColor(Palette::blockOffset(9));
	_numPsiStrength->setHighContrast(true);

	_barPsiStrength->setColor(Palette::blockOffset(12));
	_barPsiStrength->setScale(1.0);

	_txtPsiSkill->setColor(Palette::blockOffset(3));
	_txtPsiSkill->setHighContrast(true);
	_txtPsiSkill->setText(_game->getLanguage()->getString("STR_PSIONIC_SKILL"));

	_numPsiSkill->setColor(Palette::blockOffset(9));
	_numPsiSkill->setHighContrast(true);

	_barPsiSkill->setColor(Palette::blockOffset(12));
	_barPsiSkill->setScale(1.0);

	_txtFrontArmor->setColor(Palette::blockOffset(3));
	_txtFrontArmor->setHighContrast(true);
	_txtFrontArmor->setText(_game->getLanguage()->getString("STR_FRONT_ARMOR_UC"));

	_numFrontArmor->setColor(Palette::blockOffset(9));
	_numFrontArmor->setHighContrast(true);

	_barFrontArmor->setColor(Palette::blockOffset(5));
	_barFrontArmor->setScale(1.0);

	_txtLeftArmor->setColor(Palette::blockOffset(3));
	_txtLeftArmor->setHighContrast(true);
	_txtLeftArmor->setText(_game->getLanguage()->getString("STR_LEFT_ARMOR_UC"));

	_numLeftArmor->setColor(Palette::blockOffset(9));
	_numLeftArmor->setHighContrast(true);

	_barLeftArmor->setColor(Palette::blockOffset(5));
	_barLeftArmor->setScale(1.0);

	_txtRightArmor->setColor(Palette::blockOffset(3));
	_txtRightArmor->setHighContrast(true);
	_txtRightArmor->setText(_game->getLanguage()->getString("STR_RIGHT_ARMOR_UC"));

	_numRightArmor->setColor(Palette::blockOffset(9));
	_numRightArmor->setHighContrast(true);

	_barRightArmor->setColor(Palette::blockOffset(5));
	_barRightArmor->setScale(1.0);

	_txtRearArmor->setColor(Palette::blockOffset(3));
	_txtRearArmor->setHighContrast(true);
	_txtRearArmor->setText(_game->getLanguage()->getString("STR_REAR_ARMOR_UC"));

	_numRearArmor->setColor(Palette::blockOffset(9));
	_numRearArmor->setHighContrast(true);

	_barRearArmor->setColor(Palette::blockOffset(5));
	_barRearArmor->setScale(1.0);

	_txtUnderArmor->setColor(Palette::blockOffset(3));
	_txtUnderArmor->setHighContrast(true);
	_txtUnderArmor->setText(_game->getLanguage()->getString("STR_UNDER_ARMOR_UC"));

	_numUnderArmor->setColor(Palette::blockOffset(9));
	_numUnderArmor->setHighContrast(true);

	_barUnderArmor->setColor(Palette::blockOffset(5));
	_barUnderArmor->setScale(1.0);

}

/**
 *
 */
UnitInfoState::~UnitInfoState()
{

}

/**
 * The unit info can change
 * after going into other screens.
 */
void UnitInfoState::init()
{
	std::wstringstream ss;
	ss << _unit->getTimeUnits();
	_numTimeUnits->setText(ss.str());
	_barTimeUnits->setMax(_unit->getStats()->tu);
	_barTimeUnits->setValue(_unit->getTimeUnits());

	ss.str(L"");
	// aliens have their rank in their "name", soldiers don't
	if (_unit->getType() == "SOLDIER")
	{
		ss << _game->getLanguage()->getString(_unit->getRankString());
		ss << " ";
	}
	ss << _unit->getName(_game->getLanguage(), BattlescapeGame::_debugPlay);
	_txtName->setText(ss.str());

	ss.str(L"");
	ss << _unit->getEnergy();
	_numEnergy->setText(ss.str());
	_barEnergy->setMax(_unit->getStats()->stamina);
	_barEnergy->setValue(_unit->getEnergy());

	ss.str(L"");
	ss << _unit->getHealth();
	_numHealth->setText(ss.str());
	_barHealth->setMax(_unit->getStats()->health);
	_barHealth->setValue(_unit->getHealth());
	_barHealth->setValue2(_unit->getStunlevel());

	ss.str(L"");
	ss << _unit->getFatalWounds();
	_numFatalWounds->setText(ss.str());
	_barFatalWounds->setMax(_unit->getFatalWounds());
	_barFatalWounds->setValue(_unit->getFatalWounds());

	ss.str(L"");
	ss << _unit->getStats()->bravery;
	_numBravery->setText(ss.str());
	_barBravery->setMax(_unit->getStats()->bravery);
	_barBravery->setValue(_unit->getStats()->bravery);

	ss.str(L"");
	ss << _unit->getMorale();
	_numMorale->setText(ss.str());
	_barMorale->setMax(100);
	_barMorale->setValue(_unit->getMorale());

	ss.str(L"");
	ss << _unit->getStats()->reactions;
	_numReactions->setText(ss.str());
	_barReactions->setMax(_unit->getStats()->reactions);
	_barReactions->setValue(_unit->getStats()->reactions);

	ss.str(L"");
	ss << (int)(_unit->getStats()->firing * _unit->getAccuracyModifier());
	_numFiring->setText(ss.str());
	_barFiring->setMax(_unit->getStats()->firing);
	_barFiring->setValue(_unit->getStats()->firing * _unit->getAccuracyModifier());

	ss.str(L"");
	ss << (int)(_unit->getStats()->throwing * _unit->getAccuracyModifier());
	_numThrowing->setText(ss.str());
	_barThrowing->setMax(_unit->getStats()->throwing);
	_barThrowing->setValue(_unit->getStats()->throwing * _unit->getAccuracyModifier());

	ss.str(L"");
	ss << _unit->getStats()->strength;
	_numStrength->setText(ss.str());
	_barStrength->setMax(_unit->getStats()->strength);
	_barStrength->setValue(_unit->getStats()->strength);

	if (_unit->getStats()->psiSkill > 0)
	{
		ss.str(L"");
		ss << _unit->getStats()->psiStrength;
		_numPsiStrength->setText(ss.str());
		_barPsiStrength->setMax(_unit->getStats()->psiStrength);
		_barPsiStrength->setValue(_unit->getStats()->psiStrength);

		ss.str(L"");
		ss << _unit->getStats()->psiSkill;
		_numPsiSkill->setText(ss.str());
		_barPsiSkill->setMax(_unit->getStats()->psiSkill);
		_barPsiSkill->setValue(_unit->getStats()->psiSkill);

		_txtPsiStrength->setVisible(true);
		_numPsiStrength->setVisible(true);
		_barPsiStrength->setVisible(true);

		_txtPsiSkill->setVisible(true);
		_numPsiSkill->setVisible(true);
		_barPsiSkill->setVisible(true);
	}
	else
	{
		_txtPsiStrength->setVisible(false);
		_numPsiStrength->setVisible(false);
		_barPsiStrength->setVisible(false);

		_txtPsiSkill->setVisible(false);
		_numPsiSkill->setVisible(false);
		_barPsiSkill->setVisible(false);
	}

	ss.str(L"");
	ss << _unit->getArmor(SIDE_FRONT);
	_numFrontArmor->setText(ss.str());
	_barFrontArmor->setMax(_unit->getArmor()->getFrontArmor());
	_barFrontArmor->setValue(_unit->getArmor(SIDE_FRONT));

	ss.str(L"");
	ss << _unit->getArmor(SIDE_LEFT);
	_numLeftArmor->setText(ss.str());
	_barLeftArmor->setMax(_unit->getArmor()->getSideArmor());
	_barLeftArmor->setValue(_unit->getArmor(SIDE_LEFT));

	ss.str(L"");
	ss << _unit->getArmor(SIDE_RIGHT);
	_numRightArmor->setText(ss.str());
	_barRightArmor->setMax(_unit->getArmor()->getSideArmor());
	_barRightArmor->setValue(_unit->getArmor(SIDE_RIGHT));

	ss.str(L"");
	ss << _unit->getArmor(SIDE_REAR);
	_numRearArmor->setText(ss.str());
	_barRearArmor->setMax(_unit->getArmor()->getRearArmor());
	_barRearArmor->setValue(_unit->getArmor(SIDE_REAR));

	ss.str(L"");
	ss << _unit->getArmor(SIDE_UNDER);
	_numUnderArmor->setText(ss.str());
	_barUnderArmor->setMax(_unit->getArmor()->getUnderArmor());
	_barUnderArmor->setValue(_unit->getArmor(SIDE_UNDER));
}


/**
 * Closes the window on right-click.
 * @param action Pointer to an action.
 */
void UnitInfoState::handle(Action *action)
{
	State::handle(action);
	if (action->getDetails()->type == SDL_MOUSEBUTTONDOWN)
	{
		if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		{
			_game->popState();
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_X1)
		{
			_unit = _game->getSavedGame()->getBattleGame()->selectNextPlayerUnit();
			init();
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_X2)
		{
			_unit = _game->getSavedGame()->getBattleGame()->selectPreviousPlayerUnit();
			init();
		}
	}
	if (action->getDetails()->type == SDL_KEYDOWN)
	{
		// "tab" - next solider
		if (action->getDetails()->key.keysym.sym == Options::getInt("keyBattleNextUnit"))
		{
			_unit = _game->getSavedGame()->getBattleGame()->selectNextPlayerUnit();
			init();
		}
		// prev soldier
		else if (action->getDetails()->key.keysym.sym == Options::getInt("keyBattlePrevUnit"))
		{
			_unit = _game->getSavedGame()->getBattleGame()->selectPreviousPlayerUnit();
			init();
		}
	}
}

}
