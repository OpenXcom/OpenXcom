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
#include "ManufactureInfoState.h"
#include <algorithm>
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/ToggleTextButton.h"
#include "../Interface/Text.h"
#include "../Interface/ArrowButton.h"
#include "../Engine/Action.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/Options.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleCraft.h"
#include "../Mod/RuleItem.h"
#include "../Mod/RuleManufacture.h"
#include "../Savegame/Base.h"
#include "../Savegame/Production.h"
#include "../Engine/Timer.h"
#include "../Menu/ErrorMessageState.h"
#include "../Mod/RuleInterface.h"
#include <climits>

namespace OpenXcom
{

/**
 * Initializes all elements in the Production settings screen (new Production).
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param item The RuleManufacture to produce.
 */
ManufactureInfoState::ManufactureInfoState (Base *base, RuleManufacture *item) : _base(base), _item(item), _production(0)
{
	buildUi();
}

/**
 * Initializes all elements in the Production settings screen (modifying Production).
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param production The Production to modify.
 */
ManufactureInfoState::ManufactureInfoState (Base *base, Production *production) : _base(base), _item(0), _production(production)
{
	buildUi();
}

/**
 * Builds screen User Interface.
 */
void ManufactureInfoState::buildUi()
{
	_screen = false;

	_window = new Window(this, 320, 160, 0, 20, POPUP_BOTH);
	_txtTitle = new Text(320, 17, 0, 30);
	_btnOk = new TextButton(136, 16, 168, 155);
	_btnStop = new TextButton(136, 16, 16, 155);
	_btnSell = new ToggleTextButton(60, 16, 244, 61);
	_txtAvailableEngineer = new Text(160, 9, 16, 50);
	_txtAvailableSpace = new Text(160, 9, 16, 60);
	_txtMonthlyProfit = new Text(160, 9, 168, 50);
	_txtAllocatedEngineer = new Text(112, 32, 16, 80);
	_txtUnitToProduce = new Text(112, 48, 168, 64);
	_txtEngineerUp = new Text(90, 9, 40, 118);
	_txtEngineerDown = new Text(90, 9, 40, 138);
	_txtUnitUp = new Text(90, 9, 192, 118);
	_txtUnitDown = new Text(90, 9, 192, 138);
	_btnEngineerUp = new ArrowButton(ARROW_BIG_UP, 13, 14, 132, 114);
	_btnEngineerDown = new ArrowButton(ARROW_BIG_DOWN, 13, 14, 132, 136);
	_btnUnitUp = new ArrowButton(ARROW_BIG_UP, 13, 14, 284, 114);
	_btnUnitDown = new ArrowButton(ARROW_BIG_DOWN, 13, 14, 284, 136);
	_txtAllocated = new Text(40, 16, 128, 88);
	_txtTodo = new Text(40, 16, 280, 88);

	_surfaceEngineers = new InteractiveSurface(160, 150, 0, 25);
	_surfaceEngineers->onMouseClick((ActionHandler)&ManufactureInfoState::handleWheelEngineer, 0);

	_surfaceUnits = new InteractiveSurface(160, 150, 160, 25);
	_surfaceUnits->onMouseClick((ActionHandler)&ManufactureInfoState::handleWheelUnit, 0);

	// Set palette
	setInterface("manufactureInfo");

	add(_surfaceEngineers);
	add(_surfaceUnits);
	add(_window, "window", "manufactureInfo");
	add(_txtTitle, "text", "manufactureInfo");
	add(_txtAvailableEngineer, "text", "manufactureInfo");
	add(_txtAvailableSpace, "text", "manufactureInfo");
	add(_txtMonthlyProfit, "text", "manufactureInfo");
	add(_txtAllocatedEngineer, "text", "manufactureInfo");
	add(_txtAllocated, "text", "manufactureInfo");
	add(_txtUnitToProduce, "text", "manufactureInfo");
	add(_txtTodo, "text", "manufactureInfo");
	add(_txtEngineerUp, "text", "manufactureInfo");
	add(_txtEngineerDown, "text", "manufactureInfo");
	add(_btnEngineerUp, "button1", "manufactureInfo");
	add(_btnEngineerDown, "button1", "manufactureInfo");
	add(_txtUnitUp, "text", "manufactureInfo");
	add(_txtUnitDown, "text", "manufactureInfo");
	add(_btnUnitUp, "button1", "manufactureInfo");
	add(_btnUnitDown, "button1", "manufactureInfo");
	add(_btnOk, "button2", "manufactureInfo");
	add(_btnStop, "button2", "manufactureInfo");
	add(_btnSell, "button1", "manufactureInfo");

	centerAllSurfaces();

	_window->setBackground(_game->getMod()->getSurface("BACK17.SCR"));

	_txtTitle->setText(tr(_item ? _item->getName() : _production->getRules()->getName()));
	_txtTitle->setBig();
	_txtTitle->setAlign(ALIGN_CENTER);

	_txtAllocatedEngineer->setText(tr("STR_ENGINEERS__ALLOCATED"));
	_txtAllocatedEngineer->setBig();
	_txtAllocatedEngineer->setWordWrap(true);
	_txtAllocatedEngineer->setVerticalAlign(ALIGN_BOTTOM);

	_txtAllocated->setBig();

	_txtTodo->setBig();

	_txtUnitToProduce->setText(tr("STR_UNITS_TO_PRODUCE"));
	_txtUnitToProduce->setBig();
	_txtUnitToProduce->setWordWrap(true);
	_txtUnitToProduce->setVerticalAlign(ALIGN_BOTTOM);

	_txtEngineerUp->setText(tr("STR_INCREASE_UC"));

	_txtEngineerDown->setText(tr("STR_DECREASE_UC"));

	_btnEngineerUp->onMousePress((ActionHandler)&ManufactureInfoState::moreEngineerPress);
	_btnEngineerUp->onMouseRelease((ActionHandler)&ManufactureInfoState::moreEngineerRelease);
	_btnEngineerUp->onMouseClick((ActionHandler)&ManufactureInfoState::moreEngineerClick, 0);

	_btnEngineerDown->onMousePress((ActionHandler)&ManufactureInfoState::lessEngineerPress);
	_btnEngineerDown->onMouseRelease((ActionHandler)&ManufactureInfoState::lessEngineerRelease);
	_btnEngineerDown->onMouseClick((ActionHandler)&ManufactureInfoState::lessEngineerClick, 0);

	_btnUnitUp->onMousePress((ActionHandler)&ManufactureInfoState::moreUnitPress);
	_btnUnitUp->onMouseRelease((ActionHandler)&ManufactureInfoState::moreUnitRelease);
	_btnUnitUp->onMouseClick((ActionHandler)&ManufactureInfoState::moreUnitClick, 0);

	_btnUnitDown->onMousePress((ActionHandler)&ManufactureInfoState::lessUnitPress);
	_btnUnitDown->onMouseRelease((ActionHandler)&ManufactureInfoState::lessUnitRelease);
	_btnUnitDown->onMouseClick((ActionHandler)&ManufactureInfoState::lessUnitClick, 0);

	_txtUnitUp->setText(tr("STR_INCREASE_UC"));

	_txtUnitDown->setText(tr("STR_DECREASE_UC"));

	_btnSell->setText(tr("STR_SELL_PRODUCTION"));
	_btnSell->onMouseClick((ActionHandler)&ManufactureInfoState::btnSellClick, 0);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&ManufactureInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&ManufactureInfoState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&ManufactureInfoState::btnOkClick, Options::keyCancel);

