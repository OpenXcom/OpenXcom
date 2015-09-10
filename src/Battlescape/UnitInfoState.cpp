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
#include "UnitInfoState.h"
#include <sstream>
#include "../Savegame/BattleUnit.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Engine/Screen.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Bar.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Engine/InteractiveSurface.h"
#include "../Mod/Armor.h"
#include "../Mod/Unit.h"
#include "../Engine/Options.h"
#include "BattlescapeGame.h"
#include "BattlescapeState.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleInterface.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Unit Info screen.
 * @param game Pointer to the core game.
 * @param unit Pointer to the selected unit.
 * @param parent Pointer to parent Battlescape.
 * @param fromInventory Is player coming from the inventory?
 * @param mindProbe Is player using a Mind Probe?
 */
UnitInfoState::UnitInfoState(BattleUnit *unit, BattlescapeState *parent, bool fromInventory, bool mindProbe) : _unit(unit), _parent(parent), _fromInventory(fromInventory), _mindProbe(mindProbe)
{
	if (Options::maximizeInfoScreens)
	{
		Options::baseXResolution = Screen::ORIGINAL_WIDTH;
		Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
		_game->getScreen()->resetDisplay(false);
	}
	_battleGame = _game->getSavedGame()->getSavedBattle();

	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_exit = new InteractiveSurface(320, 180, 0, 20);
	_txtName = new Text(288, 17, 16, 4);

	int yPos = 38;
	int step = 9;

	_txtTimeUnits = new Text(140, 9, 8, yPos);
	_numTimeUnits = new Text(18, 9, 150, yPos);
	_barTimeUnits = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtEnergy = new Text(140, 9, 8, yPos);
	_numEnergy = new Text(18, 9, 150, yPos);
	_barEnergy = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtHealth = new Text(140, 9, 8, yPos);
	_numHealth = new Text(18, 9, 150, yPos);
	_barHealth = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtFatalWounds = new Text(140, 9, 8, yPos);
	_numFatalWounds = new Text(18, 9, 150, yPos);
	_barFatalWounds = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtBravery = new Text(140, 9, 8, yPos);
	_numBravery = new Text(18, 9, 150, yPos);
	_barBravery = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtMorale = new Text(140, 9, 8, yPos);
	_numMorale = new Text(18, 9, 150, yPos);
	_barMorale = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtReactions = new Text(140, 9, 8, yPos);
	_numReactions = new Text(18, 9, 150, yPos);
	_barReactions = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtFiring = new Text(140, 9, 8, yPos);
	_numFiring = new Text(18, 9, 150, yPos);
	_barFiring = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtThrowing = new Text(140, 9, 8, yPos);
	_numThrowing = new Text(18, 9, 150, yPos);
	_barThrowing = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtMelee = new Text(140, 9, 8, yPos);
	_numMelee = new Text(18, 9, 150, yPos);
	_barMelee = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtStrength = new Text(140, 9, 8, yPos);
	_numStrength = new Text(18, 9, 150, yPos);
	_barStrength = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtPsiStrength = new Text(140, 9, 8, yPos);
	_numPsiStrength = new Text(18, 9, 150, yPos);
	_barPsiStrength = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtPsiSkill = new Text(140, 9, 8, yPos);
	_numPsiSkill = new Text(18, 9, 150, yPos);
	_barPsiSkill = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtFrontArmor = new Text(140, 9, 8, yPos);
	_numFrontArmor= new Text(18, 9, 150, yPos);
	_barFrontArmor = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtLeftArmor = new Text(140, 9, 8, yPos);
	_numLeftArmor = new Text(18, 9, 150, yPos);
	_barLeftArmor = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtRightArmor = new Text(140, 9, 8, yPos);
	_numRightArmor = new Text(18, 9, 150, yPos);
	_barRightArmor = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtRearArmor = new Text(140, 9, 8, yPos);
	_numRearArmor = new Text(18, 9, 150, yPos);
	_barRearArmor = new Bar(150, 5, 170, yPos + 1);
	yPos += step;

	_txtUnderArmor = new Text(140, 9, 8, yPos);
	_numUnderArmor = new Text(18, 9, 150, yPos);
	_barUnderArmor = new Bar(150, 5, 170, yPos + 1);

	if (!_mindProbe)
	{
		_btnPrev = new TextButton(14, 18, 2, 2);
		_btnNext = new TextButton(14, 18, 304, 2);
	}

	// Set palette
	setPalette("PAL_BATTLESCAPE");

	add(_bg);
	add(_exit);
	add(_txtName, "textName", "stats", 0);

	add(_txtTimeUnits);
	add(_numTimeUnits);
	add(_barTimeUnits, "barTUs", "stats", 0);

	add(_txtEnergy);
	add(_numEnergy);
	add(_barEnergy, "barEnergy", "stats", 0);

	add(_txtHealth);
	add(_numHealth);
	add(_barHealth, "barHealth", "stats", 0);

	add(_txtFatalWounds);
	add(_numFatalWounds);
	add(_barFatalWounds, "barWounds", "stats", 0);

	add(_txtBravery);
	add(_numBravery);
	add(_barBravery, "barBravery", "stats", 0);

	add(_txtMorale);
	add(_numMorale);
	add(_barMorale, "barMorale", "stats", 0);

	add(_txtReactions);
	add(_numReactions);
	add(_barReactions, "barReactions", "stats", 0);

	add(_txtFiring);
	add(_numFiring);
	add(_barFiring, "barFiring", "stats", 0);

	add(_txtThrowing);
	add(_numThrowing);
	add(_barThrowing, "barThrowing", "stats", 0);

	add(_txtMelee);
	add(_numMelee);
	add(_barMelee, "barMelee", "stats", 0);

	add(_txtStrength);
	add(_numStrength);
	add(_barStrength, "barStrength", "stats", 0);

	add(_txtPsiStrength);
	add(_numPsiStrength);
	add(_barPsiStrength, "barPsiStrength", "stats", 0);

	add(_txtPsiSkill);
	add(_numPsiSkill);
	add(_barPsiSkill, "barPsiSkill", "stats", 0);

	add(_txtFrontArmor);
	add(_numFrontArmor);
	add(_barFrontArmor, "barFrontArmor", "stats", 0);

	add(_txtLeftArmor);
	add(_numLeftArmor);
	add(_barLeftArmor, "barLeftArmor", "stats", 0);

	add(_txtRightArmor);
	add(_numRightArmor);
	add(_barRightArmor, "barRightArmor", "stats", 0);

	add(_txtRearArmor);
	add(_numRearArmor);
	add(_barRearArmor, "barRearArmor", "stats", 0);

	add(_txtUnderArmor);
	add(_numUnderArmor);
	add(_barUnderArmor, "barUnderArmor", "stats", 0);

	if (!_mindProbe)
	{
		add(_btnPrev, "button", "stats");
		add(_btnNext, "button", "stats");
	}

	centerAllSurfaces();

	// Set up objects
	_game->getMod()->getSurface("UNIBORD.PCK")->blit(_bg);

	_exit->onMouseClick((ActionHandler)&UnitInfoState::exitClick);
	_exit->onKeyboardPress((ActionHandler)&UnitInfoState::exitClick, Options::keyCancel);
	_exit->onKeyboardPress((ActionHandler)&UnitInfoState::exitClick, Options::keyBattleStats);

	Uint8 color = _game->getMod()->getInterface("stats")->getElement("text")->color;
	Uint8 color2 = _game->getMod()->getInterface("stats")->getElement("text")->color2;

	_txtName->setAlign(ALIGN_CENTER);
	_txtName->setBig();
	_txtName->setHighContrast(true);

	_txtTimeUnits->setColor(color);
	_txtTimeUnits->setHighContrast(true);
	_txtTimeUnits->setText(tr("STR_TIME_UNITS"));

	_numTimeUnits->setColor(color2);
	_numTimeUnits->setHighContrast(true);

	_barTimeUnits->setScale(1.0);

	_txtEnergy->setColor(color);
	_txtEnergy->setHighContrast(true);
	_txtEnergy->setText(tr("STR_ENERGY"));

	_numEnergy->setColor(color2);
	_numEnergy->setHighContrast(true);

	_barEnergy->setScale(1.0);

	_txtHealth->setColor(color);
	_txtHealth->setHighContrast(true);
	_txtHealth->setText(tr("STR_HEALTH"));

	_numHealth->setColor(color2);
	_numHealth->setHighContrast(true);

	_barHealth->setScale(1.0);

	_txtFatalWounds->setColor(color);
	_txtFatalWounds->setHighContrast(true);
	_txtFatalWounds->setText(tr("STR_FATAL_WOUNDS"));

	_numFatalWounds->setColor(color2);
	_numFatalWounds->setHighContrast(true);

	_barFatalWounds->setScale(1.0);

	_txtBravery->setColor(color);
	_txtBravery->setHighContrast(true);
	_txtBravery->setText(tr("STR_BRAVERY"));

	_numBravery->setColor(color2);
	_numBravery->setHighContrast(true);

	_barBravery->setScale(1.0);

	_txtMorale->setColor(color);
	_txtMorale->setHighContrast(true);
	_txtMorale->setText(tr("STR_MORALE"));

	_numMorale->setColor(color2);
	_numMorale->setHighContrast(true);

	_barMorale->setScale(1.0);

	_txtReactions->setColor(color);
	_txtReactions->setHighContrast(true);
	_txtReactions->setText(tr("STR_REACTIONS"));

	_numReactions->setColor(color2);
	_numReactions->setHighContrast(true);

	_barReactions->setScale(1.0);

	_txtFiring->setColor(color);
	_txtFiring->setHighContrast(true);
	_txtFiring->setText(tr("STR_FIRING_ACCURACY"));

	_numFiring->setColor(color2);
	_numFiring->setHighContrast(true);

	_barFiring->setScale(1.0);

	_txtThrowing->setColor(color);
	_txtThrowing->setHighContrast(true);
	_txtThrowing->setText(tr("STR_THROWING_ACCURACY"));

	_numThrowing->setColor(color2);
	_numThrowing->setHighContrast(true);

	_barThrowing->setScale(1.0);

	_txtMelee->setColor(color);
	_txtMelee->setHighContrast(true);
	_txtMelee->setText(tr("STR_MELEE_ACCURACY"));

	_numMelee->setColor(color2);
	_numMelee->setHighContrast(true);

	_barMelee->setScale(1.0);

	_txtStrength->setColor(color);
	_txtStrength->setHighContrast(true);
	_txtStrength->setText(tr("STR_STRENGTH"));

	_numStrength->setColor(color2);
	_numStrength->setHighContrast(true);

	_barStrength->setScale(1.0);

	_txtPsiStrength->setColor(color);
	_txtPsiStrength->setHighContrast(true);
	_txtPsiStrength->setText(tr("STR_PSIONIC_STRENGTH"));

	_numPsiStrength->setColor(color2);
	_numPsiStrength->setHighContrast(true);

	_barPsiStrength->setScale(1.0);

	_txtPsiSkill->setColor(color);
	_txtPsiSkill->setHighContrast(true);
	_txtPsiSkill->setText(tr("STR_PSIONIC_SKILL"));

	_numPsiSkill->setColor(color2);
	_numPsiSkill->setHighContrast(true);

	_barPsiSkill->setScale(1.0);

	_txtFrontArmor->setColor(color);
	_txtFrontArmor->setHighContrast(true);
	_txtFrontArmor->setText(tr("STR_FRONT_ARMOR_UC"));

	_numFrontArmor->setColor(color2);
	_numFrontArmor->setHighContrast(true);

	_barFrontArmor->setScale(1.0);

	_txtLeftArmor->setColor(color);
	_txtLeftArmor->setHighContrast(true);
	_txtLeftArmor->setText(tr("STR_LEFT_ARMOR_UC"));

	_numLeftArmor->setColor(color2);
	_numLeftArmor->setHighContrast(true);

	_barLeftArmor->setScale(1.0);

	_txtRightArmor->setColor(color);
	_txtRightArmor->setHighContrast(true);
	_txtRightArmor->setText(tr("STR_RIGHT_ARMOR_UC"));

	_numRightArmor->setColor(color2);
	_numRightArmor->setHighContrast(true);

	_barRightArmor->setScale(1.0);

	_txtRearArmor->setColor(color);
	_txtRearArmor->setHighContrast(true);
	_txtRearArmor->setText(tr("STR_REAR_ARMOR_UC"));

	_numRearArmor->setColor(color2);
	_numRearArmor->setHighContrast(true);

	_barRearArmor->setScale(1.0);

	_txtUnderArmor->setColor(color);
	_txtUnderArmor->setHighContrast(true);
	_txtUnderArmor->setText(tr("STR_UNDER_ARMOR_UC"));

	_numUnderArmor->setColor(color2);
	_numUnderArmor->setHighContrast(true);

	_barUnderArmor->setScale(1.0);

	if (!_mindProbe)
	{
		_btnPrev->setText(L"<<");
		_btnPrev->onMouseClick((ActionHandler)&UnitInfoState::btnPrevClick);
		_btnPrev->onKeyboardPress((ActionHandler)&UnitInfoState::btnPrevClick, Options::keyBattlePrevUnit);
		_btnNext->setText(L">>");
		_btnNext->onMouseClick((ActionHandler)&UnitInfoState::btnNextClick);
		_btnNext->onKeyboardPress((ActionHandler)&UnitInfoState::btnNextClick, Options::keyBattleNextUnit);
	}

}

