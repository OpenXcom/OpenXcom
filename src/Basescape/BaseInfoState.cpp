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
#include "BaseInfoState.h"
#include <sstream>
#include <cmath>
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Options.h"
#include "../Interface/Bar.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Engine/Surface.h"
#include "MiniBaseView.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "MonthlyCostsState.h"
#include "TransfersState.h"
#include "StoresState.h"
#include "BasescapeState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Base Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param state Pointer to the Basescape state.
 */
BaseInfoState::BaseInfoState(Base *base, BasescapeState *state) : _base(base), _state(state)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_mini = new MiniBaseView(128, 16, 182, 8);
	_btnOk = new TextButton(30, 14, 10, 180);
	_btnTransfers = new TextButton(80, 14, 46, 180);
	_btnStores = new TextButton(80, 14, 132, 180);
	_btnMonthlyCosts = new TextButton(92, 14, 218, 180);
	_edtBase = new TextEdit(this, 127, 16, 8, 8);

	_txtPersonnel = new Text(300, 9, 8, 30);
	_txtSoldiers = new Text(114, 9, 8, 41);
	_numSoldiers = new Text(40, 9, 126, 41);
	_barSoldiers = new Bar(150, 5, 166, 43);
	_txtEngineers = new Text(114, 9, 8, 51);
	_numEngineers = new Text(40, 9, 126, 51);
	_barEngineers = new Bar(150, 5, 166, 53);
	_txtScientists = new Text(114, 9, 8, 61);
	_numScientists = new Text(40, 9, 126, 61);
	_barScientists = new Bar(150, 5, 166, 63);

	_txtSpace = new Text(300, 9, 8, 72);
	_txtQuarters = new Text(114, 9, 8, 83);
	_numQuarters = new Text(40, 9, 126, 83);
	_barQuarters = new Bar(150, 5, 166, 85);
	_txtStores = new Text(114, 9, 8, 93);
	_numStores = new Text(40, 9, 126, 93);
	_barStores = new Bar(150, 5, 166, 95);
	_txtLaboratories = new Text(114, 9, 8, 103);
	_numLaboratories = new Text(40, 9, 126, 103);
	_barLaboratories = new Bar(150, 5, 166, 105);
	_txtWorkshops = new Text(114, 9, 8, 113);
	_numWorkshops = new Text(40, 9, 126, 113);
	_barWorkshops = new Bar(150, 5, 166, 115);
	if (Options::storageLimitsEnforced)
	{
		_txtContainment = new Text(114, 9, 8, 123);
		_numContainment = new Text(40, 9, 126, 123);
		_barContainment = new Bar(150, 5, 166, 125);
	}
	_txtHangars = new Text(114, 9, 8, Options::storageLimitsEnforced ? 133 : 123);
	_numHangars = new Text(40, 9, 126, Options::storageLimitsEnforced ? 133 : 123);
	_barHangars = new Bar(150, 5, 166, Options::storageLimitsEnforced ? 135 : 125);

	_txtDefense = new Text(114, 9, 8, Options::storageLimitsEnforced ? 147 : 138);
	_numDefense = new Text(40, 9, 126, Options::storageLimitsEnforced ? 147 : 138);
	_barDefense = new Bar(150, 5, 166, Options::storageLimitsEnforced ? 149 : 140);
	_txtShortRange = new Text(114, 9, 8, Options::storageLimitsEnforced ? 157 : 153);
	_numShortRange = new Text(40, 9, 126, Options::storageLimitsEnforced ? 157 : 153);
	_barShortRange = new Bar(150, 5, 166, Options::storageLimitsEnforced ? 159 : 155);
	_txtLongRange = new Text(114, 9, 8, Options::storageLimitsEnforced ? 167 : 163);
	_numLongRange = new Text(40, 9, 126, Options::storageLimitsEnforced ? 167 : 163);
	_barLongRange = new Bar(150, 5, 166, Options::storageLimitsEnforced ? 169 : 165);

	// Set palette
	setInterface("baseInfo");

	add(_bg);
	add(_mini, "miniBase", "basescape");
	add(_btnOk, "button", "baseInfo");
	add(_btnTransfers, "button", "baseInfo");
	add(_btnStores, "button", "baseInfo");
	add(_btnMonthlyCosts, "button", "baseInfo");
	add(_edtBase, "text1", "baseInfo");

	add(_txtPersonnel, "text1", "baseInfo");
	add(_txtSoldiers, "text2", "baseInfo");
	add(_numSoldiers, "numbers", "baseInfo");
	add(_barSoldiers, "personnelBars", "baseInfo");
	add(_txtEngineers, "text2", "baseInfo");
	add(_numEngineers, "numbers", "baseInfo");
	add(_barEngineers, "personnelBars", "baseInfo");
	add(_txtScientists, "text2", "baseInfo");
	add(_numScientists, "numbers", "baseInfo");
	add(_barScientists, "personnelBars", "baseInfo");

	add(_txtSpace, "text1", "baseInfo");
	add(_txtQuarters, "text2", "baseInfo");
	add(_numQuarters, "numbers", "baseInfo");
	add(_barQuarters, "facilityBars", "baseInfo");
	add(_txtStores, "text2", "baseInfo");
	add(_numStores, "numbers", "baseInfo");
	add(_barStores, "facilityBars", "baseInfo");
	add(_txtLaboratories, "text2", "baseInfo");
	add(_numLaboratories, "numbers", "baseInfo");
	add(_barLaboratories, "facilityBars", "baseInfo");
	add(_txtWorkshops, "text2", "baseInfo");
	add(_numWorkshops, "numbers", "baseInfo");
	add(_barWorkshops, "facilityBars", "baseInfo");
	if (Options::storageLimitsEnforced)
	{
		add(_txtContainment, "text2", "baseInfo");
		add(_numContainment, "numbers", "baseInfo");
		add(_barContainment, "facilityBars", "baseInfo");
	}
	add(_txtHangars, "text2", "baseInfo");
	add(_numHangars, "numbers", "baseInfo");
	add(_barHangars, "facilityBars", "baseInfo");

	add(_txtDefense, "text2", "baseInfo");
	add(_numDefense, "numbers", "baseInfo");
	add(_barDefense, "defenceBar", "baseInfo");
	add(_txtShortRange, "text2", "baseInfo");
	add(_numShortRange, "numbers", "baseInfo");
	add(_barShortRange, "detectionBars", "baseInfo");
	add(_txtLongRange, "text2", "baseInfo");
	add(_numLongRange, "numbers", "baseInfo");
	add(_barLongRange, "detectionBars", "baseInfo");

	centerAllSurfaces();

	// Set up objects
	std::ostringstream ss;
	if (Options::storageLimitsEnforced)
	{
		ss << "ALT";
	}
	ss << "BACK07.SCR";
	_game->getMod()->getSurface(ss.str())->blit(_bg);

	_mini->setTexture(_game->getMod()->getSurfaceSet("BASEBITS.PCK"));
	_mini->setBases(_game->getSavedGame()->getBases());
	for (size_t i = 0; i < _game->getSavedGame()->getBases()->size(); ++i)
	{
		if (_game->getSavedGame()->getBases()->at(i) == _base)
		{
			_mini->setSelectedBase(i);
			break;
		}
	}
	_mini->onMouseClick((ActionHandler)&BaseInfoState::miniClick);
	_mini->onKeyboardPress((ActionHandler)&BaseInfoState::handleKeyPress);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BaseInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&BaseInfoState::btnOkClick, Options::keyCancel);

	_btnTransfers->setText(tr("STR_TRANSFERS_UC"));
	_btnTransfers->onMouseClick((ActionHandler)&BaseInfoState::btnTransfersClick);

	_btnStores->setText(tr("STR_STORES_UC"));
	_btnStores->onMouseClick((ActionHandler)&BaseInfoState::btnStoresClick);

	_btnMonthlyCosts->setText(tr("STR_MONTHLY_COSTS"));
	_btnMonthlyCosts->onMouseClick((ActionHandler)&BaseInfoState::btnMonthlyCostsClick);

	_edtBase->setBig();
	_edtBase->onChange((ActionHandler)&BaseInfoState::edtBaseChange);

	_txtPersonnel->setText(tr("STR_PERSONNEL_AVAILABLE_PERSONNEL_TOTAL"));

	_txtSoldiers->setText(tr("STR_SOLDIERS"));

	_barSoldiers->setScale(1.0);

	_txtEngineers->setText(tr("STR_ENGINEERS"));

	_barEngineers->setScale(1.0);

	_txtScientists->setText(tr("STR_SCIENTISTS"));

	_barScientists->setScale(1.0);


	_txtSpace->setText(tr("STR_SPACE_USED_SPACE_AVAILABLE"));

	_txtQuarters->setText(tr("STR_LIVING_QUARTERS_PLURAL"));

	_barQuarters->setScale(0.5);

	_txtStores->setText(tr("STR_STORES"));

	_barStores->setScale(0.5);

	_txtLaboratories->setText(tr("STR_LABORATORIES"));

	_barLaboratories->setScale(0.5);

	_txtWorkshops->setText(tr("STR_WORK_SHOPS"));

	_barWorkshops->setScale(0.5);

	if (Options::storageLimitsEnforced)
	{
		_txtContainment->setText(tr("STR_ALIEN_CONTAINMENT"));

		_barContainment->setScale(0.5);
	}

	_txtHangars->setText(tr("STR_HANGARS"));

	_barHangars->setScale(18.0);


	_txtDefense->setText(tr("STR_DEFENSE_STRENGTH"));

	_barDefense->setScale(0.125);

	_txtShortRange->setText(tr("STR_SHORT_RANGE_DETECTION"));

	_barShortRange->setScale(25.0);

	_txtLongRange->setText(tr("STR_LONG_RANGE_DETECTION"));

	_barLongRange->setScale(25.0);
}

