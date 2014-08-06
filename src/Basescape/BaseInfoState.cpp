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
#include "BaseInfoState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/Action.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
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
	setPalette("PAL_BASESCAPE");

	add(_bg);
	add(_mini);
	add(_btnOk);
	add(_btnTransfers);
	add(_btnStores);
	add(_btnMonthlyCosts);
	add(_edtBase);

	add(_txtPersonnel);
	add(_txtSoldiers);
	add(_numSoldiers);
	add(_barSoldiers);
	add(_txtEngineers);
	add(_numEngineers);
	add(_barEngineers);
	add(_txtScientists);
	add(_numScientists);
	add(_barScientists);

	add(_txtSpace);
	add(_txtQuarters);
	add(_numQuarters);
	add(_barQuarters);
	add(_txtStores);
	add(_numStores);
	add(_barStores);
	add(_txtLaboratories);
	add(_numLaboratories);
	add(_barLaboratories);
	add(_txtWorkshops);
	add(_numWorkshops);
	add(_barWorkshops);
	if (Options::storageLimitsEnforced)
	{
		add(_txtContainment);
		add(_numContainment);
		add(_barContainment);
	}
	add(_txtHangars);
	add(_numHangars);
	add(_barHangars);

	add(_txtDefense);
	add(_numDefense);
	add(_barDefense);
	add(_txtShortRange);
	add(_numShortRange);
	add(_barShortRange);
	add(_txtLongRange);
	add(_numLongRange);
	add(_barLongRange);

	centerAllSurfaces();

	// Set up objects
	std::ostringstream ss;
	if (Options::storageLimitsEnforced)
	{
		ss << "ALT";
	}
	ss << "BACK07.SCR";
	_game->getResourcePack()->getSurface(ss.str())->blit(_bg);

	_mini->setTexture(_game->getResourcePack()->getSurfaceSet("BASEBITS.PCK"));
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

	_btnOk->setColor(Palette::blockOffset(15)+6);
	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BaseInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&BaseInfoState::btnOkClick, Options::keyCancel);

	_btnTransfers->setColor(Palette::blockOffset(15)+6);
	_btnTransfers->setText(tr("STR_TRANSFERS_UC"));
	_btnTransfers->onMouseClick((ActionHandler)&BaseInfoState::btnTransfersClick);

	_btnStores->setColor(Palette::blockOffset(15)+6);
	_btnStores->setText(tr("STR_STORES_UC"));
	_btnStores->onMouseClick((ActionHandler)&BaseInfoState::btnStoresClick);

	_btnMonthlyCosts->setColor(Palette::blockOffset(15)+6);
	_btnMonthlyCosts->setText(tr("STR_MONTHLY_COSTS"));
	_btnMonthlyCosts->onMouseClick((ActionHandler)&BaseInfoState::btnMonthlyCostsClick);

	_edtBase->setColor(Palette::blockOffset(15)+1);
	_edtBase->setBig();
	_edtBase->onChange((ActionHandler)&BaseInfoState::edtBaseChange);

	_txtPersonnel->setColor(Palette::blockOffset(15)+1);
	_txtPersonnel->setText(tr("STR_PERSONNEL_AVAILABLE_PERSONNEL_TOTAL"));

	_txtSoldiers->setColor(Palette::blockOffset(13)+5);
	_txtSoldiers->setText(tr("STR_SOLDIERS"));

	_numSoldiers->setColor(Palette::blockOffset(13));

	_barSoldiers->setColor(Palette::blockOffset(1));
	_barSoldiers->setScale(1.0);

	_txtEngineers->setColor(Palette::blockOffset(13)+5);
	_txtEngineers->setText(tr("STR_ENGINEERS"));

	_numEngineers->setColor(Palette::blockOffset(13));

	_barEngineers->setColor(Palette::blockOffset(1));
	_barEngineers->setScale(1.0);

	_txtScientists->setColor(Palette::blockOffset(13)+5);
	_txtScientists->setText(tr("STR_SCIENTISTS"));

	_numScientists->setColor(Palette::blockOffset(13));

	_barScientists->setColor(Palette::blockOffset(1));
	_barScientists->setScale(1.0);


	_txtSpace->setColor(Palette::blockOffset(15)+1);
	_txtSpace->setText(tr("STR_SPACE_USED_SPACE_AVAILABLE"));

	_txtQuarters->setColor(Palette::blockOffset(13)+5);
	_txtQuarters->setText(tr("STR_LIVING_QUARTERS_PLURAL"));

	_numQuarters->setColor(Palette::blockOffset(13));

	_barQuarters->setColor(Palette::blockOffset(3));
	_barQuarters->setScale(0.5);

	_txtStores->setColor(Palette::blockOffset(13)+5);
	_txtStores->setText(tr("STR_STORES"));

	_numStores->setColor(Palette::blockOffset(13));

	_barStores->setColor(Palette::blockOffset(3));
	_barStores->setScale(0.5);

	_txtLaboratories->setColor(Palette::blockOffset(13)+5);
	_txtLaboratories->setText(tr("STR_LABORATORIES"));

	_numLaboratories->setColor(Palette::blockOffset(13));

	_barLaboratories->setColor(Palette::blockOffset(3));
	_barLaboratories->setScale(0.5);

	_txtWorkshops->setColor(Palette::blockOffset(13)+5);
	_txtWorkshops->setText(tr("STR_WORK_SHOPS"));

	_numWorkshops->setColor(Palette::blockOffset(13));

	_barWorkshops->setColor(Palette::blockOffset(3));
	_barWorkshops->setScale(0.5);

	if (Options::storageLimitsEnforced)
	{
		_txtContainment->setColor(Palette::blockOffset(13)+5);
		_txtContainment->setText(tr("STR_ALIEN_CONTAINMENT"));

		_numContainment->setColor(Palette::blockOffset(13));

		_barContainment->setColor(Palette::blockOffset(3));
		_barContainment->setScale(0.5);
	}

	_txtHangars->setColor(Palette::blockOffset(13)+5);
	_txtHangars->setText(tr("STR_HANGARS"));

	_numHangars->setColor(Palette::blockOffset(13));

	_barHangars->setColor(Palette::blockOffset(3));
	_barHangars->setScale(18.0);


	_txtDefense->setColor(Palette::blockOffset(13)+5);
	_txtDefense->setText(tr("STR_DEFENSE_STRENGTH"));

	_numDefense->setColor(Palette::blockOffset(13));

	_barDefense->setColor(Palette::blockOffset(2));
	_barDefense->setScale(0.125);

	_txtShortRange->setColor(Palette::blockOffset(13)+5);
	_txtShortRange->setText(tr("STR_SHORT_RANGE_DETECTION"));

	_numShortRange->setColor(Palette::blockOffset(13));

	_barShortRange->setColor(Palette::blockOffset(8));
	_barShortRange->setScale(25.0);

	_txtLongRange->setColor(Palette::blockOffset(13)+5);
	_txtLongRange->setText(tr("STR_LONG_RANGE_DETECTION"));

	_numLongRange->setColor(Palette::blockOffset(13));

	_barLongRange->setColor(Palette::blockOffset(8));
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

	std::wostringstream ss;
	int var1 = _base->getAvailableSoldiers(), var2 = _base->getTotalSoldiers();
	ss << var1 << ":" << var2;
	_numSoldiers->setText(ss.str());

	_barSoldiers->setMax(var2);
	_barSoldiers->setValue(var1);

	std::wostringstream ss2;
	ss2 << (var1 = _base->getAvailableEngineers()) << ":" << (var2 = _base->getTotalEngineers());
	_numEngineers->setText(ss2.str());

	_barEngineers->setMax(var2);
	_barEngineers->setValue(var1);

	std::wostringstream ss3;
	ss3 << (var1 = _base->getAvailableScientists()) << ":" << (var2 = _base->getTotalScientists());
	_numScientists->setText(ss3.str());

	_barScientists->setMax(var2);
	_barScientists->setValue(var1);


	std::wostringstream ss4;
	ss4 << (var1 = _base->getUsedQuarters()) << ":" << (var2 = _base->getAvailableQuarters());
	_numQuarters->setText(ss4.str());

	_barQuarters->setMax(var2);
	_barQuarters->setValue(var1);

	std::wostringstream ss5;
	ss5 << (var1 = (int)floor(_base->getUsedStores() + 0.05)) << ":" << (var2 = _base->getAvailableStores());
	_numStores->setText(ss5.str());

	_barStores->setMax(var2);
	_barStores->setValue(var1);

	std::wostringstream ss6;
	ss6 << (var1 = _base->getUsedLaboratories()) << ":" << (var2 = _base->getAvailableLaboratories());
	_numLaboratories->setText(ss6.str());

	_barLaboratories->setMax(var2);
	_barLaboratories->setValue(var1);

	std::wostringstream ss7;
	ss7 << (var1 = _base->getUsedWorkshops()) << ":" << (var2 = _base->getAvailableWorkshops());
	_numWorkshops->setText(ss7.str());

	_barWorkshops->setMax(var2);
	_barWorkshops->setValue(var1);

	if (Options::storageLimitsEnforced)
	{
		std::wostringstream ss72;
		ss72 << (var1 = _base->getUsedContainment()) << ":" << (var2 = _base->getAvailableContainment());
		_numContainment->setText(ss72.str());

		_barContainment->setMax(var2);
		_barContainment->setValue(var1);
	}

	std::wostringstream ss8;
	ss8 << (var1 = _base->getUsedHangars()) << ":" << (var2 = _base->getAvailableHangars());
	_numHangars->setText(ss8.str());

	_barHangars->setMax(var2);
	_barHangars->setValue(var1);


	std::wostringstream ss9;
	ss9 << (var1 = _base->getDefenseValue());
	_numDefense->setText(ss9.str());

	_barDefense->setMax(var1);
	_barDefense->setValue(var1);

	std::wostringstream ss10;
	ss10 << (var1 =_base->getShortRangeDetection());
	_numShortRange->setText(ss10.str());

	_barShortRange->setMax(var1);
	_barShortRange->setValue(var1);

	std::wostringstream ss11;
	ss11 << (var1 = _base->getLongRangeDetection());
	_numLongRange->setText(ss11.str());

	_barLongRange->setMax(var1);
	_barLongRange->setValue(var1);
}

/**
 * Changes the base name.
 * @param action Pointer to an action.
 */
void BaseInfoState::edtBaseChange(Action *action)
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
		SDLKey baseKeys[] = {Options::keyBaseSelect1,
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