/**
 *
 */
UnitInfoState::~UnitInfoState()
{

}

/**
 * Updates unit info which can change
 * after going into other screens.
 */
void UnitInfoState::init()
{
	State::init();
	std::wostringstream ss;
	ss << _unit->getTimeUnits();
	_numTimeUnits->setText(ss.str());
	_barTimeUnits->setMax(_unit->getBaseStats()->tu);
	_barTimeUnits->setValue(_unit->getTimeUnits());

	ss.str(L"");
	// aliens have their rank in their "name", soldiers don't
	if (_unit->getType() == "SOLDIER")
	{
		ss << tr(_unit->getRankString());
		ss << " ";
	}
	ss << _unit->getName(_game->getLanguage(), BattlescapeGame::_debugPlay);
	_txtName->setBig();
	_txtName->setText(ss.str());

	ss.str(L"");
	ss << _unit->getEnergy();
	_numEnergy->setText(ss.str());
	_barEnergy->setMax(_unit->getBaseStats()->stamina);
	_barEnergy->setValue(_unit->getEnergy());

	ss.str(L"");
	ss << _unit->getHealth();
	_numHealth->setText(ss.str());
	_barHealth->setMax(_unit->getBaseStats()->health);
	_barHealth->setValue(_unit->getHealth());
	_barHealth->setValue2(_unit->getStunlevel());

	ss.str(L"");
	ss << _unit->getFatalWounds();
	_numFatalWounds->setText(ss.str());
	_barFatalWounds->setMax(_unit->getFatalWounds());
	_barFatalWounds->setValue(_unit->getFatalWounds());

	ss.str(L"");
	ss << _unit->getBaseStats()->bravery;
	_numBravery->setText(ss.str());
	_barBravery->setMax(_unit->getBaseStats()->bravery);
	_barBravery->setValue(_unit->getBaseStats()->bravery);

	ss.str(L"");
	ss << _unit->getMorale();
	_numMorale->setText(ss.str());
	_barMorale->setMax(100);
	_barMorale->setValue(_unit->getMorale());

	ss.str(L"");
	ss << _unit->getBaseStats()->reactions;
	_numReactions->setText(ss.str());
	_barReactions->setMax(_unit->getBaseStats()->reactions);
	_barReactions->setValue(_unit->getBaseStats()->reactions);

	ss.str(L"");
	ss << (int)((_unit->getBaseStats()->firing * _unit->getHealth()) / _unit->getBaseStats()->health);
	_numFiring->setText(ss.str());
	_barFiring->setMax(_unit->getBaseStats()->firing);
	_barFiring->setValue((_unit->getBaseStats()->firing * _unit->getHealth()) / _unit->getBaseStats()->health);

	ss.str(L"");
	ss << (int)((_unit->getBaseStats()->throwing * _unit->getHealth()) / _unit->getBaseStats()->health);
	_numThrowing->setText(ss.str());
	_barThrowing->setMax(_unit->getBaseStats()->throwing);
	_barThrowing->setValue((_unit->getBaseStats()->throwing * _unit->getHealth()) / _unit->getBaseStats()->health);

	ss.str(L"");
	ss << (int)((_unit->getBaseStats()->melee * _unit->getHealth()) / _unit->getBaseStats()->health);
	_numMelee->setText(ss.str());
	_barMelee->setMax(_unit->getBaseStats()->melee);
	_barMelee->setValue((_unit->getBaseStats()->melee * _unit->getHealth()) / _unit->getBaseStats()->health);

	ss.str(L"");
	ss << _unit->getBaseStats()->strength;
	_numStrength->setText(ss.str());
	_barStrength->setMax(_unit->getBaseStats()->strength);
	_barStrength->setValue(_unit->getBaseStats()->strength);

	if (_unit->getBaseStats()->psiSkill > 0 || (Options::psiStrengthEval && _game->getSavedGame()->isResearched(_game->getMod()->getPsiRequirements())))
	{
		ss.str(L"");
		ss << _unit->getBaseStats()->psiStrength;
		_numPsiStrength->setText(ss.str());
		_barPsiStrength->setMax(_unit->getBaseStats()->psiStrength);
		_barPsiStrength->setValue(_unit->getBaseStats()->psiStrength);

		_txtPsiStrength->setVisible(true);
		_numPsiStrength->setVisible(true);
		_barPsiStrength->setVisible(true);
	}
	else
	{
		_txtPsiStrength->setVisible(false);
		_numPsiStrength->setVisible(false);
		_barPsiStrength->setVisible(false);
	}

	if (_unit->getBaseStats()->psiSkill > 0)
	{
		ss.str(L"");
		ss << _unit->getBaseStats()->psiSkill;
		_numPsiSkill->setText(ss.str());
		_barPsiSkill->setMax(_unit->getBaseStats()->psiSkill);
		_barPsiSkill->setValue(_unit->getBaseStats()->psiSkill);

		_txtPsiSkill->setVisible(true);
		_numPsiSkill->setVisible(true);
		_barPsiSkill->setVisible(true);
	}
	else
	{
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
			exitClick(action);
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_X1)
		{
			if (!_mindProbe) btnNextClick(action);
		}
		else if (action->getDetails()->button.button == SDL_BUTTON_X2)
		{
			if (!_mindProbe) btnPrevClick(action);
		}
	}
}