/**
 *
 */
BaseInfoState::~BaseInfoState()
{

}

/**
 * The player can change the selected base.
 */
void BaseInfoState::init()
{
	State::init();
	_edtBase->setText(_base->getName());

	std::ostringstream ss;
	ss << _base->getAvailableSoldiers() << ":" << _base->getTotalSoldiers();
	_numSoldiers->setText(ss.str());

	_barSoldiers->setMax(_base->getTotalSoldiers());
	_barSoldiers->setValue(_base->getAvailableSoldiers());

	std::ostringstream ss2;
	ss2 << _base->getAvailableEngineers() << ":" << _base->getTotalEngineers();
	_numEngineers->setText(ss2.str());

	_barEngineers->setMax(_base->getTotalEngineers());
	_barEngineers->setValue(_base->getAvailableEngineers());

	std::ostringstream ss3;
	ss3 << _base->getAvailableScientists() << ":" << _base->getTotalScientists();
	_numScientists->setText(ss3.str());

	_barScientists->setMax(_base->getTotalScientists());
	_barScientists->setValue(_base->getAvailableScientists());


	std::ostringstream ss4;
	ss4 << _base->getUsedQuarters() << ":" << _base->getAvailableQuarters();
	_numQuarters->setText(ss4.str());

	_barQuarters->setMax(_base->getAvailableQuarters());
	_barQuarters->setValue(_base->getUsedQuarters());

	std::ostringstream ss5;
	ss5 << (int)floor(_base->getUsedStores() + 0.05) << ":" << _base->getAvailableStores();
	_numStores->setText(ss5.str());

	_barStores->setMax(_base->getAvailableStores());
	_barStores->setValue((int)floor(_base->getUsedStores() + 0.05));

	std::ostringstream ss6;
	ss6 << _base->getUsedLaboratories() << ":" << _base->getAvailableLaboratories();
	_numLaboratories->setText(ss6.str());

	_barLaboratories->setMax(_base->getAvailableLaboratories());
	_barLaboratories->setValue(_base->getUsedLaboratories());

	std::ostringstream ss7;
	ss7 << _base->getUsedWorkshops() << ":" << _base->getAvailableWorkshops();
	_numWorkshops->setText(ss7.str());

	_barWorkshops->setMax(_base->getAvailableWorkshops());
	_barWorkshops->setValue(_base->getUsedWorkshops());

	if (Options::storageLimitsEnforced)
	{
		std::ostringstream ss72;
		ss72 << _base->getUsedContainment() << ":" << _base->getAvailableContainment();
		_numContainment->setText(ss72.str());

		_barContainment->setMax(_base->getAvailableContainment());
		_barContainment->setValue(_base->getUsedContainment());
	}

	std::ostringstream ss8;
	ss8 << _base->getUsedHangars() << ":" << _base->getAvailableHangars();
	_numHangars->setText(ss8.str());

	_barHangars->setMax(_base->getAvailableHangars());
	_barHangars->setValue(_base->getUsedHangars());


	std::ostringstream ss9;
	ss9 << _base->getDefenseValue();
	_numDefense->setText(ss9.str());

	_barDefense->setMax(_base->getDefenseValue());
	_barDefense->setValue(_base->getDefenseValue());

	std::ostringstream ss10;
	int shortRangeDetection = _base->getShortRangeDetection();
	ss10 << shortRangeDetection;
	_numShortRange->setText(ss10.str());

	_barShortRange->setMax(shortRangeDetection);
	_barShortRange->setValue(shortRangeDetection);

	std::ostringstream ss11;
	int longRangeDetection = _base->getLongRangeDetection();
	ss11 << longRangeDetection;
	_numLongRange->setText(ss11.str());

	_barLongRange->setMax(longRangeDetection);
	_barLongRange->setValue(longRangeDetection);
}