	_btnStop->setText(tr("STR_STOP_PRODUCTION"));
	_btnStop->onMouseClick((ActionHandler)&ManufactureInfoState::btnStopClick);
	if (!_production)
	{
		_production = new Production (_item, 1);
		_base->addProduction(_production);
	}
	_btnSell->setPressed(_production->getSellItems());
	initProfitInfo();
	setAssignedEngineer();

	_timerMoreEngineer = new Timer(250);
	_timerLessEngineer = new Timer(250);
	_timerMoreUnit = new Timer(250);
	_timerLessUnit = new Timer(250);
	_timerMoreEngineer->onTimer((StateHandler)&ManufactureInfoState::onMoreEngineer);
	_timerLessEngineer->onTimer((StateHandler)&ManufactureInfoState::onLessEngineer);
	_timerMoreUnit->onTimer((StateHandler)&ManufactureInfoState::onMoreUnit);
	_timerLessUnit->onTimer((StateHandler)&ManufactureInfoState::onLessUnit);
}

void ManufactureInfoState::initProfitInfo ()
{
	Mod *mod = _game->getMod();
	const RuleManufacture *item = _production->getRules();

	_producedItemsValue = 0;
	for (std::map<std::string, int>::const_iterator i = item->getProducedItems().begin(); i != item->getProducedItems().end(); ++i)
	{
		int sellValue = 0;
		if (item->getCategory() == "STR_CRAFT")
		{
			sellValue = mod->getCraft(i->first, true)->getSellCost();
		}
		else
		{
			sellValue = mod->getItem(i->first, true)->getSellCost();
		}
		_producedItemsValue += sellValue * i->second;
	}
}