/**
 * Selects the previous unit.
 * @param action Pointer to an action.
 */
void UnitInfoState::btnPrevClick(Action *action)
{
	if (_parent)
	{ // so we are here from a Battlescape Game
		_parent->selectPreviousPlayerUnit(false, false, _fromInventory);
	}
	else
	{ // so we are here from the Craft Equipment screen
		_battleGame->selectPreviousPlayerUnit(false, false, true);
	}
	_unit = _battleGame->getSelectedUnit();
	if (_unit != 0)
	{
		init();
	}
	else
	{
		exitClick(action);
	}
}

/**
 * Selects the next unit.
 * @param action Pointer to an action.
 */
void UnitInfoState::btnNextClick(Action *action)
{
	if (_parent)
	{ // so we are here from a Battlescape Game
		_parent->selectNextPlayerUnit(false, false, _fromInventory);
	}
	else
	{ // so we are here from the Craft Equipment screen
		_battleGame->selectNextPlayerUnit(false, false, true);
	}
	_unit = _battleGame->getSelectedUnit();
	if (_unit != 0)
	{
		init();
	}
	else
	{
		exitClick(action);
	}
}

/**
 * Exits the screen.
 * @param action Pointer to an action.
 */
void UnitInfoState::exitClick(Action *)
{
	if (!_fromInventory && Options::maximizeInfoScreens)
	{
		Screen::updateScale(Options::battlescapeScale, Options::battlescapeScale, Options::baseXBattlescape, Options::baseYBattlescape, true);
		_game->getScreen()->resetDisplay(false);
	}
	_game->popState();
}

}