/**
 * Changes the base name.
 * @param action Pointer to an action.
 */
void BaseInfoState::edtBaseChange(Action *)
{
	_base->setName(_edtBase->getText());
}

/**
 * Selects a new base to display.
 * @param action Pointer to an action.
 */
void BaseInfoState::miniClick(Action *)
{
	size_t base = _mini->getHoveredBase();
	if (base < _game->getSavedGame()->getBases()->size())
	{
		_mini->setSelectedBase(base);
		_base = _game->getSavedGame()->getBases()->at(base);
		_state->setBase(_base);
		init();
	}
}

/**
 * Selects a new base to display.
 * @param action Pointer to an action.
 */
void BaseInfoState::handleKeyPress(Action *action)
{
	if (action->getDetails()->type == SDL_KEYDOWN)
	{
		SDL_Keycode baseKeys[] = {Options::keyBaseSelect1,
			                 Options::keyBaseSelect2,
			                 Options::keyBaseSelect3,
			                 Options::keyBaseSelect4,
			                 Options::keyBaseSelect5,
			                 Options::keyBaseSelect6,
			                 Options::keyBaseSelect7,
			                 Options::keyBaseSelect8};
		int key = action->getDetails()->key.keysym.sym;
		for (size_t i = 0; i < _game->getSavedGame()->getBases()->size(); ++i)
		{
			if (key == baseKeys[i])
			{
				_mini->setSelectedBase(i);
				_base = _game->getSavedGame()->getBases()->at(i);
				_state->setBase(_base);
				init();
				break;
			}
		}
	}
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BaseInfoState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Goes to the Transfers window.
 * @param action Pointer to an action.
 */
void BaseInfoState::btnTransfersClick(Action *)
{
	_game->pushState(new TransfersState(_base));
}

/**
 * Goes to the Stores screen.
 * @param action Pointer to an action.
 */
void BaseInfoState::btnStoresClick(Action *)
{
	_game->pushState(new StoresState(_base));
}

/**
 * Goes to the Monthly Costs screen.
 * @param action Pointer to an action.
 */
void BaseInfoState::btnMonthlyCostsClick(Action *)
{
	_game->pushState(new MonthlyCostsState(_base));
}

}