// note that this function calculates only the change in funds, not the change
// in net worth.  after discussion in the forums, it was decided that focusing
// only on visible changes in funds was clearer and more valuable to the player
// than trying to take used materials and maintenance costs into account.
int ManufactureInfoState::getMonthlyNetFunds () const
{
	// does not take into account leap years, but a game is unlikely to take long enough for that to matter
	static const int AVG_HOURS_PER_MONTH = (365 * 24) / 12;

	const RuleManufacture *item = _production->getRules();
	int saleValue = _btnSell->getPressed() ? _producedItemsValue : 0;

	int numEngineers = _production->getAssignedEngineers();
	int manHoursPerMonth = AVG_HOURS_PER_MONTH * numEngineers;
	if (!_production->getInfiniteAmount())
	{
		// scale down to actual number of man hours required if the job will
		// take less than one month
		int manHoursRemaining = item->getManufactureTime() * (_production->getAmountTotal() - _production->getAmountProduced());
		manHoursPerMonth = std::min(manHoursPerMonth, manHoursRemaining);
	}
	float itemsPerMonth = (float)manHoursPerMonth / (float)item->getManufactureTime();

	return (saleValue - item->getManufactureCost()) * itemsPerMonth;
}

/**
 * Frees up memory that's not automatically cleaned on exit
 */
ManufactureInfoState::~ManufactureInfoState()
{
	delete _timerMoreEngineer;
	delete _timerLessEngineer;
	delete _timerMoreUnit;
	delete _timerLessUnit;
}

/**
 * Refreshes profit values.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::btnSellClick(Action *)
{
	setAssignedEngineer();
}

/**
 * Stops this Production. Returns to the previous screen.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::btnStopClick(Action *)
{
	_base->removeProduction(_production);
	exitState();
}

/**
 * Starts this Production (if new). Returns to the previous screen.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::btnOkClick(Action *)
{
	if (_item)
	{
		_production->startItem(_base, _game->getSavedGame(), _game->getMod());
	}
	_production->setSellItems(_btnSell->getPressed());
	exitState();
}

/**
 * Returns to the previous screen.
 */
void ManufactureInfoState::exitState()
{
	_game->popState();
	if (_item)
	{
		_game->popState();
	}
}

/**
 * Updates display of assigned/available engineer/workshop space.
 */
void ManufactureInfoState::setAssignedEngineer()
{
	_txtAvailableEngineer->setText(tr("STR_ENGINEERS_AVAILABLE_UC").arg(_base->getAvailableEngineers()));
	_txtAvailableSpace->setText(tr("STR_WORKSHOP_SPACE_AVAILABLE_UC").arg(_base->getFreeWorkshops()));
	std::wostringstream s3;
	s3 << L">\x01" << _production->getAssignedEngineers();
	_txtAllocated->setText(s3.str());
	std::wostringstream s4;
	s4 << L">\x01";
	if (_production->getInfiniteAmount()) s4 << Language::utf8ToWstr("∞");
	else s4 << _production->getAmountTotal();
	_txtTodo->setText(s4.str());
	_txtMonthlyProfit->setText(tr("STR_MONTHLY_PROFIT").arg(Text::formatFunding(getMonthlyNetFunds()).c_str()));
}

/**
 * Adds given number of engineers to the project if possible.
 * @param change How much we want to add.
 */
void ManufactureInfoState::moreEngineer(int change)
{
	if (change <= 0) return;
	int availableEngineer = _base->getAvailableEngineers();
	int availableWorkSpace = _base->getFreeWorkshops();
	if (availableEngineer > 0 && availableWorkSpace > 0)
	{
		change = std::min(std::min(availableEngineer, availableWorkSpace), change);
		_production->setAssignedEngineers(_production->getAssignedEngineers()+change);
		_base->setEngineers(_base->getEngineers()-change);
		setAssignedEngineer();
	}
}

/**
 * Starts the timerMoreEngineer.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreEngineerPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerMoreEngineer->start();
}

/**
 * Stops the timerMoreEngineer.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreEngineerRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerMoreEngineer->setInterval(250);
		_timerMoreEngineer->stop();
	}
}

/**
 * Allocates all engineers.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreEngineerClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) moreEngineer(INT_MAX);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) moreEngineer(1);
}

/**
 * Removes the given number of engineers from the project if possible.
 * @param change How much we want to subtract.
 */
void ManufactureInfoState::lessEngineer(int change)
{
	if (change <= 0) return;
	int assigned = _production->getAssignedEngineers();
	if (assigned > 0)
	{
		change = std::min(assigned, change);
		_production->setAssignedEngineers(assigned-change);
		_base->setEngineers(_base->getEngineers()+change);
		setAssignedEngineer();
	}
}

/**
 * Starts the timerLessEngineer.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessEngineerPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLessEngineer->start();
}

/**
 * Stops the timerLessEngineer.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessEngineerRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerLessEngineer->setInterval(250);
		_timerLessEngineer->stop();
	}
}

/**
 * Removes engineers from the production.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessEngineerClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT) lessEngineer(INT_MAX);
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) lessEngineer(1);
}

/**
 * Adds given number of units to produce to the project if possible.
 * @param change How much we want to add.
 */
void ManufactureInfoState::moreUnit(int change)
{
	if (change <= 0) return;
	if (_production->getRules()->getCategory() == "STR_CRAFT" && _base->getAvailableHangars() - _base->getUsedHangars() <= 0)
	{
		_timerMoreUnit->stop();
		_game->pushState(new ErrorMessageState(tr("STR_NO_FREE_HANGARS_FOR_CRAFT_PRODUCTION"), _palette, _game->getMod()->getInterface("basescape")->getElement("errorMessage")->color, "BACK17.SCR", _game->getMod()->getInterface("basescape")->getElement("errorPalette")->color));
	}
	else
	{
		int units = _production->getAmountTotal();
		change = std::min(INT_MAX - units, change);
		if (_production->getRules()->getCategory() == "STR_CRAFT")
			change = std::min(_base->getAvailableHangars() - _base->getUsedHangars(), change);
		_production->setAmountTotal(units+change);
		setAssignedEngineer();
	}
}

/**
 * Starts the timerMoreUnit.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreUnitPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && _production->getAmountTotal() < INT_MAX)
		_timerMoreUnit->start();
}

/**
 * Stops the timerMoreUnit.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreUnitRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerMoreUnit->setInterval(250);
		_timerMoreUnit->stop();
	}
}

/**
 * Increases the "units to produce", in the case of a right-click, to infinite, and 1 on left-click.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::moreUnitClick(Action *action)
{
	if (_production->getInfiniteAmount()) return; // We can't increase over infinite :)
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		if (_production->getRules()->getCategory() == "STR_CRAFT")
		{
			moreUnit(INT_MAX);
		}
		else
		{
			_production->setInfiniteAmount(true);
			setAssignedEngineer();
		}
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		moreUnit(1);
	}
}

/**
 * Removes the given number of units to produce from the project if possible.
 * @param change How much we want to subtract.
 */
void ManufactureInfoState::lessUnit(int change)
{
	if (change <= 0) return;
	int units = _production->getAmountTotal();
	change = std::min(units-(_production->getAmountProduced()+1), change);
	_production->setAmountTotal(units-change);
	setAssignedEngineer();
}

/**
 * Starts the timerLessUnit.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessUnitPress(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT) _timerLessUnit->start();
}

/**
 * Stops the timerLessUnit.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessUnitRelease(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_timerLessUnit->setInterval(250);
		_timerLessUnit->stop();
	}
}

/**
 * Decreases the units to produce.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::lessUnitClick(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_RIGHT
	||  action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		_production->setInfiniteAmount(false);
		if (action->getDetails()->button.button == SDL_BUTTON_RIGHT
		|| _production->getAmountTotal() <= _production->getAmountProduced())
		{ // So the produced item number is increased over the planned, OR it was simply a right-click
			_production->setAmountTotal(_production->getAmountProduced()+1);
			setAssignedEngineer();
		}
		if (action->getDetails()->button.button == SDL_BUTTON_LEFT) lessUnit(1);
	}
}

/**
 * Assigns one more engineer (if possible).
 */
void ManufactureInfoState::onMoreEngineer()
{
	_timerMoreEngineer->setInterval(50);
	moreEngineer(1);
}

/**
 * Removes one engineer (if possible).
 */
void ManufactureInfoState::onLessEngineer()
{
	_timerLessEngineer->setInterval(50);
	lessEngineer(1);
}

/**
 * Increases or decreases the Engineers according the mouse-wheel used.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::handleWheelEngineer(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP) moreEngineer(Options::changeValueByMouseWheel);
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN) lessEngineer(Options::changeValueByMouseWheel);
}

/**
 * Builds one more unit.
 */
void ManufactureInfoState::onMoreUnit()
{
	_timerMoreUnit->setInterval(50);
	moreUnit(1);
}

/**
 * Builds one less unit( if possible).
 */
void ManufactureInfoState::onLessUnit()
{
	_timerLessUnit->setInterval(50);
	lessUnit(1);
}

/**
 * Increases or decreases the Units to produce according the mouse-wheel used.
 * @param action A pointer to an Action.
 */
void ManufactureInfoState::handleWheelUnit(Action *action)
{
	if (action->getDetails()->button.button == SDL_BUTTON_WHEELUP) moreUnit(Options::changeValueByMouseWheel);
	else if (action->getDetails()->button.button == SDL_BUTTON_WHEELDOWN) lessUnit(Options::changeValueByMouseWheel);
}

/**
 * Runs state functionality every cycle (used to update the timer).
 */
void ManufactureInfoState::think()
{
	State::think();
	_timerMoreEngineer->think(this, 0);
	_timerLessEngineer->think(this, 0);
	_timerMoreUnit->think(this, 0);
	_timerLessUnit->think(this, 0);
}

}
